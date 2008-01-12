/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "cbuttons.h"

#include "util/cresmgr.h"
#include "util/cpointers.h"
#include "util/directoryutil.h"

//Qt includes
#include <QString>
#include <QToolTip>
#include <QToolButton>
#include <QHash>
#include <QMenu>

//KDE includes




CDisplaySettingsButton::CDisplaySettingsButton(CSwordBackend::DisplayOptions *displaySettings, CSwordBackend::FilterOptions *moduleSettings, const ListCSwordModuleInfo& useModules,QWidget *parent )
: QToolButton(parent) {
	//  qWarning("CDisplaySettingsButton::CDisplaySettingsButton");
	QToolButton::setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::displaywindows::displaySettings::icon));
	
	m_displaySettings = displaySettings;
	m_moduleSettings = moduleSettings;
	m_modules = useModules;

	m_popup = new QMenu(this);
	setMenu(m_popup);
	setPopupMode(QToolButton::InstantPopup);
	setToolTip(tr("Display options"));

	connect(m_popup, SIGNAL(triggered(QAction*)), this, SLOT(optionToggled(QAction*)));
	populateMenu();
}

void CDisplaySettingsButton::reset(const ListCSwordModuleInfo& useModules) {
	m_modules = useModules;
	populateMenu();
	//disable the settings button if no options are available
	if (!populateMenu()) {
		setEnabled(false);
		setToolTip(tr("Display settings: No options available"));
	}
	else {
		setEnabled(true);
		setToolTip(tr("Display settings"));
	}
}


void CDisplaySettingsButton::optionToggled(QAction* action) {
	qDebug("display settings option toggled");
	//Take each Action and set the corresponding setting.
	//Using QAction (QObject) property and OptionType enum is a dirty way to do this.
	//See populateMenu().
	foreach (QAction* act, m_popup->actions()) {
		int optionType = act->property("OptionType").toInt();
		bool checked = act->isChecked();
		switch(optionType) {
		case Linebreak:
			m_displaySettings->lineBreaks = checked;
			break;
		case Versenum:
			m_displaySettings->verseNumbers = checked;
			break;
		case Variant:
			m_moduleSettings->textualVariants = checked;
			break;
		case Vowel:
			m_moduleSettings->hebrewPoints = checked;
			break;
		case Accents:
			m_moduleSettings->greekAccents = checked;
			break;
		case Cantillation:
			m_moduleSettings->hebrewCantillation = checked;
			break;
		case Headings:
			m_moduleSettings->headings = checked;
			break;
		case Morphseg:
			m_moduleSettings->morphSegmentation = checked;
			break;
		case Xref:
			m_moduleSettings->scriptureReferences = checked;
			break;
		case WordsofJ:
			m_moduleSettings->redLetterWords = checked;
			break;
		}
	}

	emit sigChanged();
}

/** No descriptions */
int CDisplaySettingsButton::populateMenu() {
	int ret = 0;

	m_popup->clear();

	// See also optionToggled()

	ret += addMenuEntry(tr("Use linebreaks after each verse"), Linebreak, &m_displaySettings->lineBreaks, (m_modules.first()->type() == CSwordModuleInfo::Bible));

	//show the verse numbers option only for bible modules
	ret += addMenuEntry(tr("Show versenumbers"), Versenum, &m_displaySettings->verseNumbers, (m_modules.first()->type() == CSwordModuleInfo::Bible));

	ret += addMenuEntry(tr("Show headings"), Headings, &m_moduleSettings->headings,
						isOptionAvailable(CSwordModuleInfo::headings));
	
	ret += addMenuEntry(tr("Highlight words of Jesus"), WordsofJ, &m_moduleSettings->redLetterWords,
						isOptionAvailable(CSwordModuleInfo::redLetterWords ));

	ret += addMenuEntry(tr("Show Hebrew vowel points"), Vowel, &m_moduleSettings->hebrewPoints,
						isOptionAvailable(CSwordModuleInfo::hebrewPoints ));

	ret += addMenuEntry(tr("Show Hebrew cantillation marks"), Cantillation, &m_moduleSettings->hebrewCantillation,
						isOptionAvailable(CSwordModuleInfo::hebrewCantillation ));

	ret += addMenuEntry(tr("Show Greek accents"), Accents, &m_moduleSettings->greekAccents,
						isOptionAvailable(CSwordModuleInfo::greekAccents ));

	ret += addMenuEntry(tr("Use alternative textual variant"), Variant, &m_moduleSettings->textualVariants,
						isOptionAvailable(CSwordModuleInfo::textualVariants ));

	ret += addMenuEntry(tr("Show scripture cross-references"), Xref, &m_moduleSettings->scriptureReferences,
						isOptionAvailable(CSwordModuleInfo::scriptureReferences ));

	ret += addMenuEntry(tr("Show morph segmentation"), Morphseg, &m_moduleSettings->morphSegmentation,
						isOptionAvailable(CSwordModuleInfo::morphSegmentation ));

	return ret;
}

/** Adds an entry to m_popup. */
int CDisplaySettingsButton::addMenuEntry( const QString name, OptionType type, const int* option, const bool available) {
	int ret = 0;

	if (available) {
		QAction* a = m_popup->addAction(name);
		//see optionToggled()
		a->setProperty("OptionType", type);
		a->setCheckable(true);
		a->setChecked(*option);
		ret = 1;
	}

	return ret;
}

bool CDisplaySettingsButton::isOptionAvailable( const CSwordModuleInfo::FilterTypes option ) {
	bool ret = false;
	ListCSwordModuleInfo::iterator end_it = m_modules.end();
	for (ListCSwordModuleInfo::iterator it(m_modules.begin()); it != end_it; ++it) {
		ret = ret || (*it)->has(option);
	}
	return ret;
}

/** Returns the number of usable menu items in the settings menu. */
const int CDisplaySettingsButton::menuItemCount() {
	return m_popup->actions().count();
}

/** Sets the item at position pos to the state given as 2nd paramter. */
void CDisplaySettingsButton::setItemStatus( const int index, const bool checked ) {
	QAction* action = m_popup->actions().at(index);
	action->setChecked(checked);
}

/** Returns the status of the item at position "index" */
const bool CDisplaySettingsButton::itemStatus( const int index ) {
	return m_popup->actions().at(index)->isChecked();
}

/** Sets the status to changed. The signal changed will be emitted. */
void CDisplaySettingsButton::setChanged() {
	emit sigChanged();
}
