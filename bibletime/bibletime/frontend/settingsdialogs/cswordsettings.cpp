//
// C++ Implementation: cswordsettings
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cswordsettings.h"

#include "frontend/cbtconfig.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/cpointers.h"

#include <QWidget>
#include <QTabWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QString>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QList>
#include <QStringList>
#include <QLabel>

#include <klocale.h>

CSwordSettingsPage::CSwordSettingsPage(QWidget* parent)
	: QTabWidget(parent)
{
	m_worksTab = new StandardWorksTab();
	m_filtersTab = new TextFiltersTab();
	addTab(m_worksTab, i18n("Standard works"));
	addTab(m_filtersTab, i18n("Text filters"));
}

//Standard works tab

StandardWorksTab::StandardWorksTab()
	: QWidget(0)
{
	
	// move: tabCtl->addTab(currentTab, i18n("Standard works"));
	QGridLayout* gridLayout = new QGridLayout(this); //the last row is for stretching available space
	gridLayout->setSizeConstraint(QLayout::SetMinimumSize);

	gridLayout->addWidget(
		CToolClass::explanationLabel(
				this,
				i18n("Standard works"),
				i18n("Standard works are used when no particular work is specified, \
  for example when a hyperlink into a Bible or lexicon was clicked .")),
		0,0,0,-1 /*fill the horizontal space*/
	);

	//Create selection boxes

	m_standardBibleCombo = new QComboBox(this);
	QLabel* label = new QLabel( i18n("Standard Bible"), this);
	label->setBuddy(m_standardBibleCombo);
	////label->setAutoResize(true); //? not found in docs
	m_standardBibleCombo->setToolTip(CResMgr::settings::sword::modules::bible::tooltip);

	gridLayout->addWidget(label,1,0);
	gridLayout->addWidget(m_standardBibleCombo, 1, 1);

	m_standardCommentaryCombo = new QComboBox(this);
	label = new QLabel( i18n("Standard Commentary"), this);
	label->setBuddy(m_standardCommentaryCombo);
	//label->setAutoResize(true);
	m_standardCommentaryCombo->setToolTip(CResMgr::settings::sword::modules::commentary::tooltip);

	gridLayout->addWidget(label, 2, 0);
	gridLayout->addWidget(m_standardCommentaryCombo, 2, 1);

	m_standardLexiconCombo = new QComboBox(this);
	label = new QLabel(i18n("Standard Lexicon"), this);
	label->setBuddy(m_standardLexiconCombo);
	//label->setAutoResize(true);
	m_standardLexiconCombo->setToolTip(CResMgr::settings::sword::modules::lexicon::tooltip);

	gridLayout->addWidget(label,3,0);
	gridLayout->addWidget(m_standardLexiconCombo, 3, 1);

	m_standardDailyDevotionalCombo = new QComboBox(this);
	label = new QLabel(i18n("Standard Daily Devotional"), this);
	label->setBuddy(m_standardDailyDevotionalCombo);
	//label->setAutoResize(true);
	m_standardDailyDevotionalCombo->setToolTip(CResMgr::settings::sword::modules::dailyDevotional::tooltip);

	gridLayout->addWidget(label,4,0);
	gridLayout->addWidget(m_standardDailyDevotionalCombo, 4, 1);

	m_standardHebrewStrongCombo = new QComboBox(this);
	label = new QLabel(i18n("Standard Hebrew Strong's Lexicon"), this);
	label->setBuddy(m_standardHebrewStrongCombo);
	//label->setAutoResize(true);
	m_standardHebrewStrongCombo->setToolTip(CResMgr::settings::sword::modules::hebrewStrongs::tooltip);

	gridLayout->addWidget(label, 5, 0);
	gridLayout->addWidget(m_standardHebrewStrongCombo, 5, 1);

	m_standardGreekStrongCombo = new QComboBox(this);
	label = new QLabel(i18n("Standard Greek Strong's Lexicon"), this);
	label->setBuddy(m_standardGreekStrongCombo);
	//label->setAutoResize(true);
	m_standardGreekStrongCombo->setToolTip(CResMgr::settings::sword::modules::greekStrongs::tooltip);

	gridLayout->addWidget(label, 6, 0);
	gridLayout->addWidget(m_standardGreekStrongCombo, 6, 1);

	m_standardHebrewMorphCombo = new QComboBox(this);
	label = new QLabel( i18n("Standard Hebrew Morphological Lexicon"), this);
	label->setBuddy(m_standardHebrewMorphCombo);
	//label->setAutoResize(true);
	m_standardHebrewMorphCombo->setToolTip(CResMgr::settings::sword::modules::hebrewMorph::tooltip);

	gridLayout->addWidget(label, 7, 0);
	gridLayout->addWidget(m_standardHebrewMorphCombo, 7, 1);

	m_standardGreekMorphCombo = new QComboBox(this);
	label = new QLabel(i18n("Standard Greek Morphological Lexicon"), this);
	label->setBuddy(m_standardGreekMorphCombo);
	//label->setAutoResize(true);
	m_standardGreekMorphCombo->setToolTip(CResMgr::settings::sword::modules::greekMorph::tooltip);

	gridLayout->addWidget(label, 8, 0);
	gridLayout->addWidget(m_standardGreekMorphCombo, 8, 1);

	gridLayout->setRowStretch(9,5);


	//fill the comboboxes with the right modules

	ListCSwordModuleInfo& modules = backend()->moduleList();
	QString modDescript;
	ListCSwordModuleInfo::iterator end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
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

				if (!inserted) {//daily dvotionals, striong lexicons etc. are not very useful for word lookups
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

	for (int i = 0; i <= (int)CBTConfig::lastModuleType; ++i) {
		//fill the combobox list in the right order (i.e. same order as the CBTConfig::module enum list)
		CBTConfig::modules moduleType = (CBTConfig::modules)(i);
		switch (moduleType) {
			case CBTConfig::standardBible:
				comboList.append(m_standardBibleCombo);
				break;
			case CBTConfig::standardCommentary:
				comboList.append(m_standardCommentaryCombo);
				break;
			case CBTConfig::standardLexicon:
				comboList.append(m_standardLexiconCombo);
				break;
			case CBTConfig::standardDailyDevotional:
				comboList.append(m_standardDailyDevotionalCombo);
				break;
			case CBTConfig::standardHebrewStrongsLexicon:
				comboList.append(m_standardHebrewStrongCombo);
				break;
			case CBTConfig::standardGreekStrongsLexicon:
				comboList.append(m_standardGreekStrongCombo);
				break;
			case CBTConfig::standardHebrewMorphLexicon:
				comboList.append(m_standardHebrewMorphCombo);
				break;
			case CBTConfig::standardGreekMorphLexicon:
				comboList.append(m_standardGreekMorphCombo);
				break;
		}; //switch

		//fill the module list
		CSwordModuleInfo* const m = CBTConfig::get( (CBTConfig::modules)(i) );
		if (m) {
			moduleList << m->config(CSwordModuleInfo::Description);
		}
		else {
			moduleList << QString::null;
		}
	} //for

	QString module = QString::null;
	int item = 0;
	int count = 0;
	QListIterator<QComboBox*> it(comboList);
	while (it.hasNext()) {
	//for (QComboBox* combo = comboList.first(); combo; combo = comboList.next() ) {
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


TextFiltersTab::TextFiltersTab()
{
	//Q3Frame* currentTab = new Q3Frame(tabCtl);
	//tabCtl->addTab(currentTab, i18n("Text filters"));
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(5);

	layout->addWidget(
		CToolClass::explanationLabel(
			this,
			i18n("Text filters"),
			i18n("Filters control the appearance of text. \
Here you can specify default settings for all filters. \
You can change the filter settings in each display window, of course.")
		)
	);

	layout->addSpacing(5);

	m_lineBreaksCheck = new QCheckBox(this);
	m_lineBreaksCheck->setText(i18n("Insert line break after each verse"));
	m_lineBreaksCheck->setChecked(CBTConfig::get(CBTConfig::lineBreaks));
	layout->addWidget(m_lineBreaksCheck);

	m_verseNumbersCheck = new QCheckBox(this);
	m_verseNumbersCheck->setText(i18n("Show verse numbers"));
	m_verseNumbersCheck->setChecked(CBTConfig::get(CBTConfig::verseNumbers));
	layout->addWidget(m_verseNumbersCheck);

	m_headingsCheck = new QCheckBox(this);
	m_headingsCheck->setText(i18n("Show section headings"));
	m_headingsCheck->setChecked(CBTConfig::get(CBTConfig::headings));
	layout->addWidget(m_headingsCheck);


	m_scriptureReferencesCheck = new QCheckBox(this);
	m_scriptureReferencesCheck->setText(i18n("Show scripture cross-references"));
	m_scriptureReferencesCheck->setChecked(CBTConfig::get(CBTConfig::scriptureReferences));
	layout->addWidget(m_scriptureReferencesCheck);

	m_greekAccentsCheck = new QCheckBox(this);
	m_greekAccentsCheck->setText(i18n("Show Greek accents"));
	m_greekAccentsCheck->setChecked(CBTConfig::get(CBTConfig::greekAccents));
	layout->addWidget(m_greekAccentsCheck);

	m_hebrewPointsCheck = new QCheckBox(this);
	m_hebrewPointsCheck->setText(i18n("Show Hebrew vowel points"));
	m_hebrewPointsCheck->setChecked(CBTConfig::get(CBTConfig::hebrewPoints));
	layout->addWidget(m_hebrewPointsCheck);

	m_hebrewCantillationCheck = new QCheckBox(this);
	m_hebrewCantillationCheck->setText(i18n("Show Hebrew cantillation marks"));
	m_hebrewCantillationCheck->setChecked(CBTConfig::get(CBTConfig::hebrewCantillation));
	layout->addWidget(m_hebrewCantillationCheck);

	m_morphSegmentationCheck = new QCheckBox(this);
	m_morphSegmentationCheck->setText(i18n("Show morph segmentation"));
	m_morphSegmentationCheck->setChecked(CBTConfig::get(CBTConfig::morphSegmentation));
	layout->addWidget(m_morphSegmentationCheck);

	m_textualVariantsCheck = new QCheckBox(this);
	m_textualVariantsCheck->setText(i18n("Use textual variants"));
	m_textualVariantsCheck->setChecked(CBTConfig::get(CBTConfig::textualVariants));
	layout->addWidget(m_textualVariantsCheck);

	layout->addStretch(4);
}

void CSwordSettingsPage::save()
{
	m_worksTab->save();
	m_filtersTab->save();	
}

void StandardWorksTab::save()
{
	for (int i = 0; i <= (int)CBTConfig::lastModuleType; ++i) {
		QString moduleDescription = QString::null;

		CBTConfig::modules moduleType = (CBTConfig::modules)(i);
		switch (moduleType) {
			case CBTConfig::standardBible:
				moduleDescription = m_standardBibleCombo->currentText();
				break;
			case CBTConfig::standardCommentary:
				moduleDescription = m_standardCommentaryCombo->currentText();
				break;
			case CBTConfig::standardLexicon:
				moduleDescription = m_standardLexiconCombo->currentText();
				break;
			case CBTConfig::standardDailyDevotional:
				moduleDescription = m_standardDailyDevotionalCombo->currentText();
				break;
			case CBTConfig::standardHebrewStrongsLexicon:
				moduleDescription = m_standardHebrewStrongCombo->currentText();
				break;
			case CBTConfig::standardGreekStrongsLexicon:
				moduleDescription = m_standardGreekStrongCombo->currentText();
				break;
			case CBTConfig::standardHebrewMorphLexicon:
				moduleDescription = m_standardHebrewMorphCombo->currentText();
				break;
			case CBTConfig::standardGreekMorphLexicon:
				moduleDescription = m_standardGreekMorphCombo->currentText();
				break;
			default:
				qWarning("Unhandled module type.");
		};

		CSwordModuleInfo* const module = backend()->findModuleByDescription(moduleDescription);
		CBTConfig::set
			(moduleType, module);
	}
}


void TextFiltersTab::save()
{
	CBTConfig::set(CBTConfig::lineBreaks, m_lineBreaksCheck->isChecked());
	CBTConfig::set(CBTConfig::verseNumbers, m_verseNumbersCheck->isChecked());
	CBTConfig::set(CBTConfig::headings, m_headingsCheck->isChecked());
	CBTConfig::set(CBTConfig::scriptureReferences, m_scriptureReferencesCheck->isChecked());
	CBTConfig::set(CBTConfig::hebrewPoints, m_hebrewPointsCheck->isChecked());
	CBTConfig::set(CBTConfig::hebrewCantillation, m_hebrewCantillationCheck->isChecked());
	CBTConfig::set(CBTConfig::morphSegmentation, m_morphSegmentationCheck->isChecked());
	CBTConfig::set(CBTConfig::greekAccents, m_greekAccentsCheck->isChecked());
	CBTConfig::set(CBTConfig::textualVariants, m_textualVariantsCheck->isChecked());
}