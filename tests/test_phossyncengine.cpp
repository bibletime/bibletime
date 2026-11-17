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
#include "btphossyncengine.h"


namespace {

/** A minimal stand-in for the server: last write wins, one collection. */
class FakeServer final: public BtPhosHttp {

public: // types:

    struct Rec {
        QString type;
        QString key;
        qint64 rev = 0;
        QString hlc;
        bool deleted = false;
        QJsonObject payload;
        qint64 seq = 0;
    };

public: // methods:

    QJsonObject recJson(Rec const & r) const {
        QJsonObject o{{QStringLiteral("collection"), QStringLiteral("p1")},
                      {QStringLiteral("type"), r.type},
                      {QStringLiteral("key"), r.key},
                      {QStringLiteral("rev"), r.rev},
                      {QStringLiteral("hlc"), r.hlc},
                      {QStringLiteral("device"), QStringLiteral("x")},
                      {QStringLiteral("deleted"), r.deleted}};
        if (!r.payload.isEmpty())
            o.insert(QStringLiteral("payload"), r.payload);
        return o;
    }

    QJsonObject get(QString const & path, Params const & params) override {
        if (path == QStringLiteral("/status"))
            return {{QStringLiteral("personalCollection"), QStringLiteral("p1")},
                    {QStringLiteral("serverTime"), qint64(1700000000000)}};
        if (path == QStringLiteral("/conflicts"))
            return {{QStringLiteral("conflicts"), QJsonArray()}};
        if (path == QStringLiteral("/changes")) {
            qint64 cursor = 0;
            QStringList types;
            for (auto const & [k, v] : params) {
                if (k == QStringLiteral("cursor"))
                    cursor = v.toLongLong();
                if (k == QStringLiteral("types"))
                    types = v.split(u',');
            }
            QList<Rec> list;
            for (auto const & r : std::as_const(records))
                if (r.seq > cursor && types.contains(r.type))
                    list.append(r);
            std::sort(list.begin(), list.end(),
                      [](Rec const & a, Rec const & b) { return a.seq < b.seq; });
            QJsonArray changes;
            auto newCursor = cursor;
            for (auto const & r : list) {
                changes.append(recJson(r));
                newCursor = std::max(newCursor, r.seq);
            }
            return {{QStringLiteral("changes"), changes},
                    {QStringLiteral("cursor"), newCursor},
                    {QStringLiteral("hasMore"), false},
                    {QStringLiteral("collections"),
                     QJsonArray{QJsonObject{{QStringLiteral("id"), QStringLiteral("p1")},
                                            {QStringLiteral("kind"), QStringLiteral("personal")},
                                            {QStringLiteral("owner"), QStringLiteral("u")}}}}};
        }
        throw BtPhosHttpError(404, path);
    }

    QJsonObject post(QString const & path, QJsonObject const & body) override {
        if (path == QStringLiteral("/devices"))
            return {};
        if (path == QStringLiteral("/push")) {
            pushes.append(body);
            QJsonArray results;
            for (auto const & v : body.value(QStringLiteral("changes")).toArray()) {
                auto const ch = v.toObject();
                auto const type = ch.value(QStringLiteral("type")).toString();
                auto const key = ch.value(QStringLiteral("key")).toString();
                auto & r = records[type + u'\x01' + key];
                r.type = type;
                r.key = key;
                ++r.rev;
                r.hlc = ch.value(QStringLiteral("hlc")).toString();
                r.deleted = ch.value(QStringLiteral("op")).toString()
                            == QStringLiteral("delete");
                r.payload = ch.value(QStringLiteral("payload")).toObject();
                r.seq = ++m_seq;
                results.append(QJsonObject{
                    {QStringLiteral("changeId"), ch.value(QStringLiteral("changeId"))},
                    {QStringLiteral("status"), QStringLiteral("accepted")},
                    {QStringLiteral("record"), recJson(r)}});
            }
            return {{QStringLiteral("results"), results}};
        }
        throw BtPhosHttpError(404, path);
    }

    int countOfType(QString const & type) const {
        int n = 0;
        for (auto const & r : records)
            if (r.type == type)
                ++n;
        return n;
    }

public: // fields:

    QMap<QString, Rec> records;
    QList<QJsonObject> pushes;

private: // fields:

    qint64 m_seq = 0;

};

class MemStore final: public BtPhosStateStore {

public: // methods:

    QByteArray load() override { return data; }
    void save(QByteArray const & d) override { data = d; }

public: // fields:

    QByteArray data;

};

/** Items as plain JSON objects; "bm:" keys are mapped like the bookmark adapter does. */
class MapAdapter final: public BtPhosAdapter {

public: // methods:

