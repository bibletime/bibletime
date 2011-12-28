/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDSETTINGS_H
#define CSWORDSETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"
#include <QWidget>


class CConfigurationDialog;
class CSwordSettingsPage;
class QCheckBox;
class QComboBox;
class QLabel;


/*******************************************************************************
  StandardWorksTab
*******************************************************************************/

class StandardWorksTab: public QWidget {

    Q_OBJECT

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


/*******************************************************************************
  TextFiltersTab
*******************************************************************************/

class TextFiltersTab: public QWidget {

    Q_OBJECT

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


/*******************************************************************************
  CSwordSettingsPage
*******************************************************************************/

class CSwordSettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public: /* Methods: */

        CSwordSettingsPage(CConfigurationDialog *parent = 0);

        void save();

    protected: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

        QTabWidget *m_tabWidget;
        StandardWorksTab *m_worksTab;
        TextFiltersTab *m_filtersTab;

};

#endif
