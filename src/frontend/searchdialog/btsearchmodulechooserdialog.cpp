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

#include "btsearchmodulechooserdialog.h"

#include <QAction>
#include <QToolButton>
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/btconstmoduleset.h"
#include "../../backend/drivers/btmoduleset.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "../../util/tool.h"
#include "../btbookshelfwidget.h"
#include "../btmodulechooserdialog.h"


namespace {
auto const groupingOrderKey(
        QStringLiteral("GUI/SearchDialog/ModuleChooserDialog/grouping"));
}

BtSearchModuleChooserDialog::BtSearchModuleChooserDialog(QWidget *parent,
                                                         Qt::WindowFlags flags)
    : BtModuleChooserDialog(parent, flags)
{
    // Initialize the tree model:
    BtBookshelfTreeModel::Grouping grouping(btConfig(), groupingOrderKey);
    BtBookshelfTreeModel *treeModel = new BtBookshelfTreeModel(grouping, this);
    treeModel->setCheckable(true);
    BT_CONNECT(treeModel, &BtBookshelfTreeModel::groupingOrderChanged,
               [](BtBookshelfTreeModel::Grouping const & grouping)
               { grouping.saveTo(btConfig(), groupingOrderKey); });

    // Initialize the bookshelf widget:
    bookshelfWidget()->showHideAction()->setVisible(false);
    bookshelfWidget()->showHideButton()->hide();
    bookshelfWidget()->setTreeModel(treeModel);
    bookshelfWidget()->setSourceModel(CSwordBackend::instance().model());

    retranslateUi();
}

void BtSearchModuleChooserDialog::setCheckedModules(
        BtConstModuleSet const & modules)
{ bookshelfWidget()->treeModel()->setCheckedModules(modules); }

BtConstModuleSet BtSearchModuleChooserDialog::checkedModules() const
{ return bookshelfWidget()->treeModel()->checkedModules(); }

void BtSearchModuleChooserDialog::retranslateUi() {
    setWindowTitle(tr("Works to Search in"));
    util::tool::initExplanationLabel(label(), QString(),
                                     tr("Select the works which should be searched."));
}
