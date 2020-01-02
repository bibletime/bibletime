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

#ifndef BTSTANDARDWORKSTAB_H
#define BTSTANDARDWORKSTAB_H

#include <QWidget>


class CSwordSettingsPage;
class QCheckBox;
class QComboBox;
class QLabel;

class BtStandardWorksTab: public QWidget {

    Q_OBJECT

    public: /* Methods: */

        BtStandardWorksTab(CSwordSettingsPage * parent);

        void save();

    protected: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

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
};

#endif /* BTSTANDARDWORKSTAB_H */
