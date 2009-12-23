/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTSEARCHMODULECHOOSERDIALOG_H
#define BTSEARCHMODULECHOOSERDIALOG_H

#include "frontend/btmodulechooserdialog.h"

#include "backend/bookshelfmodel/btbookshelftreemodel.h"


class BtBookshelfTreeModel;
class CSwordModuleInfo;

class BtSearchModuleChooserDialog: public BtModuleChooserDialog {
    Q_OBJECT
    public:
        explicit BtSearchModuleChooserDialog(QWidget *parent = 0, Qt::WindowFlags flags = 0);
        ~BtSearchModuleChooserDialog();

        inline void setCheckedModules(const QSet<CSwordModuleInfo*> &modules) {
            bookshelfWidget()->treeModel()->setCheckedModules(modules);
        }
        inline const QSet<CSwordModuleInfo*> &checkedModules() const {
            return bookshelfWidget()->treeModel()->checkedModules();
        }

    protected:
        void retranslateUi();
};

#endif // BTSEARCHMODULECHOOSERDIALOG_H
