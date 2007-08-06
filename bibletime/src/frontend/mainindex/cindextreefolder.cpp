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
#include "cindexmoduleitem.h"
#include "cindexbookmarkitem.h"
#include "cindexoldbookmarksfolder.h"


#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/managers/cswordbackend.h"

#include "util/cpointers.h"


#include <QString>

#include <klocale.h>


CIndexTreeFolder::CIndexTreeFolder(CMainIndex* mainIndex, const Type type, const QString& language) : CIndexFolderBase(mainIndex, type) {
	m_language = language;
}

CIndexTreeFolder::CIndexTreeFolder(CIndexFolderBase* item, const Type type, const QString& language) : CIndexFolderBase(item, type) {
	m_language = language;
}

CIndexTreeFolder::~CIndexTreeFolder() {}

void CIndexTreeFolder::addGroup(const Type type, const QString language) {
	qDebug("CIndexTreeFolder::addGroup");
	CIndexTreeFolder* i = 0;
	if (type == BookmarkFolder) {
		qDebug("type: BookmarkFolder");
		i = new CIndexBookmarkFolder(this);
	}
	else if (type == OldBookmarkFolder) {
		qDebug("type: OldBookmarkFolder");
		i = new CIndexOldBookmarksFolder(this);
	}
	else {
		qDebug("type: other");
		i = new CIndexTreeFolder(this, type, language);
	}
	i->init();
	if (!i->childCount())
		delete i;
}

void CIndexTreeFolder::addModule(CSwordModuleInfo* const module) {
	CIndexModuleItem* i = new CIndexModuleItem(this, module);
	i->init();
}

void CIndexTreeFolder::addBookmark(CSwordModuleInfo* module, const QString& key, const QString& description) {
	CIndexBookmarkItem* i = new CIndexBookmarkItem(this, module, key, description);
	i->init();
}

void CIndexTreeFolder::update() {
	qDebug("CIndexTreeFolder::update");
	CIndexFolderBase::update();
}

void CIndexTreeFolder::init() {
	qDebug("CIndexTreeFolder::init");
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

	qDebug(text(0).toLatin1().data());
	initTree();
	update();
}

void CIndexTreeFolder::initTree() {
	qDebug("CTreeMgr::initTree");
	if (type() == Unknown)
		return;

	CSwordModuleInfo::ModuleType moduleType = CSwordModuleInfo::Unknown;
	if (type() == BibleModuleFolder)
		moduleType = CSwordModuleInfo::Bible;
	else if (type() == CommentaryModuleFolder)
		moduleType = CSwordModuleInfo::Commentary;
	else if (type() == LexiconModuleFolder || type() == GlossaryModuleFolder || type() == DevotionalModuleFolder)
		moduleType = CSwordModuleInfo::Lexicon;
	else if (type() == BookModuleFolder)
		moduleType = CSwordModuleInfo::GenericBook;

	//get all modules by using the given type
	ListCSwordModuleInfo allModules =CPointers::backend()->moduleList();
	ListCSwordModuleInfo usedModules;
	ListCSwordModuleInfo::iterator end_it = allModules.end();
	for (ListCSwordModuleInfo::iterator it(allModules.begin()); it != end_it; ++it) {
		//   for (CSwordModuleInfo* m = allModules.first(); m; m = allModules.next()) {
		if ((*it)->type() == moduleType) { //found a module, check if the type is correct (devotional etc.)
			if (type() == GlossaryModuleFolder && !(*it)->category() == CSwordModuleInfo::Glossary) { //not a gglossary
				continue;
			}
			if (type() == DevotionalModuleFolder && ((*it)->category() != CSwordModuleInfo::DailyDevotional)) {//not a devotional
				continue;
			}
			if (type() == LexiconModuleFolder && ( ((*it)->category() == CSwordModuleInfo::DailyDevotional) || ((*it)->category() == CSwordModuleInfo::Glossary) )) {
				//while looking for lexicons glossaries and devotionals shouldn't be used
				continue;
			}

			if (language() == QString::fromLatin1("*") || (language() != QString::fromLatin1("*") && QString::fromLatin1((*it)->module()->Lang()) == language()) )//right type and language!
				usedModules.append(*it);
		}
	}

	//we have now all modules we want to have
	if (language() == QString::fromLatin1("*")) { //create subfolders for each language
		QStringList usedLangs;
		//     for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
		/*ListCSwordModuleInfo::iterator*/
		end_it = usedModules.end();
		for (ListCSwordModuleInfo::iterator it(usedModules.begin()); it != end_it; ++it) {
			QString lang = QString::fromLatin1((*it)->module()->Lang());
			//      if (lang.isEmpty())
			//        lang = ");
			if (!usedLangs.contains(lang)) {
				usedLangs.append(lang);
			}
		}

		//ToDo:: Optimize the loop with const itrs
		QStringList::iterator lang_it;
		for (lang_it = usedLangs.begin(); lang_it != usedLangs.end(); ++lang_it) {
			addGroup(/**lang_it,*/ type(), *lang_it);
		}
	}
	else if (usedModules.count() > 0) { //create subitems with the given type and language
		/*ListCSwordModuleInfo::iterator*/ end_it = usedModules.end();
		for (ListCSwordModuleInfo::iterator it(usedModules.begin()); it != end_it; ++it) {
			//     for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
			addModule(*it);
		}
	}

	sortChildItems(0,Qt::AscendingOrder);
}

const QString& CIndexTreeFolder::language() const {
	return m_language;
}
