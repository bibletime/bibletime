/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTBOOKSHELFDOCKWIDGET_H
#define BTBOOKSHELFDOCKWIDGET_H

#include <QDockWidget>

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "frontend/btbookshelfwidget.h"


class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QPushButton;
class QStackedWidget;

class BtBookshelfDockWidget: public QDockWidget {
        Q_OBJECT
    public:
        BtBookshelfDockWidget(QWidget *parent = nullptr, Qt::WindowFlags f = nullptr);

        static inline BtBookshelfDockWidget *getInstance() { return m_instance; }

        inline const BtBookshelfTreeModel::Grouping &groupingOrder() const {
            return m_treeModel->groupingOrder();
        }

    signals:
        void moduleHovered(CSwordModuleInfo *module);
        void moduleOpenTriggered(CSwordModuleInfo *module);
        void moduleSearchTriggered(CSwordModuleInfo *module);
        void moduleUnlockTriggered(CSwordModuleInfo *module);
        void moduleAboutTriggered(CSwordModuleInfo *module);
        void groupingOrderChanged(BtBookshelfTreeModel::Grouping newGrouping);

    protected:
        void initMenus();
        void retranslateUi();

    protected slots:
        void slotModuleActivated(CSwordModuleInfo *module);
        void slotModuleChecked(CSwordModuleInfo *module, bool checked);
        void slotItemActionTriggered(QAction *action);
        void slotPrepareItemContextMenu();
        void slotModulesChanged();
        void slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g);

    protected:
        BtBookshelfTreeModel *m_treeModel;

        QStackedWidget    *m_stackedWidget;
        BtBookshelfWidget *m_bookshelfWidget;
        QWidget           *m_welcomeWidget;
            QLabel            *m_installLabel;
            QPushButton       *m_installButton;

        // Item context menu:
        QMenu *m_itemContextMenu;
        QActionGroup *m_itemActionGroup;
        QAction *m_itemOpenAction;
        QAction *m_itemSearchAction;
        QAction *m_itemUnlockAction;
        QAction *m_itemAboutAction;

        static BtBookshelfDockWidget *m_instance;
};

#endif // BTBOOKSHELFDOCKWIDGET_H
