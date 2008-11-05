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
#include "cbookmarkindex.h"
#include "cindexbookmarkfolder.h"
#include "cindexsubfolder.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QList>
#include <QTreeWidgetItem>
#include <QMimeSource>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QApplication>

#include <QDebug>




CIndexFolderBase::CIndexFolderBase(CBookmarkIndex* mainIndex, const Type type) : CIndexItemBase(mainIndex, type) {}

CIndexFolderBase::CIndexFolderBase(CIndexFolderBase* parentItem, const Type type) : CIndexItemBase(parentItem, type) {}

CIndexFolderBase::CIndexFolderBase(CIndexFolderBase* parentFolder, const QString& caption) : CIndexItemBase(parentFolder) {
	setText(0, caption);
}

CIndexFolderBase::~CIndexFolderBase() {}

bool CIndexFolderBase::isFolder() {
	return true;
}

void CIndexFolderBase::update() {
	CIndexItemBase::update();
	if (isExpanded() && childCount())
		setIcon(0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::openedFolder::icon));
	else
		setIcon(0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon));
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

/** Creates a new sub folder of this folder. */
void CIndexFolderBase::newSubFolder() {
	if (dynamic_cast<CIndexBookmarkFolder*>(this) || dynamic_cast<CIndexSubFolder*>(this) ) {
		CIndexSubFolder* f = new CIndexSubFolder(this, QObject::tr("New folder"));
		f->init();

		treeWidget()->setCurrentItem(f);
		//treeWidget()->ensureItemVisible(f);
		f->rename();
	}
}

/** Reimplementation. Returns true if the drop is accepted. */
bool CIndexFolderBase::allowAutoOpen(const QMimeData*) const {
	return true;
}

/** Reimplementation. Returns false because folders have no use for drops (except for the bookmark folders) */
bool CIndexFolderBase::acceptDrop(QDropEvent *) const {
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

void CIndexFolderBase::rename()
{
//	qDebug("CIndexFolderBase::rename");
	treeWidget()->editItem(this);
}
