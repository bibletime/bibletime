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
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/directory.h"

// Sword includes:
#include <localemgr.h>
#include <swlocale.h>


typedef std::list<sword::SWBuf>::const_iterator SBLCI;


BtLanguageSettingsPage::BtLanguageSettingsPage(QWidget *parent)
        : BtConfigPage(parent)
{
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

    initSwordLocaleCombo();
}

BtLanguageSettingsPage::~BtLanguageSettingsPage() {
}

void BtLanguageSettingsPage::save() {
    CBTConfig::set(CBTConfig::language, m_swordLocaleCombo->itemData(m_swordLocaleCombo->currentIndex()).toString());
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
    CBTConfig::set(CBTConfig::language, best);
}

QVector<QString> BtLanguageSettingsPage::bookNameAbbreviationsTryVector() {
    QVector<QString> atv;
    atv.reserve(4);
    {
        QString settingsLanguage = CBTConfig::get(CBTConfig::language);
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
        const char * abbreviation = sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName();
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

const QIcon &BtLanguageSettingsPage::icon() const {
    return util::directory::getIcon(CResMgr::settings::languages::icon);
}

QString BtLanguageSettingsPage::header() const {
    return tr("Languages");
}
