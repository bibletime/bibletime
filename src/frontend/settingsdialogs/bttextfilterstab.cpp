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

#include "bttextfilterstab.h"

#include <QCheckBox>
#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/config/btconfigcore.h"
#include "../../util/tool.h"
#include "cswordsettings.h"


#define TEXT_FILTERS_TAB_ADD_ROW(name,def) \
        m_ ## name ## Check = new QCheckBox(this); \
        m_ ## name ## Check->setChecked(conf.value<bool>(#name,(def))); \
        layout->addWidget(m_ ## name ## Check)

BtTextFiltersTab::BtTextFiltersTab(CSwordSettingsPage *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(2);

    m_explanationLabel = new QLabel(this);
    m_explanationLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    m_explanationLabel->setMaximumHeight(50);
    layout->addWidget(m_explanationLabel);

    {
        auto const conf =
                btConfig().session().group(QStringLiteral("presentation"));
        TEXT_FILTERS_TAB_ADD_ROW(verseNumbers, true);
        TEXT_FILTERS_TAB_ADD_ROW(headings, true);
        TEXT_FILTERS_TAB_ADD_ROW(hebrewPoints, true);
        TEXT_FILTERS_TAB_ADD_ROW(hebrewCantillation, true);
        TEXT_FILTERS_TAB_ADD_ROW(morphSegmentation, true);
        TEXT_FILTERS_TAB_ADD_ROW(greekAccents, true);
        TEXT_FILTERS_TAB_ADD_ROW(textualVariants, false);
        TEXT_FILTERS_TAB_ADD_ROW(scriptureReferences, true);
    }

    layout->addStretch(4);

    retranslateUi();
}

#define TEXT_FILTERS_TAB_SAVE(name) \
    conf.setValue(#name, m_ ## name ## Check->isChecked())

void BtTextFiltersTab::save() {
    auto conf = btConfig().session().group(QStringLiteral("presentation"));
    TEXT_FILTERS_TAB_SAVE(verseNumbers);
    TEXT_FILTERS_TAB_SAVE(headings);
    TEXT_FILTERS_TAB_SAVE(hebrewPoints);
    TEXT_FILTERS_TAB_SAVE(hebrewCantillation);
    TEXT_FILTERS_TAB_SAVE(morphSegmentation);
    TEXT_FILTERS_TAB_SAVE(greekAccents);
    TEXT_FILTERS_TAB_SAVE(textualVariants);
    TEXT_FILTERS_TAB_SAVE(scriptureReferences);
}


void BtTextFiltersTab::retranslateUi() {
    util::tool::initExplanationLabel(m_explanationLabel, QString(),
          tr("Filters control the appearance of text. Here you can specify "
             "default settings for all filters. These settings apply to newly "
             "opened display windows only. You can override these settings in "
             "each display window."));

    m_verseNumbersCheck->setText(tr("Show verse numbers"));
    m_headingsCheck->setText(tr("Show section headings"));
    m_scriptureReferencesCheck->setText(tr("Show scripture cross-references"));
    m_greekAccentsCheck->setText(tr("Show Greek accents"));
    m_hebrewPointsCheck->setText(tr("Show Hebrew vowel points"));
    m_hebrewCantillationCheck->setText(tr("Show Hebrew cantillation marks"));
    m_morphSegmentationCheck->setText(tr("Show morph segmentation"));
    m_textualVariantsCheck->setText(tr("Use textual variants"));
}
