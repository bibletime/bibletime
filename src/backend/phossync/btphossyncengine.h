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

#include <functional>
#include <optional>
#include <QHash>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QSet>
#include <QString>
#include <QStringList>
#include <stdexcept>


/*
  Client of the record sync API of the "Phos" Nextcloud app (see
  docs/phos-nextcloud-sync.md).

  Only the server merges: the engine detects local changes against a shadow of
  what the server last confirmed, pushes them with the revision they were based
  on and adopts whatever the server answers. Adapters translate between
  BibleTime's data and the record payloads the other Phos clients use.
*/

class BtPhosHttpError: public std::runtime_error {

public: // methods:

    BtPhosHttpError(int status, QString const & message, bool network = false)
        : std::runtime_error(message.toStdString())
        , m_status(status)
        , m_network(network)
    {}

    int status() const noexcept { return m_status; }
    bool network() const noexcept { return m_network; }

private: // fields:

    int m_status;
    bool m_network;

};

/** Thrown when a run is cancelled (e.g. on shutdown); ends the run. */
class BtPhosCancelled: public std::runtime_error {

public: // methods:

    BtPhosCancelled() : std::runtime_error("cancelled") {}

};

/** OCS v2 calls below /ocs/v2.php/apps/phos/api/v1, returning ocs.data. Blocking. */
class BtPhosHttp {

public: // types:

    using Params = QList<std::pair<QString, QString>>;

public: // methods:

    virtual ~BtPhosHttp() = default;
    virtual QJsonObject get(QString const & path, Params const & params = {}) = 0;
    virtual QJsonObject post(QString const & path, QJsonObject const & body) = 0;

};

/** Where the engine keeps its state (shadow, outbox, cursor, clock) between runs. */
class BtPhosStateStore {

public: // methods:

    virtual ~BtPhosStateStore() = default;
    virtual QByteArray load() = 0;
    virtual void save(QByteArray const & data) = 0;

};

/** One local item as an adapter renders it: the record payload this device would send. */
struct BtPhosItem {
    QString key;
    QJsonObject payload;
};

class BtPhosAdapter {

public: // methods:

    virtual ~BtPhosAdapter() = default;

    virtual QString type() const = 0;

    /** Lower first when applying (labels before the bookmarks naming them). */
    virtual int priority() const { return 50; }

    virtual QList<BtPhosItem> enumerate() = 0;

    /**
      Stores a record locally. Returns false when it can not be represented
      here (it is then kept remote-only and never deleted).
    */
    virtual bool apply(QString const & key,
                       QJsonObject const & payload,
                       bool deleted) = 0;

    /** The item's own modification time (ms), the clock of a first push. */
    virtual qint64 modifiedMs(QJsonObject const &) const { return 0; }

    /**
      The fields this adapter writes. Any other field of the server's copy
      (what another client keeps) is sent back unchanged. No value means the
      rendering is the whole record.
    */
    virtual std::optional<QSet<QString>> ownedFields() const
    { return std::nullopt; }

    /** Own way to lay the rendering over the server copy; none = by ownedFields(). */
    virtual std::optional<QJsonObject> overlay(QJsonObject const * server,
                                               QJsonObject const & mine) const
    {
        Q_UNUSED(server)
        Q_UNUSED(mine)
        return std::nullopt;
    }

    /**
      True when local keys can differ from server keys (localKeyFor). The
      engine then pushes back under the key the server knows.
    */
    virtual bool mapsKeys() const { return false; }
    virtual QString localKeyFor(QString const & serverKey) const
    { return serverKey; }

    virtual void beginApply() {}
    virtual void endApply() {}

};

/** Hybrid logical clock "<ms:13>-<counter:4 hex>-<device:16 hex>". */
class BtPhosHlc {

public: // methods:

    BtPhosHlc(QString device, QString last, std::function<qint64()> now);

    QString tick();
    void observe(QString const & remote);
    /** A record never sent before carries its own modification time. */
    QString first(qint64 modifiedMs);
    QString const & last() const noexcept { return m_last; }

private: // methods:

    QString format(qint64 ms, int counter) const;

private: // fields:

