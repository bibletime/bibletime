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
#include "backend/config/btconfig.h"
#include "bibletimeapp.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "util/cresmgr.h"
#include "util/tool.h"

// Sword includes:
#include <localemgr.h>
#include <swlocale.h>


typedef std::list<sword::SWBuf>::const_iterator SBLCI;


BtLanguageSettingsPage::BtLanguageSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(bApp->getIcon(CResMgr::settings::languages::icon), parent)
{
    m_swordLocaleCombo = new QComboBox(this);
    m_languageNamesLabel = new QLabel(this);
    m_languageNamesLabel->setBuddy(m_swordLocaleCombo);

    QFormLayout * formLayout = new QFormLayout(this);
    formLayout->addRow(m_languageNamesLabel, m_swordLocaleCombo);

    retranslateUi();

    initSwordLocaleCombo();
}

void BtLanguageSettingsPage::save() {
    btConfig().setValue("language", m_swordLocaleCombo->itemData(m_swordLocaleCombo->currentIndex()));
}

void BtLanguageSettingsPage::resetLanguage() {
    QVector<QString> atv = bookNameAbbreviationsTryVector();

    QString best = "en_US";
    Q_ASSERT(atv.contains(best));
    int i = atv.indexOf(best);
    if (i > 0) {
        atv.resize(i);
        const std::list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
        for (SBLCI it = locales.begin(); it != locales.end(); ++it) {
            const char * abbr = sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName();
            i = atv.indexOf(abbr);
            if (i >= 0) {
                best = abbr;
                if (i == 0)
                    break;
                atv.resize(i);
            }
        }
    }
    btConfig().setValue("language", best);
}

QVector<QString> BtLanguageSettingsPage::bookNameAbbreviationsTryVector() {
    QVector<QString> atv;
    atv.reserve(4);
    {
        QString settingsLanguage = btConfig().value<QString>("language");
        if (!settingsLanguage.isEmpty())
            atv.append(settingsLanguage);
    }
    {
        const QString localeLanguageAndCountry = QLocale::system().name();
        if (!localeLanguageAndCountry.isEmpty()) {
            atv.append(localeLanguageAndCountry);
            int i = localeLanguageAndCountry.indexOf('_');
            if (i > 0)
                atv.append(localeLanguageAndCountry.left(i));
        }
    }
    Q_ASSERT(CLanguageMgr::instance()->languageForAbbrev("en_US"));
    atv.append("en_US");
    return atv;
}

void BtLanguageSettingsPage::initSwordLocaleCombo() {
    typedef QMap<QString, QString>::const_iterator SSMCI;

    QMap<QString, QString> languageNames;
    Q_ASSERT(CLanguageMgr::instance()->languageForAbbrev("en_US"));
    languageNames.insert(CLanguageMgr::instance()->languageForAbbrev("en_US")->translatedName(), "en_US");

    const std::list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
    for (SBLCI it = locales.begin(); it != locales.end(); ++it) {
        const char * const abbreviation = sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName();
        const CLanguageMgr::Language * const l = CLanguageMgr::instance()->languageForAbbrev(abbreviation);

        if (l->isValid()) {
            languageNames.insert(l->translatedName(), abbreviation);
        } else {
            languageNames.insert(
                sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getDescription(),
                abbreviation);
        }
    }

    int index = 0;
    QVector<QString> atv = bookNameAbbreviationsTryVector();
    for (SSMCI it = languageNames.constBegin(); it != languageNames.constEnd(); ++it) {
        if (!atv.isEmpty()) {
            int i = atv.indexOf(it.value());
            if (i >= 0) {
                atv.resize(i);
                index = m_swordLocaleCombo->count();
            }
        }
        m_swordLocaleCombo->addItem(it.key(), it.value());
    }
    m_swordLocaleCombo->setCurrentIndex(index);
}

void BtLanguageSettingsPage::retranslateUi() {
    setHeaderText(tr("Languages"));

    m_languageNamesLabel->setText(tr("Language for names of Bible books:"));
    const QString toolTip(tr("The languages which can be used for the biblical book names. Translations are provided by the Sword library."));
    m_languageNamesLabel->setToolTip(toolTip);
    m_swordLocaleCombo->setToolTip(toolTip);
}
