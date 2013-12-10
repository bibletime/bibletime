/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTLANGUAGESETTINGS_H
#define BTLANGUAGESETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QMap>
#include <QWidget>


class CConfigurationDialog;
class QComboBox;
class QLabel;

class BtLanguageSettingsPage: public BtConfigDialog::Page {

        Q_OBJECT

    public: /* Methods: */

        BtLanguageSettingsPage(CConfigurationDialog *parent = 0);

        void save();

        static void resetLanguage();

    private: /* Methods: */

        static QVector<QString> bookNameAbbreviationsTryVector();
        void initSwordLocaleCombo();
        void retranslateUi();

    private: /* Fields: */

        QLabel *m_languageNamesLabel;
        QComboBox* m_swordLocaleCombo;

};

#endif
