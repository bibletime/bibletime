/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h"

#include <QAction>
#include <QHeaderView>
#include <QToolButton>
#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "frontend/btbookshelfview.h"
#include "util/btconnect.h"
#include "util/tool.h"


namespace {
const QString groupingOrderKey("GUI/BookshelfManager/InstallConfirmDialog/grouping");
}

BtInstallModuleChooserDialog::BtInstallModuleChooserDialog(const BtBookshelfTreeModel::Grouping &g,
                                                           QWidget *parent,
                                                           Qt::WindowFlags flags)
    : BtModuleChooserDialog(parent, flags), m_shown(false)
{
    resize(550, 340);

    // Read grouping order from settings or the default from argument:
    BtBookshelfTreeModel::Grouping groupingOrder(false);
    if (!groupingOrder.loadFrom(groupingOrderKey)) {
        groupingOrder = g;
    }

    BtInstallModuleChooserDialogModel *treeModel;
    treeModel = new BtInstallModuleChooserDialogModel(groupingOrder, this);
    BT_CONNECT(treeModel,
               SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
               this,
               SLOT(slotGroupingOrderChanged(
                            BtBookshelfTreeModel::Grouping const &)));

    m_bookshelfModel = new BtBookshelfModel(this);
    bookshelfWidget()->postFilterModel()->setShowShown(true);
    bookshelfWidget()->setTreeModel(treeModel);
    bookshelfWidget()->setSourceModel(m_bookshelfModel);
    bookshelfWidget()->showHideAction()->setVisible(false);
    bookshelfWidget()->showHideButton()->hide();
    bookshelfWidget()->treeView()->header()->show();

    retranslateUi();
}

void BtInstallModuleChooserDialog::addModuleItem(CSwordModuleInfo *module,
                                                 const QString &sourceName)
{
    module->setProperty("installSourceName", sourceName);
    m_bookshelfModel->addModule(module);
}

void BtInstallModuleChooserDialog::retranslateUi() {
    setWindowTitle(tr("Install/Update works?"));
    util::tool::initExplanationLabel(
            label(), QString::null,
            tr("Do you really want to install these works?") + "<br/><br/><small>" +
            tr("Only one version of a work can be installed at the same time. Select only "
               "one if there are items marked with red.") + "</small>");
}

void BtInstallModuleChooserDialog::showEvent(QShowEvent *event) {
    Q_UNUSED(event);

    if (m_shown) return;
    bookshelfWidget()->treeView()->expandAll();
    bookshelfWidget()->treeView()->header()->resizeSections(QHeaderView::ResizeToContents);
    m_shown = true;
}

void BtInstallModuleChooserDialog::slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g) {
    g.saveTo(groupingOrderKey);
}
