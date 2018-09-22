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

enum ContextMenuRoles {
    TitleRole = Qt::UserRole + 1,
    ActionRole = Qt::UserRole + 2
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
    getBookmarkModel();
    getFolderModel();
    setCurrentFolderToRootFolder();

    QHash<int, QByteArray> roleNames;
    roleNames[TitleRole] =  "title";
    roleNames[ActionRole] = "action";
    m_contextMenuModel.setRoleNames(roleNames);
}

QVariant BtBookmarkInterface::getBookmarkModel() {
    if (!s_bookmarksModel) {
        s_bookmarksModel = new BtBookmarksModel(this);

        // Make "Bookmarks" folder under the root folder
        if (s_bookmarksModel->rowCount() == 0) {
            QModelIndex index = s_bookmarksModel->addFolder(0, QModelIndex(), QString());
            s_bookmarksModel->setData(index, tr("Bookmarks"));
        }
    }
    QVariant var;
    var.setValue(s_bookmarksModel);
    return var;
}

BtBookmarksModel* BtBookmarkInterface::getBookmarkModelPtr() {
    getBookmarkModel();
    return s_bookmarksModel;
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

QVariant BtBookmarkInterface::getContextMenuModel() {
    QVariant var;
    var.setValue(&m_contextMenuModel);
    return var;
}

QVariantList BtBookmarkInterface::getBookmarkModelExpandableIndexesRecursive(const QModelIndex& parent) {
    QVariantList indexes;
    int count = s_bookmarksModel->rowCount(parent);
    for(int row = 0; row != count; ++row) {
        QModelIndex child = s_bookmarksModel->index(row, 0, parent);
        if (s_bookmarksModel->hasChildren(child)) {
            indexes.push_back(child);
            indexes += getBookmarkModelExpandableIndexesRecursive(child);
        }
    }
    return indexes;
}

QVariantList BtBookmarkInterface::getBookmarkModelExpandableIndexes() {
    QVariantList indexes;
    getBookmarkModelPtr();
    indexes = getBookmarkModelExpandableIndexesRecursive(QModelIndex());
    return indexes;
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

bool BtBookmarkInterface::isBookmark(const QModelIndex& index) {
    if (!index.isValid())
        return false;
    QVariant itemType = index.data(BtBookmarksModel::TypeRole);
    if (itemType.toString() == "bookmark")
        return true;
    return false;
}

bool BtBookmarkInterface::isRoot(const QModelIndex&  index) {
    return index.parent() == QModelIndex();
}

void BtBookmarkInterface::appendContextMenuItem(const QString& title, const QString& action) {
    QStandardItem* item = new QStandardItem();
    item->setData(title, TitleRole);
    item->setData(action, ActionRole);
    m_contextMenuModel.appendRow(item);
}

void BtBookmarkInterface::setupContextMenuModel(const QModelIndex& index) {
    m_contextMenuModel.clear();
    int row = index.row();
    int rowCount = index.model()->rowCount(index.parent());
    bool folder = !isBookmark(index);

    if (folder) {
        appendContextMenuItem(tr("Expand/Collapse"), "toggleexpand");

    } else {
        appendContextMenuItem(tr("Open"), "open");
    }
    if (folder)
        appendContextMenuItem(tr("Delete folder and bookmarks"), "delete");
    else
        appendContextMenuItem(tr("Delete bookmark"), "delete");
}

void BtBookmarkInterface::doContextMenu(const QString& action, const QModelIndex& index) {
    if (action == "delete") {
        s_bookmarksModel->removeRows(index.row(), 1, index.parent());
        return;
    }
}

QString BtBookmarkInterface::getReference(const QModelIndex& index) {
    if (!isBookmark(index))
        return "undefined";
    return s_bookmarksModel->key(index);
}

QString BtBookmarkInterface::getModule(const QModelIndex& index) {
    if (!isBookmark(index))
        return "undefined";
    CSwordModuleInfo* module = s_bookmarksModel->module(index);
    return module->name();
}

QModelIndex BtBookmarkInterface::currentFolder() const {
    return m_currentFolder;
}

void BtBookmarkInterface::setCurrentFolderToRootFolder() {
    Q_ASSERT(s_folderModel);
    Q_ASSERT(s_folderModel->rowCount() > 0);
    m_currentFolder = s_folderModel->index(0,0);
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

} // end of "btm" namespace


