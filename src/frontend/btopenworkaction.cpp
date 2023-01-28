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

#include "btopenworkaction.h"

#include <QAbstractItemModel>
#include <QVariant>
#include <QtGlobal>
#include <utility>
#include "../backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "../backend/bookshelfmodel/btbookshelfmodel.h"
#include "../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../backend/config/btconfigcore.h"
#include "../backend/managers/cswordbackend.h"
#include "../util/btconnect.h"
#include "../util/cresmgr.h"
#include "btbookshelfgroupingmenu.h"


BtOpenWorkActionMenu::BtOpenWorkActionMenu(BtConfigCore groupingConfigGroup,
                                           QString groupingConfigKey,
                                           QWidget * parent)
    : BtMenuView(parent)
    , m_treeModel(nullptr)
    , m_postFilterModel(nullptr)
    , m_groupingMenu(nullptr)
    , m_groupingConfigGroup(std::move(groupingConfigGroup))
    , m_groupingConfigKey(std::move(groupingConfigKey))
{
    // Setup models:
    m_treeModel = new BtBookshelfTreeModel(m_groupingConfigGroup,
                                           m_groupingConfigKey,
                                           this);
    m_postFilterModel = new BtBookshelfFilterModel(this);
    m_postFilterModel->setSourceModel(m_treeModel);
    setModel(m_postFilterModel);

    BT_CONNECT(this, &BtMenuView::triggered,
               [this](QModelIndex const & index) {
                   static constexpr auto const MPR =
                           BtBookshelfModel::ModulePointerRole;
                   if (auto * const i =
                            static_cast<CSwordModuleInfo *>(
                                    model()->data(index, MPR).value<void *>()))
                       Q_EMIT triggered(i);
               });
}

void BtOpenWorkActionMenu::setSourceModel(
        std::shared_ptr<QAbstractItemModel> model)
{ m_treeModel->setSourceModel(std::move(model)); }

void BtOpenWorkActionMenu::retranslateUi() {
    if (!m_groupingMenu)
        return;
    m_groupingMenu->setTitle(tr("&Grouping order"));
    m_groupingMenu->setStatusTip(tr("Sets the grouping order for the items in "
                                    "this menu."));
}

void BtOpenWorkActionMenu::postBuildMenu(QActionGroup * actions) {
    Q_UNUSED(actions)
    addSeparator();
    m_groupingMenu = new BtBookshelfGroupingMenu(false, this);

    BT_CONNECT(m_groupingMenu,
               &BtBookshelfGroupingMenu::signalGroupingOrderChanged,
               [this](BtBookshelfTreeModel::Grouping const & grouping) {
                   m_treeModel->setGroupingOrder(grouping);
                   grouping.saveTo(m_groupingConfigGroup, m_groupingConfigKey);
               });

    retranslateUi();
    addMenu(m_groupingMenu);
}

BtOpenWorkAction::BtOpenWorkAction(BtConfigCore groupingConfigGroup,
                                   QString groupingConfigKey,
                                   QObject * parent)
    : QAction(parent)
{
    m_menu = new BtOpenWorkActionMenu(std::move(groupingConfigGroup),
                                      std::move(groupingConfigKey));
    m_menu->setSourceModel(CSwordBackend::instance().model());

    setMenu(m_menu);
    setIcon(CResMgr::mainWindow::icon_openAction());
    retranslateUi();

    auto slotModelChanged =
            [this]{ setEnabled(m_menu->postFilterModel()->rowCount() > 0); };

    slotModelChanged();

    BtBookshelfFilterModel *filterModel = m_menu->postFilterModel();
    BT_CONNECT(m_menu, &BtOpenWorkActionMenu::triggered,
               this,   &BtOpenWorkAction::triggered);
    BT_CONNECT(filterModel, &BtBookshelfFilterModel::layoutChanged,
               slotModelChanged);
    BT_CONNECT(filterModel, &BtBookshelfFilterModel::modelReset,
               slotModelChanged);
    BT_CONNECT(filterModel, &BtBookshelfFilterModel::rowsInserted,
               slotModelChanged);
    BT_CONNECT(filterModel, &BtBookshelfFilterModel::rowsRemoved,
               std::move(slotModelChanged));
}

BtOpenWorkAction::~BtOpenWorkAction() {
    delete m_menu;
}

void BtOpenWorkAction::retranslateUi() {
    setText(tr("&Open work"));
}
