/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cindexitem.h"
#include "cmainindex.h"

#include "backend/creferencemanager.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"
#include "backend/clanguagemgr.h"

#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/cexportmanager.h"
#include "frontend/cbtconfig.h"
#include "frontend/cinputdialog.h"
#include "frontend/cexportmanager.h"
#include "frontend/cdragdropmgr.h"

#include "util/cresmgr.h"
#include "util/scoped_resource.h"
#include "util/ctoolclass.h"

#include <string.h>

//Qt includes
#include <qdragobject.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qstring.h>
#include <qtextstream.h>

//KDE includes
#include <kconfig.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#define CURRENT_SYNTAX_VERSION 1

using std::string;

CItemBase::CItemBase(CMainIndex* mainIndex, const Type type)
: KListViewItem(mainIndex),
m_type(type),
m_sortingEnabled(true) {}

CItemBase::CItemBase(CItemBase* parentItem, const Type type)
: KListViewItem(parentItem),
m_type(type),
m_sortingEnabled(true) {}

CItemBase::~CItemBase() {}

const QString CItemBase::toolTip() {
	return QString::null;
}

/** Returns the used main index. */
CMainIndex* CItemBase::listView() const {
	return dynamic_cast<CMainIndex*>( QListViewItem::listView() );
}

void CItemBase::init() {
	update();
}

void CItemBase::update() {}

const CItemBase::Type& CItemBase::type() const {
	return m_type;
};

void CItemBase::moveAfter( CItemBase* const item ) {
	if (!item)
		return;

	if ( parent() == item->parent() ) { //same parent means level
		moveItem(item); //both items are on the same level, so we can use moveItem
	}
}

/** Returns true if the given action should be enabled in the popup menu. */
const bool CItemBase::enableAction( const MenuAction /*action*/ ) {
	return false; //this base class has no valif actions
}

/** No descriptions */
const bool CItemBase::isMovable() {
	return false;
}

const bool CItemBase::allowAutoOpen( const QMimeSource* ) const {
	return false;
};

/** This function engables or disables sorting depending on the parameter. */
void CItemBase::setSortingEnabled( const bool& enableSort ) {
	m_sortingEnabled = enableSort;
}

/** Returns true whether the sorting is enabled or not. */
const bool CItemBase::isSortingEnabled() {
	return m_sortingEnabled;
}

/** Reimplementation which takes care of the our sortingEnabled setting. */
void CItemBase::sortChildItems( int col, bool asc ) {
	if (!isSortingEnabled()) {
		return;
	}
	else {
		KListViewItem::sortChildItems( col, asc );
	}
}

/** Reimplementation which takes care of the our sortingEnabled setting. */
void CItemBase::sort() {
	if (!isSortingEnabled()) {
		return;
	}
	else {
		KListViewItem::sort();
	}
}

/* ---------------------------------------------- */
/* ---------- new class: CModuleItem ------------ */
/* ---------------------------------------------- */

CModuleItem::CModuleItem(CTreeFolder* parentItem, CSwordModuleInfo* module) : CItemBase(parentItem), m_module(module) {}

CModuleItem::~CModuleItem() {}

/** No descriptions */
void CModuleItem::update() {
	if (m_module) {
		setPixmap(0, CToolClass::getIconForModule(m_module));
		setText(0,m_module->name() );
	}
}

void CModuleItem::init() {
	setDragEnabled(false);
	setDropEnabled(true);

	update();
}


/** Reimplementation to handle text drops on a module. In this case open the searchdialog. In the case of a referebnce open the module at the given position. */
bool CModuleItem::acceptDrop( const QMimeSource* src ) const {
	if (CDragDropMgr::canDecode(src)) {
	if (CDragDropMgr::dndType(src) == CDragDropMgr::Item::Bookmark) {
			CDragDropMgr::Item item = CDragDropMgr::decode(src).first();
			CSwordModuleInfo* m = CPointers::backend()->findModuleByName( item.bookmarkModule() );
			if (m && (module()->type() == m->type())) { //it makes only sense
				return true;
			}
			//but we also allow drops from bibles on commentaries and the other way from commentaries
			else if (m && (module()->type() == CSwordModuleInfo::Bible) && (m->type() == CSwordModuleInfo::Commentary)) {
				return true;
			}
			else if (m && (module()->type() == CSwordModuleInfo::Commentary) && (m->type() == CSwordModuleInfo::Bible)) {
				return true;
			}
		}
		else if(CDragDropMgr::dndType(src) == CDragDropMgr::Item::Text) { //text drop on a module
			return true;
		};
	}
	return false; //default return value
}

