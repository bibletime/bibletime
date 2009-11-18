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

#ifndef BTREMOVEPAGE_H
#define BTREMOVEPAGE_H

#include "frontend/bookshelfmanager/btconfigdialog.h"

#include <QMultiMap>
#include <QString>
#include "backend/btmoduletreeitem.h"
#include "frontend/bookshelfmanager/removepage/btremovepagetreemodel.h"


class QPushButton;
class QTreeView;

class BtRemovePage: public BtConfigPage {
        Q_OBJECT

    public:
        BtRemovePage();

        // BtConfigPage methods:
        QString header();
        QString iconName();
        QString label();

    protected slots:
        void slotRemoveModules();
        void resetRemoveButton();

    protected:
        BtRemovePageTreeModel *m_model;

        QTreeView   *m_view;
        QPushButton *m_removeButton;
};

#endif
