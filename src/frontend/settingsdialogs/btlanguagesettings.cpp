/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/btlanguagesettings.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "backend/config/cbtconfig.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/directory.h"

// Sword includes:
#include <localemgr.h>
#include <swlocale.h>


BtLanguageSettingsPage::BtLanguageSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(util::directory::getIcon(CResMgr::settings::languages::icon), parent)
{
    namespace DU = util::directory;

    m_swordLocaleCombo = new QComboBox(this);
    m_languageNamesLabel = new QLabel(this);
    m_languageNamesLabel->setBuddy(m_swordLocaleCombo);

    QStringList languageNames;
    languageNames.append(CLanguageMgr::instance()->languageForAbbrev("en_US")->translatedName());

    std::list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
    for (std::list<sword::SWBuf>::const_iterator it = locales.begin(); it != locales.end(); it++) {
        //    qWarning("working on %s", (*it).c_str());
        const CLanguageMgr::Language * const l =
            CLanguageMgr::instance()->languageForAbbrev( sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName() );

        if (l->isValid()) {
            languageNames.append( l->translatedName() );
        }
        else {
            languageNames.append(
                sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getDescription()
            );
        }
    } //for

    languageNames.sort();
    m_swordLocaleCombo->addItems( languageNames );

    const CLanguageMgr::Language * const l =
        CLanguageMgr::instance()->languageForAbbrev( CBTConfig::get(CBTConfig::language) );

    QString currentLanguageName;
    if ( l->isValid() && languageNames.contains(l->translatedName()) ) {     //tranlated language name is in the box
        currentLanguageName = l->translatedName();
    }
    else {     //a language like "German Abbrevs" might be the language to set
        sword::SWLocale* locale =
            sword::LocaleMgr::getSystemLocaleMgr()->getLocale( CBTConfig::get(CBTConfig::language).toLocal8Bit() );
        if (locale) {
            currentLanguageName = QString::fromLatin1(locale->getDescription());
        }
    }

    if (currentLanguageName.isEmpty()) {     // set english as default if nothing was chosen
        Q_ASSERT(CLanguageMgr::instance()->languageForAbbrev("en_US"));
        currentLanguageName = CLanguageMgr::instance()->languageForAbbrev("en_US")->translatedName();
    }

    int i = languageNames.indexOf(currentLanguageName);
    if (i >= 0)
        m_swordLocaleCombo->setCurrentIndex(i);

    QFormLayout *formLayout = new QFormLayout(this);
    formLayout->addRow(m_languageNamesLabel, m_swordLocaleCombo);

    retranslateUi();
}

void BtLanguageSettingsPage::save() {

    QString languageAbbrev;

    const QString currentLanguageName = m_swordLocaleCombo->currentText();
    const CLanguageMgr::Language * const l = CLanguageMgr::instance()->languageForTranslatedName( currentLanguageName );

    if (l && l->isValid()) {
        languageAbbrev = l->abbrev();
    }
    else {     //it can be the lang abbrev like de_abbrev or the Sword description
        std::list <sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();

        for (std::list <sword::SWBuf>::iterator it = locales.begin(); it != locales.end(); it++) {
            sword::SWLocale* locale = sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str());
            Q_ASSERT(locale);

            if ( locale && (QString::fromLatin1(locale->getDescription()) == currentLanguageName) ) {
                languageAbbrev = QString::fromLatin1(locale->getName()); //we found the abbrevation for the current language
                break;
            }
        }

        if (languageAbbrev.isEmpty()) {
            languageAbbrev = currentLanguageName; //probably a non-standard locale name like de_abbrev
        }
    }

    if (!languageAbbrev.isEmpty()) {
        CBTConfig::set(CBTConfig::language, languageAbbrev);
    }
}

void BtLanguageSettingsPage::retranslateUi() {
    setHeaderText(tr("Languages"));

    m_languageNamesLabel->setText(tr("Language for names of Bible books:"));
    m_swordLocaleCombo->setToolTip(tr("The languages which can be used for the biblical booknames"));
}
