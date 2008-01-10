//
// C++ Implementation: cindextreefolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cindextreefolder.h"
#include "cindexfolderbase.h"
#include "cindexbookmarkfolder.h"
#include "cindexbookmarkitem.h"
#include "cindexoldbookmarksfolder.h"


#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/managers/cswordbackend.h"

#include "util/cpointers.h"


#include <QString>




CIndexTreeFolder::CIndexTreeFolder(CBookmarkIndex* mainIndex, const Type type)
	: CIndexFolderBase(mainIndex, type)
{}

CIndexTreeFolder::CIndexTreeFolder(CIndexFolderBase* item, const Type type)
	: CIndexFolderBase(item, type)
{}

CIndexTreeFolder::~CIndexTreeFolder() {}

void CIndexTreeFolder::addGroup(const Type type) {
	//qDebug("CIndexTreeFolder::addGroup");
	CIndexTreeFolder* i = 0;
	if (type == BookmarkFolder) {
		//qDebug("type: BookmarkFolder");
		i = new CIndexBookmarkFolder(this);
	}
	else if (type == OldBookmarkFolder) {
		//qDebug("type: OldBookmarkFolder");
		i = new CIndexOldBookmarksFolder(this);
	}
	i->init();
	if (!i->childCount())
		delete i;
}



void CIndexTreeFolder::addBookmark(CSwordModuleInfo* module, const QString& key, const QString& description) {
	CIndexBookmarkItem* i = new CIndexBookmarkItem(this, module, key, description);
	i->init();
}

void CIndexTreeFolder::update() {
	//qDebug("CIndexTreeFolder::update");
	CIndexFolderBase::update();
}

void CIndexTreeFolder::init() {
	//qDebug("CIndexTreeFolder::init");
	switch (type()) {
		case BookmarkFolder:
			setText(0,QObject::tr("Bookmarks"));
			break;
		case OldBookmarkFolder:
			setText(0,QObject::tr("Old bookmarks"));
			break;
		default:
			setText(0, QObject::tr("Unknown"));
			break;
	};

	//qDebug(text(0).toLatin1().data());
	initTree();
	update();
}

void CIndexTreeFolder::initTree() {
	
}

