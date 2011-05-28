/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTLANGUAGESETTINGS_H
#define BTLANGUAGESETTINGS_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QMap>
#include <QWidget>
#include "backend/config/cbtconfig.h"


class QComboBox;
class QLabel;

class BtLanguageSettingsPage: public BtConfigPage {

        Q_OBJECT

    public: /* Methods: */

        BtLanguageSettingsPage(QWidget *parent = 0);

        void save();

        /** Reimplemented from BtConfigPage. */
        virtual const QIcon &icon() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString header() const;

    private: /* Methods: */

        void retranslateUi();

    private: /* Fields: */

        QLabel *m_languageNamesLabel;
        QComboBox* m_swordLocaleCombo;

};

#endif
