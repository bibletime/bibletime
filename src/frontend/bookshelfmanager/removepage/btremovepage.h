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

#ifndef BTREMOVEPAGE_H
#define BTREMOVEPAGE_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QMultiMap>
#include <QString>
#include "backend/btmoduletreeitem.h"
#include "frontend/bookshelfmanager/removepage/btremovepagetreemodel.h"


class BtBookshelfWidget;
class QPushButton;

class BtRemovePage: public BtConfigPage {
        Q_OBJECT

    public:
        BtRemovePage(QWidget *parent = 0);

        /** Reimplemented from BtConfigPage. */
        virtual QString header() const;

        /** Reimplemented from BtConfigPage. */
        virtual const QIcon &icon() const;

        /** Reimplemented from BtConfigPage. */
        virtual QString label() const;

    protected slots:
        void slotRemoveModules();
        void resetRemoveButton();
        void slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g);

    protected:
        BtBookshelfWidget *m_bookshelfWidget;
        QPushButton       *m_removeButton;
};

#endif
