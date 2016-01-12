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

#include "frontend/bookshelfmanager/removepage/btremovepage.h"

#include <QAction>
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/btinstallmgr.h"
#include "backend/managers/cswordbackend.h"
#include "bibletimeapp.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/btbookshelfview.h"
#include "frontend/btbookshelfwidget.h"
#include "frontend/messagedialog.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/bticons.h"
#include "util/cresmgr.h"
#include "util/directory.h"

// Sword includes:
#include <swmgr.h>


namespace {
const QString groupingOrderKey("GUI/BookshelfManager/RemovePage/grouping");
}

BtRemovePage::BtRemovePage(BtModuleManagerDialog *parent)
        : BtConfigDialog::Page(CResMgr::bookshelfmgr::removepage::icon(),
                               parent)
{
    m_worksGroupBox = new QGroupBox(this);
    m_worksGroupBox->setFlat(true);
    QVBoxLayout *wLayout = new QVBoxLayout;
    wLayout->setContentsMargins(0, 0, 0, 0);
    m_worksGroupBox->setLayout(wLayout);

    BtRemovePageTreeModel *treeModel = new BtRemovePageTreeModel(groupingOrderKey,
                                                                 this);
    BT_CONNECT(treeModel,
               SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
               this,
               SLOT(slotGroupingOrderChanged(
                            BtBookshelfTreeModel::Grouping const &)));

    m_bookshelfWidget = new BtBookshelfWidget(this);
    m_bookshelfWidget->postFilterModel()->setShowHidden(true);
    m_bookshelfWidget->setTreeModel(treeModel);
    m_bookshelfWidget->setSourceModel(CSwordBackend::instance()->model());
    m_bookshelfWidget->showHideAction()->setVisible(false);
    m_bookshelfWidget->showHideButton()->hide();
    m_bookshelfWidget->treeView()->header()->show();
    m_bookshelfWidget->treeView()->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    wLayout->addWidget(m_bookshelfWidget);

    m_uninstallGroupBox = new QGroupBox(this);
    m_uninstallGroupBox->setFlat(true);
    QHBoxLayout *uLayout = new QHBoxLayout;
    uLayout->setContentsMargins(0, 0, 0, 0);
    m_uninstallGroupBox->setLayout(uLayout);
    uLayout->addStretch(1);

    m_removeButton = new QPushButton(this);
    m_removeButton->setIcon(CResMgr::bookshelfmgr::removepage::icon_remove());
    m_removeButton->setEnabled(false);
    uLayout->addWidget(m_removeButton, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_worksGroupBox, 1);
    mainLayout->addWidget(m_uninstallGroupBox);

    BT_CONNECT(m_removeButton, SIGNAL(clicked()),
               this,           SLOT(slotRemoveModules()));
    BT_CONNECT(m_bookshelfWidget->treeModel(),
               SIGNAL(moduleChecked(CSwordModuleInfo *, bool)),
               this, SLOT(slotResetRemoveButton()));
    BT_CONNECT(m_bookshelfWidget->treeModel(),
               SIGNAL(rowsRemoved(QModelIndex const &, int, int)),
               this, SLOT(slotResetRemoveButton()));

    retranslateUi();
}

void BtRemovePage::retranslateUi() {
    setHeaderText(tr("Remove"));

    m_worksGroupBox->setTitle(tr("Select &works to uninstall:"));

    m_removeButton->setText(tr("&Remove..."));
    m_removeButton->setToolTip(tr("Remove the selected works"));

    retranslateUninstallGroupBox();
}

void BtRemovePage::retranslateUninstallGroupBox() {
    int count = m_bookshelfWidget->treeModel()->checkedModules().count();
    if (count > 0) {
        m_uninstallGroupBox->setTitle(tr("Start removal of %1 works:")
                                      .arg(count));
    } else {
        m_uninstallGroupBox->setTitle(tr("Start removal:"));
    }
}

void BtRemovePage::slotResetRemoveButton() {
    retranslateUninstallGroupBox();
    m_removeButton->setEnabled(!m_bookshelfWidget->treeModel()->checkedModules().empty());
}

void BtRemovePage::slotRemoveModules() {
    // Do nothing when this signal fires without anything selected to remove:
    if (m_bookshelfWidget->treeModel()->checkedModules().empty()) return;

    QStringList prettyModuleNames;
    const int textHeight = fontMetrics().height();
    /// \bug <nobr> is not working, Qt bug
    const QString moduleString("<nobr>%1&nbsp;%2</nobr>");
    auto const & toBeDeleted = m_bookshelfWidget->treeModel()->checkedModules();
    for (const CSwordModuleInfo * const m : toBeDeleted) {
        prettyModuleNames.append(moduleString
                                 .arg(iconToHtml(CSwordModuleInfo::moduleIcon(*m),
                                                 textHeight))
                                 .arg(m->name()));
    }
    const QString message = tr("You selected the following work(s): ")
                            .append("<br/><br/>&nbsp;&nbsp;&nbsp;&nbsp;")
                            .append(prettyModuleNames.join(",&nbsp; "))
                            .append("<br/><br/>")
                            .append(tr("Do you really want to remove them from your system?"));

    if ((message::showQuestion(this, tr("Remove Works?"), message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)) {  //Yes was pressed.
        CSwordBackend::instance()->uninstallModules(toBeDeleted);
    }
}

void BtRemovePage::slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g) {
    g.saveTo(groupingOrderKey);
}
