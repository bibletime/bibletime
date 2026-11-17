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

#include "btphossyncengine.h"

#include <algorithm>
#include <QCryptographicHash>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QRegularExpression>


namespace {

QChar const SEP(u'\u0001');

std::optional<QString> str(QJsonObject const & o, QString const & k) {
    auto const v = o.value(k);
    if (v.isString())
        return v.toString();
    if (v.isDouble())
        return QString::number(v.toInteger());
    if (v.isBool())
        return v.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    return std::nullopt;
}

QString strOr(QJsonObject const & o, QString const & k)
{ return str(o, k).value_or(QString()); }

qint64 lng(QJsonObject const & o, QString const & k) {
    auto const v = o.value(k);
    if (v.isDouble())
        return v.toInteger();
    if (v.isString()) {
        bool ok = false;
        auto const r = v.toString().toLongLong(&ok);
        return ok ? r : 0;
    }
    return 0;
}

bool boolean(QJsonObject const & o, QString const & k)
{ return o.value(k).toBool(); }

QJsonObject obj(QJsonObject const & o, QString const & k)
{ return o.value(k).toObject(); }

QJsonArray arr(QJsonObject const & o, QString const & k)
{ return o.value(k).toArray(); }

QString randomHex(int bytes) {
    QByteArray b(bytes, Qt::Uninitialized);
    QRandomGenerator::system()->generate(b.begin(), b.end());
    return QString::fromLatin1(b.toHex());
}

QStringList stringList(QJsonArray const & a) {
    QStringList out;
    for (auto const & v : a)
        if (v.isString())
            out.append(v.toString());
    return out;
}

} // anonymous namespace

namespace BtPhos {

QByteArray canonicalJson(QJsonObject const & o) {
    // QJsonObject keeps its keys sorted, so the compact form is canonical.
    return QJsonDocument(o).toJson(QJsonDocument::Compact);
}

QString hash(QJsonObject const & o) {
    return QString::fromLatin1(
                QCryptographicHash::hash(canonicalJson(o),
                                         QCryptographicHash::Md5).toHex());
}

QString newDeviceId() { return randomHex(8); }

QString newChangeId() {
    QByteArray b(16, Qt::Uninitialized);
    QRandomGenerator::system()->generate(b.begin(), b.end());
    b[6] = static_cast<char>((static_cast<unsigned char>(b[6]) & 0x0Fu) | 0x40u);
    b[8] = static_cast<char>((static_cast<unsigned char>(b[8]) & 0x3Fu) | 0x80u);
    auto const h = QString::fromLatin1(b.toHex());
    return h.mid(0, 8) + u'-' + h.mid(8, 4) + u'-' + h.mid(12, 4) + u'-'
           + h.mid(16, 4) + u'-' + h.mid(20);
}

} // namespace BtPhos


/* BtPhosHlc */

BtPhosHlc::BtPhosHlc(QString device, QString last, std::function<qint64()> now)
    : m_device(std::move(device))
    , m_last(std::move(last))
    , m_now(std::move(now))
{}

namespace {

std::pair<qint64, int> parseHlc(QString const & h) {
    auto const p = h.split(u'-');
    if (p.size() < 3)
        return {0, 0};
    bool ok1 = false;
    bool ok2 = false;
    auto const ms = p[0].toLongLong(&ok1);
    auto const c = p[1].toInt(&ok2, 16);
    return {ok1 ? ms : 0, ok2 ? c : 0};
}

} // anonymous namespace

QString BtPhosHlc::format(qint64 ms, int counter) const {
    return QStringLiteral("%1-%2-%3")
            .arg(ms, 13, 10, QChar(u'0'))
            .arg(std::min(counter, 0xFFFF), 4, 16, QChar(u'0'))
            .arg(m_device);
}

QString BtPhosHlc::tick() {
    auto const [ms, c] = parseHlc(m_last);
    auto const n = m_now();
    m_last = (n > ms) ? format(n, 0) : format(ms, c + 1);
    return m_last;
}

void BtPhosHlc::observe(QString const & remote) {
    auto const [rms, rc] = parseHlc(remote);
    auto const [ms, c] = parseHlc(m_last);
    auto const n = m_now();
    auto const m = std::max({ms, rms, n});
    int nc;
    if (m == ms && m == rms) {
        nc = std::max(c, rc) + 1;
    } else if (m == ms) {
        nc = c + 1;
    } else if (m == rms) {
        nc = rc + 1;
    } else {
        nc = 0;
    }
    m_last = format(m, nc);
}

QString BtPhosHlc::first(qint64 modifiedMs) {
    if (modifiedMs >= 1 && modifiedMs <= m_now())
        return format(modifiedMs, 0);
    return tick();
}


