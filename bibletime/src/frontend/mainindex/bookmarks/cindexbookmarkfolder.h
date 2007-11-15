//
// C++ Interface: cindexbookmarkfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

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
	virtual const bool enableAction(const MenuAction action);
	virtual void exportBookmarks();
	virtual void importBookmarks();
	virtual bool acceptDrop(const QMimeData * data) const;
	virtual void dropped(QDropEvent *e, QTreeWidgetItem* after);

	/**
	* Loads bookmarks from XML content
	*/
	const bool loadBookmarksFromXML( const QString& xml );
	/**
	* Loads bookmarks from a file.
	*/
	const bool loadBookmarks( const QString& );
	/**
	* Saves the bookmarks in a file.
	*/
	const bool saveBookmarks( const QString& filename, const bool& forceOverwrite = true );

protected: // Protected methods
	virtual void initTree();
};

#endif
