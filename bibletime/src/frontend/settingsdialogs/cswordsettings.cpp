//
// C++ Implementation: cswordsettings
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "cswordsettings.h"
#include "cswordsettings.moc"

#include "backend/config/cbtconfig.h"

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



CSwordSettingsPage::CSwordSettingsPage(QWidget* /*parent*/)
	: BtConfigPage()
{
	QVBoxLayout* vbox = new QVBoxLayout(this);
	QTabWidget* tabWidget = new QTabWidget();
	vbox->addWidget(tabWidget);
	setLayout(vbox);

	m_worksTab = new StandardWorksTab();
	m_filtersTab = new TextFiltersTab();
	tabWidget->addTab(m_worksTab, tr("Standard works"));
	tabWidget->addTab(m_filtersTab, tr("Text filters"));
}

//Standard works tab

StandardWorksTab::StandardWorksTab()
	: QWidget(0)
{
	
	// move: tabCtl->addTab(currentTab, tr("Standard works"));
	QGridLayout* gridLayout = new QGridLayout(this); //the last row is for stretching available space
	gridLayout->setSizeConstraint(QLayout::SetMinimumSize);

	gridLayout->addWidget(
		CToolClass::explanationLabel(
				this,
				tr(""),
				tr("Standard works are used when no particular work is specified, \
  for example when a hyperlink into a Bible or lexicon was clicked .")),
		0,0,1,2 /*fill the horizontal space*/
	);

	//Create selection boxes

	m_standardBibleCombo = new QComboBox(this);
	QLabel* label = new QLabel( tr("Bible:"), this);
	label->setAlignment(Qt::AlignRight);
	label->setBuddy(m_standardBibleCombo);
	////label->setAutoResize(true); //? not found in docs
	m_standardBibleCombo->setToolTip(tr("The standard Bible is used when a hyperlink into a Bible is clicked"));

	gridLayout->addWidget(label,1,0);
	gridLayout->setVerticalSpacing(0);
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

	gridLayout->addWidget(label,3,0);
	gridLayout->addWidget(m_standardLexiconCombo, 3, 1);

	m_standardDailyDevotionalCombo = new QComboBox(this);
	label = new QLabel(tr("Daily devotional:"), this);
	label->setAlignment(Qt::AlignRight);
	label->setBuddy(m_standardDailyDevotionalCombo);
	//label->setAutoResize(true);
	m_standardDailyDevotionalCombo->setToolTip(tr("The standard devotional will be used to display a short start up devotional"));

	gridLayout->addWidget(label,4,0);
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

	//fill the comboboxes with the right modules

	QList<CSwordModuleInfo*>& modules = backend()->moduleList();
	QString modDescript;
	QList<CSwordModuleInfo*>::iterator end_it = modules.end();
	for (QList<CSwordModuleInfo*>::iterator it(modules.begin()); it != end_it; ++it) 
	{
		modDescript = (*it)->config(CSwordModuleInfo::Description);

		switch ((*it)->type()) 
		{
			case CSwordModuleInfo::Bible:
				m_standardBibleCombo->addItem(modDescript);
				break;
			case CSwordModuleInfo::Commentary:
				m_standardCommentaryCombo->addItem(modDescript);
				break;
			case CSwordModuleInfo::Lexicon: 
				{
				bool inserted = false;
				if ((*it)->has(CSwordModuleInfo::HebrewDef)) 
				{
					m_standardHebrewStrongCombo->addItem(modDescript);
					inserted = true;
				}
				if ((*it)->has(CSwordModuleInfo::GreekDef)) 
				{
					m_standardGreekStrongCombo->addItem(modDescript);
					inserted = true;
				}
				if ((*it)->has(CSwordModuleInfo::HebrewParse)) 
				{
					m_standardHebrewMorphCombo->addItem(modDescript);
					inserted = true;
				}
				if ((*it)->has(CSwordModuleInfo::GreekParse)) 
				{
					m_standardGreekMorphCombo->addItem(modDescript);
					inserted = true;
				}
				if ((*it)->category() == CSwordModuleInfo::DailyDevotional) 
				{
					m_standardDailyDevotionalCombo->addItem(modDescript);
					inserted = true;
				}

				if (!inserted) 
				{	//daily dvotionals, striong lexicons etc. are not very useful for word lookups
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

	for (int i = 0; i <= (int)CBTConfig::lastModuleType; ++i) 
	{
		//fill the combobox list in the right order (i.e. same order as the CBTConfig::module enum list)
		CBTConfig::modules moduleType = (CBTConfig::modules)(i);
		switch (moduleType) 
		{
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
		if (m) 
		{
			moduleList << m->config(CSwordModuleInfo::Description);
		}
		else 
		{
			moduleList << QString::null;
		}
	} //for

	QString module = QString::null;
	int item = 0;
	int count = 0;
	QListIterator<QComboBox*> it(comboList);
	while (it.hasNext()) 
	{
	//for (QComboBox* combo = comboList.first(); combo; combo = comboList.next() ) 
		QComboBox* combo = it.next();
		module = moduleList[comboList.indexOf(combo)];
		count = combo->count();
		combo->setMaximumWidth(300);

		for (item = 0; item < count; item++) 
		{
			if (combo->itemText(item) == module ) 
			{
				combo->setCurrentIndex(item);
				break;
			}
		}
	}
}


TextFiltersTab::TextFiltersTab()
{
	QVBoxLayout* layout = new QVBoxLayout(this);	
	layout->setMargin(5);
	QWidget* eLabel = CToolClass::explanationLabel(
			this,
			tr(""),
			tr("Filters control the appearance of text. \
Here you can specify default settings for all filters. \
You can override these settings in each display window.")
		);
	eLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	eLabel->setMaximumHeight(50);
	eLabel->setMinimumWidth(300);
	layout->setSpacing(2);
	layout->addWidget(eLabel);

	m_lineBreaksCheck = new QCheckBox(this);
	m_lineBreaksCheck->setText(tr("Insert line break after each verse"));
	m_lineBreaksCheck->setChecked(CBTConfig::get(CBTConfig::lineBreaks));
	layout->addWidget(m_lineBreaksCheck);

	m_verseNumbersCheck = new QCheckBox(this);
	m_verseNumbersCheck->setText(tr("Show verse numbers"));
	m_verseNumbersCheck->setChecked(CBTConfig::get(CBTConfig::verseNumbers));
	layout->addWidget(m_verseNumbersCheck);

	m_headingsCheck = new QCheckBox(this);
	m_headingsCheck->setText(tr("Show section headings"));
	m_headingsCheck->setChecked(CBTConfig::get(CBTConfig::headings));
	layout->addWidget(m_headingsCheck);


	m_scriptureReferencesCheck = new QCheckBox(this);
	m_scriptureReferencesCheck->setText(tr("Show scripture cross-references"));
	m_scriptureReferencesCheck->setChecked(CBTConfig::get(CBTConfig::scriptureReferences));
	layout->addWidget(m_scriptureReferencesCheck);

	m_greekAccentsCheck = new QCheckBox(this);
	m_greekAccentsCheck->setText(tr("Show Greek accents"));
	m_greekAccentsCheck->setChecked(CBTConfig::get(CBTConfig::greekAccents));
	layout->addWidget(m_greekAccentsCheck);

	m_hebrewPointsCheck = new QCheckBox(this);
	m_hebrewPointsCheck->setText(tr("Show Hebrew vowel points"));
	m_hebrewPointsCheck->setChecked(CBTConfig::get(CBTConfig::hebrewPoints));
	layout->addWidget(m_hebrewPointsCheck);

	m_hebrewCantillationCheck = new QCheckBox(this);
	m_hebrewCantillationCheck->setText(tr("Show Hebrew cantillation marks"));
	m_hebrewCantillationCheck->setChecked(CBTConfig::get(CBTConfig::hebrewCantillation));
	layout->addWidget(m_hebrewCantillationCheck);

	m_morphSegmentationCheck = new QCheckBox(this);
	m_morphSegmentationCheck->setText(tr("Show morph segmentation"));
	m_morphSegmentationCheck->setChecked(CBTConfig::get(CBTConfig::morphSegmentation));
	layout->addWidget(m_morphSegmentationCheck);

	m_textualVariantsCheck = new QCheckBox(this);
	m_textualVariantsCheck->setText(tr("Use textual variants"));
	m_textualVariantsCheck->setChecked(CBTConfig::get(CBTConfig::textualVariants));
	layout->addWidget(m_textualVariantsCheck);
}

void CSwordSettingsPage::save()
{
	m_worksTab->save();
	m_filtersTab->save();	
}

QString CSwordSettingsPage::iconName()
{
	return CResMgr::settings::sword::icon;
}
QString CSwordSettingsPage::label()
{
	return tr("");
}
QString CSwordSettingsPage::header()
{
	return tr("Desk");
}

void StandardWorksTab::save()
{
	for (int i = 0; i <= (int)CBTConfig::lastModuleType; ++i) 
	{
		QString moduleDescription = QString::null;

		CBTConfig::modules moduleType = (CBTConfig::modules)(i);
		switch (moduleType) 
		{
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


