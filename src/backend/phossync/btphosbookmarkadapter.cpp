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

#include "btphosbookmarkadapter.h"

#include <QMetaObject>
#include <QThread>
#include "../btbookmarksmodel.h"
#include "../config/btconfig.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/cswordbackend.h"
#include "btphosbookmarkcodec.h"

// Sword includes:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swmodule.h>
#include <versekey.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


namespace {

using Codec = BtPhosBookmarkCodec::Bookmark;

BtPhosBookmarkCodec::ModuleInfo moduleInfo(QString const & name) {
    BtPhosBookmarkCodec::ModuleInfo info;
    auto const * const m = CSwordBackend::instance().findModuleByName(name);
    if (!m)
        return info;
    info.installed = true;
    info.verseKeyed = m->type() == CSwordModuleInfo::Bible
                      || m->type() == CSwordModuleInfo::Commentary;
    if (info.verseKeyed)
        if (auto const * const vk =
                dynamic_cast<sword::VerseKey const *>(m->swordModule().getKey()))
            info.versification = QString::fromUtf8(vk->getVersificationSystem());
    return info;
}

QString defaultBible() {
    if (auto const * const m =
            btConfig().getDefaultSwordModuleByType(QStringLiteral("standardBible")))
        return m->name();
    for (auto const * const m : CSwordBackend::instance().moduleList())
        if (m->type() == CSwordModuleInfo::Bible)
            return m->name();
    return {};
}

} // anonymous namespace


/* BtPhosModelAccess */

BtPhosModelAccess::BtPhosModelAccess(BtBookmarksModel * model,
                                     std::atomic_bool const & cancel,
                                     std::atomic_bool & applying)
    : m_model(model)
    , m_cancel(cancel)
    , m_applying(applying)
{}

void BtPhosModelAccess::run(std::function<void(BtBookmarksModel &)> const & work) {
    if (m_cancel)
        throw BtPhosCancelled();
    auto * const model = m_model.data();
    if (!model)
        throw BtPhosCancelled();
    if (QThread::currentThread() == model->thread()) {
        work(*model);
        return;
    }
    bool done = false;
    QMetaObject::invokeMethod(
                model,
                [this, &work, &done] {
                    if (auto * const m = m_model.data()) {
                        work(*m);
                        done = true;
                    }
                },
                Qt::BlockingQueuedConnection);
    if (!done)
        throw BtPhosCancelled();
}


/* BtPhosLabelAdapter */

QList<BtPhosItem> BtPhosLabelAdapter::enumerate() {
    QList<BtPhosItem> out;
    m_access.run([&out](BtBookmarksModel & model) {
        for (auto const & folder : model.syncFolders())
            out.append({QStringLiteral("label:")
                        + BtPhosBookmarkCodec::labelName(folder),
                        BtPhosBookmarkCodec::labelToJson(folder)});
    });
    return out;
}

bool BtPhosLabelAdapter::apply(QString const & key,
                               QJsonObject const & payload,
                               bool const deleted)
{
    Q_UNUSED(payload)
    auto const folder = BtPhosBookmarkCodec::folderOfLabel(
                key.mid(QStringLiteral("label:").size()));
    if (folder.isEmpty())
        return false;
    bool exists = false;
    m_access.run([&](BtBookmarksModel & model) {
        if (deleted) {
            // a folder that still has bookmarks stays
            model.removeSyncFolderIfEmpty(folder);
        } else {
            for (auto const & f : model.syncFolders())
                if (f == folder)
                    exists = true;
        }
    });
    /* A label becomes a folder when a bookmark arrives with it (labels other
       clients keep for themselves, e.g. highlight colours, would otherwise
       turn into empty folders). Until then it stays remote-only. */
    return deleted || exists;
}

std::optional<QSet<QString>> BtPhosLabelAdapter::ownedFields() const
{ return BtPhosBookmarkCodec::labelFields(); }


/* BtPhosBookmarkAdapter */

QList<BtPhosItem> BtPhosBookmarkAdapter::enumerate() {
    QList<BtPhosItem> out;
    m_access.run([&out](BtBookmarksModel & model) {
        for (auto const & e : model.syncEntries()) {
            if (e.id.isEmpty())
                continue;
            Codec bm{e.id, e.folder, e.module, e.key, e.description, e.title,
                     e.created, e.modified};
            if (auto p = BtPhosBookmarkCodec::bookmarkToJson(bm, moduleInfo(e.module)))
                out.append({QStringLiteral("bm:") + e.id, std::move(*p)});
        }
    });
    return out;
}

bool BtPhosBookmarkAdapter::apply(QString const & key,
                                  QJsonObject const & payload,
                                  bool const deleted)
{
    auto const id = key.mid(QStringLiteral("bm:").size());
    if (id.isEmpty())
        return false;
    bool ok = true;
    m_access.run([&](BtBookmarksModel & model) {
        if (deleted) {
            model.removeSyncEntry(id);
            return;
        }
        auto const bm = BtPhosBookmarkCodec::bookmarkFromJson(
                    id, payload, moduleInfo, defaultBible());
        if (!bm) {
            ok = false;
            return;
        }
        BtBookmarksModel::SyncEntry e;
        e.id = bm->id;
        e.folder = bm->folder;
        e.module = bm->module;
        e.key = bm->key;
        e.description = bm->description;
        e.title = bm->title;
        e.created = bm->created;
        e.modified = bm->modified;
        model.setSyncEntry(e);
    });
    return ok;
}

qint64 BtPhosBookmarkAdapter::modifiedMs(QJsonObject const & payload) const
{ return payload.value(QStringLiteral("modified")).toInteger() * 1000; }

std::optional<QSet<QString>> BtPhosBookmarkAdapter::ownedFields() const
{ return BtPhosBookmarkCodec::bookmarkFields(); }

std::optional<QJsonObject> BtPhosBookmarkAdapter::overlay(
        QJsonObject const * server,
        QJsonObject const & mine) const
{ return BtPhosBookmarkCodec::bookmarkOverlay(server, mine); }

QString BtPhosBookmarkAdapter::localKeyFor(QString const & serverKey) const {
    if (!serverKey.startsWith(QStringLiteral("bm:")))
        return serverKey;
    return QStringLiteral("bm:")
            + BtPhosIds::bookmarkUuidHex(serverKey.mid(3));
}
