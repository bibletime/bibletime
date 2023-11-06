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

#include "btstandardworkstab.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QStringList>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordlexiconmoduleinfo.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/tool.h"
#include "cswordsettings.h"


BtStandardWorksTab::BtStandardWorksTab(CSwordSettingsPage *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(2);

    m_explanationLabel = new QLabel(this);
    m_explanationLabel->setSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::Preferred);
    m_explanationLabel->setMaximumHeight(50);
    m_explanationLabel->setMinimumWidth(300);
    mainLayout->addWidget(m_explanationLabel);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    static constexpr auto const toLexModule =
            [](CSwordModuleInfo const * module)
            { return static_cast<CSwordLexiconModuleInfo const *>(module); };
#define STANDARD_WORKS_TAB_ADD_ROW(fname,...) \
    if (true) { \
        m_ ## fname ## Label = new QLabel(this); \
        m_ ## fname ## Combo = new QComboBox(this); \
        formLayout->addRow(m_ ## fname ## Label, m_ ## fname ## Combo); \
        auto const * const defaultModule = \
            btConfig().getDefaultSwordModuleByType(#fname); \
        for (auto const * const m : CSwordBackend::instance().moduleList()) { \
            if (m->type() == CSwordModuleInfo::__VA_ARGS__) { \
                QString itemText; \
                if (auto description = \
                            m->config(CSwordModuleInfo::Description); \
                    !description.isEmpty()) \
                { \
                    itemText = \
                            tr("%1: %2", \
                               "module name: module description").arg( \
                                    m->name(), \
                                    std::move(description)); \
                } else { \
                    itemText = m->name(); \
                } \
                auto itemData = \
                        QVariant::fromValue( \
                            const_cast<void *>(static_cast<void const *>(m))); \
                m_ ## fname ## Combo->addItem(std::move(itemText), \
                                              std::move(itemData)); \
                if (m == defaultModule) \
                    m_ ## fname ## Combo->setCurrentIndex( \
                            m_ ## fname ## Combo->count() - 1); \
            } \
        } \
    } else (void) 0

    STANDARD_WORKS_TAB_ADD_ROW(standardBible, Bible);
    STANDARD_WORKS_TAB_ADD_ROW(standardCommentary, Commentary);
    STANDARD_WORKS_TAB_ADD_ROW(
                standardLexicon,
                Lexicon
                && !(m->category() == CSwordModuleInfo::DailyDevotional)
                && !(m->has(CSwordModuleInfo::FeatureHebrewDef)
                     && toLexModule(m)->hasStrongsKeys())
                && !(m->has(CSwordModuleInfo::FeatureGreekDef)
                     && toLexModule(m)->hasStrongsKeys())
                && !m->has(CSwordModuleInfo::FeatureHebrewParse)
                && !m->has(CSwordModuleInfo::FeatureGreekParse));
    STANDARD_WORKS_TAB_ADD_ROW(
                standardDailyDevotional,
                Lexicon && m->category() == CSwordModuleInfo::DailyDevotional);
    STANDARD_WORKS_TAB_ADD_ROW(
                standardHebrewStrongsLexicon,
                Lexicon
                && m->has(CSwordModuleInfo::FeatureHebrewDef)
                && toLexModule(m)->hasStrongsKeys());
    STANDARD_WORKS_TAB_ADD_ROW(
                standardGreekStrongsLexicon,
                Lexicon
                && m->has(CSwordModuleInfo::FeatureGreekDef)
                && toLexModule(m)->hasStrongsKeys());
    STANDARD_WORKS_TAB_ADD_ROW(
                standardHebrewMorphLexicon,
                Lexicon && m->has(CSwordModuleInfo::FeatureHebrewParse));
    STANDARD_WORKS_TAB_ADD_ROW(
                standardGreekMorphLexicon,
                Lexicon && m->has(CSwordModuleInfo::FeatureGreekParse));
#undef STANDARD_WORKS_TAB_ADD_ROW

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    retranslateUi();
}

#define STANDARD_WORKS_SET_DEFAULT(name) \
    btConfig().setDefaultSwordModuleByType(\
        #name, \
        static_cast<CSwordModuleInfo const *>( \
                m_ ## name ## Combo->currentData().value<void *>()))
void BtStandardWorksTab::save() {
    STANDARD_WORKS_SET_DEFAULT(standardBible);
    STANDARD_WORKS_SET_DEFAULT(standardCommentary);
    STANDARD_WORKS_SET_DEFAULT(standardLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardDailyDevotional);
    STANDARD_WORKS_SET_DEFAULT(standardHebrewStrongsLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardGreekStrongsLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardHebrewMorphLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardGreekMorphLexicon);
}
#undef STANDARD_WORKS_SET_DEFAULT

void BtStandardWorksTab::retranslateUi() {
    util::tool::initExplanationLabel(
        m_explanationLabel, QString(),
        tr("Standard works are used when no particular work is specified, for "
           "example when a hyperlink into a Bible or lexicon was clicked."));

    m_standardBibleLabel->setText(tr("Bible:"));
    m_standardBibleCombo->setToolTip(
                tr("The standard Bible is used when a hyperlink into a Bible "
                   "is clicked"));

    m_standardCommentaryLabel->setText(tr("Commentary:"));
    m_standardCommentaryCombo->setToolTip(
                tr("The standard commentary is used when a hyperlink into a "
                   "commentary is clicked"));

    m_standardLexiconLabel->setText(tr("Lexicon:"));
    m_standardLexiconCombo->setToolTip(
                tr("The standard lexicon is used when a hyperlink into a "
                   "lexicon is clicked"));

    m_standardDailyDevotionalLabel->setText(tr("Daily devotional:"));
    m_standardDailyDevotionalCombo->setToolTip(
                tr("The standard devotional will be used to display a short "
                   "start up devotional"));

    m_standardHebrewStrongsLexiconLabel->setText(
                tr("Hebrew Strong's lexicon:"));
    m_standardHebrewStrongsLexiconCombo->setToolTip(
                tr("The standard Hebrew lexicon is used when a hyperlink into "
                   "a Hebrew lexicon is clicked"));

    m_standardGreekStrongsLexiconLabel->setText(tr("Greek Strong's lexicon:"));
    m_standardGreekStrongsLexiconCombo->setToolTip(
                tr("The standard Greek lexicon is used when a hyperlink into a "
                   "Greek lexicon is clicked"));

    m_standardHebrewMorphLexiconLabel->setText(
                tr("Hebrew morphological lexicon:"));
    m_standardHebrewMorphLexiconCombo->setToolTip(
                tr("The standard morphological lexicon for Hebrew texts is "
                   "used when a hyperlink of a morphological tag in a Hebrew "
                   "text is clicked"));

    m_standardGreekMorphLexiconLabel->setText(
                tr("Greek morphological lexicon:"));
    m_standardGreekMorphLexiconCombo->setToolTip(
                tr("The standard morphological lexicon for Greek texts is used "
                   "when a hyperlink of a morphological tag in a Greek text is "
                   "clicked"));
}
