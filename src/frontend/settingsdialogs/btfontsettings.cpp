/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btfontsettings.h"

#include <set>
#include <QCheckBox>
#include <QComboBox>
#include <QFontDialog>
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

    struct Comp {
        bool operator()(CLanguageMgr::Language const * lhs,
                        CLanguageMgr::Language const * rhs) const
        {
            int cmp = lhs->translatedName().compare(rhs->translatedName());
            if (cmp == 0) {
                cmp = lhs->abbrev().compare(rhs->abbrev());
                BT_ASSERT(cmp != 0);
            }
            return cmp < 0;
        }
    };
    std::set<CLanguageMgr::Language const *, Comp> languages;
    for (auto const * const language
         : CLanguageMgr::instance()->availableLanguages().values())
        languages.emplace(language);

    for (auto const * const l : languages) {
        m_workSettings.emplace_back(
                    WorkSetting{*l, btConfig().getFontForLanguage(*l)});
        auto const & k = l->translatedName().isEmpty()
                         ? l->abbrev()
                         : l->translatedName();
        if (m_workSettings.back().settings.first) { // show font icon
            m_languageComboBox->addItem(CResMgr::settings::fonts::icon(), k);
        } else { // don't show icon for font
            m_languageComboBox->addItem(k);
        }
    }

    /// \todo remember the last selected font and jump there.

    m_fontChooser = new QFontDialog(this);
    m_fontChooser->setOptions(QFontDialog::NoButtons
                              | QFontDialog::DontUseNativeDialog);
    /**
      \todo Eeli's wishlist: why not show something relevant here, like a Bible
            verse in chosen (not tr()'ed!) language?
    */
    // m_fontChooser->setSampleText("SOMETHING");

    BT_CONNECT(m_fontChooser, &QFontDialog::currentFontChanged,
               [this](QFont const & newFont) {
                   auto & work =
                           m_workSettings[m_languageComboBox->currentIndex()];
                   work.settings.second = newFont;
               });
    BT_CONNECT(m_languageComboBox,
               QOverload<int>::of(&QComboBox::currentIndexChanged),
               [this](int const newIndex) {
                   auto const i = static_cast<std::size_t>(newIndex);
                   auto const & p = m_workSettings[i].settings;
                   useOwnFontClicked(p.first);
                   m_languageCheckBox->setChecked(p.first);
                   m_fontChooser->setCurrentFont(p.second);
               });

    auto const & v =
            m_workSettings[m_languageComboBox->currentIndex()].settings;
    m_fontChooser->setCurrentFont(v.second);
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
    for (auto const & work : m_workSettings)
        btConfig().setFontForLanguage(work.language, work.settings);
}

void BtFontSettingsPage::useOwnFontClicked(bool isOn) {
    m_fontChooser->setEnabled(isOn);
    m_workSettings[m_languageComboBox->currentIndex()].settings.first = isOn;
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