/** No descriptions */
void CModuleItem::dropped( QDropEvent* e, QListViewItem* /*after*/) {
	/* Something was dropped on a module item
	*
	* 1. If the drop type is plain text open the searchdialog for this text and start the search
	* 2. If the type is Bookmark, open the module at the specified position
	*
	* We support only the first drop item, more is not useful
	*/

	if (acceptDrop(e)) {
		CDragDropMgr::ItemList dndItems = CDragDropMgr::decode(e);
		CDragDropMgr::Item item = dndItems.first();
		if (CDragDropMgr::dndType(e) == CDragDropMgr::Item::Text) { //open the searchdialog
			//      qWarning("Text dropped!");
			if ( module() ) {
				ListCSwordModuleInfo modules;
				modules.append(module());

				Search::CSearchDialog::openDialog(modules, item.text());
			}
		}
		else if (CDragDropMgr::dndType(e) == CDragDropMgr::Item::Bookmark) { //open the module
			CSwordModuleInfo* m = CPointers::backend()->findModuleByName( item.bookmarkModule() );
			if (m) { //it makes only sense to create a new window for a module with the same type
				if ((module()->type() == m->type()) ||
						((module()->type() == CSwordModuleInfo::Bible || module()->type() == CSwordModuleInfo::Commentary)
						 && (m->type() == CSwordModuleInfo::Bible || m->type() == CSwordModuleInfo::Commentary))) { //same base type of module
					ListCSwordModuleInfo modules;
					modules.append(module());

					listView()->emitModulesChosen(modules, item.bookmarkKey());
				};
			}
		}
	};
}


/** Reimplementation. */
const QString CModuleItem::toolTip() {
	return CToolClass::moduleToolTip(module());
}

/** Returns the used module. */
CSwordModuleInfo* const CModuleItem::module() const {
	return m_module;
}

/** Reimplementation from  CItemBase. */
const bool CModuleItem::enableAction( const MenuAction action ) {
	if (action == EditModule) {
		if (!module())
			return false;
		return module()->isWritable();
	}

	if (action == SearchInModules || action == AboutModule)
		return true;
	if (module()->isEncrypted() && action == UnlockModule)
		return true;
	return false;
}

/* ----------------------------------------------*/
/* ---------- new class: CBookmarkItem ------------*/
/* ----------------------------------------------*/

CBookmarkItem::CBookmarkItem(CFolderBase* parentItem, CSwordModuleInfo* module, const QString& key, const QString& description)
: CItemBase(parentItem),
m_description(description),
m_moduleName(module ? module->name() : QString::null) {
	if ((module && (module->type() == CSwordModuleInfo::Bible) || (module->type() == CSwordModuleInfo::Commentary))  ) {
		CSwordVerseKey vk(0);
		vk = key;
		vk.setLocale("en");
		m_key = vk.key(); //the m_key member is always the english key!
	}
	else {
		m_key = key;
	};

	m_startupXML = QDomElement(); //empty XML code
}

CBookmarkItem::CBookmarkItem(CFolderBase* parentItem, QDomElement& xml )
: CItemBase(parentItem),
m_key(QString::null),
m_description(QString::null),
m_moduleName(QString::null) {
	m_startupXML = xml;
}

CBookmarkItem::~CBookmarkItem() {}

/** No descriptions */
void CBookmarkItem::update() {
	setMultiLinesEnabled(true);
	setPixmap(0,SmallIcon(CResMgr::mainIndex::bookmark::icon,16));

	const QString title = QString::fromLatin1("%1 (%2)")
						  .arg(key())
						  .arg(module() ? module()->name() : i18n("unknown"));
	setText(0, title);
}

void CBookmarkItem::init() {
	if (!m_startupXML.isNull()) { //we have some XML code to parse
		loadFromXML(m_startupXML);
	}

	update();
	setDropEnabled(false);
	setDragEnabled(false);

	//  if (!module())
	//    setSelectable(false);
}

