/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/btfontsettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "frontend/settingsdialogs/cfontchooser.h"
#include "util/cresmgr.h"
#include "util/tool.h"
#include "util/directory.h"

// Sword includes:
#include <localemgr.h>
#include <swlocale.h>


BtFontSettingsPage::BtFontSettingsPage(QWidget *parent)
        : BtConfigPage(parent)
{
    namespace DU = util::directory;

    m_languageLabel = new QLabel(tr("&Language:"), this);

    m_languageComboBox = new QComboBox(this);
    m_languageComboBox->setToolTip(tr("The font selection below will apply to all texts in this language"));
    m_languageLabel->setBuddy(m_languageComboBox);

    m_languageCheckBox = new QCheckBox(tr("Use custom font"), this);
    connect(m_languageCheckBox, SIGNAL(toggled(bool)),
            this,               SLOT(useOwnFontClicked(bool)) );


    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(m_languageLabel, 0, Qt::AlignRight);
    hLayout->addWidget(m_languageComboBox, 1);
    hLayout->addWidget(m_languageCheckBox);

    CLanguageMgr::LangMap langMap = CLanguageMgr::instance()->availableLanguages();

    for (CLanguageMgr::LangMapIterator it = langMap.constBegin() ; it != langMap.constEnd(); ++it ) {
        const QString name =
            (*it)->translatedName().isEmpty()
            ? (*it)->abbrev()
            : (*it)->translatedName();

        m_fontMap.insert(name, CBTConfig::get(*it) );
    }

    for ( QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it ) {
        if ( m_fontMap[it.key()].first ) {     //show font icon
            m_languageComboBox->addItem(DU::getIcon("fonts.svg"), it.key() );
        }
        else {     //don't show icon for font
            m_languageComboBox->addItem(it.key());
        }
    }

    /// \todo remember the last selected font and jump there.

    m_fontChooser = new CFontChooser(this);

    /// \todo Eeli's wishlist: why not show something relevant here, like a Bible verse in chosen (not tr()'ed!) language?
    QString sampleText;
    sampleText.append("1 In the beginning God created the heaven and the earth.  ");
    sampleText.append("2 And the earth was without form, and void; and darkness was on the face of the deep.");
    sampleText.append(" And the Spirit of God moved on the face of the waters.");

    m_fontChooser->setSampleText(sampleText);

    connect(m_fontChooser, SIGNAL(fontSelected(const QFont&)), SLOT(newDisplayWindowFontSelected(const QFont&)));
    connect(m_languageComboBox, SIGNAL(activated(const QString&)), SLOT(newDisplayWindowFontAreaSelected(const QString&)));

    m_fontChooser->setFont( m_fontMap[m_languageComboBox->currentText()].second );
    useOwnFontClicked( m_fontMap[m_languageComboBox->currentText()].first );
    m_languageCheckBox->setChecked( m_fontMap[m_languageComboBox->currentText()].first );
    m_fontChooser->setMinimumSize(m_fontChooser->sizeHint());

    QVBoxLayout *fLayout = new QVBoxLayout;
    fLayout->setContentsMargins(0, 0, 0, 0);
    fLayout->addLayout(hLayout);
    fLayout->addWidget(m_fontChooser);

    m_fontsGroupBox = new QGroupBox(tr("Optionally specify a custom font for each language:"), this);
    m_fontsGroupBox->setFlat(true);
    m_fontsGroupBox->setLayout(fLayout);

    Q_ASSERT(qobject_cast<QVBoxLayout*>(layout()) != 0);
    static_cast<QVBoxLayout*>(layout())->addWidget(m_fontsGroupBox);
}


BtFontSettingsPage::~BtFontSettingsPage() {
}

void BtFontSettingsPage::save() {
    for (QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_fontMap.begin(); it != m_fontMap.end(); ++it ) {
        const CLanguageMgr::Language * const lang = CLanguageMgr::instance()->languageForTranslatedName(it.key());
        if (!lang->isValid()) {     //we possibly use a language, for which we have only the abbrev
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
    CBTConfig::FontSettingsPair oldSettings = m_fontMap[ m_languageComboBox->currentText() ];
    m_fontMap.insert( m_languageComboBox->currentText(), CBTConfig::FontSettingsPair(oldSettings.first, newFont) );
}

/** Called when the combobox contents is changed */
void BtFontSettingsPage::newDisplayWindowFontAreaSelected(const QString& usage) {
    //belongs to fonts/languages
    useOwnFontClicked( m_fontMap[usage].first );
    m_languageCheckBox->setChecked( m_fontMap[usage].first );

    m_fontChooser->setFont( m_fontMap[usage].second );
}


/** This slot is called when the "Use own font for language" bo was clicked. */
void BtFontSettingsPage::useOwnFontClicked(bool isOn) {
    namespace DU = util::directory;

    //belongs to fonts/languages

    m_fontChooser->setEnabled(isOn);
    m_fontMap[ m_languageComboBox->currentText() ].first = isOn;

    if (isOn) {     //show font icon
        m_languageComboBox->setItemIcon(m_languageComboBox->currentIndex(), DU::getIcon("fonts.svg"));
    }
    else {   //don't show
        m_languageComboBox->setItemText(m_languageComboBox->currentIndex(), m_languageComboBox->currentText() ); /// \todo should this change icon to empty?
    }
}


const QIcon &BtFontSettingsPage::icon() const {
    return util::directory::getIcon(CResMgr::settings::fonts::icon);
}

QString BtFontSettingsPage::header() const {
    return tr("Fonts");
}
