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

#include "btphossyncengine.h"

#include <atomic>
#include <functional>
#include <QPointer>


class BtBookmarksModel;

/**
  Runs model work on the thread of the model (the GUI thread) and waits for
  it. Throws when the sync is cancelled or the model is gone.
*/
class BtPhosModelAccess {

public: // methods:

    BtPhosModelAccess(BtBookmarksModel * model,
                      std::atomic_bool const & cancel,
                      std::atomic_bool & applying);

    void run(std::function<void(BtBookmarksModel &)> const & work);

    void setApplying(bool applying) { m_applying = applying; }

private: // fields:

    QPointer<BtBookmarksModel> m_model;
    std::atomic_bool const & m_cancel;
    std::atomic_bool & m_applying;

};

/** Bookmark folders as Phos labels "label:<Folder/Subfolder>". */
class BtPhosLabelAdapter final: public BtPhosAdapter {

public: // methods:

    explicit BtPhosLabelAdapter(BtPhosModelAccess & access)
        : m_access(access)
    {}

    QString type() const final override { return QStringLiteral("label"); }
    int priority() const final override { return 10; }
    QList<BtPhosItem> enumerate() final override;
    bool apply(QString const & key,
               QJsonObject const & payload,
               bool deleted) final override;
    std::optional<QSet<QString>> ownedFields() const final override;
    void beginApply() final override { m_access.setApplying(true); }
    void endApply() final override { m_access.setApplying(false); }

private: // fields:

    BtPhosModelAccess & m_access;

};

/** Bookmarks as Phos records "bm:<id>". */
class BtPhosBookmarkAdapter final: public BtPhosAdapter {

public: // methods:

    explicit BtPhosBookmarkAdapter(BtPhosModelAccess & access)
        : m_access(access)
    {}

    QString type() const final override { return QStringLiteral("bookmark"); }
    int priority() const final override { return 20; }
    QList<BtPhosItem> enumerate() final override;
    bool apply(QString const & key,
               QJsonObject const & payload,
               bool deleted) final override;
    qint64 modifiedMs(QJsonObject const & payload) const final override;
    std::optional<QSet<QString>> ownedFields() const final override;
    std::optional<QJsonObject> overlay(QJsonObject const * server,
                                       QJsonObject const & mine) const final override;
    bool mapsKeys() const final override { return true; }
    QString localKeyFor(QString const & serverKey) const final override;
    void beginApply() final override { m_access.setApplying(true); }
    void endApply() final override { m_access.setApplying(false); }

private: // fields:

    BtPhosModelAccess & m_access;

};
