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

	/** See the base class. */
	virtual bool enableAction(const MenuAction action);

	/** User gives a file from which to load items into this folder. */
	virtual void exportBookmarks();
	/** User gives a file to which items from this folder are saved. */
	virtual void importBookmarks();

	/** Creates a new folder under this. */
	void newSubFolder();

	/** Returns a list of direct childs of this item. */
	QList<QTreeWidgetItem*> getChildList() const;

	/** Returns true if the given item is this or a direct or indirect subitem of this. */
	bool hasDescendant(QTreeWidgetItem* item) const;

	/** Creates a deep copy of this item. */
	BtBookmarkFolder* deepCopy();

	void rename();
	void update();

	QString toolTip();
};

#endif
