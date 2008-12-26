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
	Q_OBJECT
public:
	BtBookmarkFolder(QTreeWidgetItem* parent);
	~BtBookmarkFolder();

	virtual void init();
	void addFirstChild(BtBookmarkItemBase* item);
	virtual const bool enableAction(const MenuAction action);
	virtual void exportBookmarks();
	virtual void importBookmarks();
};

#endif