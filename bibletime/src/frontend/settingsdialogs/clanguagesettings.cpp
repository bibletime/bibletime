/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/clanguagesettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "frontend/settingsdialogs/cfontchooser.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/directory.h"

// Sword includes:
#include <localemgr.h>
#include <swlocale.h>


CLanguageSettingsPage::CLanguageSettingsPage(QWidget* /*parent*/)
        : BtConfigPage() {
    namespace DU = util::directory;

    QVBoxLayout* layout = new QVBoxLayout(this);

    //Sword locales
    layout->addWidget(
        util::tool::explanationLabel(
            this,
            tr(""),
            tr("Select the language in which the Biblical book names are displayed.")
        ));

    m_swordLocaleCombo = new QComboBox(this);
    QLabel* label = new QLabel( tr("Language for names of Bible books:"), this);
    label->setBuddy(m_swordLocaleCombo);
    m_swordLocaleCombo->setToolTip(tr("The languages which can be used for the biblical booknames"));


    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(m_swordLocaleCombo);
    hBoxLayout->addStretch();
    layout->addLayout(hBoxLayout);

    QStringList languageNames;
    languageNames.append( languageMgr()->languageForAbbrev("en_US")->translatedName() );

    std::list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
    for (std::list<sword::SWBuf>::const_iterator it = locales.begin(); it != locales.end(); it++) {
        //    qWarning("working on %s", (*it).c_str());
        const CLanguageMgr::Language* const l =
            CPointers::languageMgr()->languageForAbbrev( sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName() );

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

    const CLanguageMgr::Language* const l =
        CPointers::languageMgr()->languageForAbbrev( CBTConfig::get(CBTConfig::language) );

    QString currentLanguageName;
    if ( l->isValid() && languageNames.contains(l->translatedName()) ) { 	//tranlated language name is in the box
        currentLanguageName = l->translatedName();
    }
    else { 	//a language like "German Abbrevs" might be the language to set
        sword::SWLocale* locale =
            sword::LocaleMgr::getSystemLocaleMgr()->getLocale( CBTConfig::get(CBTConfig::language).toLocal8Bit() );
        if (locale) {
            currentLanguageName = QString::fromLatin1(locale->getDescription());
        }
    }

    if (currentLanguageName.isEmpty()) { 	// set english as default if nothing was chosen
        Q_ASSERT(languageMgr()->languageForAbbrev("en_US"));
        currentLanguageName = languageMgr()->languageForAbbrev("en_US")->translatedName();
    }

    //now set the item with the right name as current item
    for (int i = 0; i < m_swordLocaleCombo->count(); ++i) {
        if (currentLanguageName == m_swordLocaleCombo->itemText(i)) {
            m_swordLocaleCombo->setCurrentIndex(i);
            break; //item found, finish the loop
        }
    }

    layout->addSpacing(20);

    //Font settings

    layout->addWidget(
        util::tool::explanationLabel(
            this,
            tr("Fonts"),
            tr("You can specify a custom font for each language.")
        )
    );
    QHBoxLayout* hLayout = new QHBoxLayout();

    m_usageCombo = new QComboBox(this);
    m_usageCombo->setToolTip(tr("The font selection below will apply to all texts in this language"));

    hLayout->addWidget(m_usageCombo);

    CLanguageMgr::LangMap langMap = languageMgr()->availableLanguages();

    for (CLanguageMgr::LangMapIterator it = langMap.constBegin() ; it != langMap.constEnd(); ++it ) {
        const QString name =
            (*it)->translatedName().isEmpty()
            ? (*it)->abbrev()
            : (*it)->translatedName();

        m_fontMap.insert(name, CBTConfig::get(*it) );
    }

    for ( QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it ) {
        if ( m_fontMap[it.key()].first ) { 	//show font icon
            m_usageCombo->addItem(DU::getIcon("fonts.svg"), it.key() );
        }
        else { 	//don't show icon for font
            m_usageCombo->addItem(it.key());
        }
    }

    m_useOwnFontCheck = new QCheckBox(tr("Use custom font"), this);
    m_useOwnFontCheck->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_useOwnFontCheck, SIGNAL(toggled(bool)), SLOT(useOwnFontClicked(bool)) );
    hLayout->addWidget(m_useOwnFontCheck);

    layout->addLayout(hLayout);
    hLayout->setContentsMargins(0, 0, 0, 0);
    /// \todo remember the last selected font and jump there.

    m_fontChooser = new CFontChooser(this);

    /// \todo Eeli's wishlist: why not show something relevant here, like a Bible verse in chosen (not tr()'ed!) language?
    QString sampleText;
    sampleText.append("1 In the beginning God created the heaven and the earth.  ");
    sampleText.append("2 And the earth was without form, and void; and darkness was on the face of the deep.");
    sampleText.append(" And the Spirit of God moved on the face of the waters.");

    m_fontChooser->setSampleText(sampleText);
    layout->addWidget(m_fontChooser);

    connect(m_fontChooser, SIGNAL(fontSelected(const QFont&)), SLOT(newDisplayWindowFontSelected(const QFont&)));
    connect(m_usageCombo, SIGNAL(activated(const QString&)), SLOT(newDisplayWindowFontAreaSelected(const QString&)));

    m_fontChooser->setFont( m_fontMap[m_usageCombo->currentText()].second );
    useOwnFontClicked( m_fontMap[m_usageCombo->currentText()].first );
    m_useOwnFontCheck->setChecked( m_fontMap[m_usageCombo->currentText()].first );
    m_fontChooser->setMinimumSize(m_fontChooser->sizeHint());
}


