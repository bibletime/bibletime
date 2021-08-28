/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QMenu>

#include <QObject>
#include <QString>
#include "../backend/bookshelfmodel/btbookshelftreemodel.h"


class QAction;
class QActionGroup;
class QWidget;

class BtBookshelfGroupingMenu: public QMenu {
    Q_OBJECT
    public:
        explicit BtBookshelfGroupingMenu(QWidget * parent = nullptr)
            : QMenu(parent) { initMenu(true); }

        explicit BtBookshelfGroupingMenu(bool showNoGrouping,
                                         QWidget * parent = nullptr)
        : QMenu(parent) { initMenu(showNoGrouping); }

    Q_SIGNALS:
        void signalGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &);

    private:
        void initMenu(bool showNoGrouping);
        void retranslateUi();

    private:
        QActionGroup *m_groupingActionGroup;
        QAction *m_groupingCatLangAction;
        QAction *m_groupingCatAction;
        QAction *m_groupingLangCatAction;
        QAction *m_groupingLangAction;
        QAction *m_groupingNoneAction;
};