/* BtPhosSyncEngine */

BtPhosSyncEngine::BtPhosSyncEngine(BtPhosHttp & http,
                                   BtPhosStateStore & store,
                                   QList<BtPhosAdapter *> adapters,
                                   QString deviceName,
                                   QString appVersion,
                                   QString platform,
                                   std::function<qint64()> now,
                                   std::function<void(QString const &)> log)
    : m_http(http)
    , m_store(store)
    , m_adapters(std::move(adapters))
    , m_deviceName(std::move(deviceName))
    , m_appVersion(std::move(appVersion))
    , m_platform(std::move(platform))
    , m_now(now ? std::move(now)
                : std::function<qint64()>(
                      []{ return QDateTime::currentMSecsSinceEpoch(); }))
    , m_log(std::move(log))
{
    std::stable_sort(m_adapters.begin(), m_adapters.end(),
                     [](BtPhosAdapter * a, BtPhosAdapter * b)
                     { return a->priority() < b->priority(); });
    load();
}

QString BtPhosSyncEngine::sk(QString const & type, QString const & key)
{ return type + SEP + key; }

int BtPhosSyncEngine::outboxIndex(QString const & key) const {
    for (int i = 0; i < m_outbox.size(); ++i)
        if (m_outbox[i].sk == key)
            return i;
    return -1;
}

void BtPhosSyncEngine::load() {
    auto const data = m_store.load();
    auto const doc = QJsonDocument::fromJson(data);
    auto const root = doc.object();
    static QRegularExpression const deviceRe(QStringLiteral("^[0-9a-f]{16}$"));
    auto const dev = strOr(root, QStringLiteral("device"));
    m_device = deviceRe.match(dev).hasMatch() ? dev : BtPhos::newDeviceId();
    m_hlc.emplace(m_device, strOr(root, QStringLiteral("hlc")), m_now);
    if (!doc.isObject())
        return;
    m_personal = strOr(root, QStringLiteral("personal"));
    m_me = strOr(root, QStringLiteral("me"));
    m_cursor = lng(root, QStringLiteral("cursor"));
    for (auto const & t : stringList(arr(root, QStringLiteral("types"))))
        m_pulledTypes.insert(t);
    auto const shadow = obj(root, QStringLiteral("shadow"));
    for (auto it = shadow.begin(); it != shadow.end(); ++it) {
        auto const o = it.value().toObject();
        Shadow s;
        s.c = strOr(o, QStringLiteral("c"));
        s.k = strOr(o, QStringLiteral("k"));
        s.r = lng(o, QStringLiteral("r"));
        s.h = strOr(o, QStringLiteral("h"));
        if (o.value(QStringLiteral("p")).isObject())
            s.p = obj(o, QStringLiteral("p"));
        m_shadow.insert(it.key(), std::move(s));
    }
    for (auto const & v : arr(root, QStringLiteral("outbox"))) {
        auto const o = v.toObject();
        if (!o.value(QStringLiteral("change")).isObject())
            continue;
        m_outbox.append({strOr(o, QStringLiteral("sk")),
                         strOr(o, QStringLiteral("h")),
                         obj(o, QStringLiteral("change"))});
    }
    auto const labels = obj(root, QStringLiteral("labels"));
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        auto const o = it.value().toObject();
        m_labels.insert(it.key(), {strOr(o, QStringLiteral("c")),
                                   strOr(o, QStringLiteral("r"))});
    }
    auto const colls = obj(root, QStringLiteral("colls"));
    for (auto it = colls.begin(); it != colls.end(); ++it) {
        auto const o = it.value().toObject();
        m_colls.insert(it.key(), {strOr(o, QStringLiteral("owner")),
                                  strOr(o, QStringLiteral("ownerName")),
                                  strOr(o, QStringLiteral("kind"))});
    }
}