/** Reimplementation. */
const QString CBookmarkItem::toolTip() {
	if (!module()) {
		return QString::null;
	}

	CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults();
	filterOptions.footnotes = false;
	filterOptions.scriptureReferences = false;
	CPointers::backend()->setFilterOptions(filterOptions);

	QString ret;
	util::scoped_ptr<CSwordKey> k( CSwordKey::createInstance(module()) );
	k->key(this->key());

	const CLanguageMgr::Language* lang = module()->language();
	CBTConfig::FontSettingsPair fontPair = CBTConfig::get
											   (lang);

	Q_ASSERT(k.get());
	if (fontPair.first) { //use a special font
		qWarning("using a font, %s", fontPair.second.family().latin1());
		ret = QString::fromLatin1("<b>%1 (%2)</b><br/><small>%3</small><hr><font face=\"%4\" size=\"4\">%5</font>")
			  .arg(key())
			  .arg(module()->name())
			  .arg(description())
			  .arg(fontPair.second.family())
			  .arg(k->renderedText());
	}
	else {
		ret = QString::fromLatin1("<b>%1 (%2)</b><br/><small>%3</small><hr>%4")
			  .arg(key())
			  .arg(module()->name())
			  .arg(description())
			  .arg(k->renderedText());
	}

	return ret;
}

/** Returns the used module. */
CSwordModuleInfo* const CBookmarkItem::module() {
	CSwordModuleInfo* const m = CPointers::backend()->findModuleByName(m_moduleName);
	Q_ASSERT(m);
	return m;
}

/** Returns the used key. */
const QString CBookmarkItem::key() {
	const QString englishKeyName = englishKey();
	if (!module()) {
		return englishKeyName;
	}

	QString returnKeyName = englishKeyName;
	if ((module()->type() == CSwordModuleInfo::Bible) || (module()->type() == CSwordModuleInfo::Commentary)) {
		CSwordVerseKey vk(0);
		vk = englishKeyName;
		vk.setLocale(CPointers::backend()->booknameLanguage().latin1() );

		returnKeyName = vk.key(); //the returned key is always in the currently set bookname language
	}

	return returnKeyName;
}

/** Returns the used description. */
const QString& CBookmarkItem::description() {
	return m_description;
}

/** No descriptions */
const bool CBookmarkItem::isMovable() {
	return true;
}

/** Reimplementation to handle  the menu entries of the main index. */
const bool CBookmarkItem::enableAction(const MenuAction action) {
	if (action == ChangeBookmark || (module() && (action == PrintBookmarks)) || action == DeleteEntries)
		return true;

	return false;
}

/** Changes this bookmark. */
void CBookmarkItem::rename() {
	bool ok  = false;
	const QString newDescription = CInputDialog::getText(i18n("Change description ..."), i18n("Enter a new description for the chosen bookmark."), description(), &ok, listView(), true);

	if (ok) {
		m_description = newDescription;
		update();
	}
}

/** Reimplementation of CItemBase::saveToXML. */
QDomElement CBookmarkItem::saveToXML( QDomDocument& doc ) {
	QDomElement elem = doc.createElement("Bookmark");

	elem.setAttribute("key", englishKey());
	elem.setAttribute("description", description());
	elem.setAttribute("modulename", m_moduleName);
	elem.setAttribute("moduledescription", module() ? module()->config(CSwordModuleInfo::Description) : QString::null);

	return elem;
}

void CBookmarkItem::loadFromXML( QDomElement& element ) {
	if (element.isNull())
		return;

	//find the right module
	if (element.hasAttribute("modulename")) {
		//we use the name in all cases, even if the module isn't installed anymore
		m_moduleName = element.attribute("modulename");
	}

	if (element.hasAttribute("key")) {
		m_key = element.attribute("key");
	}

	if (element.hasAttribute("description")) {
		m_description = element.attribute("description");
	}
}

/** Returns the english key. */
const QString& CBookmarkItem::englishKey() const {
	return m_key;
}

/** Reimplementation. Returns false everytime because a bookmarks  has not possible drops. */
bool CBookmarkItem::acceptDrop(const QMimeSource* /*src*/) const {
	return false;
}

/****************************************/
/*****  class: CItemFolder  *************/
/****************************************/

CFolderBase::CFolderBase(CMainIndex* mainIndex, const Type type) : CItemBase(mainIndex, type) {}

CFolderBase::CFolderBase(CFolderBase* parentItem, const Type type) : CItemBase(parentItem, type) {}

CFolderBase::CFolderBase(CFolderBase* parentFolder, const QString& caption) : CItemBase(parentFolder) {
	setText(0, caption);
}

CFolderBase::~CFolderBase() {}

const bool CFolderBase::isFolder() {
	return true;
}

