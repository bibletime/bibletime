//
// C++ Interface: cswordsettings
//
// Description: Widgets for "Sword" ("Desk") settings of configuration dialog.
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 1999-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CSWORDSETTINGS_H
#define CSWORDSETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include "util/cpointers.h"

#include <QTabWidget>


class QComboBox;
class QCheckBox;

class StandardWorksTab;
class TextFiltersTab;

class CSwordSettingsPage : public BtConfigPage
{
	Q_OBJECT

public:
	CSwordSettingsPage(QWidget* parent);
	void save();
	QString iconName();
	QString label();
	QString header();

private:
	StandardWorksTab* m_worksTab;
	TextFiltersTab* m_filtersTab;
};

//Tab pages. To be used only in Sword settings page.

class StandardWorksTab : public QWidget, CPointers
{
	Q_OBJECT

public:
	StandardWorksTab();
	void save();

private:
	QComboBox* m_standardBibleCombo;
	QComboBox* m_standardCommentaryCombo;
	QComboBox* m_standardLexiconCombo;
	QComboBox* m_standardDailyDevotionalCombo;
	QComboBox* m_standardHebrewStrongCombo;
	QComboBox* m_standardGreekStrongCombo;
	QComboBox* m_standardHebrewMorphCombo;
	QComboBox* m_standardGreekMorphCombo;
};

class TextFiltersTab : public QWidget
{
	Q_OBJECT

public:
	TextFiltersTab();
	void save();

private:
	QCheckBox* m_lineBreaksCheck;
	QCheckBox* m_verseNumbersCheck;
	QCheckBox* m_headingsCheck;
	QCheckBox* m_hebrewPointsCheck;
	QCheckBox* m_hebrewCantillationCheck;
	QCheckBox* m_morphSegmentationCheck;
	QCheckBox* m_greekAccentsCheck;
	QCheckBox* m_textualVariantsCheck;
	QCheckBox* m_scriptureReferencesCheck;
};

#endif
