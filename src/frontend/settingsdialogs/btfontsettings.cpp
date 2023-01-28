/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btfontsettings.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <QCheckBox>
#include <QComboBox>
#include <QFont>
#include <QFontDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QNonConstOverload>
#include <QPair>
#include <Qt>
#include <QVBoxLayout>
#include <set>
#include "../../backend/config/btconfig.h"
#include "../../backend/language.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "cconfigurationdialog.h"


BtFontSettingsPage::BtFontSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(CResMgr::settings::fonts::icon(), parent)
{
    m_languageLabel = new QLabel(this);
    m_languageComboBox = new QComboBox(this);
    m_languageLabel->setBuddy(m_languageComboBox);
    m_languageCheckBox = new QCheckBox(this);
    BT_CONNECT(m_languageCheckBox, &QCheckBox::toggled,
               this, &BtFontSettingsPage::useOwnFontClicked);


    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(m_languageLabel, 0, Qt::AlignRight);
    hLayout->addWidget(m_languageComboBox, 1);
    hLayout->addWidget(m_languageCheckBox);

    struct Comp {
        bool operator()(std::shared_ptr<Language const> const & lhs,
                        std::shared_ptr<Language const> const & rhs) const
        {
            int cmp = lhs->translatedName().compare(rhs->translatedName());
            if (cmp == 0) {
                cmp = lhs->abbrev().compare(rhs->abbrev());
                BT_ASSERT(cmp != 0);
            }
            return cmp < 0;
        }
    };
    std::set<std::shared_ptr<Language const>, Comp> languages;
    {
        auto const availableLanguages =
                CSwordBackend::instance().availableLanguages();
        BT_ASSERT(availableLanguages);
        for (auto const & language : *availableLanguages)
            languages.emplace(language);
    }

    for (auto const & l : languages) {
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

    if (m_languageComboBox->count() > 0) {
        BT_CONNECT(m_fontChooser, &QFontDialog::currentFontChanged,
                   [this](QFont const & newFont) {
            auto & work =
                    m_workSettings[m_languageComboBox->currentIndex()];
            work.settings.second = newFont;
        });
        BT_CONNECT(m_languageComboBox,
                   qOverload<int>(&QComboBox::currentIndexChanged),
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
    }

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
