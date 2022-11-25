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

#pragma once

#include <QWidget>

#include <QObject>
#include <QString>


class CSwordSettingsPage;
class QComboBox;
class QLabel;

class BtStandardWorksTab: public QWidget {

    Q_OBJECT

    public: // methods:

        BtStandardWorksTab(CSwordSettingsPage * parent);

        void save();

    protected: // methods:

        void retranslateUi();

    private: // fields:

        QLabel * m_explanationLabel;

#define STANDARD_WORKS_TAB_FIELD(name) \
        QLabel * m_ ## name ## Label; \
        QComboBox * m_ ## name ## Combo

        STANDARD_WORKS_TAB_FIELD(standardBible);
        STANDARD_WORKS_TAB_FIELD(standardCommentary);
        STANDARD_WORKS_TAB_FIELD(standardLexicon);
        STANDARD_WORKS_TAB_FIELD(standardDailyDevotional);
        STANDARD_WORKS_TAB_FIELD(standardHebrewStrongsLexicon);
        STANDARD_WORKS_TAB_FIELD(standardGreekStrongsLexicon);
        STANDARD_WORKS_TAB_FIELD(standardHebrewMorphLexicon);
        STANDARD_WORKS_TAB_FIELD(standardGreekMorphLexicon);
#undef STANDARD_WORKS_TAB_FIELD
};
