//
// C++ Interface: cindextreefolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CINDEXTREEFOLDER_H
#define CINDEXTREEFOLDER_H


#include "cindexfolderbase.h"


#include <QString>


class CSwordModuleInfo;
class CMainIndex;


/**
 * The toplevel folder for the bookmarks.
 * @author The BibleTime team
 */
class CIndexTreeFolder : public CIndexFolderBase {
public:
	CIndexTreeFolder(CBookmarkIndex* mainIndex, const Type type);
	CIndexTreeFolder(CIndexFolderBase* parentFolder, const Type type);
	virtual ~CIndexTreeFolder();

	/** */
	virtual void addGroup(const Type type);

	virtual void addBookmark(CSwordModuleInfo* module, const QString& key, const QString& description);

	virtual void initTree();

	virtual void update();
	virtual void init();

};

#endif
