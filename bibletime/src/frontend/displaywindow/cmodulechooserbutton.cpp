/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cmodulechooserbutton.h"
#include "cmodulechooserbutton.moc"

#include "cmodulechooserbar.h"

#include "backend/managers/cswordbackend.h"

#include "frontend/cbtconfig.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"

//Qt includes
#include <QString>
#include <QToolTip>
#include <QHash>
#include <QToolButton>
#include <QMenu>
#include <QtDebug>

CModuleChooserButton::CModuleChooserButton(CSwordModuleInfo* useModule,CSwordModuleInfo::ModuleType type, const int id, CModuleChooserBar *parent)
	: QToolButton(parent),
	m_id(id), m_popup(0), m_moduleChooserBar(parent)
{
	m_moduleType = type;
	m_module = useModule;
	m_hasModule = (m_module) ? true : false;

	setIcon( util::filesystem::DirectoryUtil::getIcon(iconName()) );
	setPopupMode(QToolButton::InstantPopup);

	populateMenu();
}

CModuleChooserButton::~CModuleChooserButton() {
	qDeleteAll(m_submenus);
	m_submenus.clear();
	delete m_popup; //not necessary, because has "this" as parent?
}

/** Returns the icon used for the current status. */
const QString CModuleChooserButton::iconName() {
	switch (m_moduleType) {
		case CSwordModuleInfo::Bible:
			return (m_hasModule) ? CResMgr::modules::bible::icon_unlocked : CResMgr::modules::bible::icon_add;
		case CSwordModuleInfo::Commentary:
			return (m_hasModule) ? CResMgr::modules::commentary::icon_unlocked : CResMgr::modules::commentary::icon_add;
		case CSwordModuleInfo::Lexicon:
			return m_hasModule ? CResMgr::modules::lexicon::icon_unlocked : CResMgr::modules::lexicon::icon_add;
		case CSwordModuleInfo::GenericBook:
			return m_hasModule ? CResMgr::modules::book::icon_unlocked : CResMgr::modules::book::icon_add;
		default: //return as default the bible icon
			return CResMgr::modules::bible::icon_unlocked;
	}
}

CSwordModuleInfo* CModuleChooserButton::module() {
	foreach (QMenu* popup, m_submenus) {
		foreach (QAction* action, popup->actions()) {
			if ( action->isChecked() ) { //idAt -> , isItemChecked -> QAction::isChecked
				QString mod = action->text(); //popup->text(popup->idAt(i)); //text -> 
				mod.remove(QChar('&')); //remove hotkey indicators
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
	
	QListIterator<QMenu*> it(m_submenus);
	while (it.hasNext()) {
		QMenu* popup = it.next();
		for (unsigned int i = 0; i < popup->actions().count(); i++) {
			popup->actions().at(i)->setChecked(false);
		}
		action->setChecked(true);
	}

	m_noneAction->setChecked(false); //uncheck the "none" item
	if (action->text().remove(QChar('&')) == tr("NONE")) { // note: this is for m_popup, the toplevel!
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
		
		setIcon( util::filesystem::DirectoryUtil::getIcon(iconName()) );
		emit sigChanged();

		if (m_module) {
			setToolTip( tr("Select a work") + " [" + m_module->name() + "]" );
		}
		else {
			setToolTip( tr("Select an additional work") );
		}
	}
}

/** No descriptions */
void CModuleChooserButton::populateMenu() {
	qDeleteAll(m_submenus);
	m_submenus.clear();
	delete m_popup;
	m_popup = new QMenu(this);
	
	if (m_module) {
		this->setToolTip( tr("Select a work") + " [" + m_module->name() + "]" );
	}
	else {
		this->setToolTip( tr("Select an additional work") );
	}

	m_noneAction = m_popup->addAction(tr("NONE"));
	m_noneAction->setCheckable(true);
	if (!m_module) m_noneAction->setChecked(true);

	m_popup->addSeparator();
	connect(m_popup, SIGNAL(triggered(QAction*)), this, SLOT(moduleChosen(QAction*)));
	setMenu(m_popup);


	// ******* Add languages and modules ********

	// Filters: add only non-hidden and right type
	BTModuleTreeItem::HiddenOff hiddenFilter;
	TypeFilter typeFilter(m_moduleType);
	QList<BTModuleTreeItem::Filter*> filters;
	if (!CBTConfig::get(CBTConfig::bookshelfShowHidden)) {
		filters.append(&hiddenFilter);
	}
	filters.append(&typeFilter);
	BTModuleTreeItem root(filters, BTModuleTreeItem::LangMod);
	// add all items recursively
	addItemToMenu(&root, m_popup);

}

void CModuleChooserButton::addItemToMenu(BTModuleTreeItem* item, QMenu* menu)
{
	foreach (BTModuleTreeItem* i, item->children()) {

		if (i->type() == BTModuleTreeItem::Language) {
			// argument menu was m_popup, create and add a new lang menu to it
			QMenu* langMenu = new QMenu(i->text(), this);
			menu->addMenu(langMenu);
			m_submenus.append(langMenu);
			connect(langMenu, SIGNAL(triggered(QAction*)), this, SLOT(moduleChosen(QAction*)));
			// add the module items to the lang menu
			addItemToMenu(i, langMenu);
		}
		else {
			// item must be module, create and add it to the lang menu
			QString name(i->text());
			name.append(" ").append(i->moduleInfo()->isLocked() ? tr("[locked]") : QString::null);
			QAction* modItem = new QAction(name, menu);
			modItem->setCheckable(true);
			if ( m_module && i->text() == m_module->name()) modItem->setChecked(true);
			menu->addAction(modItem);
		}
	}
}

void CModuleChooserButton::updateMenuItems() {
	QString moduleName;
	CSwordModuleInfo* module = 0;
	ListCSwordModuleInfo chosenModules = m_moduleChooserBar->getModuleList();

	//for ( QMenu* popup = m_submenus.first(); popup; popup = m_submenus.next() ) {
	QListIterator<QMenu*> it(m_submenus);
	while (it.hasNext()) {
		QMenu* popup = it.next();
		for (unsigned int i = 0; i < popup->actions().count(); i++) {
			moduleName = popup->actions().at(i)->text();
			moduleName.remove(QChar('&')); //remove Hotkey indicator
			module = backend()->findModuleByName( moduleName.left(moduleName.lastIndexOf(" ")) );

 			//Q_ASSERT(module);
 			if (!module) qWarning("Can't find module with name %s", moduleName.toLatin1().data());

			bool alreadyChosen = chosenModules.contains( module );
			if (m_module) {
				alreadyChosen = alreadyChosen && (m_module->name() != moduleName);
			}
			//grey it out, it was chosen already
			popup->actions().at(i)->setEnabled(!alreadyChosen);
		}
	}
}
