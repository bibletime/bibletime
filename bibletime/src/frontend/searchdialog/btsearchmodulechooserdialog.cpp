/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/searchdialog/btsearchmodulechooserdialog.h"

#include <QAction>
#include <QSettings>
#include <QToolButton>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/config/cbtconfig.h"
#include "frontend/btbookshelfview.h"
#include "util/cpointers.h"
#include "util/tool.h"


#define HINT BtBookshelfWidget::HintSearchModuleChooserDialog

BtSearchModuleChooserDialog::BtSearchModuleChooserDialog(QWidget *parent,
                                                         Qt::WindowFlags flags)
    : BtModuleChooserDialog(HINT, parent, flags)
{
    bookshelfWidget()->showHideAction()->setVisible(false);
    bookshelfWidget()->showHideButton()->hide();
    bookshelfWidget()->setSourceModel(CPointers::backend()->model());

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
