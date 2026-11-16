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

#include <QJsonArray>
#include <QMap>
#include <QTest>
#include "btphosbookmarkcodec.h"
#include "btphoshttpclient.h"
#include "btphossyncengine.h"


/*
  Against a real Nextcloud with the Phos app. Runs only when PHOS_LIVE_URL,
  PHOS_LIVE_USER and PHOS_LIVE_PASSWORD are set; use a fresh test account.
*/

namespace C = BtPhosBookmarkCodec;

namespace {

class MemStore final: public BtPhosStateStore {
public:
    QByteArray load() override { return data; }
    void save(QByteArray const & d) override { data = d; }
    QByteArray data;
};

class Items final: public BtPhosAdapter {

public: // methods:

    Items(QString type, int priority) : m_type(std::move(type)), m_priority(priority) {}

    QString type() const override { return m_type; }
    int priority() const override { return m_priority; }

    QList<BtPhosItem> enumerate() override {
        QList<BtPhosItem> out;
        for (auto it = items.cbegin(); it != items.cend(); ++it)
            out.append({it.key(), it.value()});
        return out;
    }

    bool apply(QString const & key, QJsonObject const & payload, bool deleted) override {
        if (deleted) {
            items.remove(key);
        } else {
            items.insert(key, payload);
        }
        return true;
    }

    std::optional<QSet<QString>> ownedFields() const override {
        return m_type == QStringLiteral("label") ? C::labelFields() : C::bookmarkFields();
    }

    std::optional<QJsonObject> overlay(QJsonObject const * server,
                                       QJsonObject const & mine) const override
    {
        if (m_type == QStringLiteral("bookmark"))
            return C::bookmarkOverlay(server, mine);
        return std::nullopt;
    }

    bool mapsKeys() const override { return m_type == QStringLiteral("bookmark"); }

    QString localKeyFor(QString const & serverKey) const override {
        if (!mapsKeys())
            return serverKey;
        return QStringLiteral("bm:") + BtPhosIds::bookmarkUuidHex(serverKey.mid(3));
    }

public: // fields:

    QMap<QString, QJsonObject> items;

private: // fields:

    QString m_type;
    int m_priority;

};

} // anonymous namespace

class PhosLiveTest: public QObject {

    Q_OBJECT

private Q_SLOTS:

    void roundTripThroughServer() {
        auto const url = qEnvironmentVariable("PHOS_LIVE_URL");
        auto const user = qEnvironmentVariable("PHOS_LIVE_USER");
        auto const password = qEnvironmentVariable("PHOS_LIVE_PASSWORD");
        if (url.isEmpty() || user.isEmpty() || password.isEmpty())
            QSKIP("PHOS_LIVE_URL, PHOS_LIVE_USER and PHOS_LIVE_PASSWORD not set");

        auto const appPassword = BtPhosHttpClient::appPassword(
                    url, user, password, QStringLiteral("BibleTime test"));

        C::ModuleInfo const kjv{true, true, QStringLiteral("KJV")};
        C::Bookmark bm;
        bm.id = BtPhosIds::derivedUuidHex(QStringLiteral("bibletime-test"),
                                          QStringLiteral("1"));
        bm.folder = {QStringLiteral("BibleTime"), QStringLiteral("Test")};
        bm.module = QStringLiteral("KJV");
        bm.key = QStringLiteral("John 3:16");
        bm.description = QStringLiteral("Gott liebt");
        bm.created = 1700000000;
        bm.modified = 1700000100;
        C::Bookmark entry;
        entry.id = BtPhosIds::derivedUuidHex(QStringLiteral("bibletime-test"),
                                             QStringLiteral("2"));
        entry.module = QStringLiteral("StrongsGreek");
        entry.key = QStringLiteral("00025");
        entry.title = QStringLiteral("agapao");
        entry.created = entry.modified = 1700000200;

        Items labelsA(QStringLiteral("label"), 10);
        Items bookmarksA(QStringLiteral("bookmark"), 20);
        labelsA.items.insert(QStringLiteral("label:BibleTime/Test"),
                             C::labelToJson(bm.folder));
        bookmarksA.items.insert(QStringLiteral("bm:") + bm.id,
                                *C::bookmarkToJson(bm, kjv));
        bookmarksA.items.insert(QStringLiteral("bm:") + entry.id,
                                *C::bookmarkToJson(entry, {true, false, {}}));

        BtPhosHttpClient http(url, user, appPassword);
        MemStore storeA;
        auto const r1 = BtPhosSyncEngine(http, storeA, {&labelsA, &bookmarksA},
                                         QStringLiteral("live-a")).run();
        QCOMPARE(r1.problems, QStringList());
        QCOMPARE(r1.pushed, 3);

        Items labelsB(QStringLiteral("label"), 10);
        Items bookmarksB(QStringLiteral("bookmark"), 20);
        MemStore storeB;
        auto const r2 = BtPhosSyncEngine(http, storeB, {&labelsB, &bookmarksB},
                                         QStringLiteral("live-b")).run();
        QCOMPARE(r2.problems, QStringList());
        QCOMPARE(labelsB.items.value(QStringLiteral("label:BibleTime/Test"))
                 .value(QStringLiteral("name")).toString(),
                 QStringLiteral("BibleTime/Test"));
        QCOMPARE(bookmarksB.items.value(QStringLiteral("bm:") + bm.id),
                 bookmarksA.items.value(QStringLiteral("bm:") + bm.id));
        QCOMPARE(bookmarksB.items.value(QStringLiteral("bm:") + entry.id),
                 bookmarksA.items.value(QStringLiteral("bm:") + entry.id));

        // nothing changed: a second run sends nothing
        auto const r3 = BtPhosSyncEngine(http, storeB, {&labelsB, &bookmarksB},
                                         QStringLiteral("live-b")).run();
        QCOMPARE(r3.pushed, 0);

        // a deletion travels back
        bookmarksB.items.remove(QStringLiteral("bm:") + entry.id);
        BtPhosSyncEngine(http, storeB, {&labelsB, &bookmarksB},
                         QStringLiteral("live-b")).run();
        BtPhosSyncEngine(http, storeA, {&labelsA, &bookmarksA},
                         QStringLiteral("live-a")).run();
        QVERIFY(!bookmarksA.items.contains(QStringLiteral("bm:") + entry.id));
    }

};

QTEST_GUILESS_MAIN(PhosLiveTest)
#include "test_phoslive.moc"
