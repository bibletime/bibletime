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

#include "backend/btbookmarksmodel.h"
#include "backend/managers/cswordbackend.h"
#include "mobile/ui/btbookmarkinterface.h"
#include "QSortFilterProxyModel"
#include "QVariant"

namespace btm {


// FolderModel provides a TreeView of bookmarks that only
// shows folders.

class FolderModel : public QSortFilterProxyModel {

public:
    FolderModel(QObject * parent)
        : QSortFilterProxyModel(parent) {
    }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
        QModelIndex child = sourceModel()->index(source_row, 0, source_parent);
        QVariant itemType = child.data(BtBookmarksModel::TypeRole);
        if (!child.isValid())
            return false;
        if (itemType.toString() == "bookmark")
            return false;
        return true;
    }
};

static BtBookmarksModel * s_bookmarksModel = nullptr;
static FolderModel * s_folderModel = nullptr;

static QModelIndex getSourceIndex(const QModelIndex& index) {
    QModelIndex sourceIndex;

    // First item in tree is considered the root index
    if (index == QModelIndex()) {
        sourceIndex = s_folderModel->index(0, 0, QModelIndex());
    }

    sourceIndex = s_folderModel->mapToSource(index);
    return sourceIndex;
}


// API Interface between QML and c++ bookmark functionality

BtBookmarkInterface::BtBookmarkInterface(QObject* parent)
    : QObject(parent),
      m_currentFolder(QModelIndex()){
}

QVariant BtBookmarkInterface::getBookmarkModel() {
    if (!s_bookmarksModel) {
        s_bookmarksModel = new BtBookmarksModel(this);

        // Make new "blank" folder as the root folder
        if (s_bookmarksModel->rowCount() == 0) {
            QModelIndex index = s_bookmarksModel->addFolder(0, QModelIndex(), QString());
            s_bookmarksModel->setData(index, "");
            m_currentFolder = s_folderModel->index(0,0);
        }
    }

    QVariant var;
    var.setValue(s_bookmarksModel);
    return var;
}

FolderModel* BtBookmarkInterface::getFolderModelPtr() {
    if (!s_folderModel) {
        s_folderModel = new FolderModel(this);
        getBookmarkModel();
        s_folderModel->setSourceModel(s_bookmarksModel);
    }
    return s_folderModel;
}

QVariant BtBookmarkInterface::getFolderModel() {
    getFolderModelPtr();
    QVariant var;
    var.setValue(s_folderModel);
    return var;
}

QVariantList BtBookmarkInterface::getFolderModelExpandableIndexesRecursive(const QModelIndex& parent) {
    QVariantList indexes;
    int count = s_folderModel->rowCount(parent);
    for(int row = 0; row != count; ++row) {
        QModelIndex child = s_folderModel->index(row, 0, parent);
        if (s_folderModel->hasChildren(child)) {
            indexes.push_back(child);
            indexes += getFolderModelExpandableIndexesRecursive(child);
        }
    }
    return indexes;
}

QVariantList BtBookmarkInterface::getFolderModelExpandableIndexes() {
    QVariantList indexes;
    getFolderModelPtr();
    indexes = getFolderModelExpandableIndexesRecursive(QModelIndex());
    return indexes;
}

QModelIndex BtBookmarkInterface::currentFolder() const {
    return m_currentFolder;
}

void BtBookmarkInterface::setCurrentFolder(const QModelIndex& folder) {
    m_currentFolder = folder;
    currentFolderChanged();
}

QString BtBookmarkInterface::folderName(const QModelIndex& index) {
    QString name;
    if (!index.isValid())
        return name;
    if (index == QModelIndex())
        return name;
    name = index.data().toString();
    return name;
}

void BtBookmarkInterface::addBookmark(const QString& reference, const QString& moduleName) {
    QModelIndex sourceIndex = getSourceIndex(m_currentFolder);
    const CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    QString description = reference;
    int row = s_bookmarksModel->rowCount(sourceIndex);
    s_bookmarksModel->addBookmark(row, sourceIndex, *module, reference, description);
    s_bookmarksModel->save();
}

void BtBookmarkInterface::addFolder(const QString& folderName) {
    QModelIndex sourceIndex = getSourceIndex(m_currentFolder);
    int row = s_bookmarksModel->rowCount(sourceIndex);
    s_bookmarksModel->addFolder(row, sourceIndex, folderName);
    s_bookmarksModel->save();
}

}


