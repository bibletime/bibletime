/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookmarks/btbookmarkfolder.h"

#include <QFileDialog>
#include "bibletimeapp.h"
#include "frontend/bookmarks/btbookmarkitembase.h"
#include "frontend/bookmarks/btbookmarkitem.h"
#include "frontend/bookmarks/btbookmarkloader.h"
#include "util/cresmgr.h"
#include "util/geticon.h"


BtBookmarkFolder::BtBookmarkFolder(const QString &name, QTreeWidgetItem *parent)
        : BtBookmarkItemBase(parent) {
    setText(0, name);
    setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled);
}

bool BtBookmarkFolder::enableAction(MenuAction action) {
    if (action == ChangeFolder || action == NewFolder || action == DeleteEntries || action == ImportBookmarks )
        return true;
    if (action == SortFolderBookmarks || action == ExportBookmarks || action == ImportBookmarks )
        return true;
    if ((action == PrintBookmarks) && childCount())
        return true;
    return false;
}

void BtBookmarkFolder::exportBookmarks() {
    QString filter = QObject::tr("BibleTime bookmark files") + QString(" (*.btb);;") + QObject::tr("All files") + QString(" (*.*)");
    QString fileName = QFileDialog::getSaveFileName(0, QObject::tr("Export Bookmarks"), "", filter);

    if (!fileName.isEmpty()) {
        BtBookmarkLoader loader;
        loader.saveTreeFromRootItem(this, fileName, false ); //false: don't overwrite without asking
    };

}

void BtBookmarkFolder::importBookmarks() {
    QString filter = QObject::tr("BibleTime bookmark files") + QString(" (*.btb);;") + QObject::tr("All files") + QString(" (*.*)");
    QString fileName = QFileDialog::getOpenFileName(0, QObject::tr("Import bookmarks"), "", filter);
    if (!fileName.isEmpty()) {
        BtBookmarkLoader loader;
        QList<QTreeWidgetItem*> itemList = loader.loadTree(fileName);
        this->insertChildren(0, itemList);
    };
}

void BtBookmarkFolder::newSubFolder() {
    if (dynamic_cast<BtBookmarkFolder*>(this)) {
        BtBookmarkFolder* f = new BtBookmarkFolder(QObject::tr("New folder"), this);

        treeWidget()->setCurrentItem(f);
        f->update();
        f->rename();
    }
}

QList<QTreeWidgetItem*> BtBookmarkFolder::getChildList() const {
    QList<QTreeWidgetItem*> list;
    for (int i = 0; i < childCount(); i++) {
        list.append(child(i));
    }
    return list;
}

void BtBookmarkFolder::rename() {
    treeWidget()->editItem(this);
}

void BtBookmarkFolder::update() {
    setIcon(0, util::getIcon(isExpanded() && childCount()
                             ? CResMgr::mainIndex::openedFolder::icon
                             : CResMgr::mainIndex::closedFolder::icon));
}

bool BtBookmarkFolder::hasDescendant(QTreeWidgetItem* item) const {
    if (this == item) {
        return true;
    }
    if (getChildList().indexOf(item) > -1) {
        return true;
    }
    foreach(QTreeWidgetItem* childItem, getChildList()) {
        bool subresult = false;
        BtBookmarkFolder* folder = 0;
        if ( (folder = dynamic_cast<BtBookmarkFolder*>(childItem)) ) {
            subresult = folder->hasDescendant(childItem);
        }

        if (subresult == true) {
            return true;
        }
    }
    return false;
}

BtBookmarkFolder* BtBookmarkFolder::deepCopy() {
    BtBookmarkFolder* newFolder = new BtBookmarkFolder(this->text(0));
    foreach(QTreeWidgetItem* subitem, getChildList()) {
        if (BtBookmarkItem* bmItem = dynamic_cast<BtBookmarkItem*>(subitem)) {
            newFolder->addChild(new BtBookmarkItem(*bmItem));
        }
        else {
            if (BtBookmarkFolder* bmFolder = dynamic_cast<BtBookmarkFolder*>(subitem)) {
                newFolder->addChild(bmFolder->deepCopy());
            }
        }
    }
    newFolder->update();
    return newFolder;
}

