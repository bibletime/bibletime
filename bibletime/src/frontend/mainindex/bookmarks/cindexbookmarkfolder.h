/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CINDEXBOOKMARKFOLDER_H
#define CINDEXBOOKMARKFOLDER_H

#include "cindextreefolder.h"

#define CURRENT_SYNTAX_VERSION 1

class CIndexFolderBase;

class QMimeData;
class QTreeWidgetItem;
class QDropEvent;
class QString;

class CIndexBookmarkFolder : public CIndexTreeFolder
{
public:
	CIndexBookmarkFolder(CBookmarkIndex* bookmarkIndex, const Type type = BookmarkFolder);
	CIndexBookmarkFolder(CIndexFolderBase* parentItem, const Type type = BookmarkFolder);
	virtual ~CIndexBookmarkFolder();
	virtual bool enableAction(const MenuAction action);
	virtual void exportBookmarks();
	virtual void importBookmarks();
	virtual bool acceptDrop(QDropEvent* e) const;
	virtual void droppedItem(QDropEvent *e, QTreeWidgetItem* after);

	/**
	* Loads bookmarks from XML content
	*/
	bool loadBookmarksFromXML( const QString& xml );
	/**
	* Loads bookmarks from a file.
	*/
	bool loadBookmarks( const QString& );
	/**
	* Saves the bookmarks in a file.
	*/
	bool saveBookmarks( const QString& filename, const bool& forceOverwrite = true );

protected: // Protected methods
	virtual void initTree();
};

#endif
