/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKMARKITEMBASE_H
#define BTBOOKMARKITEMBASE_H


#include "cindexitembase.h"
#include "cbookmarkindex.h"

#include <QTreeWidgetItem>
#include <QString>
#include <QMimeData>
#include <QDropEvent>




class BtBookmarkItemBase : public QTreeWidgetItem
{
public:

	enum MenuAction {
		NewFolder = 0,
		ChangeFolder,

		ChangeBookmark,
		ImportBookmarks,
		ExportBookmarks,
		PrintBookmarks,

		DeleteEntries,

		ActionBegin = NewFolder,
		ActionEnd = DeleteEntries
	};
	virtual void init();
	virtual const QString toolTip();
	virtual CBookmarkIndex* treeWidget() const;

	virtual void addPreviousSibling(BtBookmarkItemBase* item);
	virtual void addNextSibling(BtBookmarkItemBase* item);

	/**
	* Returns true if the given action should be enabled in the popup menu.
	*/
	virtual bool enableAction( const MenuAction action );

	virtual QList<QTreeWidgetItem*> getChildList();

	virtual bool acceptDrop(QDropEvent* e) const;

	virtual void rename();

protected:
	friend class CMainIndex;

	virtual void dropped( QDropEvent* e);
};

#endif