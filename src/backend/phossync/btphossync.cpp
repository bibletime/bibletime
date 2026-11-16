/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btphossync.h"

#include <QCoreApplication>
#include <QDeadlineTimer>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QSysInfo>
#include <QThread>
#include "../../util/btconnect.h"
#include "../../util/directory.h"
#include "../btbookmarksmodel.h"
#include "../config/btconfig.h"
#include "btphosbookmarkadapter.h"
#include "btphoshttpclient.h"
#include "btphossyncengine.h"


namespace {

auto const ENABLED = QStringLiteral("PhosSync/enabled");
auto const SERVER = QStringLiteral("PhosSync/server");
auto const USER = QStringLiteral("PhosSync/user");
auto const APP_PASSWORD = QStringLiteral("PhosSync/appPassword");
auto const LAST_RUN = QStringLiteral("PhosSync/lastRun");
auto const LAST_RESULT = QStringLiteral("PhosSync/lastResult");

constexpr int SOON_MS = 30 * 1000;
constexpr int PERIODIC_MS = 5 * 60 * 1000;

QString userAgent() {
    return QStringLiteral("BibleTime (%1)").arg(QSysInfo::machineHostName());
}

class FileStateStore final: public BtPhosStateStore {

public: // methods:

    explicit FileStateStore(QString fileName) : m_fileName(std::move(fileName)) {}

    QByteArray load() final override {
        QFile f(m_fileName);
        if (!f.open(QIODevice::ReadOnly))
            return {};
        return f.readAll();
    }

    void save(QByteArray const & data) final override {
        QSaveFile f(m_fileName);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(data);
            f.commit();
        }
    }

private: // fields:

    QString m_fileName;

};

} // anonymous namespace

BtPhosSync * BtPhosSync::m_instance = nullptr;

BtPhosSync::BtPhosSync(BtBookmarksModel * model, QObject * parent)
    : QObject(parent)
    , m_model(model)
{
    m_instance = this;
    m_soon.setSingleShot(true);
    BT_CONNECT(&m_soon, &QTimer::timeout, this, &BtPhosSync::startRun);
    BT_CONNECT(&m_periodic, &QTimer::timeout, this, &BtPhosSync::startRun);
    if (model)
        BT_CONNECT(model, &BtBookmarksModel::bookmarksChanged,
                   this, &BtPhosSync::bookmarksChanged);
    reloadSettings();
    if (settings().configured())
        m_soon.start(3000);
}

BtPhosSync::~BtPhosSync() {
    shutdown();
    if (m_instance == this)
        m_instance = nullptr;
}

BtPhosSync::Settings BtPhosSync::settings() {
    auto & c = btConfig();
    Settings s;
    s.enabled = c.value<bool>(ENABLED, false);
    s.server = c.value<QString>(SERVER, QString());
    s.user = c.value<QString>(USER, QString());
    s.appPassword = c.value<QString>(APP_PASSWORD, QString());
    return s;
}

void BtPhosSync::saveSettings(Settings const & s) {
    auto & c = btConfig();
    auto const old = settings();
    c.setValue(ENABLED, s.enabled);
    c.setValue(SERVER, s.server);
    c.setValue(USER, s.user);
    c.setValue(APP_PASSWORD, s.appPassword);
    if (old.server != s.server || old.user != s.user) {
        c.setValue(LAST_RUN, QDateTime());
        c.setValue(LAST_RESULT, QString());
    }
}

QDateTime BtPhosSync::lastRun()
{ return btConfig().value<QDateTime>(LAST_RUN, QDateTime()); }

QString BtPhosSync::lastResult()
{ return btConfig().value<QString>(LAST_RESULT, QString()); }

QString BtPhosSync::exchangePassword(QString const & server,
                                     QString const & user,
                                     QString const & password)
{ return BtPhosHttpClient::appPassword(server, user, password, userAgent()); }

QString BtPhosSync::stateFile() {
    return util::directory::getUserBaseDir().absolutePath()
            + QStringLiteral("/phos-sync-state.json");
}

void BtPhosSync::reloadSettings() {
    if (settings().configured()) {
        m_periodic.start(PERIODIC_MS);
    } else {
        m_periodic.stop();
        m_soon.stop();
    }
}

void BtPhosSync::bookmarksChanged() {
    if (m_applying || !settings().configured())
        return;
    if (!m_soon.isActive())
        m_soon.start(SOON_MS);
}

void BtPhosSync::syncNow() {
    m_soon.stop();
    startRun();
}

void BtPhosSync::reset() {
    if (m_running)
        return;
    QFile::remove(stateFile());
    btConfig().setValue(LAST_RUN, QDateTime());
    btConfig().setValue(LAST_RESULT, QString());
}

void BtPhosSync::startRun() {
    auto const s = settings();
    if (!s.configured() || !m_model)
        return;
    if (m_running) {
        m_again = true;
        return;
    }
    m_running = true;
    m_cancel = false;
    Q_EMIT started();

    auto * const model = m_model.data();
    m_thread = QThread::create([this, s, model] {
        bool ok = false;
        QString summary;
        try {
            BtPhosHttpClient http(s.server, s.user, s.appPassword, userAgent(),
                                  &m_cancel);
            FileStateStore store(stateFile());
            BtPhosModelAccess access(model, m_cancel, m_applying);
            BtPhosLabelAdapter labels(access);
            BtPhosBookmarkAdapter bookmarks(access);
            BtPhosSyncEngine engine(http, store, {&labels, &bookmarks},
                                    QSysInfo::machineHostName(),
                                    QStringLiteral(BT_VERSION),
                                    QStringLiteral("bibletime"));
            auto const r = engine.run();
            QStringList parts;
            parts.append(tr("%1 sent, %2 received").arg(r.pushed).arg(r.pulled));
            if (r.conflicts > 0)
                parts.append(tr("%1 conflicts").arg(r.conflicts));
            if (r.heldDeletes > 0)
                parts.append(tr("%1 deletions held back").arg(r.heldDeletes));
            parts.append(r.problems.mid(0, 3));
            summary = parts.join(QStringLiteral(", "));
            ok = true;
        } catch (std::exception const & e) {
            m_applying = false;
            summary = tr("Failed: %1").arg(QString::fromUtf8(e.what()));
        }
        QMetaObject::invokeMethod(this,
                                  [this, ok, summary] { runFinished(ok, summary); },
                                  Qt::QueuedConnection);
    });
    BT_CONNECT(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    m_thread->start();
}

void BtPhosSync::runFinished(bool const ok, QString const & summary) {
    m_thread = nullptr;
    m_running = false;
    btConfig().setValue(LAST_RUN, QDateTime::currentDateTimeUtc());
    btConfig().setValue(LAST_RESULT, summary);
    Q_EMIT finished(ok, summary);
    if (m_again && !m_cancel) {
        m_again = false;
        m_soon.start(1000);
    }
}

void BtPhosSync::shutdown() {
    m_soon.stop();
    m_periodic.stop();
    if (!m_running || !m_thread)
        return;
    m_cancel = true;
    // let the worker finish its calls to the GUI thread, then wait for it
    QDeadlineTimer deadline(5000);
    while (m_thread && !m_thread->isFinished() && !deadline.hasExpired()) {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents, 50);
        if (m_thread)
            m_thread->wait(50);
    }
    if (m_thread && !m_thread->isFinished())
        m_thread->wait();
}
