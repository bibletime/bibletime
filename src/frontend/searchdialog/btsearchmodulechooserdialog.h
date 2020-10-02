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

#ifndef BTSEARCHMODULECHOOSERDIALOG_H
#define BTSEARCHMODULECHOOSERDIALOG_H

#include "../btmodulechooserdialog.h"

#include "../../backend/drivers/btconstmoduleset.h"
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"


class BtBookshelfTreeModel;
class CSwordModuleInfo;

class BtSearchModuleChooserDialog: public BtModuleChooserDialog {
    Q_OBJECT
    public:
        BtSearchModuleChooserDialog(QWidget *parent = nullptr,
                                    Qt::WindowFlags flags = Qt::WindowFlags());

        inline void setCheckedModules(BtConstModuleSet const & modules)
        { bookshelfWidget()->treeModel()->setCheckedModules(modules); }

        inline BtConstModuleSet checkedModules() const
        { return bookshelfWidget()->treeModel()->checkedModules(); }

    protected slots:
        void slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g);

    protected:
        void retranslateUi();
};

#endif // BTSEARCHMODULECHOOSERDIALOG_H
