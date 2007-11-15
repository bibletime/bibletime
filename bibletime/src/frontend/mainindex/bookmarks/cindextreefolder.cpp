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

#include <klocale.h>


CIndexTreeFolder::CIndexTreeFolder(CBookmarkIndex* mainIndex, const Type type, const QString& language) : CIndexFolderBase(mainIndex, type) {
	m_language = language;
}

CIndexTreeFolder::CIndexTreeFolder(CIndexFolderBase* item, const Type type, const QString& language) : CIndexFolderBase(item, type) {
	m_language = language;
}

CIndexTreeFolder::~CIndexTreeFolder() {}

void CIndexTreeFolder::addGroup(const Type type, const QString language) {
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
	else {
		//qDebug("type: other");
		i = new CIndexTreeFolder(this, type, language);
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
	if (language() == "*") {
		switch (type()) {
			case BibleModuleFolder:
			setText(0,i18n("Bibles"));
			break;
			case CommentaryModuleFolder:
			setText(0,i18n("Commentaries"));
			break;
			case LexiconModuleFolder:
			setText(0,i18n("Lexicons"));
			break;
			case BookModuleFolder:
			setText(0,i18n("Books"));
			break;
			case DevotionalModuleFolder:
			setText(0,i18n("Daily devotionals"));
			break;
			case GlossaryModuleFolder:
			setText(0,i18n("Glossaries"));
			break;
			case ImageModuleFolder:
			setText(0,i18n("Maps and Images"));
			break;

			case BookmarkFolder:
			setText(0,i18n("Bookmarks"));
			break;
			case OldBookmarkFolder:
			setText(0,i18n("Old bookmarks"));
			break;
			default:
			setText(0, i18n("Unknown"));
			break;
		};
	}
	else {
		const CLanguageMgr::Language* const lang = CPointers::languageMgr()->languageForAbbrev( language() );
		setText(0, !language().isEmpty() ? ( lang->isValid() ? lang->translatedName() : language()) : i18n("Unknown language"));
	}

	//qDebug(text(0).toLatin1().data());
	initTree();
	update();
}

void CIndexTreeFolder::initTree() {
	
}

const QString& CIndexTreeFolder::language() const {
	return m_language;
}