void CFolderBase::update() {
	CItemBase::update();
	if (isOpen() && childCount())
		setPixmap(0, SmallIcon(CResMgr::mainIndex::openedFolder::icon, 16));
	else
		setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
}

void CFolderBase::init() {
	CItemBase::init();
	setDropEnabled(false);
	setDragEnabled(false);
}

/** No descriptions */
void CFolderBase::setOpen( bool open ) {
	KListViewItem::setOpen(open);
	update();
}

/** The function which renames this folder. */
void CFolderBase::rename() {
	startRename(0);
}

/** Creates a new sub folder of this folder. */
void CFolderBase::newSubFolder() {
	if (dynamic_cast<CBookmarkFolder*>(this) || dynamic_cast<Bookmarks::SubFolder*>(this) ) {
		Bookmarks::SubFolder* f = new Bookmarks::SubFolder(this, i18n("New folder"));
		f->init();

		listView()->setCurrentItem(f);
		listView()->ensureItemVisible(f);
		f->rename();
	}
}

/** Reimplementation. Returns true if the drop is accepted. */
const bool CFolderBase::allowAutoOpen( const QMimeSource* ) const {
	return true;
}

/** Reimplementation. Returns false because folders have no use for drops (except for the bookmark folders) */
bool CFolderBase::acceptDrop(const QMimeSource*) const {
	return false;
}

QPtrList<QListViewItem> CFolderBase::getChildList() {
	QPtrList<QListViewItem> childs;
	if (!childCount()) //no childs available
		return childs;

	QListViewItem* i = firstChild();
	while (i && (i->parent() == this)) {
		CItemBase* item = dynamic_cast<CItemBase*>(i);
		if (item) { //we found a valid item
			childs.append(item);

			CFolderBase* folder = dynamic_cast<CFolderBase*>(i);
			if (folder) {
				QPtrList<QListViewItem> subChilds = folder->getChildList();
				for (QListViewItem* ci = subChilds.first(); ci; ci = subChilds.next()) {
					childs.append(ci);
				}
			}
		}

		do {
			i = i->nextSibling();
		}
		while (i && (i->parent() != this));
	}

	return childs;
}

/****************************************/
/*****  class: CTreeFolder  *************/
/****************************************/


CTreeFolder::CTreeFolder(CMainIndex* mainIndex, const Type type, const QString& language) : CFolderBase(mainIndex, type) {
	m_language = language;
}

CTreeFolder::CTreeFolder(CFolderBase* item, const Type type, const QString& language) : CFolderBase(item, type) {
	m_language = language;
}

CTreeFolder::~CTreeFolder() {}

void CTreeFolder::addGroup(const Type type, const QString language) {
	CTreeFolder* i = 0;
	if (type == BookmarkFolder) {
		i = new CBookmarkFolder(this);
	}
	else if (type == OldBookmarkFolder) {
		i = new Bookmarks::OldBookmarksFolder(this);
	}
	else {
		i = new CTreeFolder(this, type, language);
	}
	i->init();
	if (!i->childCount())
		delete i;
}

void CTreeFolder::addModule(CSwordModuleInfo* const module) {
	CModuleItem* i = new CModuleItem(this, module);
	i->init();
}

void CTreeFolder::addBookmark(CSwordModuleInfo* module, const QString& key, const QString& description) {
	CBookmarkItem* i = new CBookmarkItem(this, module, key, description);
	i->init();
}

void CTreeFolder::update() {
	CFolderBase::update();
}

void CTreeFolder::init() {
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
	initTree();
	update();
}

void CTreeFolder::initTree() {
	//  qWarning("CTreeMgr::initTree");
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

	sortChildItems(0,true);
}

const QString& CTreeFolder::language() const {
	return m_language;
};

/* --------------------------------------------------*/
/* ---------- new class: CBookmarkFolder::SubFolder--*/
/* --------------------------------------------------*/

