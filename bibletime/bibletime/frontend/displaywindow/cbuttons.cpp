/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//HibleTime includes
#include "cbuttons.h"

#include "util/cresmgr.h"
#include "util/cpointers.h"

//Qt includes
#include <QString>
#include <QToolTip>
#include <QToolButton>
#include <QHash>

//KDE includes
#include <klocale.h>
#include <kmenu.h>



/************************************************
 *********** CDisplaySettingsButton *************
 ************************************************/

CDisplaySettingsButton::CDisplaySettingsButton(CSwordBackend::DisplayOptions *displaySettings, CSwordBackend::FilterOptions *moduleSettings, const ListCSwordModuleInfo& useModules,QWidget *parent )
: QToolButton(parent) {
	//  qWarning("CDisplaySettingsButton::CDisplaySettingsButton");
	QToolButton::setIcon(QIcon(CResMgr::displaywindows::displaySettings::icon));
	
	m_displaySettings = displaySettings;
	m_moduleSettings = moduleSettings;
	m_modules = useModules;

	m_popup = new KMenu(this);
	setMenu(m_popup);
	//setPopupDelay(0001); //Fix, O only opens menu on mouse release or move
	setPopupMode(QToolButton::InstantPopup);

	connect(m_popup, SIGNAL(triggered(QAction*)), this, SLOT(optionToggled(QAction*)));
	populateMenu();
}

void CDisplaySettingsButton::reset(const ListCSwordModuleInfo& useModules) {
	m_modules = useModules;
	populateMenu();
	//disable the settings button if no options are available
	if (!populateMenu()) {
		setEnabled(false);
		setToolTip(i18n("Display settings: No options available"));
	}
	else {
		setEnabled(true);
		setToolTip(i18n("Display settings"));
	}
}


void CDisplaySettingsButton::optionToggled(QAction* action) {
	//m_popup->setItemChecked( ID, !(m_popup->isItemChecked(ID)));
	action->toggle();
	if ( action->text().isEmpty() )
		m_dict[action->text()] =  action->isChecked();
	emit sigChanged();
}

/** No descriptions */
int CDisplaySettingsButton::populateMenu() {
	int ret = 0;

	m_popup->clear();
	m_popup->setTitle(i18n("Display options"));
	
	ret += addMenuEntry(i18n("Use linebreaks after each verse"), &m_displaySettings->lineBreaks, (m_modules.first()->type() == CSwordModuleInfo::Bible));

	//show the verse numbers option only for bible modules
	ret += addMenuEntry(i18n("Show versenumbers"), &m_displaySettings->verseNumbers, (m_modules.first()->type() == CSwordModuleInfo::Bible));

	ret += addMenuEntry(i18n("Show headings"), &m_moduleSettings->headings,
						isOptionAvailable(CSwordModuleInfo::headings));
	
	ret += addMenuEntry(i18n("Highlight words of Jesus"), &m_moduleSettings->redLetterWords,
						isOptionAvailable(CSwordModuleInfo::redLetterWords ));

	ret += addMenuEntry(i18n("Show Hebrew vowel points"), &m_moduleSettings->hebrewPoints,
						isOptionAvailable(CSwordModuleInfo::hebrewPoints ));
	ret += addMenuEntry(i18n("Show Hebrew cantillation marks"), &m_moduleSettings->hebrewCantillation,
						isOptionAvailable(CSwordModuleInfo::hebrewCantillation ));
	ret += addMenuEntry(i18n("Show Greek accents"), &m_moduleSettings->greekAccents,
						isOptionAvailable(CSwordModuleInfo::greekAccents ));

	ret += addMenuEntry(i18n("Use alternative textual variant"), &m_moduleSettings->textualVariants,
						isOptionAvailable(CSwordModuleInfo::textualVariants ));
	ret += addMenuEntry(i18n("Show scripture cross-references"), &m_moduleSettings->scriptureReferences,
						isOptionAvailable(CSwordModuleInfo::scriptureReferences ));
	ret += addMenuEntry(i18n("Show morph segmentation"), &m_moduleSettings->morphSegmentation,
						isOptionAvailable(CSwordModuleInfo::morphSegmentation ));

	return ret;
}

/** No descriptions */
int CDisplaySettingsButton::addMenuEntry( const QString name, const int* option, const bool available) {
	int ret = 0;

	if (available) {
		m_dict.insert( name, *option );
		//m_popup->setItemChecked(m_popup->insertItem( name ), *option );
		m_popup->addAction(name)->setChecked(*option);
		ret = 1;
	}

	return ret;
}

bool CDisplaySettingsButton::isOptionAvailable( const CSwordModuleInfo::FilterTypes option ) {
	bool ret = false;
	//  for (m_modules.first(); m_modules.current() && !ret; m_modules.next()) {
	ListCSwordModuleInfo::iterator end_it = m_modules.end();
	for (ListCSwordModuleInfo::iterator it(m_modules.begin()); it != end_it; ++it) {
		ret = ret || (*it)->has(option);
	}

	return ret;
}

/** Returns the number of usable menu items in the setttings menu. */
const int CDisplaySettingsButton::menuItemCount() {
	return m_popup->actions().count();
}

/** Sets the item at position pos to the satet given as 2nd paramter. */
void CDisplaySettingsButton::setItemStatus( const int index, const bool checked ) {
	//const int ID = m_popup->idAt(index);
	QAction* action = m_popup->actions().at(index);
	action->setChecked(checked); //m_popup->setItemChecked(ID, checked);
	const QString text = action->text();
	if (m_dict[text]) {
		m_dict[text] = checked;
	}
}

/** Returns the status of the item at position "index" */
const bool CDisplaySettingsButton::itemStatus( const int index ) {
	return m_popup->actions().at(index)->isChecked(); //m_popup->isItemChecked( m_popup->idAt(index) );
}

/** Sets the status to changed. The signal changed will be emitted. */
void CDisplaySettingsButton::setChanged() {
	emit sigChanged();
}
