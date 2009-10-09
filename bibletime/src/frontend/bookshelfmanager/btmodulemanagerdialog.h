/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#ifndef BTMODULEMANAGERDIALOG_H
#define BTMODULEMANAGERDIALOG_H

#include "btconfigdialog.h"

class QWidget;

/**
* The Bookshelf Manager dialog. Includes pages for Install, Remove, Indexes.
*/
class BtModuleManagerDialog : public BtConfigDialog {
        Q_OBJECT

    public:
        static BtModuleManagerDialog* getInstance(QWidget* parent);

        ~BtModuleManagerDialog();


        //void slotClose();
    protected:
        /** Reimplementation from QWidget. */
        virtual void closeEvent(QCloseEvent* event);

        /** Adds the pages and the button box. */
        BtModuleManagerDialog(QWidget* parent);

    private:
        // Load the settings from the resource file
        void loadDialogSettings();

        // Save the settings to the resource file
        void saveDialogSettings();


//signals:
//	void swordSetupChanged();
};


#endif
