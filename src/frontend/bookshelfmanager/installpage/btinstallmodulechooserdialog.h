/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTINSTALLMODULECHOOSERDIALOG_H
#define BTINSTALLMODULECHOOSERDIALOG_H

#include "frontend/btmodulechooserdialog.h"

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
        explicit BtInstallModuleChooserDialog(QWidget *parent = 0,
                                              Qt::WindowFlags flags = 0);
        ~BtInstallModuleChooserDialog();

        inline const QSet<CSwordModuleInfo*> &checkedModules() const {
            return bookshelfWidget()->treeModel()->checkedModules();
        }

        void addModuleItem(CSwordModuleInfo *module, const QString &sourceName);

    protected:
        void retranslateUi();
        void showEvent(QShowEvent *event);

    private:
        BtBookshelfModel                  *m_bookshelfModel;

        bool m_shown;
};

#endif // BTINSTALLMODULECHOOSERDIALOG_H
