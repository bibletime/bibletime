/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINDEXPAGE_H
#define BTINDEXPAGE_H

#include "frontend/bookshelfmanager/btconfigdialog.h"


class BtModuleManagerDialog;
class QCheckBox;
class QTreeWidget;
class QTreeWidgetItem;

/** \todo add void retranslateUi(); */

/**
* This class encapsulates the "Manage search indices" page of the Bookshelf
* Manager.  It allows for creation and deletion of search indicies for each
* installed module.  It also allows for deletion of orphaned indices.
*/
class BtIndexPage: public BtConfigDialog::Page {

        Q_OBJECT

    public:

        BtIndexPage(BtModuleManagerDialog *parent = 0);
        ~BtIndexPage();

    public slots:
        void slotSwordSetupChanged();

    protected:

        /**
        * Populates the module list with installed modules and orphaned indices
        */
        void populateModuleList();

        void retranslateUi();

    public slots:
        /**
        * Creates indices for selected modules if no index currently exists
        */
        void createIndices();
        /**
        * Deletes indices for selected modules
        */
        void deleteIndices();

    private:

        QCheckBox *m_autoDeleteOrphanedIndicesBox;
        QTreeWidget *m_moduleList;
        QPushButton *m_deleteButton;
        QPushButton *m_createButton;

        QTreeWidgetItem* m_modsWithIndices;
        QTreeWidgetItem* m_modsWithoutIndices;

};


#endif
