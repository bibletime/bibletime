/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmodulechooserbutton.h"
#include "cmodulechooserbar.h"

#include "backend/cswordbackend.h"

#include "util/cresmgr.h"

//Qt includes
#include <qstring.h>
#include <qtooltip.h>
#include <qdict.h>
#include <qvaluelist.h>

//KDE includes
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

CModuleChooserButton::CModuleChooserButton(CSwordModuleInfo* useModule,CSwordModuleInfo::ModuleType type, const int id, CModuleChooserBar *parent, const char *name )
: KToolBarButton(iconName(), id, parent, name),
m_id(id), m_popup(0), m_moduleChooserBar(parent) {
	m_moduleType = type;
	m_module = useModule;
	if (!m_module) {
		m_hasModule = false;
	}
	else {
		m_hasModule = true;
	}

	setIcon( iconName() );
	setPopupDelay(1);

	populateMenu();
}

CModuleChooserButton::~CModuleChooserButton() {
	m_submenus.setAutoDelete(true); //delete all submenus
	m_submenus.clear();

	delete m_popup;
}

/** Returns the icon used for the current status. */
const QString CModuleChooserButton::iconName() {
	switch (m_moduleType) {
		case CSwordModuleInfo::Bible:
		if (m_hasModule)
			return CResMgr::modules::bible::icon_unlocked;
		else
			return CResMgr::modules::bible::icon_add;
		case CSwordModuleInfo::Commentary:
		if (m_hasModule)
			return CResMgr::modules::commentary::icon_unlocked;
		else
			return CResMgr::modules::commentary::icon_add;
		case CSwordModuleInfo::Lexicon:
		if (m_hasModule)
			return CResMgr::modules::lexicon::icon_unlocked;
		else
			return CResMgr::modules::lexicon::icon_add;
		case CSwordModuleInfo::GenericBook:
		if (m_hasModule)
			return CResMgr::modules::book::icon_unlocked;
		else
			return CResMgr::modules::book::icon_add;
		default: //return as default the bible icon
		return CResMgr::modules::bible::icon_unlocked;
	}
}

CSwordModuleInfo* CModuleChooserButton::module() {
	for ( KPopupMenu* popup = m_submenus.first(); popup; popup = m_submenus.next() ) {
		for (unsigned int i = 0; i < popup->count(); i++) {
			if ( m_popup->isItemChecked(popup->idAt(i)) ) {
				QString mod = popup->text(popup->idAt(i));
				return backend()->findModuleByName( mod.left(mod.find(" ")) );
			}
		}

	}
	return 0; //"none" selected
}

/** Returns the id used for this button. */
int CModuleChooserButton::getId() const {
	return m_id;
}

/** Is called after a module was selected in the popup */
void CModuleChooserButton::moduleChosen( int ID ) {
	for ( KPopupMenu* popup = m_submenus.first(); popup; popup = m_submenus.next() ) {
		for (unsigned int i = 0; i < popup->count(); i++) {
			popup->setItemChecked(popup->idAt(i),false);
		}
		popup->setItemChecked(ID, true);
	}

	m_popup->setItemChecked(m_noneId, false); //uncheck the "none" item

	if (m_popup->text(ID) == i18n("NONE")) { // note: this is for m_popup, the toplevel!
		if (m_hasModule) {
			emit sigRemoveButton(m_id);
			return;
		}
	}
	else {
		if (!m_hasModule) {
			emit sigAddButton();
		}

		m_hasModule = true;
		m_module = module();
		setIcon( iconName() );
		emit sigChanged();

		setText( i18n("Select a work") );
		m_popup->changeTitle(m_titleId, i18n("Select a work"));

		QToolTip::remove
			(this);
		if (module()) {
			QToolTip::add
				(this, module()->name());
		}
	}
}