namespace Bookmarks {
	/* --------------------------------------------------------------------------*/
	/* ---------- new class: CBookmarkFolder::OldBookmarkImport -----------------*/
	/* --------------------------------------------------------------------------*/
	const QString OldBookmarkImport::oldBookmarksXML( const QString& configFileName ) {
		QString fileName = (configFileName.isEmpty()) ? "bt-groupmanager" : configFileName;
		KConfig* config = new KSimpleConfig( fileName );

		KConfigGroupSaver groupSaver(config, configFileName.isEmpty() ? "Groupmanager" : "Bookmarks");

		QDomDocument doc("DOC");
		doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

		QDomElement content = doc.createElement("SwordBookmarks");
		content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
		doc.appendChild(content);

		//first create the bookmark groups in the XML document, then add the bookmarks to each parent
		QMap<int, QDomElement> parentMap; //maps parent ids to dom elements


		QStringList groupList = config->readListEntry("Groups");
		QValueList<int> parentList = config->readIntListEntry("Group parents");

		QStringList::Iterator it_groups = groupList.begin();
		QValueList<int>::Iterator it_parents = parentList.begin();

		int parentIDCounter = 0;
		while ( (it_groups != groupList.end()) && (it_parents != parentList.end()) ) {
			QDomElement parentElement = (*it_parents == -1) ? content : parentMap[*it_parents];
			if (parentElement.isNull()) {
				qWarning("EMPTY PARENT FOUND!");
				parentElement = content;
			};

			QDomElement elem = doc.createElement("Folder");
			elem.setAttribute("caption", (*it_groups));
			parentMap.insert(parentIDCounter, elem);

			parentElement.appendChild( elem );


			++it_parents;
			++it_groups;
			++parentIDCounter;
		}

		//groups are now read in, create now the bookmarks
		parentList  = config->readIntListEntry("Bookmark parents");
		QStringList bookmarkList        = config->readListEntry("Bookmarks");
		QStringList bookmarkModulesList    = config->readListEntry("Bookmark modules");
		QStringList bookmarkDescriptionsList  = config->readListEntry("Bookmark descriptions");

		it_parents  = parentList.begin();
		QStringList::Iterator it_bookmarks    = bookmarkList.begin();
		QStringList::Iterator it_modules     = bookmarkModulesList.begin();
		QStringList::Iterator it_descriptions = bookmarkDescriptionsList.begin();

		while ( it_bookmarks != bookmarkList.end()
				&& it_parents != parentList.end()
				&& it_modules != bookmarkModulesList.end()
			  ) {
			QDomElement parentElement = ((*it_parents) == -1) ? content : parentMap[(*it_parents)];
			if (parentElement.isNull()) {
				qWarning("EMPTY PARENT FOUND!");
				parentElement = content;
			};
			QDomElement elem = doc.createElement("Bookmark");

			elem.setAttribute("key", *it_bookmarks);
			elem.setAttribute("description", *it_descriptions);
			elem.setAttribute("modulename", *it_modules);

			CSwordModuleInfo* m = CPointers::backend()->findModuleByName( *it_modules );
			elem.setAttribute("moduledescription", m ? m->config(CSwordModuleInfo::Description) : QString::null);

			parentElement.appendChild( elem );


			++it_parents;
			++it_modules;
			++it_descriptions;
			++it_bookmarks;
		};

		return doc.toString();
	};

	/********************
	*   New class: OldBookmarkFolder
	*********************/

OldBookmarksFolder::OldBookmarksFolder(CTreeFolder* folder) : CBookmarkFolder(folder, OldBookmarkFolder) {}

	OldBookmarksFolder::~OldBookmarksFolder() {}

	/** Reimplementation to handle special bookmark tree. */
	void OldBookmarksFolder::initTree() {
		// Import the bookmarks of the previous BibleTime versions
		if (!CBTConfig::get
					( CBTConfig::readOldBookmarks )) { //if we havn't yet loaded the old bookmarks
				loadBookmarksFromXML( Bookmarks::OldBookmarkImport::oldBookmarksXML() );
			}
	}


	QDomElement OldBookmarksFolder::saveToXML( QDomDocument& doc ) {
		QDomElement elem = doc.createElement("Folder");
		elem.setAttribute("caption", text(0));

		// Append the XML nodes of all child items
		CItemBase* i = dynamic_cast<CItemBase*>(firstChild());
		while( i ) {
			if (i->parent() == this) {
				QDomElement newElem = i->saveToXML( doc );
				if (!newElem.isNull()) {
					elem.appendChild( newElem ); //append to this folder
				}
			}
			i = dynamic_cast<CItemBase*>( i->nextSibling() );
		}

		// Save to config, that we imported the old bookmarks and that we have them on disk
		CBTConfig::set
			( CBTConfig::readOldBookmarks, true );

		return elem;
	}

	void OldBookmarksFolder::loadFromXML( QDomElement& /*element*/ ) {
		//this function is empty because the folder imports the old 1.2 bookmarks from the bt-groupmanager config file
	}


