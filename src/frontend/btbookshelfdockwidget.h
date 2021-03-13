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

#pragma once

#include <QDockWidget>

#include "../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "btbookshelfwidget.h"


class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QPushButton;
class QStackedWidget;

class BtBookshelfDockWidget: public QDockWidget {
        Q_OBJECT
    public:
        BtBookshelfDockWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

        static BtBookshelfDockWidget * getInstance() { return m_instance; }

        BtBookshelfTreeModel::Grouping groupingOrder() const
        { return m_treeModel->groupingOrder(); }

        void loadBookshelfState();
        void saveBookshelfState();

    Q_SIGNALS:
        void moduleHovered(CSwordModuleInfo *module);
        void moduleOpenTriggered(CSwordModuleInfo *module);
        void moduleSearchTriggered(CSwordModuleInfo *module);
        void moduleUnlockTriggered(CSwordModuleInfo *module);
        void moduleAboutTriggered(CSwordModuleInfo *module);
        void groupingOrderChanged(BtBookshelfTreeModel::Grouping newGrouping);

    protected:
        void initMenus();
        void retranslateUi();

    protected:
        BtBookshelfTreeModel *m_treeModel;

        QStackedWidget    *m_stackedWidget;
        BtBookshelfWidget *m_bookshelfWidget;
        QWidget           *m_welcomeWidget;
            QLabel            *m_installLabel;
            QPushButton       *m_installButton;

        // Item context menu:
        QMenu *m_itemContextMenu;
        QAction *m_itemOpenAction;
        QAction *m_itemSearchAction;
        QAction *m_itemUnlockAction;
        QAction *m_itemAboutAction;

        static BtBookshelfDockWidget *m_instance;
};
