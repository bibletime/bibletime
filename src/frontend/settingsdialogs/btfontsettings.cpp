/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2019 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btfontsettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../bibletimeapp.h"
#include "btfontchooserwidget.h"
#include "cconfigurationdialog.h"


BtFontSettingsPage::BtFontSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(CResMgr::settings::fonts::icon(), parent)
{
    m_languageLabel = new QLabel(this);
    m_languageComboBox = new QComboBox(this);
    m_languageLabel->setBuddy(m_languageComboBox);
    m_languageCheckBox = new QCheckBox(this);
    BT_CONNECT(m_languageCheckBox, SIGNAL(toggled(bool)),
               this,               SLOT(useOwnFontClicked(bool)) );


    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(m_languageLabel, 0, Qt::AlignRight);
    hLayout->addWidget(m_languageComboBox, 1);
    hLayout->addWidget(m_languageCheckBox);

    CLanguageMgr::LangMap langMap = CLanguageMgr::instance()->availableLanguages();
    using L = CLanguageMgr::Language;
    for (CLanguageMgr::LangMapIterator it = langMap.constBegin();
         it != langMap.constEnd();
         it++)
    {
        const L * const l = *it;
        const QString &(L::*f)() const =
            l->translatedName().isEmpty()
            ? &L::abbrev
            : &L::translatedName;

        m_fontMap.insert((l->*f)(), btConfig().getFontForLanguage(*l));
    }

    for (FontMap::ConstIterator it = m_fontMap.constBegin(); it != m_fontMap.constEnd(); ++it) {
        const QString &k = it.key();
        if (m_fontMap[k].first) { // show font icon
            m_languageComboBox->addItem(CResMgr::settings::fonts::icon(), k);
        } else { // don't show icon for font
            m_languageComboBox->addItem(k);
        }
    }

    /// \todo remember the last selected font and jump there.

    m_fontChooser = new BtFontChooserWidget(this);
    /**
      \todo Eeli's wishlist: why not show something relevant here, like a Bible
            verse in chosen (not tr()'ed!) language?
    */
    // m_fontChooser->setSampleText("SOMETHING");

    BT_CONNECT(m_fontChooser, SIGNAL(fontSelected(QFont const &)),
               this, SLOT(newDisplayWindowFontSelected(QFont const &)));
    BT_CONNECT(m_languageComboBox, SIGNAL(activated(QString const &)),
               this, SLOT(newDisplayWindowFontAreaSelected(QString const &)));

    const BtConfig::FontSettingsPair &v = m_fontMap.value(m_languageComboBox->currentText());
    m_fontChooser->setFont(v.second);
    useOwnFontClicked(v.first);
    m_languageCheckBox->setChecked(v.first);
    m_fontChooser->setMinimumSize(m_fontChooser->sizeHint());

    QVBoxLayout *fLayout = new QVBoxLayout;
    fLayout->setContentsMargins(0, 0, 0, 0);
    fLayout->addLayout(hLayout);
    fLayout->addWidget(m_fontChooser);

    m_fontsGroupBox = new QGroupBox(this);
    m_fontsGroupBox->setFlat(true);
    m_fontsGroupBox->setLayout(fLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_fontsGroupBox);

    retranslateUi();
}

void BtFontSettingsPage::save() const {
    for (FontMap::ConstIterator it = m_fontMap.constBegin();
         it != m_fontMap.constEnd();
         it++)
    {
        const QString &k = it.key();
        const CLanguageMgr::Language * const lang = CLanguageMgr::instance()->languageForTranslatedName(k);
        if (!lang->isValid()) {
            // We possibly use a language, for which we have only the abbrevation
            if (!lang->abbrev().isEmpty()) {
                // Create a temp language:
                const CLanguageMgr::Language l(k, k, k);
                btConfig().setFontForLanguage(l, it.value());
            }
        }
        else {
            btConfig().setFontForLanguage(*lang, it.value());
        }
    }
}

void BtFontSettingsPage::newDisplayWindowFontSelected(const QFont &newFont) {
    const QString languageName = m_languageComboBox->currentText();
    m_fontMap.insert(languageName,
                     BtConfig::FontSettingsPair(m_fontMap[languageName].first, newFont));
}

void BtFontSettingsPage::newDisplayWindowFontAreaSelected(const QString &usage) {
    const BtConfig::FontSettingsPair &p = m_fontMap[usage];
    useOwnFontClicked(p.first);
    m_languageCheckBox->setChecked(p.first);
    m_fontChooser->setFont(p.second);
}

void BtFontSettingsPage::useOwnFontClicked(bool isOn) {
    m_fontChooser->setEnabled(isOn);
    m_fontMap[m_languageComboBox->currentText()].first = isOn;
    m_languageComboBox->setItemIcon(m_languageComboBox->currentIndex(),
                                    isOn
                                    ? CResMgr::settings::fonts::icon()
                                    : QIcon());
}

void BtFontSettingsPage::retranslateUi() {
    setHeaderText(tr("Fonts"));
    m_languageLabel->setText(tr("&Language:"));
    m_languageComboBox->setToolTip(tr("The font selection below will apply to all texts in this language"));
    m_languageCheckBox->setText(tr("Use custom font"));
}
