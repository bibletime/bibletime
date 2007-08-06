/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmodulechooserbutton.h"
#include "cmodulechooserbutton.moc"

#include "cmodulechooserbar.h"

#include "backend/managers/cswordbackend.h"

#include "util/cresmgr.h"

//Qt includes
#include <QString>
#include <QToolTip>
#include <QHash>
#include <QToolButton>

//for better debug
#include <QtDebug>

//KDE includes
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kmenu.h>

//TODO: test all (context)menu situations

CModuleChooserButton::CModuleChooserButton(CSwordModuleInfo* useModule,CSwordModuleInfo::ModuleType type, const int id, CModuleChooserBar *parent)
	: QToolButton(parent),
	m_id(id), m_popup(0), m_moduleChooserBar(parent)
{
	qDebug("CModuleChooserButton::CModuleChooserButton");
	m_moduleType = type;
	m_module = useModule;
	if (!m_module) {
		m_hasModule = false;
	}
	else {
		m_hasModule = true;
	}

	setIcon( QIcon(iconName()) );
	//setPopupDelay(1); //see comment in cbuttons.cpp
	setPopupMode(QToolButton::InstantPopup);

	populateMenu();
	qDebug("CModuleChooserButton::CModuleChooserButton end");
}

CModuleChooserButton::~CModuleChooserButton() {
	//m_submenus.setAutoDelete(true); //delete all submenus
	qDeleteAll(m_submenus);
	m_submenus.clear();

	delete m_popup; //not necessary, because has "this" as parent?
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
	//for ( KMenu* popup = m_submenus.first(); popup; popup = m_submenus.next() ) {
	QListIterator<KMenu*> it(m_submenus);
	while (it.hasNext()) {
		KMenu* popup = it.next();
		for (unsigned int i = 0; i < popup->actions().count(); i++) { //qt4:added "actions"
			if ( m_popup->actions().at(i)->isChecked() ) { //idAt -> , isItemChecked -> QAction::isChecked
				QString mod = popup->actions().at(i)->text(); //popup->text(popup->idAt(i)); //text -> 
				return backend()->findModuleByName( mod.left(mod.indexOf(" ")) );
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
void CModuleChooserButton::moduleChosen( QAction* action ) {
	
	//for ( KMenu* popup = m_submenus.first(); popup; popup = m_submenus.next() ) {
	QListIterator<KMenu*> it(m_submenus);
	while (it.hasNext()) {
		KMenu* popup = it.next();
		for (unsigned int i = 0; i < popup->actions().count(); i++) {
			popup->actions().at(i)->setChecked(false);//popup->setItemChecked(popup->idAt(i),false);
		}
		action->setChecked(true); //setItemChecked(ID, true);
	}

	m_noneAction->setChecked(false);//m_popup->setItemChecked(m_noneId, false); //uncheck the "none" item

	if (action->text() == i18n("NONE")) { // note: this is for m_popup, the toplevel!
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
		setIcon( QIcon(iconName()) ); //TODO
		emit sigChanged();

		setText( i18n("Select a work") );
		m_titleAction->setText(i18n("Select a work"));//m_popup->changeTitle(m_titleId, i18n("Select a work"));

		setToolTip(QString::null); //TODO: how to remove tooltip? Does this leave an empty tooltip box?
		if (module()) {setToolTip(module()->name());}
	}
}

/** No descriptions */
void CModuleChooserButton::populateMenu() {
	qDebug("CModuleChooserButton::populateMenu");
	
	qDeleteAll(m_submenus);
	m_submenus.clear();

	delete m_popup;

	//create a new, empty popup
	m_popup = new KMenu(this);
	
	if (m_module) {
		m_titleAction = m_popup->addTitle( i18n("Select a work") );
	}
	else {
		m_titleAction = m_popup->addTitle( i18n("Select an additional work") );
	}

	m_noneAction = m_popup->addAction(i18n("NONE"));
	if ( !m_module ) {
		m_noneAction->setChecked(true);
	}

	m_popup->addSeparator();
	connect(m_popup, SIGNAL(triggered(QAction*)), this, SLOT(moduleChosen(QAction*)));
	setMenu(m_popup);


	//*********************** Add languages and modules*******************************

	QStringList languages;
	QHash<QString, KMenu*> langdict;

	//the modules list contains only the modules we can use, i.e. same type and same features
	ListCSwordModuleInfo modules;
	ListCSwordModuleInfo allMods = backend()->moduleList();

	// add all modules of the right type into the module list
	foreach (CSwordModuleInfo* moduleInfo, allMods) {
		if (moduleInfo->type() != m_moduleType) {
			continue;
		}
		modules.append( moduleInfo );
	};

	// iterate through all found modules of the type we support
	// adding all different languages into languages list
	// and adding a new {language name/KMenu} pair into the dict
	foreach (CSwordModuleInfo* moduleInfo, modules) {
		QString lang = moduleInfo->language()->translatedName();

		if (lang.isEmpty()) {
			//lang = QString::fromLatin1("xx"); //unknown language -- do not use English as default!!
			lang = moduleInfo->language()->abbrev();
			if (lang.isEmpty()) {
				lang = "xx";
			}
		}

		if (!languages.contains(lang) ) { //this lang was not yet added
			languages += lang;

			KMenu* menu = new KMenu;
			langdict[lang] = menu;
			m_submenus.append(menu);
			connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(moduleChosen(QAction*)));
		}
	}

	// Check the appropriate entry
	foreach (CSwordModuleInfo* moduleInfo, modules) {
		QString lang = moduleInfo->language()->translatedName();
		if (lang.isEmpty()) {
			lang = moduleInfo->language()->abbrev();
			if (lang.isEmpty()) {
				lang = "xx";
			}
		}

		QString name(moduleInfo->name());
		name.append(" ").append(moduleInfo->isLocked() ? i18n("[locked]") : QString::null);
		
		Q_ASSERT(langdict[lang]);
		QAction* id = langdict[lang]->addAction( name );
		
		if ( m_module && moduleInfo->name() == m_module->name()) {
			id->setChecked(true);
		}
	}


	languages.sort();

	foreach ( QString langName, languages ) {
		langdict[langName]->addTitle(langName);
		m_popup->addMenu(langdict[langName]); //insertItem ->  QMenu::addMenu(QMenu)
	}

	if (module()) { setToolTip(module()->name());}
	else { setToolTip(i18n("No work selected"));}
	qDebug("CModuleChooserButton::populateMenu end");
}


/*!
    \fn CModuleChooserButton::updateMenuItems()
 */
void CModuleChooserButton::updateMenuItems() {
	QString moduleName;
	CSwordModuleInfo* module = 0;
	ListCSwordModuleInfo chosenModules = m_moduleChooserBar->getModuleList();

	//for ( KMenu* popup = m_submenus.first(); popup; popup = m_submenus.next() ) {
	QListIterator<KMenu*> it(m_submenus);
	while (it.hasNext()) {
		KMenu* popup = it.next();
		for (unsigned int i = 0; i < popup->actions().count(); i++) {
			moduleName = popup->actions().at(i)->text();
			qDebug() << "moduleName: " << moduleName << " , " << moduleName.left(moduleName.lastIndexOf(" "));
			module = backend()->findModuleByName( moduleName.left(moduleName.lastIndexOf(" ")) );

 			//Q_ASSERT(module);
 			if (!module) {
 				qWarning("Can't find module with name %s", moduleName.toLatin1().data());
 			}

			bool alreadyChosen = chosenModules.contains( module );
			if (m_module) {
				alreadyChosen = alreadyChosen && (m_module->name() != moduleName);
			}

			popup->actions().at(i)->setEnabled(!alreadyChosen);// setItemEnabled(popup->idAt(i), !alreadyChosen); //grey it out, it was chosen already
		}
	}
}
