/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTMODULEMANAGERDIALOG_H
#define BTMODULEMANAGERDIALOG_H

#include "frontend/bookshelfmanager/btconfigdialog.h"


/**
* The Bookshelf Manager dialog. Includes pages for Install, Remove, Indexes.
*/
class BtModuleManagerDialog: public BtConfigDialog {

        Q_OBJECT

    public:

        static BtModuleManagerDialog *getInstance(QWidget *parent = 0,
                                                  Qt::WindowFlags flags = 0);
        ~BtModuleManagerDialog();

    private:

        BtModuleManagerDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);

        void retranslateUi();

        /** Loads the settings from the resource file. */
        void loadDialogSettings();

        /** Saves the settings to the resource file. */
        void saveDialogSettings() const;

};


#endif
