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

#ifndef BTBOOKSHELFWIDGET_H
#define BTBOOKSHELFWIDGET_H

#include <QWidget>

#include "../backend/bookshelfmodel/btbookshelftreemodel.h"


class BtBookshelfFilterModel;
class BtBookshelfGroupingMenu;
class BtBookshelfTreeModel;
class BtBookshelfView;
class QAbstractItemModel;
class QAction;
class QActionGroup;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QMenu;
class QToolButton;

class BtBookshelfWidget: public QWidget {
    Q_OBJECT
    public:
        explicit BtBookshelfWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        void setSourceModel(QAbstractItemModel *model);

        // Getters for models:
        inline BtBookshelfTreeModel *treeModel() const { return m_treeModel; }
        inline BtBookshelfFilterModel *postFilterModel() const { return m_postFilterModel; }

        // Setters for models:
        void setTreeModel(BtBookshelfTreeModel *model);

        // Getters for widgets:
        inline QWidget *leftCornerWidget() const { return m_leftCornerWidget; }
        inline QLabel *nameFilterLabel() const { return m_nameFilterLabel; }
        inline QLineEdit *nameFilterEdit() const { return m_nameFilterEdit; }
        inline QToolButton *groupingButton() const { return m_groupingButton; }
        inline QToolButton *showHideButton() const { return m_showHideButton; }
        inline QWidget *rightCornerWidget() const { return m_rightCornerWidget; }
        inline BtBookshelfView *treeView() const { return m_treeView; }
        inline BtBookshelfGroupingMenu *groupingMenu() const { return m_groupingMenu; }
        inline QMenu *contextMenu() const { return m_contextMenu; }
        inline QMenu *itemContextMenu() const { return m_itemContextMenu; }

        // Setters for widgets:
        void setLeftCornerWidget(QWidget *w);
        void setRightCornerWidget(QWidget *w);

        // Getters for actions:
        inline QAction *showHideAction() const { return m_showHideAction; }

        // Setters for context menus:
        inline void setContextMenu(QMenu *newMenu) { m_contextMenu = newMenu; }
        inline void setItemContextMenu(QMenu *newMenu) { m_itemContextMenu = newMenu; }

        bool eventFilter(QObject *object, QEvent *event) override;

        void loadBookshelfState();
        void saveBookshelfState();

    protected:
        void findExpanded(const QModelIndex& index, QString prefix, QStringList * expandedPaths);
        void initActions();
        void initMenus();
        void initWidgets();
        void restoreExpanded(const QModelIndex& index, const QStringList& nodeList);
        void retranslateUi();

    protected slots:
        void slotGroupingActionTriggered(const BtBookshelfTreeModel::Grouping &grouping);
        void slotShowContextMenu(const QPoint &pos);
        void slotShowItemContextMenu(CSwordModuleInfo *module, const QPoint &pos);

    private:
        // Models:
        QAbstractItemModel     *m_sourceModel;
        BtBookshelfTreeModel   *m_treeModel;
        BtBookshelfFilterModel *m_postFilterModel;

        // Widgets:
        QHBoxLayout     *m_toolBar;
        QWidget         *m_leftCornerWidget;
        QWidget         *m_rightCornerWidget;
        QLabel          *m_nameFilterLabel;
        QLineEdit       *m_nameFilterEdit;
        QToolButton     *m_groupingButton;
        QToolButton     *m_showHideButton;
        BtBookshelfView *m_treeView;

        // Popup menus:
        QMenu *m_contextMenu;
        BtBookshelfGroupingMenu *m_groupingMenu;
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

#endif // BTBOOKSHELFWIDGET_H
