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

#pragma once

#include <atomic>
#include <QDateTime>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QThread>
#include <QTimer>


class BtBookmarksModel;

/**
  Synchronizes the bookmarks (and their folders) with the "Phos" app on a
  Nextcloud server. Runs in a worker thread; all bookmark changes happen on the
  GUI thread.
*/
class BtPhosSync: public QObject {

    Q_OBJECT

public: // types:

    struct Settings {
        bool enabled = false;
        QString server;
        QString user;
        /** A Nextcloud app password (a login password is exchanged first). */
        QString appPassword;
        bool configured() const {
            return enabled && !server.isEmpty() && !user.isEmpty()
                   && !appPassword.isEmpty();
        }
    };

public: // methods:

    explicit BtPhosSync(BtBookmarksModel * model, QObject * parent = nullptr);
    ~BtPhosSync() override;

    static BtPhosSync * instance() noexcept { return m_instance; }

    static Settings settings();
    static void saveSettings(Settings const & settings);

    static QDateTime lastRun();
    /** A one line summary of the last run, for the settings. */
    static QString lastResult();

    /**
      Asks the server for an app password in exchange for the login password.
      Blocks for the time of one request. Throws BtPhosHttpError.
    */
    static QString exchangePassword(QString const & server,
                                    QString const & user,
                                    QString const & password);

    bool isRunning() const noexcept { return m_running; }

    /** Re-reads the settings and starts or stops the timers. */
    void reloadSettings();

    /** Starts a run now (unless one is running). */
    void syncNow();

    /** Forgets everything about the server. Local bookmarks stay. */
    void reset();

    /** Cancels a running sync and waits (a few seconds at most) for it. */
    void shutdown();

Q_SIGNALS:

    void started();
    void finished(bool ok, QString const & summary);

private: // methods:

    void bookmarksChanged();
    void startRun();
    void runFinished(bool ok, QString const & summary);
    static QString stateFile();

private: // fields:

    static BtPhosSync * m_instance;
    QPointer<BtBookmarksModel> m_model;
    QTimer m_soon;
    QTimer m_periodic;
    QPointer<QThread> m_thread;
    std::atomic_bool m_running{false};
    std::atomic_bool m_cancel{false};
    std::atomic_bool m_applying{false};
    bool m_again = false;

};
