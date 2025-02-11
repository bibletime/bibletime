/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btdisplaysettingsbutton.h"

#include <QAction>
#include <QMenu>
#include <QString>
#include <QToolButton>
#include "../../backend/btglobal.h"
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"


BtDisplaySettingsButton::BtDisplaySettingsButton(QWidget *parent)
        : QToolButton(parent) {
    initMenu();

    setIcon(CResMgr::displaywindows::displaySettings::icon());
    setPopupMode(QToolButton::InstantPopup);
    setEnabled(false);

    initMenu();
    retranslateUi();

    BT_CONNECT(m_popup, &QMenu::triggered,
               [this](QAction * const action) {
                   bool checked = action->isChecked();

                   if (action == m_verseNumbersAction) {
                       m_displayOptions.verseNumbers = checked;
                       Q_EMIT sigDisplayOptionsChanged(m_displayOptions);
                   } else if (action == m_variantAction) {
                       m_filterOptions.textualVariants = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_hebrewPointsAction) {
                       m_filterOptions.hebrewPoints = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_greekAccentsAction) {
                       m_filterOptions.greekAccents = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_hebrewCantillationAction) {
                       m_filterOptions.hebrewCantillation = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_headingsAction) {
                       m_filterOptions.headings = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_morphSegmentationAction) {
                       m_filterOptions.morphSegmentation = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_scriptureReferencesAction) {
                       m_filterOptions.scriptureReferences = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_footnotesAction) {
                       m_filterOptions.footnotes = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else if (action == m_redWordsAction) {
                       m_filterOptions.redLetterWords = checked;
                       Q_EMIT sigFilterOptionsChanged(m_filterOptions);
                   } else {
                       BT_ASSERT(false && "Shouldn't happen!");
                       return;
                   }

                   Q_EMIT sigChanged();
               });
}

void BtDisplaySettingsButton::setDisplayOptionsNoRepopulate(
        DisplayOptions const & displaySettings)
{ m_displayOptions = displaySettings; }


void BtDisplaySettingsButton::setFilterOptionsNoRepopulate(
        FilterOptions const & moduleSettings)
{ m_filterOptions = moduleSettings; }

void BtDisplaySettingsButton::setDisplayOptions(
        DisplayOptions const & displaySettings)
{
    m_displayOptions = displaySettings;
    repopulateMenu();
}

void BtDisplaySettingsButton::setFilterOptions(
        FilterOptions const & moduleSettings)
{
    m_filterOptions = moduleSettings;
    repopulateMenu();
}

void BtDisplaySettingsButton::setModules(
        const BtConstModuleList &modules)
{
    m_modules = modules;
    repopulateMenu();
}

void BtDisplaySettingsButton::initMenu() {
    m_popup = new QMenu(this);
    setMenu(m_popup);

    m_verseNumbersAction = new QAction(this);
    m_verseNumbersAction->setCheckable(true);

    m_headingsAction = new QAction(this);
    m_headingsAction->setCheckable(true);

    m_redWordsAction = new QAction(this);
    m_redWordsAction->setCheckable(true);

    m_hebrewPointsAction = new QAction(this);
    m_hebrewPointsAction->setCheckable(true);

    m_hebrewCantillationAction = new QAction(this);
    m_hebrewCantillationAction->setCheckable(true);

    m_greekAccentsAction = new QAction(this);
    m_greekAccentsAction->setCheckable(true);

    m_variantAction = new QAction(this);
    m_variantAction->setCheckable(true);

    m_scriptureReferencesAction = new QAction(this);
    m_scriptureReferencesAction->setCheckable(true);

    m_footnotesAction = new QAction(this);
    m_footnotesAction->setCheckable(true);

    m_morphSegmentationAction = new QAction(this);
    m_morphSegmentationAction->setCheckable(true);
}

void BtDisplaySettingsButton::retranslateUi() {
    m_verseNumbersAction->setText(tr("Show verse numbers"));
    m_headingsAction->setText(tr("Show headings"));
    m_redWordsAction->setText(tr("Highlight words of Jesus"));
    m_hebrewPointsAction->setText(tr("Show Hebrew vowel points"));
    m_hebrewCantillationAction->setText(tr("Show Hebrew cantillation marks"));
    m_greekAccentsAction->setText(tr("Show Greek accents"));
    m_variantAction->setText(tr("Use alternative textual variant"));
    m_scriptureReferencesAction->setText(tr("Show scripture cross-references"));
    m_footnotesAction->setText(tr("Show footnotes"));
    m_morphSegmentationAction->setText(tr("Show morph segmentation"));

    retranslateToolTip();
}

void BtDisplaySettingsButton::retranslateToolTip() {
    if (isEnabled()) {
        setToolTip(tr("Display settings"));
    }
    else {
        setToolTip(tr("Display settings: No options available"));
    }
}

/** No descriptions */
void BtDisplaySettingsButton::repopulateMenu() {
    bool enable = false;

    m_popup->clear();
    if (!m_modules.isEmpty()) {
        if (m_modules.first()->type() == CSwordModuleInfo::Bible) {
            addMenuEntry(m_verseNumbersAction, m_displayOptions.verseNumbers);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::headings)) {
            addMenuEntry(m_headingsAction, m_filterOptions.headings);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::redLetterWords)) {
            addMenuEntry(m_redWordsAction, m_filterOptions.redLetterWords);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::hebrewPoints)) {
            addMenuEntry(m_hebrewPointsAction, m_filterOptions.hebrewPoints);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::hebrewCantillation)) {
            addMenuEntry(m_hebrewCantillationAction, m_filterOptions.hebrewCantillation);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::greekAccents)) {
            addMenuEntry(m_greekAccentsAction, m_filterOptions.greekAccents);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::textualVariants)) {
            addMenuEntry(m_variantAction, m_filterOptions.textualVariants);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::scriptureReferences)) {
            addMenuEntry(m_scriptureReferencesAction, m_filterOptions.scriptureReferences);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::footnotes)) {
            addMenuEntry(m_footnotesAction, m_filterOptions.footnotes);
            enable = true;
        }

        if (isOptionAvailable(CSwordModuleInfo::morphSegmentation)) {
            addMenuEntry(m_morphSegmentationAction, m_filterOptions.morphSegmentation);
            enable = true;
        }
    }

    // Disable the settings button if no options are available:
    setEnabled(enable);
    retranslateToolTip();
}

/** Adds an entry to m_popup. */
void BtDisplaySettingsButton::addMenuEntry(QAction *action, bool checked) {
    action->setChecked(checked);
    m_popup->addAction(action);
}

bool BtDisplaySettingsButton::isOptionAvailable(
        CSwordModuleInfo::FilterOption const & option)
{
    for (auto const * const module : m_modules)
        if (module->has(option))
            return true;
    return false;
}