CLanguageSettingsPage::~CLanguageSettingsPage() {
}

void CLanguageSettingsPage::save() {
    for (QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it ) {
        const CLanguageMgr::Language* const lang = languageMgr()->languageForTranslatedName(it.key());
        if (!lang->isValid()) { 	//we possibly use a language, for which we have only the abbrev
            if (!lang->abbrev().isEmpty()) {
                CLanguageMgr::Language l(it.key(), it.key(), it.key()); //create a temp language
                CBTConfig::set(&l, it.value());
            }
        }
        else {
            CBTConfig::set(lang, it.value());
        }
    }


    QString languageAbbrev;

    const QString currentLanguageName = m_swordLocaleCombo->currentText();
    const CLanguageMgr::Language* const l = CPointers::languageMgr()->languageForTranslatedName( currentLanguageName );

    if (l && l->isValid()) {
        languageAbbrev = l->abbrev();
    }
    else { 	//it can be the lang abbrev like de_abbrev or the Sword description
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

/**  */
void CLanguageSettingsPage::newDisplayWindowFontSelected(const QFont &newFont) {
    //belongs to the languages/fonts page
    CBTConfig::FontSettingsPair oldSettings = m_fontMap[ m_usageCombo->currentText() ];
    m_fontMap.insert( m_usageCombo->currentText(), CBTConfig::FontSettingsPair(oldSettings.first, newFont) );
}

/** Called when the combobox contents is changed */
void CLanguageSettingsPage::newDisplayWindowFontAreaSelected(const QString& usage) {
    //belongs to fonts/languages
    useOwnFontClicked( m_fontMap[usage].first );
    m_useOwnFontCheck->setChecked( m_fontMap[usage].first );

    m_fontChooser->setFont( m_fontMap[usage].second );
}


/** This slot is called when the "Use own font for language" bo was clicked. */
void CLanguageSettingsPage::useOwnFontClicked(bool isOn) {
    namespace DU = util::directory;

    //belongs to fonts/languages

    m_fontChooser->setEnabled(isOn);
    m_fontMap[ m_usageCombo->currentText() ].first = isOn;

    if (isOn) { 	//show font icon
        m_usageCombo->setItemIcon(m_usageCombo->currentIndex(), DU::getIcon("fonts.svg"));
    }
    else {   //don't show
        m_usageCombo->setItemText(m_usageCombo->currentIndex(), m_usageCombo->currentText() ); /// \todo should this change icon to empty?
    }
}


QString CLanguageSettingsPage::iconName() {
    return CResMgr::settings::fonts::icon;
}
QString CLanguageSettingsPage::label() {
    //: Empty string, don't translate
    return tr("");
}
QString CLanguageSettingsPage::header() {
    return tr("Languages");
}
