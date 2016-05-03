/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_BOOKMARK_INTERFACE_H
#define BT_BOOKMARK_INTERFACE_H

// API Interface between QML and c++ bookmark functionality

#include "backend/btbookmarksmodel.h"
#include <QObject>

namespace btm {

class FolderModel;

class BtBookmarkInterface : public QObject {

    Q_OBJECT

    Q_PROPERTY(QVariant bookmarkModel READ getBookmarkModel NOTIFY bookmarkModelChanged)
    Q_PROPERTY(QVariant folderModel   READ getFolderModel  NOTIFY folderModelChanged)
    Q_PROPERTY(QModelIndex currentFolder READ currentFolder WRITE setCurrentFolder NOTIFY currentFolderChanged)

public:
    BtBookmarkInterface(QObject *parent = 0);

    // Folders and Bookmarks
    QVariant getBookmarkModel();

    // Only Folders
    QVariant getFolderModel();
    QModelIndex currentFolder() const;
    void setCurrentFolder(const QModelIndex& folder);

    Q_INVOKABLE void addBookmark(const QString& reference, const QString& moduleName);
    Q_INVOKABLE QString folderName(const QModelIndex& index);
    Q_INVOKABLE QVariantList getFolderModelExpandableIndexes();


signals:
    void bookmarkModelChanged();
    void folderModelChanged();
    void currentFolderChanged();

private:
    QVariantList getFolderModelExpandableIndexesRecursive(const QModelIndex& index);

    FolderModel* getFolderModelPtr();

    BtBookmarksModel* m_bookmarkModel;
    FolderModel* m_folderModel;
    QModelIndex m_currentFolder;
};

} // end namespace

#endif
