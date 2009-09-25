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

class CSwordModuleInfo;
class BtBookshelfView;
class BtBookshelfModel;
class BtBookshelfTreeModel;
class BtCheckStateFilterProxyModel;
class QAction;
class QActionGroup;
class QMenu;
class QToolBar;
class QToolButton;

class BtBookshelfDockWidget: public QDockWidget {
    Q_OBJECT
    public:
        BtBookshelfDockWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);

        // CSwordModuleInfo *selectedModule() const;

    signals:
        void moduleOpenTriggered(CSwordModuleInfo *module);
        void moduleSearchTriggered(CSwordModuleInfo *module);
        void moduleEditPlainTriggered(CSwordModuleInfo *module);
        void moduleEditHtmlTriggered(CSwordModuleInfo *module);
        void moduleUnlockTriggered(CSwordModuleInfo *module);
        void moduleAboutTriggered(CSwordModuleInfo *module);

    protected:
        void initMenus();
        void retranslateInterface();

    protected slots:
        void swordSetupChanged();
        void showContextMenu(QPoint pos);
            void groupingActionTriggered(QAction *action);
            void showHideEnabled(bool enable);
        void showItemContextMenu(CSwordModuleInfo *module, QPoint pos);
            void itemActionTriggered(QAction *action);

    protected:
        // Models:
        BtBookshelfModel             *m_bookshelfModel;
        BtBookshelfTreeModel         *m_bookshelfTreeModel;
        BtCheckStateFilterProxyModel *m_filterProxyModel;

        // Widgets:
        QWidget *m_widget;
        BtBookshelfView *m_view;
        QToolButton     *m_groupingButton;
        QToolButton     *m_showHideButton;

        // Popup menus:
        QMenu *m_contextMenu;
            QMenu *m_groupingMenu;
                QActionGroup *m_groupingActionGroup;
                QAction *m_groupingCatLangAction;
                QAction *m_groupingCatAction;
                QAction *m_groupingLangCatAction;
                QAction *m_groupingLangAction;
                QAction *m_groupingNoneAction;
            QAction *m_showHideAction;
        QMenu *m_itemContextMenu;
            QActionGroup *m_itemActionGroup;
            QAction *m_itemOpenAction;
            QAction *m_itemSearchAction;
            QMenu   *m_itemEditMenu;
                QAction *m_itemEditPlainAction;
                QAction *m_itemEditHtmlAction;
            QAction *m_itemUnlockAction;
            QAction *m_itemAboutAction;
};

#endif // BTBOOKSHELFDOCKWIDGET_H
