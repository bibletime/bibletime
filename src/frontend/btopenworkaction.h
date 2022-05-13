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

#include "btmenuview.h"

#include <memory>
#include <QAction>
#include <QObject>
#include <QString>
#include "../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../backend/config/btconfigcore.h"


class BtBookshelfGroupingMenu;
class BtBookshelfFilterModel;
class CSwordModuleInfo;
class QAbstractItemModel;
class QActionGroup;
class QWidget;

class BtOpenWorkActionMenu: public BtMenuView {
    Q_OBJECT
    public:
        BtOpenWorkActionMenu(BtConfigCore groupingConfigGroup,
                             QString groupingConfigKey,
                             QWidget * parent = nullptr);

        void setSourceModel(std::shared_ptr<QAbstractItemModel> model);

        std::shared_ptr<QAbstractItemModel> sourceModel() const
        { return m_treeModel->sourceModel(); }

        BtBookshelfTreeModel * treeModel() const { return m_treeModel; }

        BtBookshelfFilterModel * postFilterModel() const
        { return m_postFilterModel; }

    Q_SIGNALS:
        void triggered(CSwordModuleInfo *module);

    private:
        void retranslateUi();

        void postBuildMenu(QActionGroup * actions) override;

    private:
        // Models:
        BtBookshelfTreeModel   *m_treeModel;
        BtBookshelfFilterModel *m_postFilterModel;

        // Grouping menu:
        BtBookshelfGroupingMenu *m_groupingMenu;
        BtConfigCore m_groupingConfigGroup;
        QString const m_groupingConfigKey;
};

class BtOpenWorkAction: public QAction {
    Q_OBJECT
    public:
        explicit BtOpenWorkAction(BtConfigCore groupingConfigGroup,
                                  QString groupingConfigKey,
                                  QObject * parent = nullptr);
        ~BtOpenWorkAction() override;

    Q_SIGNALS:
        void triggered(CSwordModuleInfo *module);

    protected:
        void retranslateUi();

    private:
        BtOpenWorkActionMenu *m_menu;
};
