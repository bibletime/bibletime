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
    FolderModel(QObject * parent) {
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



// API Interface between QML and c++ bookmark functionality

BtBookmarkInterface::BtBookmarkInterface(QObject* parent)
    : QObject(parent),
      m_bookmarkModel(0),
      m_folderModel(0),
      m_currentFolder(QModelIndex()){
}

QVariant BtBookmarkInterface::getBookmarkModel() {
    if (!m_bookmarkModel)
        m_bookmarkModel = new BtBookmarksModel(this);

    QVariant var;
    var.setValue(m_bookmarkModel);
    return var;
}

FolderModel* BtBookmarkInterface::getFolderModelPtr() {
    if (!m_folderModel) {
        m_folderModel = new FolderModel(this);
        getBookmarkModel();
        m_folderModel->setSourceModel(m_bookmarkModel);
    }
    return m_folderModel;
}

QVariant BtBookmarkInterface::getFolderModel() {
    getFolderModelPtr();
    QVariant var;
    var.setValue(m_folderModel);
    return var;
}

QVariantList BtBookmarkInterface::getFolderModelExpandableIndexesRecursive(const QModelIndex& parent) {
    QVariantList indexes;
    int count = m_folderModel->rowCount(parent);
    for(int row = 0; row != count; ++row) {
        QModelIndex child = m_folderModel->index(row, 0, parent);
        if (m_folderModel->hasChildren(child)) {
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

    int row = 0;
      QModelIndex sourceIndex = m_folderModel->mapToSource(m_currentFolder);

    const CSwordModuleInfo* module = CSwordBackend::instance()->findModuleByName(moduleName);
    QString description = reference;

    m_bookmarkModel->addBookmark(row,
                                 sourceIndex,
                                 *module,
                                 reference,
                                 description);
    m_bookmarkModel->save();
}

}
