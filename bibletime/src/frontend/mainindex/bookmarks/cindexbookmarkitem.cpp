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

//BibleTime
#include "cindexbookmarkitem.h"
#include "cindexitembase.h"
#include "cindexfolderbase.h"
#include "cbookmarkindex.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "backend/managers/clanguagemgr.h"

#include "frontend/cbtconfig.h"
#include "frontend/cinputdialog.h"

#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

//Qt
#include <QString>
#include <QtXml/qdom.h>

#include <boost/scoped_ptr.hpp>

//KDE


CIndexBookmarkItem::CIndexBookmarkItem(CIndexFolderBase* parentItem, CSwordModuleInfo* module, const QString& key, const QString& description)
	: CIndexItemBase(parentItem),
	m_description(description),
	m_moduleName(module ? module->name() : QString::null)
{
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
	update();
}

CIndexBookmarkItem::CIndexBookmarkItem(CIndexFolderBase* parentItem, QDomElement& xml )
	: CIndexItemBase(parentItem),
	m_key(QString::null),
	m_description(QString::null),
	m_moduleName(QString::null)
{
	m_startupXML = xml;
}

CIndexBookmarkItem::~CIndexBookmarkItem() {}

/** No descriptions */
void CIndexBookmarkItem::update() {
	//setMultiLinesEnabled(true); //TODO: ???
	setIcon(0, util::filesystem::DirectoryUtil::getIcon(CResMgr::mainIndex::bookmark::icon));

	const QString title = QString::fromLatin1("%1 (%2)").arg(key()).arg(module() ? module()->name() : QObject::tr("unknown"));
	setText(0, title);
	setToolTip(0, toolTip());
}

void CIndexBookmarkItem::init() {
	if (!m_startupXML.isNull()) { //we have some XML code to parse
		loadFromXML(m_startupXML);
	}

	update();
	//setDropEnabled(false);
	//setDragEnabled(false);

	//  if (!module())
	//    setSelectable(false);
}

/** Reimplementation. */
const QString CIndexBookmarkItem::toolTip() {
	if (!module()) {
		return QString::null;
	}

	CSwordBackend::FilterOptions filterOptions = CBTConfig::getFilterOptionDefaults();
	filterOptions.footnotes = false;
	filterOptions.scriptureReferences = false;
	CPointers::backend()->setFilterOptions(filterOptions);

	QString ret;
	boost::scoped_ptr<CSwordKey> k( CSwordKey::createInstance(module()) );
	k->key(this->key());

	const CLanguageMgr::Language* lang = module()->language();
	CBTConfig::FontSettingsPair fontPair = CBTConfig::get
											   (lang);

	Q_ASSERT(k.get());
	if (fontPair.first) { //use a special font
		//qWarning("using a font, %s", fontPair.second.family().toLatin1());
		//ret = QString::fromLatin1("<b>%1 (%2)</b><br/><small>%3</small><hr><font face=\"%4\" size=\"4\">%5</font>")
		ret = QString::fromLatin1("<b>%1 (%2)</b><hr>%3")
			  .arg(key())
			  .arg(module()->name())
			  .arg(description())
			  //.arg(fontPair.second.family())
			  //.arg(k->renderedText())
			;
	}
	else {
		ret = QString::fromLatin1("<b>%1 (%2)</b><hr>%3")
			  .arg(key())
			  .arg(module()->name())
			  .arg(description())
			  //.arg(k->renderedText())
			;
	}

	return ret;
}

/** Returns the used module. */
CSwordModuleInfo* const CIndexBookmarkItem::module() {
	CSwordModuleInfo* const m = CPointers::backend()->findModuleByName(m_moduleName);
	Q_ASSERT(m);
	return m;
}

/** Returns the used key. */
const QString CIndexBookmarkItem::key() {
	const QString englishKeyName = englishKey();
	if (!module()) {
		return englishKeyName;
	}

	QString returnKeyName = englishKeyName;
	if ((module()->type() == CSwordModuleInfo::Bible) || (module()->type() == CSwordModuleInfo::Commentary)) {
		CSwordVerseKey vk(0);
		vk = englishKeyName;
		vk.setLocale(CPointers::backend()->booknameLanguage().toLatin1() );

		returnKeyName = vk.key(); //the returned key is always in the currently set bookname language
	}

	return returnKeyName;
}

/** Returns the used description. */
const QString& CIndexBookmarkItem::description() {
	return m_description;
}

/** No descriptions */
const bool CIndexBookmarkItem::isMovable() {
	return true;
}

/** Reimplementation to handle  the menu entries of the main index. */
const bool CIndexBookmarkItem::enableAction(const MenuAction action) {
	if (action == ChangeBookmark || (module() && (action == PrintBookmarks)) || action == DeleteEntries)
		return true;

	return false;
}

/** Changes this bookmark. */
void CIndexBookmarkItem::rename() {
	bool ok  = false;
	const QString newDescription = CInputDialog::getText(QObject::tr("Change description ..."), QObject::tr("Enter a new description for the chosen bookmark."), description(), &ok, treeWidget());

	if (ok) {
		m_description = newDescription;
		update();
	}
}

/** Reimplementation of CItemBase::saveToXML. */
QDomElement CIndexBookmarkItem::saveToXML( QDomDocument& doc ) {
	QDomElement elem = doc.createElement("Bookmark");

	elem.setAttribute("key", englishKey());
	elem.setAttribute("description", description());
	elem.setAttribute("modulename", m_moduleName);
	elem.setAttribute("moduledescription", module() ? module()->config(CSwordModuleInfo::Description) : QString::null);

	return elem;
}

void CIndexBookmarkItem::loadFromXML( QDomElement& element ) {
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
const QString& CIndexBookmarkItem::englishKey() const {
	return m_key;
}

/** Reimplementation. Returns false everytime because a bookmarks  has not possible drops. */
bool CIndexBookmarkItem::acceptDrop(const QMimeSource* /*src*/) const {
	return false;
}

void CIndexBookmarkItem::setDescription(QString text)
{
	m_description = text;
	setToolTip(0, toolTip());
}
