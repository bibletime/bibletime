/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTINSTALLMODULECHOOSERDIALOG_H
#define BTINSTALLMODULECHOOSERDIALOG_H

#include "frontend/btmodulechooserdialog.h"

#include "backend/drivers/btmoduleset.h"
#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.h"


class BtBookshelfModel;
class BtInstallModuleChooserDialogModel;
class CSwordModuleInfo;

/**
* Confirmation dialog for installation. Lets the user
* uncheck modules from the list.
*/
class BtInstallModuleChooserDialog: public BtModuleChooserDialog {
    Q_OBJECT
    public:
        BtInstallModuleChooserDialog(const BtBookshelfTreeModel::Grouping &g,
                                     QWidget *parent = 0,
                                     Qt::WindowFlags flags = 0);

        inline BtModuleSet const & checkedModules() const
        { return bookshelfWidget()->treeModel()->checkedModules(); }

        void addModuleItem(CSwordModuleInfo *module, const QString &sourceName);

    protected:
        void retranslateUi();
        void showEvent(QShowEvent *event);

    protected slots:
        void slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g);

    private:
        BtBookshelfModel                  *m_bookshelfModel;

        bool m_shown;
};

#endif // BTINSTALLMODULECHOOSERDIALOG_H
