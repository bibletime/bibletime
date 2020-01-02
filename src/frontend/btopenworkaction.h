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

#ifndef BTOPENWORKACTION_H
#define BTOPENWORKACTION_H

#include <QAction>
#include "btmenuview.h"

#include "../backend/bookshelfmodel/btbookshelftreemodel.h"


class BtBookshelfGroupingMenu;
class BtBookshelfTreeModel;
class BtBookshelfFilterModel;
class CSwordModuleInfo;

class BtOpenWorkActionMenu: public BtMenuView {
    Q_OBJECT
    public:
        BtOpenWorkActionMenu(const QString &groupingConfigKey,
                             QWidget *parent = nullptr);

        void setSourceModel(QAbstractItemModel *model);
        inline QAbstractItemModel *sourceModel() const { return m_treeModel->sourceModel(); }
        inline BtBookshelfTreeModel *treeModel() const { return m_treeModel; }
        inline BtBookshelfFilterModel *postFilterModel() const { return m_postFilterModel; }

    signals:
        void triggered(CSwordModuleInfo *module);

    private:
        void retranslateUi();

        void postBuildMenu(QActionGroup * actions) override;

    private slots:
        void slotIndexTriggered(const QModelIndex &index);
        void slotGroupingActionTriggered(const BtBookshelfTreeModel::Grouping &grouping);

    private:
        // Models:
        BtBookshelfTreeModel   *m_treeModel;
        BtBookshelfFilterModel *m_postFilterModel;

        // Grouping menu:
        BtBookshelfGroupingMenu *m_groupingMenu;
        const QString m_groupingConfigKey;
};

class BtOpenWorkAction: public QAction {
    Q_OBJECT
    public:
        explicit BtOpenWorkAction(const QString &groupingConfigKey,
                                  QObject *parent = nullptr);
        ~BtOpenWorkAction();

    signals:
        void triggered(CSwordModuleInfo *module);

    protected:
        void retranslateUi();

    private slots:
        void slotModelChanged();

    private:
        BtOpenWorkActionMenu *m_menu;
};

#endif // BTOPENWORKACTION_H