	// New class SubFolder

SubFolder::SubFolder(CFolderBase* parentItem, const QString& caption) : CBookmarkFolder(parentItem, BookmarkFolder) {
		m_startupXML = QDomElement();
		setText( 0, caption );
	}

SubFolder::SubFolder(CFolderBase* parentItem, QDomElement& xml ) : CBookmarkFolder(parentItem, BookmarkFolder) {
		m_startupXML = xml;
	}

	SubFolder::~SubFolder() {}

	void SubFolder::init() {
		CFolderBase::init();
		if (!m_startupXML.isNull())
			loadFromXML(m_startupXML);

		setDropEnabled(true);
		setRenameEnabled(0,true);
	}

	/** Reimplementation from  CItemBase. */
	const bool SubFolder::enableAction(const MenuAction action) {
		if (action == ChangeFolder || action == NewFolder || action == DeleteEntries || action == ImportBookmarks )
			return true;

		if (action == ExportBookmarks || action == ImportBookmarks )
			return true; //not yet implemented

		if ((action == PrintBookmarks) && childCount())
			return true;

		return false;
	}

	/** Returns the XML code which represents the content of this folder. */
	QDomElement SubFolder::saveToXML( QDomDocument& doc ) {
		/**
		* Save all subitems (bookmarks and folders) to the XML file.
		* We get the XML code for the items by calling their own saveToXML implementations.
		*/
		QDomElement elem = doc.createElement("Folder");
		elem.setAttribute("caption", text(0));

		//append the XML nodes of all child items
		CItemBase* i = dynamic_cast<CItemBase*>(firstChild());
		while( i ) {
			if (i->parent() == this) {
				QDomElement newElem = i->saveToXML( doc );
				if (!newElem.isNull()) {
					elem.appendChild( newElem ); //append to this folder
				}
			}
			i = dynamic_cast<CItemBase*>( i->nextSibling() );
		}
		return elem;
	}

	/** Loads the content of this folder from the XML code passed as argument to this function. */
	void SubFolder::loadFromXML( QDomElement& elem ) {
		//get the caption and restore all child items!
		if (elem.hasAttribute("caption"))
			setText(0, elem.attribute("caption"));

		//restore all child items
		QDomElement child = elem.firstChild().toElement();
		CItemBase* oldItem = 0;
		while ( !child.isNull() && child.parentNode() == elem ) {
			CItemBase* i = 0;
			if (child.tagName() == "Folder") {
				i = new Bookmarks::SubFolder(this, child);
			}
			else if (child.tagName() == "Bookmark") {
				i = new CBookmarkItem(this, child);
			}
			i->init();
			if (oldItem)
				i->moveAfter(oldItem);
			oldItem = i;

			child = child.nextSibling().toElement();
		}
	}
};


/* --------------------------------------------------*/
/* ---------- new class: CBookmarkFolder ------------*/
/* --------------------------------------------------*/

CBookmarkFolder::CBookmarkFolder(CMainIndex* mainIndex, const Type type) : CTreeFolder(mainIndex, type, "*") {
	setSortingEnabled(false);
}

CBookmarkFolder::CBookmarkFolder(CFolderBase* parentItem, const Type type) : CTreeFolder(parentItem, type, "*") {
	setSortingEnabled(false);
}

CBookmarkFolder::~CBookmarkFolder() {}

void CBookmarkFolder::initTree() {
	addGroup(OldBookmarkFolder, "*");

	KStandardDirs stdDirs;
	const QString path = stdDirs.saveLocation("data", "bibletime/");
	if (!path.isEmpty()) {
		loadBookmarks(path + "bookmarks.xml");
	}
}

/** Reimplementation. */
const bool CBookmarkFolder::enableAction(const MenuAction action) {
	if ((action == NewFolder) || (action == ImportBookmarks))
		return true;

	if ((action == ExportBookmarks) && childCount())
		return true;

	if ((action == PrintBookmarks) && childCount())
		return true;

	return false;
}


void CBookmarkFolder::exportBookmarks() {
	QString fileName = KFileDialog::getSaveFileName(QString::null, i18n("*.btb | BibleTime bookmark files (*.btb)\n*.* | All files (*.*)"), 0, i18n("BibleTime - Export bookmarks"));
	if (!fileName.isEmpty()) {
		saveBookmarks( fileName, false ); //false means we don't want to overwrite the file without asking the user
	};
}


