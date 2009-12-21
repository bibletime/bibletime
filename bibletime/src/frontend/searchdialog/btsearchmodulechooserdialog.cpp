/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
b* version 2.0.
*
**********/

#include "frontend/searchdialog/btsearchmodulechooserdialog.h"

#include <QSettings>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/config/cbtconfig.h"
#include "frontend/mainindex/btbookshelfview.h"
#include "util/cpointers.h"
#include "util/tool.h"


#define ISGROUPING(v) (v).canConvert<BtBookshelfTreeModel::Grouping>()
#define TOGROUPING(v) (v).value<BtBookshelfTreeModel::Grouping>()

BtSearchModuleChooserDialog::BtSearchModuleChooserDialog(QWidget *parent,
                                                         Qt::WindowFlags flags)
    : BtModuleChooserDialog(parent, flags)
{
    // Setup filter model to filter out hidden modules:
    BtBookshelfFilterModel *fm = new BtBookshelfFilterModel(this);
    fm->setSourceModel(CPointers::backend()->model());

    // Setup tree model on top of the filter model:
    QSettings *settings(CBTConfig::getConfig());
    settings->beginGroup("GUI");
    {
        /*
          If BtSearchModuleChooserDialog does not have its own grouping, we read the grouping
          from the main window bookshelf dock.
        */
        QVariant v;

        if (settings->value("SearchDialog/ModuleChooserDialog/hasOwnGrouping", false).toBool()) {
            v = settings->value("SearchDialog/ModuleChooserDialog/grouping");
            if (ISGROUPING(v)) {
                v = settings->value("MainWindow/Docks/Bookshelf/grouping");
            }
        } else {
            v = settings->value("MainWindow/Docks/Bookshelf/grouping");
        }

        if (ISGROUPING(v)) {
            m_bookshelfTreeModel = new BtBookshelfTreeModel(TOGROUPING(v), this);
        } else {
            m_bookshelfTreeModel = new BtBookshelfTreeModel(this);
        }
    }
    settings->endGroup();
    m_bookshelfTreeModel->setDefaultChecked(BtBookshelfTreeModel::UNCHECKED);
    m_bookshelfTreeModel->setCheckable(true);
    m_bookshelfTreeModel->setSourceModel(fm);

    // Setup view:
    treeView()->setModel(m_bookshelfTreeModel);

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
