/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


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
