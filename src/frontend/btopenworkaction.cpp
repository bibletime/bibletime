/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/btopenworkaction.h"

#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/btbookshelfdockwidget.h"
#include "util/cpointers.h"
#include "util/directory.h"


BtOpenWorkActionMenu::BtOpenWorkActionMenu(QWidget *parent)
    : BtMenuView(parent), m_treeModel(0), m_postFilterModel(0)
{
    // Setup models:
    const BtBookshelfDockWidget *bookshelfDock(BtBookshelfDockWidget::getInstance());
    m_treeModel = new BtBookshelfTreeModel(bookshelfDock->groupingOrder(), this);
    m_postFilterModel = new BtBookshelfFilterModel(this);
    m_postFilterModel->setSourceModel(m_treeModel);
    setModel(m_postFilterModel);

    connect(this, SIGNAL(triggered(QModelIndex)),
            this, SLOT(slotIndexTriggered(QModelIndex)));
    connect(bookshelfDock, SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
            m_treeModel,   SLOT(setGroupingOrder(BtBookshelfTreeModel::Grouping)));

}

BtOpenWorkActionMenu::~BtOpenWorkActionMenu() {
    // Intentionally empty
}

void BtOpenWorkActionMenu::setSourceModel(QAbstractItemModel *model) {
    m_treeModel->setSourceModel(model);
}

void BtOpenWorkActionMenu::slotIndexTriggered(const QModelIndex &index) {
    static const int MPR = BtBookshelfModel::ModulePointerRole;

    CSwordModuleInfo *i;
    i = static_cast<CSwordModuleInfo *>(model()->data(index, MPR).value<void*>());
    if (i != 0) {
        emit triggered(i);
    }
}

BtOpenWorkAction::BtOpenWorkAction(QObject *parent)
    : QAction(parent)
{
    m_menu = new BtOpenWorkActionMenu();
    m_menu->setSourceModel(CPointers::backend()->model());

    setMenu(m_menu);
    setIcon(util::directory::getIcon("folder-open.svg"));
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
