//
// C++ Implementation: cindexbookmarkitem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

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
