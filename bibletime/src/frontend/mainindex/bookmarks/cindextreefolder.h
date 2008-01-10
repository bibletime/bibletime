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


/** The base class for all items in the tree. Subclasses for module folders, modules and bookmarks exist.
  * @author The BibleTime team
  */
class CIndexTreeFolder : public CIndexFolderBase {
public:
	CIndexTreeFolder(CBookmarkIndex* mainIndex, const Type type);
	CIndexTreeFolder(CIndexFolderBase* parentFolder, const Type type);
	virtual ~CIndexTreeFolder();

	virtual void addGroup(const Type type);

	virtual void addBookmark(CSwordModuleInfo* module, const QString& key, const QString& description);

	virtual void initTree();

	virtual void update();
	virtual void init();

};

#endif
