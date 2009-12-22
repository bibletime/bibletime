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

#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialog.h"

#include <QHeaderView>
#include <QSettings>
#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/cbtconfig.h"
#include "frontend/btbookshelfview.h"
#include "util/cpointers.h"
#include "util/tool.h"


#define ISGROUPING(v) (v).canConvert<BtBookshelfTreeModel::Grouping>()
#define TOGROUPING(v) (v).value<BtBookshelfTreeModel::Grouping>()

BtInstallModuleChooserDialog::BtInstallModuleChooserDialog(QWidget *parent,
                                                           Qt::WindowFlags flags)
    : BtModuleChooserDialog(parent, flags), m_shown(false)
{
    resize(550, 340);

    m_bookshelfModel = new BtBookshelfModel(this);

    // Setup tree model on top of the regular model:
    /**
      \todo Subclass bookshelf tree model to a multi-column one which would highlight
            conflicting modules.
    */
    QSettings *settings(CBTConfig::getConfig());
    settings->beginGroup("GUI");
    {
        /*
          If BtInstallModuleChooserDialog does not have its own grouping, we read the grouping
          from the main window bookshelf dock.
        */
        QVariant v;

        if (settings->value("BookshelfManager/InstallPage/ConfirmDialog/hasOwnGrouping", false).toBool()) {
            v = settings->value("BookshelfManager/InstallPage/ConfirmDialog/grouping");
            if (ISGROUPING(v)) {
                v = settings->value("MainWindow/Docks/Bookshelf/grouping");
            }
        } else {
            v = settings->value("MainWindow/Docks/Bookshelf/grouping");
        }

        if (ISGROUPING(v)) {
            m_bookshelfTreeModel = new BtInstallModuleChooserDialogModel(TOGROUPING(v), this);
        } else {
            m_bookshelfTreeModel = new BtInstallModuleChooserDialogModel(this);
        }
    }
    settings->endGroup();
    m_bookshelfTreeModel->setDefaultChecked(BtBookshelfTreeModel::CHECKED);
    m_bookshelfTreeModel->setCheckable(true);
    m_bookshelfTreeModel->setSourceModel(m_bookshelfModel);

    // Setup view:
    treeView()->setModel(m_bookshelfTreeModel);
    treeView()->header()->show();

    retranslateUi();
}

BtInstallModuleChooserDialog::~BtInstallModuleChooserDialog() {
    // Intentionally empty
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
    treeView()->expandAll();
    treeView()->header()->resizeSections(QHeaderView::ResizeToContents);
    m_shown = true;
}