void CBookmarkFolder::importBookmarks() {
	QString fileName = KFileDialog::getOpenFileName(QString::null, i18n("*.btb | BibleTime bookmark files (*.btb)\n*.* | All files (*.*)"), 0, i18n("BibleTime - Import bookmarks"));
	if (!fileName.isEmpty()) {
		//we have to decide if we should load an old bookmark file from 1.2 or earlier or the new XML format of > 1.3
		if ( !loadBookmarks(fileName) ) { //if this failed try to load it as old bookmark file
			loadBookmarksFromXML( Bookmarks::OldBookmarkImport::oldBookmarksXML( fileName ) );
		};
	};
}

bool CBookmarkFolder::acceptDrop(const QMimeSource * src) const {
	//   qWarning("bool CBookmarkFolder::acceptDrop(const QMimeSource * src): return%ii", (CDragDropMgr::canDecode(src) && (CDragDropMgr::dndType(src) == CDragDropMgr::Item::Bookmark)));

	return CDragDropMgr::canDecode(src)
		   && (CDragDropMgr::dndType(src) == CDragDropMgr::Item::Bookmark);
}

void CBookmarkFolder::dropped(QDropEvent *e, QListViewItem* after) {
	if (acceptDrop(e)) {
		CDragDropMgr::ItemList dndItems = CDragDropMgr::decode(e);
		CDragDropMgr::ItemList::Iterator it;
		CItemBase* previousItem = dynamic_cast<CItemBase*>(after);

		for( it = dndItems.begin(); it != dndItems.end(); ++it) {
			CSwordModuleInfo* module = CPointers::backend()->findModuleByName(
										   (*it).bookmarkModule()
									   );

			CBookmarkItem* i = new CBookmarkItem(
								   this,
								   module,
								   (*it).bookmarkKey(),
								   (*it).bookmarkDescription()
							   );

			if (previousItem) {
				i->moveAfter( previousItem );
			}

			i->init();
			previousItem = i;
		};
	};
}

