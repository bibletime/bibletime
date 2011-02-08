/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/btlanguagesettings.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/directory.h"
#include "backend/config/btconfig.h"

// Sword includes:
#include <localemgr.h>
#include <swlocale.h>


BtLanguageSettingsPage::BtLanguageSettingsPage(QWidget *parent)
        : BtConfigPage(parent)
{
    namespace DU = util::directory;

    Q_ASSERT(qobject_cast<QVBoxLayout*>(layout()) != 0);
    QVBoxLayout *mainLayout = static_cast<QVBoxLayout*>(layout());

    //Sword locales
    m_swordLocaleCombo = new QComboBox(this);
    QLabel* label = new QLabel( tr("Language for names of Bible books:"), this);
    label->setBuddy(m_swordLocaleCombo);
    m_swordLocaleCombo->setToolTip(tr("The languages which can be used for the biblical booknames"));


    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(m_swordLocaleCombo);
    hBoxLayout->addStretch();
    mainLayout->addLayout(hBoxLayout);

    mainLayout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding));

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
        CLanguageMgr::instance()->languageForAbbrev( getBtConfig().getValue<QString>("language") );

    QString currentLanguageName;
    if ( l->isValid() && languageNames.contains(l->translatedName()) ) {     //tranlated language name is in the box
        currentLanguageName = l->translatedName();
    }
    else {     //a language like "German Abbrevs" might be the language to set
        sword::SWLocale* locale =
            sword::LocaleMgr::getSystemLocaleMgr()->getLocale( getBtConfig().getValue<QString>("language").toLocal8Bit() );
        if (locale) {
            currentLanguageName = QString::fromLatin1(locale->getDescription());
        }
    }

    if (currentLanguageName.isEmpty()) {     // set english as default if nothing was chosen
        Q_ASSERT(CLanguageMgr::instance()->languageForAbbrev("en_US"));
        currentLanguageName = CLanguageMgr::instance()->languageForAbbrev("en_US")->translatedName();
    }

    //now set the item with the right name as current item
    for (int i = 0; i < m_swordLocaleCombo->count(); ++i) {
        if (currentLanguageName == m_swordLocaleCombo->itemText(i)) {
            m_swordLocaleCombo->setCurrentIndex(i);
            break; //item found, finish the loop
        }
    }

}


BtLanguageSettingsPage::~BtLanguageSettingsPage() {
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
        getBtConfig().setValue("language", languageAbbrev);
    }
}

const QIcon &BtLanguageSettingsPage::icon() const {
    return util::directory::getIcon(CResMgr::settings::languages::icon);
}

QString BtLanguageSettingsPage::header() const {
    return tr("Languages");
}
