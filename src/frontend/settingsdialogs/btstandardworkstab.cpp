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

#include "btstandardworkstab.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordlexiconmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/tool.h"
#include "cswordsettings.h"


BtStandardWorksTab::BtStandardWorksTab(CSwordSettingsPage *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(5);
    mainLayout->setSpacing(2);

    m_explanationLabel = new QLabel(this);
    m_explanationLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    m_explanationLabel->setMaximumHeight(50);
    m_explanationLabel->setMinimumWidth(300);
    mainLayout->addWidget(m_explanationLabel);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

#define STANDARD_WORKS_TAB_ADD_ROW(name) \
    if (true) { \
        m_ ## name ## Label = new QLabel(this); \
        m_ ## name ## Combo = new QComboBox(this); \
        formLayout->addRow(m_ ## name ## Label, m_ ## name ## Combo); \
    } else (void) 0

    STANDARD_WORKS_TAB_ADD_ROW(standardBible);
    STANDARD_WORKS_TAB_ADD_ROW(standardCommentary);
    STANDARD_WORKS_TAB_ADD_ROW(standardLexicon);
    STANDARD_WORKS_TAB_ADD_ROW(standardDailyDevotional);
    STANDARD_WORKS_TAB_ADD_ROW(standardHebrewStrongsLexicon);
    STANDARD_WORKS_TAB_ADD_ROW(standardGreekStrongsLexicon);
    STANDARD_WORKS_TAB_ADD_ROW(standardHebrewMorphLexicon);
    STANDARD_WORKS_TAB_ADD_ROW(standardGreekMorphLexicon);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    //fill the comboboxes with the right modules

    QString modDescript;
    Q_FOREACH(CSwordModuleInfo const * const m,
              CSwordBackend::instance()->moduleList())
    {
        modDescript = m->config(CSwordModuleInfo::Description);

        switch (m->type()) {
            case CSwordModuleInfo::Bible:
                m_standardBibleCombo->addItem(modDescript);
                break;
            case CSwordModuleInfo::Commentary:
                m_standardCommentaryCombo->addItem(modDescript);
                break;
            case CSwordModuleInfo::Lexicon: {
                bool inserted = false;
                auto lexModule = qobject_cast<CSwordLexiconModuleInfo const *>(m);
                if (m->has(CSwordModuleInfo::HebrewDef) && lexModule->hasStrongsKeys()) {
                    m_standardHebrewStrongsLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if (m->has(CSwordModuleInfo::GreekDef) && lexModule->hasStrongsKeys()) {
                    m_standardGreekStrongsLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if (m->has(CSwordModuleInfo::HebrewParse)) {
                    m_standardHebrewMorphLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if (m->has(CSwordModuleInfo::GreekParse)) {
                    m_standardGreekMorphLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if (m->category() == CSwordModuleInfo::DailyDevotional) {
                    m_standardDailyDevotionalCombo->addItem(modDescript);
                    inserted = true;
                }

                if (!inserted) {    //daily dvotionals, striong lexicons etc. are not very useful for word lookups
                    m_standardLexiconCombo->addItem(modDescript);
                }
                break;
            }
            default://unknown type
                break;
        } //switch
    } //for

    //using two lists and one loop is better than six loops with almost the same code :)
    QList<QComboBox*> comboList;
    QStringList moduleList;

    // fill combobox and modulelist
    const CSwordModuleInfo* m;

#define STANDARD_WORKS_COMBO_ADD(name) \
    comboList.append(m_ ## name ## Combo); \
    m = btConfig().getDefaultSwordModuleByType(#name); \
    moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString());

        STANDARD_WORKS_COMBO_ADD(standardBible);
        STANDARD_WORKS_COMBO_ADD(standardCommentary);
        STANDARD_WORKS_COMBO_ADD(standardLexicon);
        STANDARD_WORKS_COMBO_ADD(standardDailyDevotional);
        STANDARD_WORKS_COMBO_ADD(standardHebrewStrongsLexicon);
        STANDARD_WORKS_COMBO_ADD(standardGreekStrongsLexicon);
        STANDARD_WORKS_COMBO_ADD(standardHebrewMorphLexicon);
        STANDARD_WORKS_COMBO_ADD(standardGreekMorphLexicon);

    QString module = QString();
    int item = 0;
    int count = 0;
    QListIterator<QComboBox*> it(comboList);
    while (it.hasNext()) {
        //for (QComboBox* combo = comboList.first(); combo; combo = comboList.next() )
        QComboBox* combo = it.next();
        module = moduleList[comboList.indexOf(combo)];
        count = combo->count();

        for (item = 0; item < count; item++) {
            if (combo->itemText(item) == module ) {
                combo->setCurrentIndex(item);
                break;
            }
        }
    }

    retranslateUi();
}

#define STANDARD_WORKS_SET_DEFAULT(name) \
    btConfig().setDefaultSwordModuleByType(\
        #name, \
        CSwordBackend::instance()->findModuleByDescription(m_ ## name ## Combo->currentText()) \
    );

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

void BtStandardWorksTab::retranslateUi() {
    util::tool::initExplanationLabel(
        m_explanationLabel, "",
        tr("Standard works are used when no particular work is specified, for example "
           "when a hyperlink into a Bible or lexicon was clicked."));

    m_standardBibleLabel->setText(tr("Bible:"));
    m_standardBibleCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardBibleCombo->setToolTip(tr("The standard Bible is used when a hyperlink into a Bible is clicked"));

    m_standardCommentaryLabel->setText(tr("Commentary:"));
    m_standardCommentaryCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardCommentaryCombo->setToolTip(tr("The standard commentary is used when a hyperlink into a commentary is clicked"));

    m_standardLexiconLabel->setText(tr("Lexicon:"));
    m_standardLexiconCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardLexiconCombo->setToolTip(tr("The standard lexicon is used when a hyperlink into a lexicon is clicked"));

    m_standardDailyDevotionalLabel->setText(tr("Daily devotional:"));
    m_standardDailyDevotionalCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardDailyDevotionalCombo->setToolTip(tr("The standard devotional will be used to display a short start up devotional"));

    m_standardHebrewStrongsLexiconLabel->setText(tr("Hebrew Strong's lexicon:"));
    m_standardHebrewStrongsLexiconCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardHebrewStrongsLexiconCombo->setToolTip(tr("The standard Hebrew lexicon is used when a hyperlink into a Hebrew lexicon is clicked"));

    m_standardGreekStrongsLexiconLabel->setText(tr("Greek Strong's lexicon:"));
    m_standardGreekStrongsLexiconCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardGreekStrongsLexiconCombo->setToolTip(tr("The standard Greek lexicon is used when a hyperlink into a Greek lexicon is clicked"));

    m_standardHebrewMorphLexiconLabel->setText(tr("Hebrew morphological lexicon:"));
    m_standardHebrewMorphLexiconCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardHebrewMorphLexiconCombo->setToolTip(tr("The standard morphological lexicon for Hebrew texts is used when a hyperlink of a morphological tag in a Hebrew text is clicked"));

    m_standardGreekMorphLexiconLabel->setText(tr("Greek morphological lexicon:"));
    m_standardGreekMorphLexiconCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    m_standardGreekMorphLexiconCombo->setToolTip(tr("The standard morphological lexicon for Greek texts is used when a hyperlink of a morphological tag in a Greek text is clicked"));
}