    explicit MapAdapter(QString type, bool mapKeys = false)
        : m_type(std::move(type))
        , m_mapKeys(mapKeys)
    {}

    QString type() const override { return m_type; }

    QList<BtPhosItem> enumerate() override {
        QList<BtPhosItem> out;
        for (auto it = items.cbegin(); it != items.cend(); ++it)
            out.append({it.key(), it.value()});
        return out;
    }

    bool apply(QString const & key, QJsonObject const & payload, bool deleted) override {
        ++applied;
        if (failOn == key)
            throw std::runtime_error("disk full");
        if (deleted) {
            items.remove(key);
        } else {
            items.insert(key, payload);
        }
        return true;
    }

    bool mapsKeys() const override { return m_mapKeys; }

    QString localKeyFor(QString const & serverKey) const override {
        if (!m_mapKeys)
            return serverKey;
        return QStringLiteral("bm:") + BtPhosIds::bookmarkUuidHex(serverKey.mid(3));
    }

public: // fields:

    QMap<QString, QJsonObject> items;
    int applied = 0;
    QString failOn;

private: // fields:

    QString m_type;
    bool m_mapKeys;

};

BtPhosRunResult runOnce(FakeServer & server,
                        MemStore & store,
                        QList<BtPhosAdapter *> adapters,
                        bool confirmDeletes = false)
{
    BtPhosSyncEngine engine(server, store, std::move(adapters),
                            QStringLiteral("test"), {}, QStringLiteral("bibletime"),
                            [] { return qint64(1700000100000); });
    return engine.run(confirmDeletes);
}

QJsonObject named(QString const & name)
{ return {{QStringLiteral("name"), name}}; }

} // anonymous namespace

class PhosSyncEngineTest: public QObject {

    Q_OBJECT

private Q_SLOTS:

    void pushThenPullOnSecondDevice() {
        FakeServer server;
        MapAdapter a(QStringLiteral("label"));
        a.items.insert(QStringLiteral("label:Gebet"),
                       {{QStringLiteral("name"), QStringLiteral("Gebet")},
                        {QStringLiteral("color"), QStringLiteral("#ff0000")}});
        MemStore sa;
        QCOMPARE(runOnce(server, sa, {&a}).pushed, 1);

        MapAdapter b(QStringLiteral("label"));
        MemStore sb;
        auto const r2 = runOnce(server, sb, {&b});
        QCOMPARE(r2.pulled, 1);
        QCOMPARE(b.items.value(QStringLiteral("label:Gebet"))
                 .value(QStringLiteral("color")).toString(),
                 QStringLiteral("#ff0000"));
        QCOMPARE(r2.appliedKeys.value(QStringLiteral("label")),
                 QSet<QString>{QStringLiteral("label:Gebet")});
    }

    void unchangedItemsAreNotSentAgain() {
        FakeServer server;
        MapAdapter a(QStringLiteral("label"));
        a.items.insert(QStringLiteral("label:A"), named(QStringLiteral("A")));
        MemStore store;
        runOnce(server, store, {&a});
        QCOMPARE(runOnce(server, store, {&a}).pushed, 0);
        QCOMPARE(server.pushes.size(), 1);
    }

    void pulledRecordIsNotEchoed() {
        FakeServer server;
        MapAdapter a(QStringLiteral("label"));
        a.items.insert(QStringLiteral("label:A"), named(QStringLiteral("A")));
        MemStore sa;
        runOnce(server, sa, {&a});
        MapAdapter b(QStringLiteral("label"));
        MemStore sb;
        runOnce(server, sb, {&b});
        auto const before = server.pushes.size();
        runOnce(server, sb, {&b});
        QCOMPARE(server.pushes.size(), before);
    }

    void deletionTravels() {
        FakeServer server;
        MapAdapter a(QStringLiteral("label"));
        a.items.insert(QStringLiteral("label:A"), named(QStringLiteral("A")));
        MemStore sa;
        runOnce(server, sa, {&a});
        MapAdapter b(QStringLiteral("label"));
        MemStore sb;
        runOnce(server, sb, {&b});
        QCOMPARE(b.items.size(), 1);
        a.items.clear();
        runOnce(server, sa, {&a});
        runOnce(server, sb, {&b});
        QVERIFY(b.items.isEmpty());
    }

