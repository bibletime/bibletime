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

#include "frontend/searchdialog/btsearchmodulechooserdialog.h"

#include <QAction>
#include <QToolButton>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/btbookshelfdockwidget.h"
#include "util/tool.h"


BtSearchModuleChooserDialog::BtSearchModuleChooserDialog(QWidget *parent,
                                                         Qt::WindowFlags flags)
    : BtModuleChooserDialog(parent, flags)
{
    // Initialize the tree model:
    const BtBookshelfDockWidget *dw(BtBookshelfDockWidget::getInstance());
    BtBookshelfTreeModel *treeModel = new BtBookshelfTreeModel(dw->groupingOrder(), this);
    treeModel->setCheckable(true);

    // Initialize the bookshelf widget:
    bookshelfWidget()->showHideAction()->setVisible(false);
    bookshelfWidget()->showHideButton()->hide();
    bookshelfWidget()->setTreeModel(treeModel);
    bookshelfWidget()->setSourceModel(CSwordBackend::instance()->model());

    retranslateUi();
}

BtSearchModuleChooserDialog::~BtSearchModuleChooserDialog() {
    // Intentionally empty
}

void BtSearchModuleChooserDialog::retranslateUi() {
    setWindowTitle(tr("Works to Search in"));
    util::tool::initExplanationLabel(label(), QString::null,
                                     tr("Select the works which should be searched."));
}
