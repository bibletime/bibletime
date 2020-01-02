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

#include "btsearchmodulechooserdialog.h"

#include <QAction>
#include <QToolButton>
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "../../util/tool.h"


namespace {
const QString groupingOrderKey("GUI/SearchDialog/ModuleChooserDialog/grouping");
}

BtSearchModuleChooserDialog::BtSearchModuleChooserDialog(QWidget *parent,
                                                         Qt::WindowFlags flags)
    : BtModuleChooserDialog(parent, flags)
{
    // Initialize the tree model:
    BtBookshelfTreeModel::Grouping grouping(groupingOrderKey);
    BtBookshelfTreeModel *treeModel = new BtBookshelfTreeModel(grouping, this);
    treeModel->setCheckable(true);
    BT_CONNECT(treeModel,
               SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
               this,
               SLOT(slotGroupingOrderChanged(
                            BtBookshelfTreeModel::Grouping const &)));

    // Initialize the bookshelf widget:
    bookshelfWidget()->showHideAction()->setVisible(false);
    bookshelfWidget()->showHideButton()->hide();
    bookshelfWidget()->setTreeModel(treeModel);
    bookshelfWidget()->setSourceModel(CSwordBackend::instance()->model());

    retranslateUi();
}

void BtSearchModuleChooserDialog::retranslateUi() {
    setWindowTitle(tr("Works to Search in"));
    util::tool::initExplanationLabel(label(), QString(),
                                     tr("Select the works which should be searched."));
}

void BtSearchModuleChooserDialog::slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g) {
    g.saveTo(groupingOrderKey);
}
