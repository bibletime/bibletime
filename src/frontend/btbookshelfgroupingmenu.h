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

#ifndef BTBOOKSHELFGROUPINGMENU_H
#define BTBOOKSHELFGROUPINGMENU_H

#include <QMenu>

#include "../backend/bookshelfmodel/btbookshelftreemodel.h"


class QAction;
class QActionGroup;

class BtBookshelfGroupingMenu: public QMenu {
    Q_OBJECT
    public:
        explicit inline BtBookshelfGroupingMenu(QWidget *parent = nullptr)
            : QMenu(parent) { initMenu(true); }

        explicit inline BtBookshelfGroupingMenu(bool showNoGrouping,
                                                QWidget *parent = nullptr)
        : QMenu(parent) { initMenu(showNoGrouping); }

    signals:
        void signalGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &);

    private:
        void initMenu(bool showNoGrouping);
        void retranslateUi();

    private slots:
        void slotGroupingActionTriggered(QAction *action);

    private:
        QActionGroup *m_groupingActionGroup;
        QAction *m_groupingCatLangAction;
        QAction *m_groupingCatAction;
        QAction *m_groupingLangCatAction;
        QAction *m_groupingLangAction;
        QAction *m_groupingNoneAction;
};

#endif // BTBOOKSHELFGROUPINGMENU_H