/** Saves the bookmarks in a file. */
const bool CBookmarkFolder::saveBookmarks( const QString& filename, const bool& forceOverwrite ) {
	QDomDocument doc("DOC");
	doc.appendChild( doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

	QDomElement content = doc.createElement("SwordBookmarks");
	content.setAttribute("syntaxVersion", CURRENT_SYNTAX_VERSION);
	doc.appendChild(content);

	//append the XML nodes of all child items
	CItemBase* i = dynamic_cast<CItemBase*>( firstChild() );
	while( i ) {
		if (i->parent() == this) { //only one level under this folder
			QDomElement newElem = i->saveToXML( doc ); // the cild creates it's own XML code
			if (!newElem.isNull()) {
				content.appendChild( newElem ); //append to this folder
			}
		}
		i = dynamic_cast<CItemBase*>( i->nextSibling() );
	}

	return CToolClass::savePlainFile(filename, doc.toString(), forceOverwrite, QTextStream::UnicodeUTF8);
}

const bool CBookmarkFolder::loadBookmarksFromXML( const QString& xml ) {
	QDomDocument doc;
	doc.setContent(xml);
	QDomElement document = doc.documentElement();
	if( document.tagName() != "SwordBookmarks" ) {
		qWarning("Not a BibleTime Bookmark XML file");
		return false;
	}

	CItemBase* oldItem = 0;
	//restore all child items
	QDomElement child = document.firstChild().toElement();
	while ( !child.isNull() && child.parentNode() == document) {
		CItemBase* i = 0;
		if (child.tagName() == "Folder") {
			i = new Bookmarks::SubFolder(this, child);
		}
		else if (child.tagName() == "Bookmark") {
			i = new CBookmarkItem(this, child);
		}
		if (!i) {
			break;
		}

		i->init();
		if (oldItem) {
			i->moveAfter(oldItem);
		}
		oldItem = i;

		if (!child.nextSibling().isNull()) {
			child = child.nextSibling().toElement();
		}
		else {
			break;
		}
	}
	return true;
}

/** Loads bookmarks from a file. */
const bool CBookmarkFolder::loadBookmarks( const QString& filename ) {
	QFile file(filename);
	if (!file.exists())
		return false;

	QString xml;
	if (file.open(IO_ReadOnly)) {
		QTextStream t;
		t.setEncoding(QTextStream::UnicodeUTF8); //set encoding before file is used for input!
		t.setDevice(&file);
		xml = t.read();
		file.close();
	}

	return loadBookmarksFromXML( xml );
}

/* NEW CLASS */

CGlossaryFolder::CGlossaryFolder(CMainIndex* mainIndex, const Type type, const QString& fromLanguage, const QString& toLanguage)
: CTreeFolder(mainIndex, type, fromLanguage) {
	m_fromLanguage = fromLanguage;
	m_toLanguage = toLanguage;
}

CGlossaryFolder::CGlossaryFolder(CFolderBase* item, const Type type, const QString& fromLanguage, const QString& toLanguage)
: CTreeFolder(item, type, fromLanguage) {
	m_fromLanguage = fromLanguage;
	m_toLanguage = toLanguage;
}

CGlossaryFolder::~CGlossaryFolder() {}

void CGlossaryFolder::initTree() {
	if (type() == Unknown)
		return;

	//get all modules by using the lexicon type
	ListCSwordModuleInfo allModules =CPointers::backend()->moduleList();
	ListCSwordModuleInfo usedModules;
	//   for (CSwordModuleInfo* m = allModules.first(); m; m = allModules.next()) {

	ListCSwordModuleInfo::iterator end_it = allModules.end();
	for (ListCSwordModuleInfo::iterator it(allModules.begin()); it != end_it; ++it) {
		if ((*it)->type() == CSwordModuleInfo::Lexicon) { //found a module, check if the type is correct (devotional etc.)
			if ((type() == GlossaryModuleFolder) && ((*it)->category() != CSwordModuleInfo::Glossary)) { //not a glossary
				continue;
			}
			//found a glossary
			//ToDo: this is ugly code
			if (language() == QString::fromLatin1("*")
					|| (language() != QString::fromLatin1("*")
						&& (*it)->config(CSwordModuleInfo::GlossaryFrom) == fromLanguage()
						&& (*it)->config(CSwordModuleInfo::GlossaryTo) == toLanguage()
					   )
			   ) { //right type and language!
				usedModules.append(*it);
			}
		}
	}

	//we have now all modules we want to have
	if (language() == QString::fromLatin1("*")) { //create subfolders for each language
		typedef std::pair<QString, QString> LanguagePair;
		typedef QValueList<LanguagePair> LanguagePairList;

		LanguagePairList usedLangs;
		//     for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
		ListCSwordModuleInfo::iterator end_it = usedModules.end();
		for (ListCSwordModuleInfo::iterator it(usedModules.begin()); it != end_it; ++it) {
			LanguagePair langPair(
				(*it)->config(CSwordModuleInfo::GlossaryFrom),
				(*it)->config(CSwordModuleInfo::GlossaryTo)
			);

			if (!usedLangs.contains(langPair)) {
				usedLangs.append(langPair);
			}
		}

		LanguagePairList::iterator lang_it;
		for (lang_it = usedLangs.begin(); lang_it != usedLangs.end(); ++lang_it) {
			addGroup(type(), (*lang_it).first, (*lang_it).second);
		}
	}
	else if (usedModules.count() > 0) { //create subitems with the given type and languages
		//     for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
		ListCSwordModuleInfo::iterator end_it = usedModules.end();
		for (ListCSwordModuleInfo::iterator it(usedModules.begin()); it != end_it; ++it) {
			addModule(*it);
		}
	}

	sortChildItems(0,true);
}

void CGlossaryFolder::init() {
	if (language() == "*") {
		setText(0,i18n("Glossaries"));
	}
	else {
		const CLanguageMgr::Language* const fromLang = CPointers::languageMgr()->languageForAbbrev( m_fromLanguage );
		const CLanguageMgr::Language* const toLang = CPointers::languageMgr()->languageForAbbrev( m_toLanguage );

		QString fromLangString  = fromLang->translatedName();
		QString toLangString    = toLang->translatedName();

		if (fromLangString.isEmpty()) { //use abbrev!
			fromLangString = m_fromLanguage;
		};
		if (toLangString.isEmpty()) { //use abbrev!
			toLangString = m_toLanguage;
		};

		setText(0, fromLangString + " - " + toLangString );
	}
	initTree();
	update();
}

/** Returns the language this glossary folder maps from. */
const QString& CGlossaryFolder::fromLanguage() const {
	return m_fromLanguage;
}

/** Returns the language this glossary folder maps to. */
const QString& CGlossaryFolder::toLanguage() const {
	return m_toLanguage;
}

void CGlossaryFolder::addGroup(const Type type, const QString& fromLanguage, const QString& toLanguage) {
	CTreeFolder* i = new CGlossaryFolder(this, type, fromLanguage, toLanguage);
	i->init();
	if (!i->childCount()) {
		delete i;
	}
}

