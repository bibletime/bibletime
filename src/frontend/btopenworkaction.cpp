/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/btopenworkaction.h"

#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/managers/cswordbackend.h"
#include "bibletimeapp.h"
#include "frontend/btbookshelfgroupingmenu.h"
#include "util/geticon.h"


BtOpenWorkActionMenu::BtOpenWorkActionMenu(const QString &groupingConfigKey,
                                           QWidget *parent)
    : BtMenuView(parent), m_treeModel(0), m_postFilterModel(0),
      m_groupingConfigKey(groupingConfigKey)
{
    // Setup models:
    m_treeModel = new BtBookshelfTreeModel(groupingConfigKey, this);
    m_postFilterModel = new BtBookshelfFilterModel(this);
    m_postFilterModel->setSourceModel(m_treeModel);
    setModel(m_postFilterModel);

    connect(this, SIGNAL(triggered(QModelIndex)),
            this, SLOT(slotIndexTriggered(QModelIndex)));
}

void BtOpenWorkActionMenu::setSourceModel(QAbstractItemModel *model) {
    m_treeModel->setSourceModel(model);
}

void BtOpenWorkActionMenu::retranslateUi() {
    m_groupingMenu->setTitle(tr("&Grouping order"));
    m_groupingMenu->setStatusTip(tr("Sets the grouping order for the items in "
                                    "this menu."));
}

void BtOpenWorkActionMenu::postBuildMenu() {
    addSeparator();
    m_groupingMenu = new BtBookshelfGroupingMenu(false, this);

    connect(m_groupingMenu, SIGNAL(signalGroupingOrderChanged(BtBookshelfTreeModel::Grouping)),
            this,           SLOT(slotGroupingActionTriggered(BtBookshelfTreeModel::Grouping)));

    retranslateUi();
    addMenu(m_groupingMenu);
}

void BtOpenWorkActionMenu::slotIndexTriggered(const QModelIndex &index) {
    static const int MPR = BtBookshelfModel::ModulePointerRole;

    CSwordModuleInfo *i;
    i = static_cast<CSwordModuleInfo *>(model()->data(index, MPR).value<void*>());
    if (i != 0) {
        emit triggered(i);
    }
}

void BtOpenWorkActionMenu::slotGroupingActionTriggered(const BtBookshelfTreeModel::Grouping &grouping) {
    m_treeModel->setGroupingOrder(grouping);
    grouping.saveTo(m_groupingConfigKey);
}

BtOpenWorkAction::BtOpenWorkAction(const QString &groupingConfigKey,
                                   QObject *parent)
    : QAction(parent)
{
    m_menu = new BtOpenWorkActionMenu(groupingConfigKey);
    m_menu->setSourceModel(CSwordBackend::instance()->model());

    setMenu(m_menu);
    setIcon(util::getIcon("folder-open.svg"));
    retranslateUi();
    slotModelChanged();

    BtBookshelfFilterModel *filterModel = m_menu->postFilterModel();
    connect(m_menu, SIGNAL(triggered(CSwordModuleInfo*)),
            this,   SIGNAL(triggered(CSwordModuleInfo*)));
    connect(filterModel, SIGNAL(layoutChanged()),
            this,        SLOT(slotModelChanged()));
    connect(filterModel, SIGNAL(modelReset()),
            this,        SLOT(slotModelChanged()));
    connect(filterModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
            this,        SLOT(slotModelChanged()));
    connect(filterModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
            this,        SLOT(slotModelChanged()));
}

BtOpenWorkAction::~BtOpenWorkAction() {
    delete m_menu;
}

void BtOpenWorkAction::retranslateUi() {
    setText(tr("&Open work"));
}

void BtOpenWorkAction::slotModelChanged() {
    setEnabled(m_menu->postFilterModel()->rowCount() > 0);
}
