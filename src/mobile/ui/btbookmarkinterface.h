/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_BOOKMARK_INTERFACE_H
#define BT_BOOKMARK_INTERFACE_H

// API Interface between QML and c++ bookmark functionality

#include "backend/btbookmarksmodel.h"
#include "mobile/models/roleitemmodel.h"
#include <QObject>

namespace btm {

class FolderModel;

class BtBookmarkInterface : public QObject {

    Q_OBJECT

    Q_PROPERTY(QVariant bookmarkModel READ getBookmarkModel NOTIFY bookmarkModelChanged)
    Q_PROPERTY(QVariant folderModel   READ getFolderModel  NOTIFY folderModelChanged)
    Q_PROPERTY(QModelIndex currentFolder READ currentFolder WRITE setCurrentFolder NOTIFY currentFolderChanged)
    Q_PROPERTY(QVariant contextMenuModel READ getContextMenuModel NOTIFY contextMenuModelChanged)

public:
    BtBookmarkInterface(QObject *parent = 0);

    // Folders and Bookmarks
    QVariant getBookmarkModel();

    // Only Folders
    QVariant getFolderModel();
    QModelIndex currentFolder() const;
    void setCurrentFolderToRootFolder();
    void setCurrentFolder(const QModelIndex& folder);


    Q_INVOKABLE void addBookmark(const QString& reference, const QString& moduleName);
    Q_INVOKABLE void addFolder(const QString& folderName);
    Q_INVOKABLE void doContextMenu(const QString& action, const QModelIndex& index);
    Q_INVOKABLE QString folderName(const QModelIndex& index);
    Q_INVOKABLE QVariantList getBookmarkModelExpandableIndexes();
    Q_INVOKABLE QVariantList getFolderModelExpandableIndexes();
    Q_INVOKABLE QString getModule(const QModelIndex& index);
    Q_INVOKABLE QString getReference(const QModelIndex& index);
    Q_INVOKABLE bool isBookmark(const QModelIndex& index);
    Q_INVOKABLE bool isRoot(const QModelIndex&  index);
    Q_INVOKABLE void setupContextMenuModel(const QModelIndex& index);

    QVariant getContextMenuModel();

signals:
    void bookmarkModelChanged();
    void currentFolderChanged();
    void contextMenuModelChanged();
    void folderModelChanged();

private:
    void appendContextMenuItem(const QString& title, const QString& action);
    QVariantList getBookmarkModelExpandableIndexesRecursive(const QModelIndex& index);
    QVariantList getFolderModelExpandableIndexesRecursive(const QModelIndex& index);
    BtBookmarksModel* getBookmarkModelPtr();
    FolderModel* getFolderModelPtr();

    RoleItemModel m_contextMenuModel;
    QModelIndex m_currentFolder;
};

} // end namespace

#endif
