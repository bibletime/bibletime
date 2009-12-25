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

#ifndef BTBOOKSHELFDOCKWIDGET_H
#define BTBOOKSHELFDOCKWIDGET_H

#include <QDockWidget>

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "frontend/btbookshelfwidget.h"


class QAction;
class QActionGroup;
class QMenu;

class BtBookshelfDockWidget: public QDockWidget {
        Q_OBJECT
    public:
        BtBookshelfDockWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);

        static inline BtBookshelfDockWidget *getInstance() { return m_instance; }

        const BtBookshelfTreeModel::Grouping &groupingOrder() const;

    signals:
        void moduleOpenTriggered(CSwordModuleInfo *module);
        void moduleSearchTriggered(CSwordModuleInfo *module);
        void moduleEditPlainTriggered(CSwordModuleInfo *module);
        void moduleEditHtmlTriggered(CSwordModuleInfo *module);
        void moduleUnlockTriggered(CSwordModuleInfo *module);
        void moduleAboutTriggered(CSwordModuleInfo *module);
        void groupingOrderChanged(BtBookshelfTreeModel::Grouping newGrouping);

    protected:
        void initMenus();
        void retranslateUi();
        BtBookshelfTreeModel::Grouping loadGroupingSetting() const;

    protected slots:
        void slotModuleChecked(CSwordModuleInfo *module, bool checked);
        void slotItemActionTriggered(QAction *action);
        void slotPrepareItemContextMenu();

    protected:
        BtBookshelfWidget *m_bookshelfWidget;

        // Item context menu:
        QMenu *m_itemContextMenu;
        QActionGroup *m_itemActionGroup;
        QAction *m_itemOpenAction;
        QAction *m_itemSearchAction;
        QMenu   *m_itemEditMenu;
        QAction *m_itemEditPlainAction;
        QAction *m_itemEditHtmlAction;
        QAction *m_itemUnlockAction;
        QAction *m_itemAboutAction;

        static BtBookshelfDockWidget *m_instance;
};

#endif // BTBOOKSHELFDOCKWIDGET_H
