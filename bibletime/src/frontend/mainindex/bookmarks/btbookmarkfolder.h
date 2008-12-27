/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTBOOKMARKFOLDER_H
#define BTBOOKMARKFOLDER_H

#include "btbookmarkitembase.h"

#define CURRENT_SYNTAX_VERSION 1


class BtBookmarkFolder : public BtBookmarkItemBase
{
public:
	friend class BtBookmarkLoader;
	BtBookmarkFolder(QTreeWidgetItem* parent, QString name);
	~BtBookmarkFolder() {}

	//virtual void init();
	void addFirstChild(BtBookmarkItemBase* item);
	virtual bool enableAction(const MenuAction action);
	virtual void exportBookmarks();
	virtual void importBookmarks();

	void newSubFolder();
	QList<QTreeWidgetItem*> getChildList() const;
	bool hasDescendant(QTreeWidgetItem* item) const;
	BtBookmarkFolder* deepCopy();
	void rename();
	void update();

	QString toolTip();
};

#endif