/** No descriptions */
void CModuleChooserButton::populateMenu() {
	m_submenus.setAutoDelete(true); //delete all submenus
	m_submenus.clear();

	delete m_popup;

	//create a new, empty popup
	m_popup = new KPopupMenu(this);

	if (m_module) {
		m_titleId = m_popup->insertTitle( i18n("Select a work") );
	}
	else {
		m_titleId = m_popup->insertTitle( i18n("Select an additional work") );
	}

	m_popup->setCheckable(true);

	m_noneId = m_popup->insertItem(i18n("NONE"));
	if ( !m_module ) {
		m_popup->setItemChecked(m_noneId, true);
	}

	m_popup->insertSeparator();
	connect(m_popup, SIGNAL(activated(int)), this, SLOT(moduleChosen(int)));
	setPopup(m_popup, true);

	QStringList languages;
	QDict<KPopupMenu> langdict;

	//the modules list contains only the modules we can use, i.e. same type and same features
	ListCSwordModuleInfo modules;
	ListCSwordModuleInfo allMods = backend()->moduleList();

	//   for (allMods.first(); allMods.current(); allMods.next()) {
	ListCSwordModuleInfo::iterator end_it = allMods.end();
	for (ListCSwordModuleInfo::iterator it(allMods.begin()); it != end_it; ++it) {
		if ((*it)->type() != m_moduleType) {
			continue;
		}

		modules.append( *it );
	};

	//iterate through all found modules of the type we support
	//  for (modules.first(); modules.current(); modules.next()) {
	/*ListCSwordModuleInfo::iterator*/
	end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
		QString lang = (*it)->language()->translatedName();

		if (lang.isEmpty()) {
			//lang = QString::fromLatin1("xx"); //unknown language -- do not use English as default!!
			lang = (*it)->language()->abbrev();
			if (lang.isEmpty()) {
				lang = "xx";
			}
		}

		if (languages.find( lang ) == languages.end() ) { //this lang was not yet added
			languages += lang;

			KPopupMenu* menu = new KPopupMenu;
			langdict.insert(lang, menu );
			m_submenus.append(menu);
			connect(menu, SIGNAL(activated(int)), this, SLOT(moduleChosen(int)));
		}
	}


	//Check the appropriate entry
	//  for (modules.first(); modules.current(); modules.next()) {
	/*ListCSwordModuleInfo::iterator*/ end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
		QString lang = (*it)->language()->translatedName();

		if (lang.isEmpty()) {
			lang = (*it)->language()->abbrev();
			if (lang.isEmpty()) {
				lang = "xx";
			}
		}

		QString name((*it)->name());
		name.append(" ").append((*it)->isLocked() ? i18n("[locked]") : QString::null);

		const int id = langdict[lang]->insertItem( name );
		if ( m_module && (*it)->name() == m_module->name()) {
			langdict[lang]->setItemChecked(id, true);
		}
	}

	languages.sort();
	for ( QStringList::Iterator it = languages.begin(); it != languages.end(); ++it ) {
		m_popup->insertItem( *it, langdict[*it]);
	}

	if (module()) {
		QToolTip::add
			(this, module()->name());
	}
	else {
		QToolTip::add
			(this, i18n("No work selected"));
	}
}


/*!
    \fn CModuleChooserButton::updateMenuItems()
 */
void CModuleChooserButton::updateMenuItems() {
	QString moduleName;
	CSwordModuleInfo* module = 0;
	ListCSwordModuleInfo chosenModules = m_moduleChooserBar->getModuleList();

	for ( KPopupMenu* popup = m_submenus.first(); popup; popup = m_submenus.next() ) {

		for (unsigned int i = 0; i < popup->count(); i++) {
			moduleName = popup->text(popup->idAt(i));
			module = backend()->findModuleByName( moduleName.left(moduleName.findRev(" ")) );

 			Q_ASSERT(module);
// 			if (!module) {
// 				qWarning("Can't find module with name %s", moduleName.latin1());
// 			}

			bool alreadyChosen = chosenModules.contains( module );
			if (m_module) {
				alreadyChosen = alreadyChosen && (m_module->name() != moduleName);
			}

			popup->setItemEnabled(popup->idAt(i), !alreadyChosen); //grey it out, it was chosen already
		}
	}
}
