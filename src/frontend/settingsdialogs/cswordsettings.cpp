/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/cswordsettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "backend/config/btconfig.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/tool.h"
#include "backend/managers/cswordbackend.h"


/*******************************************************************************
  StandardWorksTab
*******************************************************************************/

class StandardWorksTab: public QWidget {

    public: /* Methods: */

        StandardWorksTab(CSwordSettingsPage *parent);

        void save();

    protected: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

        QLabel *m_explanationLabel;

#define STANDARD_WORKS_TAB_FIELD(name) \
        QLabel *m_ ## name ## Label; \
        QComboBox *m_ ## name ## Combo

        STANDARD_WORKS_TAB_FIELD(standardBible);
        STANDARD_WORKS_TAB_FIELD(standardCommentary);
        STANDARD_WORKS_TAB_FIELD(standardLexicon);
        STANDARD_WORKS_TAB_FIELD(standardDailyDevotional);
        STANDARD_WORKS_TAB_FIELD(standardHebrewStrongsLexicon);
        STANDARD_WORKS_TAB_FIELD(standardGreekStrongsLexicon);
        STANDARD_WORKS_TAB_FIELD(standardHebrewMorphLexicon);
        STANDARD_WORKS_TAB_FIELD(standardGreekMorphLexicon);
};

