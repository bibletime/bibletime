/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "btbookmarkfolder.h"
#include "btbookmarkitembase.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QDebug>

BtBookmarkFolder::BtBookmarkFolder(QTreeWidgetItem* parent, QString name)
	: BtBookmarkItemBase(parent)
{
	setText(0, name);
	setFlags(Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsEnabled);
}

bool BtBookmarkFolder::enableAction(MenuAction action)
{
	if (action == ChangeFolder || action == NewFolder || action == DeleteEntries || action == ImportBookmarks )
		return true;
	if (action == ExportBookmarks || action == ImportBookmarks )
		return true;
	if ((action == PrintBookmarks) && childCount())
		return true;
	return false;
}

void BtBookmarkFolder::addFirstChild(BtBookmarkItemBase* item)
{

}

void BtBookmarkFolder::exportBookmarks()
{

}

void BtBookmarkFolder::importBookmarks()
{

}

QString BtBookmarkFolder::toolTip()
{
	return QString();
}

void BtBookmarkFolder::newSubFolder()
{
	if (dynamic_cast<BtBookmarkFolder*>(this)) {
		BtBookmarkFolder* f = new BtBookmarkFolder(this, QObject::tr("New folder"));

		treeWidget()->setCurrentItem(f);
		//treeWidget()->ensureItemVisible(f);
		f->update();
		f->rename();
	}
}

QList<QTreeWidgetItem*> BtBookmarkFolder::getChildList()
{
	QList<QTreeWidgetItem*> list;
	for (int i = 0; i < childCount(); i++) {
		list.append(child(i));
	}
	return list;
}

void BtBookmarkFolder::rename()
{
	treeWidget()->editItem(this);
}

void BtBookmarkFolder::update()
{
	qDebug() << "BtBookmarkFolder::update()";
	BtBookmarkItemBase::update();
	if (isExpanded() && childCount())
		setIcon(0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::openedFolder::icon));
	else
		setIcon(0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::closedFolder::icon));
}