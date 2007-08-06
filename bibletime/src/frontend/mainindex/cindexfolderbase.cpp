//
// C++ Implementation: cindexfolderbase
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cindexfolderbase.h"

#include "cindexitembase.h"
#include "cmainindex.h"
#include "cindexbookmarkfolder.h"
#include "cindexsubfolder.h"

#include "util/cresmgr.h"


#include <QList>
#include <QTreeWidgetItem>
#include <QMimeSource>
#include <QList>


#include <kiconloader.h>
#include <klocale.h>


CIndexFolderBase::CIndexFolderBase(CMainIndex* mainIndex, const Type type) : CIndexItemBase(mainIndex, type) {}

CIndexFolderBase::CIndexFolderBase(CIndexFolderBase* parentItem, const Type type) : CIndexItemBase(parentItem, type) {}

CIndexFolderBase::CIndexFolderBase(CIndexFolderBase* parentFolder, const QString& caption) : CIndexItemBase(parentFolder) {
	setText(0, caption);
}

CIndexFolderBase::~CIndexFolderBase() {}

const bool CIndexFolderBase::isFolder() {
	return true;
}

void CIndexFolderBase::update() {
	CIndexItemBase::update();
	if (isExpanded() && childCount())
		setIcon(0, SmallIcon(CResMgr::mainIndex::openedFolder::icon, 16));
	else
		setIcon(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
}

void CIndexFolderBase::init() {
	CIndexItemBase::init();
	//setDropEnabled(false);
	//setDragEnabled(false);
}

/** No descriptions */
void CIndexFolderBase::setExpanded( bool open ) {
	QTreeWidgetItem::setExpanded(open);
	update();
}

/** The function which renames this folder. */
void CIndexFolderBase::rename() {
	//startRename(0); //TODO: not in qt4
}

/** Creates a new sub folder of this folder. */
void CIndexFolderBase::newSubFolder() {
	if (dynamic_cast<CIndexBookmarkFolder*>(this) || dynamic_cast<CIndexSubFolder*>(this) ) {
		CIndexSubFolder* f = new CIndexSubFolder(this, i18n("New folder"));
		f->init();

		treeWidget()->setCurrentItem(f);
		//treeWidget()->ensureItemVisible(f);
		f->rename();
	}
}

/** Reimplementation. Returns true if the drop is accepted. */
const bool CIndexFolderBase::allowAutoOpen( const QMimeSource* ) const {
	return true;
}

/** Reimplementation. Returns false because folders have no use for drops (except for the bookmark folders) */
bool CIndexFolderBase::acceptDrop(const QMimeSource*) const {
	return false;
}

int CIndexFolderBase::getDirectChildCount()
{
	return childCount();
}

QList<QTreeWidgetItem*> CIndexFolderBase::getChildList() {
	QList<QTreeWidgetItem*> itemList;
	for (int i = 0; i < childCount(); i++) {
		CIndexItemBase* item = dynamic_cast<CIndexItemBase*>(child(i));
		if (item) {
			itemList.append(item);
			itemList << item->getChildList();
		}
	}
	return itemList; 
}



//I don't quite get the purpose of this...
// do we want to get all children recursively?

// /*
// 	QList<QTreeWidgetItem*> childs;
// 	if (!childCount()) //no childs available
// 		return childs;
// 
// 	QTreeWidgetItem* i = firstChild();
// 	while (i && (i->parent() == this)) {
// 		CIndexItemBase* item = dynamic_cast<CIndexItemBase*>(i);
// 		if (item) { //we found a valid item
// 			childs.append(item);
// 
// 			CIndexFolderBase* folder = dynamic_cast<CIndexFolderBase*>(i);
// 			if (folder) {
// 				QList<QTreeWidgetItem*> subChilds = folder->getChildList();
// 				//for (QTreeWidgetItem* ci = subChilds.first(); ci; ci = subChilds.next()) {
// 				foreach (QTreeWidgetItem* ci, subChilds) {
// 					childs.append(ci);
// 				}
// 			}
// 		}
// 
// 		do {
// 			i = i->nextSibling();
// 		}
// 		while (i && (i->parent() != this));
// 	}
// 
// 	return childs;
// }*/
