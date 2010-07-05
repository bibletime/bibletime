/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/btfontsettings.h"

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
#include "btfontsettings.h"


BtFontSettingsPage::BtFontSettingsPage(QWidget *parent)
        : BtConfigPage(parent)
{
    namespace DU = util::directory;

    Q_ASSERT(qobject_cast<QVBoxLayout*>(layout()) != 0);
    QVBoxLayout *mainLayout = static_cast<QVBoxLayout*>(layout());
 
    //Font settings

    QHBoxLayout* hLayout = new QHBoxLayout();

    m_usageCombo = new QComboBox(this);
    m_usageCombo->setToolTip(tr("The font selection below will apply to all texts in this language"));

    hLayout->addWidget(m_usageCombo);

    CLanguageMgr::LangMap langMap = CPointers::languageMgr()->availableLanguages();

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

    mainLayout->addLayout(hLayout);
    hLayout->setContentsMargins(0, 0, 0, 0);
    /// \todo remember the last selected font and jump there.

    m_fontChooser = new CFontChooser(this);

    /// \todo Eeli's wishlist: why not show something relevant here, like a Bible verse in chosen (not tr()'ed!) language?
    QString sampleText;
    sampleText.append("1 In the beginning God created the heaven and the earth.  ");
    sampleText.append("2 And the earth was without form, and void; and darkness was on the face of the deep.");
    sampleText.append(" And the Spirit of God moved on the face of the waters.");

    m_fontChooser->setSampleText(sampleText);
    mainLayout->addWidget(m_fontChooser);

    connect(m_fontChooser, SIGNAL(fontSelected(const QFont&)), SLOT(newDisplayWindowFontSelected(const QFont&)));
    connect(m_usageCombo, SIGNAL(activated(const QString&)), SLOT(newDisplayWindowFontAreaSelected(const QString&)));

    m_fontChooser->setFont( m_fontMap[m_usageCombo->currentText()].second );
    useOwnFontClicked( m_fontMap[m_usageCombo->currentText()].first );
    m_useOwnFontCheck->setChecked( m_fontMap[m_usageCombo->currentText()].first );
    m_fontChooser->setMinimumSize(m_fontChooser->sizeHint());
}


BtFontSettingsPage::~BtFontSettingsPage() {
}

void BtFontSettingsPage::save() {
    for (QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it ) {
        const CLanguageMgr::Language* const lang = CPointers::languageMgr()->languageForTranslatedName(it.key());
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
}

/**  */
void BtFontSettingsPage::newDisplayWindowFontSelected(const QFont &newFont) {
    //belongs to the languages/fonts page
    CBTConfig::FontSettingsPair oldSettings = m_fontMap[ m_usageCombo->currentText() ];
    m_fontMap.insert( m_usageCombo->currentText(), CBTConfig::FontSettingsPair(oldSettings.first, newFont) );
}

/** Called when the combobox contents is changed */
void BtFontSettingsPage::newDisplayWindowFontAreaSelected(const QString& usage) {
    //belongs to fonts/languages
    useOwnFontClicked( m_fontMap[usage].first );
    m_useOwnFontCheck->setChecked( m_fontMap[usage].first );

    m_fontChooser->setFont( m_fontMap[usage].second );
}


/** This slot is called when the "Use own font for language" bo was clicked. */
void BtFontSettingsPage::useOwnFontClicked(bool isOn) {
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


const QIcon &BtFontSettingsPage::icon() const {
    return util::directory::getIcon(CResMgr::settings::fonts::icon);
}

QString BtFontSettingsPage::label() const {
    return tr("You can specify a custom font for each language.");
}

QString BtFontSettingsPage::header() const {
    return tr("Fonts");
}