    QString m_device;
    QString m_last;
    std::function<qint64()> m_now;

};

struct BtPhosRunResult {
    int pushed = 0;
    int pulled = 0;
    int conflicts = 0;
    /** Deletions held back by the mass-delete guard. */
    int heldDeletes = 0;
    QStringList problems;
    bool firstContact = false;
    /** Local keys stored from the server in this run, by record type. */
    QHash<QString, QSet<QString>> appliedKeys;
};

namespace BtPhos {

/** Canonical JSON (object keys sorted, compact). */
QByteArray canonicalJson(QJsonObject const & o);
/** Lowercase md5 hex of the canonical JSON. */
QString hash(QJsonObject const & o);
QString newDeviceId();
QString newChangeId();

} // namespace BtPhos

class BtPhosSyncEngine {

public: // methods:

    BtPhosSyncEngine(BtPhosHttp & http,
                     BtPhosStateStore & store,
                     QList<BtPhosAdapter *> adapters,
                     QString deviceName,
                     QString appVersion = {},
                     QString platform = QStringLiteral("bibletime"),
                     std::function<qint64()> now = {},
                     std::function<void(QString const &)> log = {});

    BtPhosRunResult run(bool confirmDeletes = false);

    /** Forget everything about the server. Local data stays. */
    void reset();

    QString const & device() const noexcept { return m_device; }
    int outboxSize() const noexcept { return m_outbox.size(); }

private: // types:

    struct Shadow {
        QString c; // collection
        QString k; // server key
        qint64 r = 0; // revision
        QString h; // hash of the local rendering, "-" = remote-only, "?" = to rehash
        std::optional<QJsonObject> p; // server payload
    };
    struct Queued {
        QString sk;
        QString h;
        QJsonObject change;
    };
    struct Binding {
        QString c;
        QString r;
    };
    struct CollInfo {
        QString owner;
        QString ownerName;
        QString kind;
    };
    struct Outgoing {
        QString collection;
        QString key;
        QJsonObject payload;
    };

private: // methods:

    static QString sk(QString const & type, QString const & key);
    void load();
    void save();
    int detect(bool confirmDeletes);
    QJsonObject overlay(std::optional<QJsonObject> const & server,
                        QJsonObject const & mine,
                        std::optional<QSet<QString>> const & owned) const;
    int push(QStringList & problems);
    int pull();
    std::optional<QString> applyIfNewer(BtPhosAdapter & a,
                                        QJsonObject const & rec,
                                        QHash<QString, QString> const & live);
    void reconcileAfterReset(QList<QJsonObject> const & records,
                             std::optional<QSet<QString>> const & readable);
    std::optional<QString> applyRecord(BtPhosAdapter & a,
                                       QJsonObject const & rec,
                                       std::optional<QString> knownLocalKey = {});
    void rehash(BtPhosAdapter & a, QSet<QString> const & keys);
    void revoke(QSet<QString> const & readable);
    Outgoing outgoing(QString const & type,
                      QString const & key,
                      QJsonObject const & payload);
    std::optional<QString> localKeyOf(BtPhosAdapter & a, QJsonObject const & rec);
    QString localLabelFor(QString const & coll, QString const & remote);
    int outboxIndex(QString const & key) const;
    void log(QString const & msg) const { if (m_log) m_log(msg); }

private: // fields:

    BtPhosHttp & m_http;
    BtPhosStateStore & m_store;
    QList<BtPhosAdapter *> m_adapters;
    QString m_deviceName;
    QString m_appVersion;
    QString m_platform;
    std::function<qint64()> m_now;
    std::function<void(QString const &)> m_log;

    QString m_device;
    QString m_personal;
    QString m_me;
    qint64 m_cursor = 0;
    QSet<QString> m_pulledTypes;
    QMap<QString, Shadow> m_shadow;
    QList<Queued> m_outbox;
    QMap<QString, Binding> m_labels;
    QMap<QString, CollInfo> m_colls;
    QHash<QString, QSet<QString>> m_appliedKeys;
    std::optional<BtPhosHlc> m_hlc;

};
