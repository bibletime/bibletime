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


class QCheckBox;
class QComboBox;
class StandardWorksTab;
class TextFiltersTab;

class CSwordSettingsPage : public BtConfigPage {
        Q_OBJECT
    public:
        CSwordSettingsPage(QWidget *parent = 0);

        void save();

        /** Reimplemented from BtConfigPage. */
        virtual const QIcon &icon() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString header() const;

    private:
        StandardWorksTab* m_worksTab;
        TextFiltersTab* m_filtersTab;
};

//Tab pages. To be used only in Sword settings page.

class StandardWorksTab: public QWidget {
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

class TextFiltersTab : public QWidget {
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
