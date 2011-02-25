/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/settingsdialogs/cswordsettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/tool.h"
#include "backend/managers/cswordbackend.h"
#include "backend/config/btconfig.h"


CSwordSettingsPage::CSwordSettingsPage(QWidget *parent)
        : BtConfigPage(parent)
{
    Q_ASSERT(qobject_cast<QVBoxLayout*>(layout()) != 0);
    QVBoxLayout *vbox = static_cast<QVBoxLayout*>(layout());

    QTabWidget* tabWidget = new QTabWidget();
    vbox->addWidget(tabWidget);

    m_worksTab = new StandardWorksTab();
    m_filtersTab = new TextFiltersTab();
    tabWidget->addTab(m_worksTab, tr("Standard works"));
    tabWidget->addTab(m_filtersTab, tr("Text filters"));
}

//Standard works tab

StandardWorksTab::StandardWorksTab()
        : QWidget(0) {
    typedef QList<CSwordModuleInfo*>::const_iterator MLCI;

    // move: tabCtl->addTab(currentTab, tr("Standard works"));
    QGridLayout* gridLayout = new QGridLayout(this); //the last row is for stretching available space
    gridLayout->setSizeConstraint(QLayout::SetMinimumSize);

    gridLayout->addWidget(
        util::tool::explanationLabel(
            this, "",
            tr("Standard works are used when no particular work is specified, for example "
               "when a hyperlink into a Bible or lexicon was clicked.")),
        0, 0, 1, 2 /*fill the horizontal space*/
    );

    //Create selection boxes

    m_standardBibleCombo = new QComboBox(this);
    QLabel* label = new QLabel( tr("Bible:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardBibleCombo);
    ////label->setAutoResize(true); //? not found in docs
    m_standardBibleCombo->setToolTip(tr("The standard Bible is used when a hyperlink into a Bible is clicked"));

    gridLayout->addWidget(label, 1, 0);
    gridLayout->addWidget(m_standardBibleCombo, 1, 1);

    m_standardCommentaryCombo = new QComboBox(this);
    label = new QLabel( tr("Commentary:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardCommentaryCombo);
    //label->setAutoResize(true);
    m_standardCommentaryCombo->setToolTip(tr("The standard commentary is used when a hyperlink into a commentary is clicked"));

    gridLayout->addWidget(label, 2, 0);
    gridLayout->addWidget(m_standardCommentaryCombo, 2, 1);

    m_standardLexiconCombo = new QComboBox(this);
    label = new QLabel(tr("Lexicon:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardLexiconCombo);
    //label->setAutoResize(true);
    m_standardLexiconCombo->setToolTip(tr("The standard lexicon is used when a hyperlink into a lexicon is clicked"));

    gridLayout->addWidget(label, 3, 0);
    gridLayout->addWidget(m_standardLexiconCombo, 3, 1);

    m_standardDailyDevotionalCombo = new QComboBox(this);
    label = new QLabel(tr("Daily devotional:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardDailyDevotionalCombo);
    //label->setAutoResize(true);
    m_standardDailyDevotionalCombo->setToolTip(tr("The standard devotional will be used to display a short start up devotional"));

    gridLayout->addWidget(label, 4, 0);
    gridLayout->addWidget(m_standardDailyDevotionalCombo, 4, 1);

    m_standardHebrewStrongCombo = new QComboBox(this);
    label = new QLabel(tr("Hebrew Strong's lexicon:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardHebrewStrongCombo);
    //label->setAutoResize(true);
    m_standardHebrewStrongCombo->setToolTip(tr("The standard Hebrew lexicon is used when a hyperlink into a Hebrew lexicon is clicked"));

    gridLayout->addWidget(label, 5, 0);
    gridLayout->addWidget(m_standardHebrewStrongCombo, 5, 1);

    m_standardGreekStrongCombo = new QComboBox(this);
    label = new QLabel(tr("Greek Strong's lexicon:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardGreekStrongCombo);
    //label->setAutoResize(true);
    m_standardGreekStrongCombo->setToolTip(tr("The standard Greek lexicon is used when a hyperlink into a Greek lexicon is clicked"));

    gridLayout->addWidget(label, 6, 0);
    gridLayout->addWidget(m_standardGreekStrongCombo, 6, 1);

    m_standardHebrewMorphCombo = new QComboBox(this);
    label = new QLabel( tr("Hebrew morphological lexicon:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardHebrewMorphCombo);
    //label->setAutoResize(true);
    m_standardHebrewMorphCombo->setToolTip(tr("The standard morphological lexicon for Hebrew texts is used when a hyperlink of a morphological tag in a Hebrew text is clicked"));

    gridLayout->addWidget(label, 7, 0);
    gridLayout->addWidget(m_standardHebrewMorphCombo, 7, 1);

    m_standardGreekMorphCombo = new QComboBox(this);
    label = new QLabel(tr("Greek morphological lexicon:"), this);
    label->setAlignment(Qt::AlignRight);
    label->setBuddy(m_standardGreekMorphCombo);
    //label->setAutoResize(true);
    m_standardGreekMorphCombo->setToolTip(tr("The standard morphological lexicon for Greek texts is used when a hyperlink of a morphological tag in a Greek text is clicked"));

    gridLayout->addWidget(label, 8, 0);
    gridLayout->addWidget(m_standardGreekMorphCombo, 8, 1);

    gridLayout->setRowStretch(9, 5);

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
                    m_standardHebrewStrongCombo->addItem(modDescript);
                    inserted = true;
                }
                if ((*it)->has(CSwordModuleInfo::GreekDef)) {
                    m_standardGreekStrongCombo->addItem(modDescript);
                    inserted = true;
                }
                if ((*it)->has(CSwordModuleInfo::HebrewParse)) {
                    m_standardHebrewMorphCombo->addItem(modDescript);
                    inserted = true;
                }
                if ((*it)->has(CSwordModuleInfo::GreekParse)) {
                    m_standardGreekMorphCombo->addItem(modDescript);
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
        const CSwordModuleInfo*  m;
    // fill combobox and modulelist
        comboList.append(m_standardBibleCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardBible");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);

        comboList.append(m_standardCommentaryCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardCommentary");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);

        comboList.append(m_standardLexiconCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardLexicon");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);
        
        comboList.append(m_standardDailyDevotionalCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardDailyDevotional");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);
        
        comboList.append(m_standardHebrewStrongCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardHebrewStrongsLexicon");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);
        
        comboList.append(m_standardGreekStrongCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardGreekStrongsLexicon");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);
        
        comboList.append(m_standardHebrewMorphCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardHebrewMorphLexicon");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);
        
        comboList.append(m_standardGreekMorphCombo);
        m = getBtConfig().getDefaultSwordModuleByType("standardGreekMorphLexicon");
        moduleList << (m != 0 ? m->config(CSwordModuleInfo::Description) : QString::null);

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
}


TextFiltersTab::TextFiltersTab() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(5);
    QWidget* eLabel = util::tool::explanationLabel(
                          this, "",
                          tr("Filters control the appearance of text. Here you can specify "
                             "default settings for all filters. You can override these "
                             "settings in each display window.")
                      );
    eLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    eLabel->setMaximumHeight(50);
    eLabel->setMinimumWidth(300);
    layout->setSpacing(2);
    layout->addWidget(eLabel);

    getBtConfig().beginGroup("presentation");
        m_lineBreaksCheck = new QCheckBox(this);
        m_lineBreaksCheck->setText(tr("Insert line break after each verse"));
        m_lineBreaksCheck->setChecked(getBtConfig().getValue<bool>("lineBreaks"));
        layout->addWidget(m_lineBreaksCheck);

        m_verseNumbersCheck = new QCheckBox(this);
        m_verseNumbersCheck->setText(tr("Show verse numbers"));
        m_verseNumbersCheck->setChecked(getBtConfig().getValue<bool>("verseNumbers"));
        layout->addWidget(m_verseNumbersCheck);

        m_headingsCheck = new QCheckBox(this);
        m_headingsCheck->setText(tr("Show section headings"));
        m_headingsCheck->setChecked(getBtConfig().getValue<bool>("headings"));
        layout->addWidget(m_headingsCheck);

        m_scriptureReferencesCheck = new QCheckBox(this);
        m_scriptureReferencesCheck->setText(tr("Show scripture cross-references"));
        m_scriptureReferencesCheck->setChecked(getBtConfig().getValue<bool>("scriptureReferences"));
        layout->addWidget(m_scriptureReferencesCheck);

        m_greekAccentsCheck = new QCheckBox(this);
        m_greekAccentsCheck->setText(tr("Show Greek accents"));
        m_greekAccentsCheck->setChecked(getBtConfig().getValue<bool>("greekAccents"));
        layout->addWidget(m_greekAccentsCheck);

        m_hebrewPointsCheck = new QCheckBox(this);
        m_hebrewPointsCheck->setText(tr("Show Hebrew vowel points"));
        m_hebrewPointsCheck->setChecked(getBtConfig().getValue<bool>("hebrewPoints"));
        layout->addWidget(m_hebrewPointsCheck);

        m_hebrewCantillationCheck = new QCheckBox(this);
        m_hebrewCantillationCheck->setText(tr("Show Hebrew cantillation marks"));
        m_hebrewCantillationCheck->setChecked(getBtConfig().getValue<bool>("hebrewCantillation"));
        layout->addWidget(m_hebrewCantillationCheck);

        m_morphSegmentationCheck = new QCheckBox(this);
        m_morphSegmentationCheck->setText(tr("Show morph segmentation"));
        m_morphSegmentationCheck->setChecked(getBtConfig().getValue<bool>("morphSegmentation"));
        layout->addWidget(m_morphSegmentationCheck);

        m_textualVariantsCheck = new QCheckBox(this);
        m_textualVariantsCheck->setText(tr("Use textual variants"));
        m_textualVariantsCheck->setChecked(getBtConfig().getValue<bool>("textualVariants"));
        layout->addWidget(m_textualVariantsCheck);
    getBtConfig().endGroup();

    layout->addStretch(4);
}

void CSwordSettingsPage::save() {
    m_worksTab->save();
    m_filtersTab->save();
}

const QIcon &CSwordSettingsPage::icon() const {
    return util::directory::getIcon(CResMgr::settings::sword::icon);
}

QString CSwordSettingsPage::header() const {
    return tr("Desk");
}

void StandardWorksTab::save() {
    getBtConfig().setDefaultSwordModuleByType("standardBible",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardBibleCombo->currentText())
                                             );
    getBtConfig().setDefaultSwordModuleByType("standardCommentary",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardCommentaryCombo->currentText())
                                             );
    getBtConfig().setDefaultSwordModuleByType("standardLexicon",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardLexiconCombo->currentText())
                                             );
    getBtConfig().setDefaultSwordModuleByType("standardDailyDevotional",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardDailyDevotionalCombo->currentText())
                                             );
    getBtConfig().setDefaultSwordModuleByType("standardHebrewStrongsLexicon",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardHebrewStrongCombo->currentText())
                                             );
    getBtConfig().setDefaultSwordModuleByType("standardGreekStrongsLexicon",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardGreekStrongCombo->currentText())
                                             );
    getBtConfig().setDefaultSwordModuleByType("standardHebrewMorphLexicon",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardHebrewMorphCombo->currentText())
                                             );
    getBtConfig().setDefaultSwordModuleByType("standardGreekMorphLexicon",
                                              CSwordBackend::instance()->findModuleByDescription(m_standardGreekMorphCombo->currentText())
                                             );
}

void TextFiltersTab::save() {
    getBtConfig().beginGroup("presentation");
        getBtConfig().setValue("lineBreaks", m_lineBreaksCheck->isChecked());
        getBtConfig().setValue("verseNumbers", m_verseNumbersCheck->isChecked());
        getBtConfig().setValue("headings", m_headingsCheck->isChecked());
        getBtConfig().setValue("scriptureReferences", m_scriptureReferencesCheck->isChecked());
        getBtConfig().setValue("hebrewPoints", m_hebrewPointsCheck->isChecked());
        getBtConfig().setValue("hebrewCantillation", m_hebrewCantillationCheck->isChecked());
        getBtConfig().setValue("morphSegmentation", m_morphSegmentationCheck->isChecked());
        getBtConfig().setValue("greekAccents", m_greekAccentsCheck->isChecked());
        getBtConfig().setValue("textualVariants", m_textualVariantsCheck->isChecked());
    getBtConfig().endGroup();
}