StandardWorksTab::StandardWorksTab(CSwordSettingsPage *parent)
    : QWidget(parent)
{
    typedef QList<CSwordModuleInfo*>::const_iterator MLCI;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    m_explanationLabel = new QLabel(this);
    mainLayout->addWidget(m_explanationLabel);

    QFormLayout *formLayout = new QFormLayout;

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

    //fill the comboboxes with the right modules

    const QList<CSwordModuleInfo*> &modules(CSwordBackend::instance()->moduleList());
    QString modDescript;
    for (MLCI it(modules.begin()); it != modules.end(); it++) {
        modDescript = (*it)->config(CSwordModuleInfo::Description);

        switch ((*it)->type()) {
            case CSwordModuleInfo::Bible:
                m_standardBibleCombo->addItem(modDescript);
                break;
            case CSwordModuleInfo::Commentary:
                m_standardCommentaryCombo->addItem(modDescript);
                break;
            case CSwordModuleInfo::Lexicon: {
                bool inserted = false;
                if ((*it)->has(CSwordModuleInfo::HebrewDef)) {
                    m_standardHebrewStrongsLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if ((*it)->has(CSwordModuleInfo::GreekDef)) {
                    m_standardGreekStrongsLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if ((*it)->has(CSwordModuleInfo::HebrewParse)) {
                    m_standardHebrewMorphLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if ((*it)->has(CSwordModuleInfo::GreekParse)) {
                    m_standardGreekMorphLexiconCombo->addItem(modDescript);
                    inserted = true;
                }
                if ((*it)->category() == CSwordModuleInfo::DailyDevotional) {
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
        const CSwordModuleInfo*  m;

#define STANDARD_WORKS_COMBO_ADD(name) \
    comboList.append(m_ ## name ## Combo); \
    m = getBtConfig().getDefaultSwordModuleByType(#name); \
    moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);
        STANDARD_WORKS_COMBO_ADD(standardBible);
        STANDARD_WORKS_COMBO_ADD(standardCommentary);
        STANDARD_WORKS_COMBO_ADD(standardLexicon);
        STANDARD_WORKS_COMBO_ADD(standardDailyDevotional);
        STANDARD_WORKS_COMBO_ADD(standardHebrewStrongsLexicon);
        STANDARD_WORKS_COMBO_ADD(standardGreekStrongsLexicon);
        STANDARD_WORKS_COMBO_ADD(standardHebrewMorphLexicon);
        STANDARD_WORKS_COMBO_ADD(standardGreekMorphLexicon);

    QString module = QString::null;
    int item = 0;
    int count = 0;
    QListIterator<QComboBox*> it(comboList);
    while (it.hasNext()) {
        //for (QComboBox* combo = comboList.first(); combo; combo = comboList.next() )
        QComboBox* combo = it.next();
        module = moduleList[comboList.indexOf(combo)];
        count = combo->count();
        combo->setMaximumWidth(300);

        for (item = 0; item < count; item++) {
            if (combo->itemText(item) == module ) {
                combo->setCurrentIndex(item);
                break;
            }
        }
    }

    retranslateUi();
}

void StandardWorksTab::save() {
#define STANDARD_WORKS_SET_DEFAULT(name) \
    getBtConfig().setDefaultSwordModuleByType(#name, \
                                    CSwordBackend::instance()->findModuleByDescription(m_ ## name ## Combo->currentText()) \
                                    );

    STANDARD_WORKS_SET_DEFAULT(standardBible);
    STANDARD_WORKS_SET_DEFAULT(standardCommentary);
    STANDARD_WORKS_SET_DEFAULT(standardLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardDailyDevotional);
    STANDARD_WORKS_SET_DEFAULT(standardHebrewStrongsLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardGreekStrongsLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardHebrewMorphLexicon);
    STANDARD_WORKS_SET_DEFAULT(standardGreekMorphLexicon);
}

void StandardWorksTab::retranslateUi() {
    util::tool::initExplanationLabel(
        m_explanationLabel, "",
        tr("Standard works are used when no particular work is specified, for example "
           "when a hyperlink into a Bible or lexicon was clicked."));

    m_standardBibleLabel->setText(tr("Bible:"));
    m_standardBibleCombo->setToolTip(tr("The standard Bible is used when a hyperlink into a Bible is clicked"));

    m_standardCommentaryLabel->setText(tr("Commentary:"));
    m_standardCommentaryCombo->setToolTip(tr("The standard commentary is used when a hyperlink into a commentary is clicked"));

    m_standardLexiconLabel->setText(tr("Lexicon:"));
    m_standardLexiconCombo->setToolTip(tr("The standard lexicon is used when a hyperlink into a lexicon is clicked"));

    m_standardDailyDevotionalLabel->setText(tr("Daily devotional:"));
    m_standardDailyDevotionalCombo->setToolTip(tr("The standard devotional will be used to display a short start up devotional"));

    m_standardHebrewStrongsLexiconLabel->setText(tr("Hebrew Strong's lexicon:"));
    m_standardHebrewStrongsLexiconCombo->setToolTip(tr("The standard Hebrew lexicon is used when a hyperlink into a Hebrew lexicon is clicked"));

    m_standardGreekStrongsLexiconLabel->setText(tr("Greek Strong's lexicon:"));
    m_standardGreekStrongsLexiconCombo->setToolTip(tr("The standard Greek lexicon is used when a hyperlink into a Greek lexicon is clicked"));

    m_standardHebrewMorphLexiconLabel->setText(tr("Hebrew morphological lexicon:"));
    m_standardHebrewMorphLexiconCombo->setToolTip(tr("The standard morphological lexicon for Hebrew texts is used when a hyperlink of a morphological tag in a Hebrew text is clicked"));

    m_standardGreekMorphLexiconLabel->setText(tr("Greek morphological lexicon:"));
    m_standardGreekMorphLexiconCombo->setToolTip(tr("The standard morphological lexicon for Greek texts is used when a hyperlink of a morphological tag in a Greek text is clicked"));
}


/*******************************************************************************
  TextFiltersTab
*******************************************************************************/

class TextFiltersTab: public QWidget {

    public: /* Methods: */

        TextFiltersTab(CSwordSettingsPage *parent);

        void save();

    protected: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

        QLabel *m_explanationLabel;

#define TEXT_FILTERS_TAB_FIELD(name) QCheckBox *m_ ## name ## Check

        TEXT_FILTERS_TAB_FIELD(lineBreaks);
        TEXT_FILTERS_TAB_FIELD(verseNumbers);
        TEXT_FILTERS_TAB_FIELD(headings);
        TEXT_FILTERS_TAB_FIELD(hebrewPoints);
        TEXT_FILTERS_TAB_FIELD(hebrewCantillation);
        TEXT_FILTERS_TAB_FIELD(morphSegmentation);
        TEXT_FILTERS_TAB_FIELD(greekAccents);
        TEXT_FILTERS_TAB_FIELD(textualVariants);
        TEXT_FILTERS_TAB_FIELD(scriptureReferences);

};

TextFiltersTab::TextFiltersTab(CSwordSettingsPage *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(5);

    m_explanationLabel = new QLabel(this);
    m_explanationLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    m_explanationLabel->setMaximumHeight(50);
    m_explanationLabel->setMinimumWidth(300);
    layout->setSpacing(2);
    layout->addWidget(m_explanationLabel);

    getBtConfig().beginGroup("presentation");
#define TEXT_FILTERS_TAB_ADD_ROW(name) \
        m_ ## name ## Check = new QCheckBox(this); \
        m_ ## name ## Check->setChecked(getBtConfig().getValue<bool>(#name)); \
        layout->addWidget(m_ ## name ## Check);

        TEXT_FILTERS_TAB_ADD_ROW(lineBreaks);
        TEXT_FILTERS_TAB_ADD_ROW(verseNumbers);
        TEXT_FILTERS_TAB_ADD_ROW(headings);
        TEXT_FILTERS_TAB_ADD_ROW(hebrewPoints);
        TEXT_FILTERS_TAB_ADD_ROW(hebrewCantillation);
        TEXT_FILTERS_TAB_ADD_ROW(morphSegmentation);
        TEXT_FILTERS_TAB_ADD_ROW(greekAccents);
        TEXT_FILTERS_TAB_ADD_ROW(textualVariants);
        TEXT_FILTERS_TAB_ADD_ROW(scriptureReferences);
    getBtConfig().endGroup();

    layout->addStretch(4);

    retranslateUi();
}

void TextFiltersTab::save() {
    getBtConfig().beginGroup("presentation");
#define TEXT_FILTERS_TAB_SAVE(name) getBtConfig().setValue(#name, m_ ## name ## Check->isChecked())
        TEXT_FILTERS_TAB_SAVE(lineBreaks);
        TEXT_FILTERS_TAB_SAVE(verseNumbers);
        TEXT_FILTERS_TAB_SAVE(headings);
        TEXT_FILTERS_TAB_SAVE(hebrewPoints);
        TEXT_FILTERS_TAB_SAVE(hebrewCantillation);
        TEXT_FILTERS_TAB_SAVE(morphSegmentation);
        TEXT_FILTERS_TAB_SAVE(greekAccents);
        TEXT_FILTERS_TAB_SAVE(textualVariants);
        TEXT_FILTERS_TAB_SAVE(scriptureReferences);
    getBtConfig().endGroup();
}


void TextFiltersTab::retranslateUi() {
    util::tool::initExplanationLabel(m_explanationLabel, "",
          tr("Filters control the appearance of text. Here you can specify "
             "default settings for all filters. You can override these "
             "settings in each display window."));

    m_lineBreaksCheck->setText(tr("Insert line break after each verse"));
    m_verseNumbersCheck->setText(tr("Show verse numbers"));
    m_headingsCheck->setText(tr("Show section headings"));
    m_scriptureReferencesCheck->setText(tr("Show scripture cross-references"));
    m_greekAccentsCheck->setText(tr("Show Greek accents"));
    m_hebrewPointsCheck->setText(tr("Show Hebrew vowel points"));
    m_hebrewCantillationCheck->setText(tr("Show Hebrew cantillation marks"));
    m_morphSegmentationCheck->setText(tr("Show morph segmentation"));
    m_textualVariantsCheck->setText(tr("Use textual variants"));
}

/*******************************************************************************
  CSwordSettingsPage
*******************************************************************************/

CSwordSettingsPage::CSwordSettingsPage(CConfigurationDialog *parent)
        : BtConfigDialog::Page(util::directory::getIcon(CResMgr::settings::sword::icon), parent)
{
    static const QString nullString;

    m_tabWidget = new QTabWidget(this);
        m_worksTab = new StandardWorksTab(this);
        m_tabWidget->addTab(m_worksTab, nullString);

        m_filtersTab = new TextFiltersTab(this);
        m_tabWidget->addTab(m_filtersTab, nullString);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);

    retranslateUi();
}

void CSwordSettingsPage::retranslateUi() {
    setHeaderText(tr("Desk"));

    m_tabWidget->setTabText(m_tabWidget->indexOf(m_worksTab), tr("Standard works"));
    m_tabWidget->setTabText(m_tabWidget->indexOf(m_filtersTab), tr("Text filters"));
}

void CSwordSettingsPage::save() {
    m_worksTab->save();
    m_filtersTab->save();
}