    void foreignBookmarkIdIsMappedAndPushedBackUnderItsOwnKey() {
        FakeServer server;
        MapAdapter desktop(QStringLiteral("bookmark"));
        desktop.items.insert(QStringLiteral("bm:6A168692-F30E62E0"),
                             {{QStringLiteral("id"), QStringLiteral("6A168692-F30E62E0")},
                              {QStringLiteral("noteMd"), QStringLiteral("alt")}});
        MemStore sd;
        runOnce(server, sd, {&desktop});

        MapAdapter bt(QStringLiteral("bookmark"), true);
        MemStore sb;
        runOnce(server, sb, {&bt});
        auto const local = QStringLiteral("bm:244349bdbc34450a83dc66bb6ef728a4");
        QVERIFY(bt.items.contains(local));
        QVERIFY(!bt.items.contains(QStringLiteral("bm:6A168692-F30E62E0")));

        bt.items[local].insert(QStringLiteral("noteMd"), QStringLiteral("neu"));
        runOnce(server, sb, {&bt});
        auto const changes =
                server.pushes.last().value(QStringLiteral("changes")).toArray();
        QCOMPARE(changes.size(), 1);
        QCOMPARE(changes[0].toObject().value(QStringLiteral("key")).toString(),
                 QStringLiteral("bm:6A168692-F30E62E0"));
        QCOMPARE(server.countOfType(QStringLiteral("bookmark")), 1);
    }

    void massDeleteIsHeldBack() {
        FakeServer server;
        MapAdapter a(QStringLiteral("label"));
        for (int i = 1; i <= 20; ++i)
            a.items.insert(QStringLiteral("label:L%1").arg(i),
                           named(QStringLiteral("L%1").arg(i)));
        MemStore store;
        runOnce(server, store, {&a});
        a.items.clear();
        auto const r = runOnce(server, store, {&a});
        QCOMPARE(r.heldDeletes, 20);
        for (auto const & rec : std::as_const(server.records))
            QVERIFY(!rec.deleted);
        runOnce(server, store, {&a}, true);
        for (auto const & rec : std::as_const(server.records))
            QVERIFY(rec.deleted);
    }

    void failedApplyDoesNotStopTheRun() {
        FakeServer server;
        MapAdapter a(QStringLiteral("label"));
        a.items.insert(QStringLiteral("label:A"), named(QStringLiteral("A")));
        a.items.insert(QStringLiteral("label:B"), named(QStringLiteral("B")));
        MemStore sa;
        runOnce(server, sa, {&a});

        MapAdapter b(QStringLiteral("label"));
        b.failOn = QStringLiteral("label:A");
        MemStore sb;
        auto const r = runOnce(server, sb, {&b});
        QCOMPARE(r.pulled, 1); // B arrived, A failed without stopping the run
        QVERIFY(b.items.contains(QStringLiteral("label:B")));
        QVERIFY(!b.items.contains(QStringLiteral("label:A")));
    }

    void cancelEndsTheRun() {
        FakeServer server;
        MapAdapter a(QStringLiteral("label"));
        a.items.insert(QStringLiteral("label:A"), named(QStringLiteral("A")));
        MemStore sa;
        runOnce(server, sa, {&a});

        class Cancelling final: public BtPhosAdapter {
        public:
            QString type() const override { return QStringLiteral("label"); }
            QList<BtPhosItem> enumerate() override { return {}; }
            bool apply(QString const &, QJsonObject const &, bool) override
            { throw BtPhosCancelled(); }
        } c;
        MemStore sc;
        QVERIFY_THROWS_EXCEPTION(BtPhosCancelled, runOnce(server, sc, {&c}));
    }

    void hlcIsLowercaseAndMonotonic() {
        qint64 t = 1000;
        BtPhosHlc h(QStringLiteral("0123456789abcdef"), QString(), [&t] { return t; });
        auto const a = h.tick();
        auto const b = h.tick();
        QVERIFY(a < b);
        QCOMPARE(b, QStringLiteral("0000000001000-0001-0123456789abcdef"));
        h.observe(QStringLiteral("0000000005000-00ff-fedcba9876543210"));
        QCOMPARE(h.last(), QStringLiteral("0000000005000-0100-0123456789abcdef"));
        t = 6000;
        QCOMPARE(h.tick(), QStringLiteral("0000000006000-0000-0123456789abcdef"));
    }

    void canonicalJsonSortsKeys() {
        QJsonObject const o{{QStringLiteral("b"), 1},
                            {QStringLiteral("a"),
                             QJsonObject{{QStringLiteral("z"), true},
                                         {QStringLiteral("y"), QJsonArray{2, 1}}}}};
        QCOMPARE(BtPhos::canonicalJson(o),
                 QByteArray(R"({"a":{"y":[2,1],"z":true},"b":1})"));
    }

};

QTEST_GUILESS_MAIN(PhosSyncEngineTest)
#include "test_phossyncengine.moc"