void BtPhosSyncEngine::save() {
    QJsonObject root;
    root.insert(QStringLiteral("v"), 1);
    root.insert(QStringLiteral("device"), m_device);
    root.insert(QStringLiteral("hlc"), m_hlc->last());
    root.insert(QStringLiteral("personal"), m_personal);
    root.insert(QStringLiteral("me"), m_me);
    root.insert(QStringLiteral("cursor"), m_cursor);
    QJsonArray types;
    for (auto const & t : m_pulledTypes)
        types.append(t);
    root.insert(QStringLiteral("types"), types);
    QJsonObject shadow;
    for (auto it = m_shadow.cbegin(); it != m_shadow.cend(); ++it) {
        QJsonObject o{{QStringLiteral("c"), it->c},
                      {QStringLiteral("k"), it->k},
                      {QStringLiteral("r"), it->r},
                      {QStringLiteral("h"), it->h}};
        if (it->p)
            o.insert(QStringLiteral("p"), *it->p);
        shadow.insert(it.key(), o);
    }
    root.insert(QStringLiteral("shadow"), shadow);
    QJsonArray outbox;
    for (auto const & q : m_outbox)
        outbox.append(QJsonObject{{QStringLiteral("sk"), q.sk},
                                  {QStringLiteral("h"), q.h},
                                  {QStringLiteral("change"), q.change}});
    root.insert(QStringLiteral("outbox"), outbox);
    QJsonObject labels;
    for (auto it = m_labels.cbegin(); it != m_labels.cend(); ++it)
        labels.insert(it.key(), QJsonObject{{QStringLiteral("c"), it->c},
                                            {QStringLiteral("r"), it->r}});
    root.insert(QStringLiteral("labels"), labels);
    QJsonObject colls;
    for (auto it = m_colls.cbegin(); it != m_colls.cend(); ++it)
        colls.insert(it.key(),
                     QJsonObject{{QStringLiteral("owner"), it->owner},
                                 {QStringLiteral("ownerName"), it->ownerName},
                                 {QStringLiteral("kind"), it->kind}});
    root.insert(QStringLiteral("colls"), colls);
    m_store.save(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

void BtPhosSyncEngine::reset() {
    m_personal.clear();
    m_me.clear();
    m_cursor = 0;
    m_pulledTypes.clear();
    m_shadow.clear();
    m_outbox.clear();
    m_labels.clear();
    m_colls.clear();
    save();
}

BtPhosRunResult BtPhosSyncEngine::run(bool confirmDeletes) {
    BtPhosRunResult result;
    m_appliedKeys.clear();
    auto const st = m_http.get(QStringLiteral("/status"));
    auto const pc = strOr(st, QStringLiteral("personalCollection"));
    if (!m_personal.isEmpty() && pc != m_personal) {
        log(QStringLiteral("other account than last time, starting over"));
        reset();
    }
    result.firstContact = m_cursor == 0 && m_shadow.isEmpty();
    m_personal = pc;
    if (st.value(QStringLiteral("serverTime")).isDouble())
        m_hlc->observe(QStringLiteral("%1-0000-%2")
                       .arg(lng(st, QStringLiteral("serverTime")), 13, 10,
                            QChar(u'0'))
                       .arg(m_device));
    m_http.post(QStringLiteral("/devices"),
                QJsonObject{{QStringLiteral("id"), m_device},
                            {QStringLiteral("name"), m_deviceName},
                            {QStringLiteral("platform"), m_platform},
                            {QStringLiteral("appVersion"), m_appVersion}});
    result.heldDeletes = detect(confirmDeletes);
    save();
    result.pushed = push(result.problems);
    save();
    result.pulled = pull();
    save();
    try {
        result.conflicts =
                arr(m_http.get(QStringLiteral("/conflicts")),
                    QStringLiteral("conflicts")).size();
    } catch (std::exception const &) {
        result.conflicts = 0;
    }
    result.appliedKeys = m_appliedKeys;
    return result;
}

int BtPhosSyncEngine::detect(bool confirmDeletes) {
    int held = 0;
    for (auto * const a : m_adapters) {
        auto const type = a->type();
        auto const items = a->enumerate();
        QSet<QString> present;
        for (auto const & item : items) {
            auto const key = sk(type, item.key);
            if (present.contains(key))
                continue; // a key twice: the first wins
            present.insert(key);
            auto const h = BtPhos::hash(item.payload);
            auto const shIt = m_shadow.constFind(key);
            Shadow const * const sh =
                    (shIt != m_shadow.cend()) ? &*shIt : nullptr;
            // a mapped key goes back under the server's own key
            QString serverKey = item.key;
            if (a->mapsKeys() && sh && !sh->k.isEmpty())
                serverKey = sh->k;
            auto const out = outgoing(type, serverKey, item.payload);
            auto const q = outboxIndex(key);
            // clean against the shadow and nothing queued: nothing to do. With
            // something queued (an edit reverted since, a delete and re-create
            // of the same content), the current state is sent instead.
            if (sh && sh->h == h && sh->c == out.collection && q < 0)
                continue;
            if (q >= 0 && m_outbox[q].h == h
                && strOr(m_outbox[q].change, QStringLiteral("collection"))
                   == out.collection)
                continue;
            auto payload = a->overlay(sh && sh->p ? &*sh->p : nullptr,
                                      out.payload);
            if (!payload)
                payload = overlay(sh ? sh->p : std::nullopt, out.payload,
                                  a->ownedFields());
            QJsonObject change{
                {QStringLiteral("changeId"), BtPhos::newChangeId()},
                {QStringLiteral("collection"), out.collection},
                {QStringLiteral("type"), type},
                {QStringLiteral("key"), out.key},
                {QStringLiteral("op"), QStringLiteral("upsert")},
                {QStringLiteral("hlc"),
                 sh ? m_hlc->tick()
                    : m_hlc->first(a->modifiedMs(item.payload))},
                {QStringLiteral("baseRev"),
                 (sh && sh->c == out.collection) ? sh->r : qint64(0)},
                {QStringLiteral("payload"), *payload}};
            if (sh && sh->c != out.collection && !sh->c.isEmpty())
                change.insert(QStringLiteral("moveFrom"), sh->c);
            if (q >= 0) {
                m_outbox[q] = {key, h, change};
            } else {
                m_outbox.append({key, h, change});
            }
        }
        // gone here: in the shadow, not local, not remote-only
        auto const prefix = type + SEP;
        QList<QString> gone;
        int ofType = 0;
        for (auto it = m_shadow.cbegin(); it != m_shadow.cend(); ++it) {
            if (!it.key().startsWith(prefix))
                continue;
            ++ofType;
            if (!present.contains(it.key()) && it->h != QStringLiteral("-"))
                gone.append(it.key());
        }
        if (!confirmDeletes && gone.size() > 10 && gone.size() * 3 > ofType) {
            log(QStringLiteral("%1 of %2 %3 records gone locally, held back")
                .arg(gone.size()).arg(ofType).arg(type));
            held += gone.size();
            continue;
        }
        for (auto const & key : gone) {
            bool queued = false;
            for (auto const & q : m_outbox)
                if (q.sk == key
                    && strOr(q.change, QStringLiteral("op"))
                       == QStringLiteral("delete"))
                    queued = true;
            if (queued)
                continue;
            auto const & s = m_shadow[key];
            QJsonObject change{
                {QStringLiteral("changeId"), BtPhos::newChangeId()},
                {QStringLiteral("collection"), s.c},
                {QStringLiteral("type"), type},
                {QStringLiteral("key"), s.k},
                {QStringLiteral("op"), QStringLiteral("delete")},
                {QStringLiteral("hlc"), m_hlc->tick()},
                {QStringLiteral("baseRev"), s.r}};
            m_outbox.removeIf([&key](Queued const & q) { return q.sk == key; });
            m_outbox.append({key, QStringLiteral("deleted"), change});
        }
    }
    return held;
}

QJsonObject BtPhosSyncEngine::overlay(
        std::optional<QJsonObject> const & server,
        QJsonObject const & mine,
        std::optional<QSet<QString>> const & owned) const
{
    if (!server || !owned)
        return mine;
    QJsonObject out;
    for (auto it = server->begin(); it != server->end(); ++it)
        if (!owned->contains(it.key()))
            out.insert(it.key(), it.value());
    for (auto it = mine.begin(); it != mine.end(); ++it)
        out.insert(it.key(), it.value());
    return out;
}

int BtPhosSyncEngine::push(QStringList & problems) {
    int sent = 0;
    while (!m_outbox.isEmpty()) {
        auto const batch = m_outbox.mid(0, 500);
        QJsonArray changes;
        for (auto const & q : batch)
            changes.append(q.change);
        auto const res =
                m_http.post(QStringLiteral("/push"),
                            QJsonObject{{QStringLiteral("device"), m_device},
                                        {QStringLiteral("changes"), changes}});
        QHash<QString, QJsonObject> byId;
        for (auto const & v : arr(res, QStringLiteral("results"))) {
            auto const o = v.toObject();
            byId.insert(strOr(o, QStringLiteral("changeId")), o);
        }
        QList<std::pair<BtPhosAdapter *, QSet<QString>>> applied;
        auto appliedFor = [&applied](BtPhosAdapter * a) -> QSet<QString> & {
            for (auto & p : applied)
                if (p.first == a)
                    return p.second;
            applied.append(std::make_pair(a, QSet<QString>()));
            return applied.last().second;
        };
        for (auto const & q : batch) {
            auto const rIt =
                    byId.constFind(strOr(q.change, QStringLiteral("changeId")));
            if (rIt == byId.cend())
                continue;
            auto const & r = *rIt;
            auto const status = strOr(r, QStringLiteral("status"));
            auto const type = strOr(q.change, QStringLiteral("type"));
            auto const localKey = q.sk.mid(q.sk.indexOf(SEP) + 1);
            if (status == QStringLiteral("invalid")) {
                problems.append(QStringLiteral("%1 %2: %3")
                                .arg(type, localKey,
                                     strOr(r, QStringLiteral("error"))));
                // not sent again until it changes once more
                auto it = m_shadow.find(q.sk);
                if (it == m_shadow.end()) {
                    Shadow s;
                    s.c = strOr(q.change, QStringLiteral("collection"));
                    s.k = strOr(q.change, QStringLiteral("key"));
                    it = m_shadow.insert(q.sk, s);
                }
                it->h = q.h;
                continue;
            }
            ++sent;
            if (!r.value(QStringLiteral("record")).isObject())
                continue;
            auto const rec = obj(r, QStringLiteral("record"));
            if (auto const h = str(rec, QStringLiteral("hlc")))
                m_hlc->observe(*h);
            auto const deleted = boolean(rec, QStringLiteral("deleted"));
            // "duplicate": the server had this change already (a retry after a
            // lost answer), but its record may have moved on since
            auto const sameAsSent = status == QStringLiteral("accepted");
            if (status == QStringLiteral("conflict")
                || status == QStringLiteral("forbidden"))
                problems.append(QStringLiteral("%1 %2: %3")
                                .arg(type, localKey, status));
            if (sameAsSent) {
                if (deleted) {
                    m_shadow.remove(q.sk);
                } else {
                    Shadow s;
                    s.c = strOr(rec, QStringLiteral("collection"));
                    s.k = strOr(rec, QStringLiteral("key"));
                    s.r = lng(rec, QStringLiteral("rev"));
                    s.h = q.h;
                    if (rec.value(QStringLiteral("payload")).isObject())
                        s.p = obj(rec, QStringLiteral("payload"));
                    m_shadow.insert(q.sk, s);
                }
            } else {
                // merged / superseded / conflict / forbidden: the server's version wins
                BtPhosAdapter * a = nullptr;
                for (auto * const x : m_adapters)
                    if (x->type() == type)
                        a = x;
                if (!a)
                    continue;
                // one record at a time: a failure leaves the others alone and
                // writes no shadow for this one
                a->beginApply();
                try {
                    if (auto const k = applyRecord(*a, rec, localKey))
                        appliedFor(a).insert(*k);
                } catch (BtPhosCancelled const &) {
                    a->endApply();
                    throw;
                } catch (std::exception const & e) {
                    problems.append(QStringLiteral("%1 %2: %3")
                                    .arg(type, localKey,
                                         QString::fromUtf8(e.what())));
                    log(QStringLiteral("applying %1 %2 failed: %3")
                        .arg(type, localKey, QString::fromUtf8(e.what())));
                }
                a->endApply();
            }
        }
        for (auto & [a, keys] : applied)
            rehash(*a, keys);
        QSet<QString> ids;
        for (auto const & q : batch)
            ids.insert(strOr(q.change, QStringLiteral("changeId")));
        m_outbox.removeIf([&ids](Queued const & q) {
            return ids.contains(strOr(q.change, QStringLiteral("changeId")));
        });
        save();
    }
    return sent;
}

int BtPhosSyncEngine::pull() {
    QStringList allTypes;
    for (auto * const a : m_adapters)
        allTypes.append(a->type());
    auto const types = allTypes.join(u',');
    QList<QJsonObject> records;
    std::optional<QSet<QString>> readable;
    bool wasReset = false;
    auto c = m_cursor;
    QStringList newTypes;
    for (auto const & t : allTypes)
        if (!m_pulledTypes.contains(t))
            newTypes.append(t);
    auto const limit = QStringLiteral("1000");
    auto collect = [&records](QJsonObject const & r) {
        for (auto const & v : arr(r, QStringLiteral("changes")))
            if (v.isObject())
                records.append(v.toObject());
    };
    if (c > 0 && !newTypes.isEmpty()) {
        qint64 bc = 0;
        for (;;) {
            auto const br = m_http.get(
                        QStringLiteral("/changes"),
                        {{QStringLiteral("cursor"), QString::number(bc)},
                         {QStringLiteral("limit"), limit},
                         {QStringLiteral("types"), newTypes.join(u',')},
                         {QStringLiteral("device"), m_device}});
            collect(br);
            bc = lng(br, QStringLiteral("cursor"));
            if (!boolean(br, QStringLiteral("hasMore")))
                break;
        }
    }
    for (;;) {
        auto const r = m_http.get(
                    QStringLiteral("/changes"),
                    {{QStringLiteral("cursor"), QString::number(c)},
                     {QStringLiteral("limit"), limit},
                     {QStringLiteral("types"), types},
                     {QStringLiteral("device"), m_device}});
        if (boolean(r, QStringLiteral("resetRequired"))) {
            wasReset = true;
            c = 0;
            records.clear();
            continue;
        }
        if (r.value(QStringLiteral("collections")).isArray()) {
            QSet<QString> ids;
            for (auto const & v : arr(r, QStringLiteral("collections"))) {
                auto const o = v.toObject();
                auto const id = str(o, QStringLiteral("id"));
                if (!id)
                    continue;
                ids.insert(*id);
                m_colls.insert(*id, {strOr(o, QStringLiteral("owner")),
                                     strOr(o, QStringLiteral("ownerName")),
                                     strOr(o, QStringLiteral("kind"))});
                if (strOr(o, QStringLiteral("kind")) == QStringLiteral("personal"))
                    m_me = strOr(o, QStringLiteral("owner"));
            }
            readable = ids;
        }
        collect(r);
        for (auto const & j : arr(r, QStringLiteral("joined"))) {
            if (!j.isString())
                continue;
            qint64 jc = 0;
            for (;;) {
                auto const jr = m_http.get(
                            QStringLiteral("/changes"),
                            {{QStringLiteral("cursor"), QString::number(jc)},
                             {QStringLiteral("limit"), limit},
                             {QStringLiteral("types"), types},
                             {QStringLiteral("device"), m_device},
                             {QStringLiteral("collections"), j.toString()}});
                collect(jr);
                jc = lng(jr, QStringLiteral("cursor"));
                if (!boolean(jr, QStringLiteral("hasMore")))
                    break;
            }
        }
        c = lng(r, QStringLiteral("cursor"));
        if (!boolean(r, QStringLiteral("hasMore")))
            break;
    }
    if (readable)
        revoke(*readable);
    if (wasReset)
        reconcileAfterReset(records, readable);
    int applied = 0;
    for (auto * const a : m_adapters) {
        QList<QJsonObject> mine;
        for (auto const & rec : records)
            if (strOr(rec, QStringLiteral("type")) == a->type())
                mine.append(rec);
        if (mine.isEmpty())
            continue;
        // the local state now: a record is only overwritten while the item is
        // still as clean as detect() found it (see applyIfNewer)
        QHash<QString, QString> live;
        for (auto const & item : a->enumerate())
            live.insert(item.key, BtPhos::hash(item.payload));
        QSet<QString> keys;
        for (auto const & rec : mine) {
            if (auto const h = str(rec, QStringLiteral("hlc")))
                m_hlc->observe(*h);
            // one record at a time: a failure leaves the others alone
            a->beginApply();
            try {
                if (auto const k = applyIfNewer(*a, rec, live)) {
                    keys.insert(*k);
                    ++applied;
                }
            } catch (BtPhosCancelled const &) {
                a->endApply();
                throw;
            } catch (std::exception const & e) {
                log(QStringLiteral("applying %1 record failed: %2")
                    .arg(a->type(), QString::fromUtf8(e.what())));
            }
            a->endApply();
        }
        rehash(*a, keys);
    }
    m_cursor = c;
    for (auto const & t : allTypes)
        m_pulledTypes.insert(t);
    return applied;
}

std::optional<QString> BtPhosSyncEngine::applyIfNewer(
        BtPhosAdapter & a,
        QJsonObject const & rec,
        QHash<QString, QString> const & live)
{
    auto const coll = strOr(rec, QStringLiteral("collection"));
    auto const localKey = localKeyOf(a, rec);
    if (!localKey)
        return std::nullopt;
    auto const key = sk(a.type(), *localKey);
    auto const shIt = m_shadow.constFind(key);
    Shadow const * const sh = (shIt != m_shadow.cend()) ? &*shIt : nullptr;
    if (sh && sh->r >= lng(rec, QStringLiteral("rev")) && sh->c == coll)
        return std::nullopt;
    if (outboxIndex(key) >= 0)
        return std::nullopt; // dirty: the server merges on our push
    auto const deleted = boolean(rec, QStringLiteral("deleted"));
    // the old half of a move: the item lives in another collection now
    if (deleted && sh && sh->c != coll)
        return std::nullopt;
    if (deleted && !sh)
        return std::nullopt;
    if (sh) {
        // edited here while this run was under way: keep the edit, only adopt
        // the new revision so the next run sends it on the right base
        auto const l = live.constFind(*localKey);
        auto const liveHash = (l != live.cend()) ? *l : QStringLiteral("-");
        if (liveHash != sh->h) {
            Shadow s;
            s.c = coll;
            s.k = strOr(rec, QStringLiteral("key"));
            s.r = lng(rec, QStringLiteral("rev"));
            s.h = sh->h;
            if (rec.value(QStringLiteral("payload")).isObject())
                s.p = obj(rec, QStringLiteral("payload"));
            m_shadow.insert(key, s);
            return std::nullopt;
        }
    }
    return applyRecord(a, rec, *localKey);
}

std::optional<QString> BtPhosSyncEngine::applyRecord(
        BtPhosAdapter & a,
        QJsonObject const & rec,
        std::optional<QString> knownLocalKey)
{
    auto const coll = strOr(rec, QStringLiteral("collection"));
    auto const deleted = boolean(rec, QStringLiteral("deleted"));
    auto const payloadIn = obj(rec, QStringLiteral("payload"));
    auto localKey = knownLocalKey;
    if (!localKey)
        localKey = localKeyOf(a, rec);
    if (!localKey)
        return std::nullopt;

    // labels and bookmarks of shared collections carry local label names
    auto payload = payloadIn;
    if (!coll.isEmpty() && coll != m_personal && !payloadIn.isEmpty()) {
        if (a.type() == QStringLiteral("label")) {
            payload.insert(QStringLiteral("name"),
                           localKey->mid(QStringLiteral("label:").size()));
        } else if (a.type() == QStringLiteral("bookmark")) {
            QJsonArray names;
            for (auto const & n :
                 stringList(arr(payloadIn, QStringLiteral("labelNames"))))
                names.append(localLabelFor(coll, n));
            payload.insert(QStringLiteral("labelNames"), names);
        }
    }

    auto const key = sk(a.type(), *localKey);
    if (payload.contains(QStringLiteral("$file"))) {
        log(QStringLiteral("%1 %2 is stored as a file, not supported here")
            .arg(a.type(), *localKey));
        Shadow s;
        s.c = coll;
        s.k = strOr(rec, QStringLiteral("key"));
        s.r = lng(rec, QStringLiteral("rev"));
        s.h = QStringLiteral("-");
        m_shadow.insert(key, s);
        return std::nullopt;
    }
    // an exception goes to the caller; no shadow is written for the record
    auto const ok = a.apply(*localKey, payload, deleted);
    if (ok)
        m_appliedKeys[a.type()].insert(*localKey);
    if (deleted) {
        m_shadow.remove(key);
        if (a.type() == QStringLiteral("label"))
            m_labels.remove(localKey->mid(QStringLiteral("label:").size()));
        return std::nullopt;
    }
    Shadow s;
    s.c = coll;
    s.k = strOr(rec, QStringLiteral("key"));
    s.r = lng(rec, QStringLiteral("rev"));
    s.h = ok ? QStringLiteral("?") : QStringLiteral("-");
    if (rec.value(QStringLiteral("payload")).isObject())
        s.p = payloadIn;
    m_shadow.insert(key, s);
    return ok ? localKey : std::nullopt;
}

void BtPhosSyncEngine::rehash(BtPhosAdapter & a, QSet<QString> const & keys) {
    if (keys.isEmpty())
        return;
    QHash<QString, QJsonObject> now;
    for (auto const & item : a.enumerate())
        now.insert(item.key, item.payload);
    for (auto const & k : keys) {
        auto it = m_shadow.find(sk(a.type(), k));
        if (it == m_shadow.end())
            continue;
        auto const n = now.constFind(k);
        it->h = (n != now.cend()) ? BtPhos::hash(*n) : QStringLiteral("-");
    }
}

void BtPhosSyncEngine::revoke(QSet<QString> const & readable) {
    QList<QString> lost;
    for (auto it = m_shadow.cbegin(); it != m_shadow.cend(); ++it)
        if (!it->c.isEmpty() && it->c != m_personal && !readable.contains(it->c))
            lost.append(it.key());
    if (lost.isEmpty())
        return;
    for (auto * const a : m_adapters) {
        auto const prefix = a->type() + SEP;
        QList<QString> mine;
        for (auto const & k : lost)
            if (k.startsWith(prefix))
                mine.append(k);
        if (mine.isEmpty())
            continue;
        for (auto const & key : mine) {
            auto const & s = m_shadow[key];
            auto const ci = m_colls.constFind(s.c);
            auto const own = !m_me.isEmpty() && ci != m_colls.cend()
                             && ci->owner == m_me;
            if (!own && s.h != QStringLiteral("-")) {
                auto const localKey = key.mid(prefix.size());
                a->beginApply();
                try {
                    if (a->apply(localKey, {}, true))
                        m_appliedKeys[a->type()].insert(localKey);
                } catch (BtPhosCancelled const &) {
                    a->endApply();
                    throw;
                } catch (std::exception const & e) {
                    log(QStringLiteral("revoking %1 %2 failed: %3")
                        .arg(a->type(), localKey, QString::fromUtf8(e.what())));
                }
                a->endApply();
            }
            m_shadow.remove(key);
        }
    }
    m_labels.removeIf([&readable](QMap<QString, Binding>::iterator it)
                      { return !readable.contains(it.value().c); });
}

void BtPhosSyncEngine::reconcileAfterReset(
        QList<QJsonObject> const & records,
        std::optional<QSet<QString>> const & readable)
{
    // After a reset the feed only carries what exists now: an item whose
    // tombstone is gone from the server would never be seen deleted. Items
    // with a pending local change stay so they can still be pushed.
    QSet<QString> seen;
    for (auto const & rec : records) {
        for (auto * const a : m_adapters) {
            if (a->type() != strOr(rec, QStringLiteral("type")))
                continue;
            if (auto const lk = localKeyOf(*a, rec))
                seen.insert(sk(a->type(), *lk));
        }
    }
    auto covered = readable.value_or(QSet<QString>());
    covered.insert(m_personal);
    for (auto * const a : m_adapters) {
        auto const prefix = a->type() + SEP;
        QList<QString> gone;
        for (auto it = m_shadow.cbegin(); it != m_shadow.cend(); ++it)
            if (it.key().startsWith(prefix) && !seen.contains(it.key())
                && it->h != QStringLiteral("-") && covered.contains(it->c)
                && outboxIndex(it.key()) < 0)
                gone.append(it.key());
        for (auto const & key : gone) {
            auto const localKey = key.mid(prefix.size());
            a->beginApply();
            try {
                a->apply(localKey, {}, true);
                m_shadow.remove(key);
            } catch (BtPhosCancelled const &) {
                a->endApply();
                throw;
            } catch (std::exception const & e) {
                log(QStringLiteral("reconciling %1 %2 after reset failed: %3")
                    .arg(a->type(), localKey, QString::fromUtf8(e.what())));
            }
            a->endApply();
        }
    }
}

BtPhosSyncEngine::Outgoing BtPhosSyncEngine::outgoing(QString const & type,
                                                      QString const & key,
                                                      QJsonObject const & payload)
{
    if (type == QStringLiteral("label")) {
        auto const it = m_labels.constFind(key.mid(QStringLiteral("label:").size()));
        if (it == m_labels.cend())
            return {m_personal, key, payload};
        auto p = payload;
        p.insert(QStringLiteral("name"), it->r);
        return {it->c, QStringLiteral("label:") + it->r, p};
    }
    if (type == QStringLiteral("bookmark")) {
        auto const names = stringList(arr(payload, QStringLiteral("labelNames")));
        // the first shared label decides the collection
        QString coll;
        for (auto const & n : names) {
            auto const it = m_labels.constFind(n);
            if (it != m_labels.cend()) {
                coll = it->c;
                break;
            }
        }
        if (coll.isEmpty())
            return {m_personal, key, payload};
        QJsonArray remote;
        for (auto const & n : names) {
            auto const it = m_labels.constFind(n);
            if (it != m_labels.cend() && it->c == coll)
                remote.append(it->r);
        }
        auto p = payload;
        p.insert(QStringLiteral("labelNames"), remote);
        return {coll, key, p};
    }
    return {m_personal, key, payload};
}

std::optional<QString> BtPhosSyncEngine::localKeyOf(BtPhosAdapter & a,
                                                    QJsonObject const & rec)
{
    auto const coll = strOr(rec, QStringLiteral("collection"));
    auto const key = str(rec, QStringLiteral("key"));
    if (!key)
        return std::nullopt;
    if (a.type() != QStringLiteral("label") || coll == m_personal
        || coll.isEmpty())
        return a.localKeyFor(*key);
    auto const remote = key->mid(QStringLiteral("label:").size());
    for (auto it = m_labels.cbegin(); it != m_labels.cend(); ++it)
        if (it->c == coll && it->r == remote)
            return QStringLiteral("label:") + it.key();
    if (boolean(rec, QStringLiteral("deleted")))
        return std::nullopt;
    return QStringLiteral("label:") + localLabelFor(coll, remote);
}

QString BtPhosSyncEngine::localLabelFor(QString const & coll,
                                        QString const & remote)
{
    for (auto it = m_labels.cbegin(); it != m_labels.cend(); ++it)
        if (it->c == coll && it->r == remote)
            return it.key();
    auto const ci = m_colls.constFind(coll);
    QString local = remote;
    if (ci != m_colls.cend() && !ci->owner.isEmpty() && ci->owner != m_me)
        local = QStringLiteral("%1 (%2)")
                .arg(remote, ci->ownerName.isEmpty() ? ci->owner : ci->ownerName);
    m_labels.insert(local, {coll, remote});
    return local;
}
