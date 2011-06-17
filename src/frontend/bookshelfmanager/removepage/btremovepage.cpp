/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include "backend/bookshelfmodel/btbookshelffiltermodel.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/btbookshelfview.h"
#include "frontend/btbookshelfwidget.h"
#include "util/cresmgr.h"
#include "util/dialogutil.h"
#include "util/directory.h"

// Sword includes:
#include <swmgr.h>
#include <installmgr.h>


namespace {
const QString groupingOrderKey("GUI/BookshelfManager/RemovePage/grouping");
}

BtRemovePage::BtRemovePage(BtModuleManagerDialog *parent)
        : BtConfigDialog::Page(parent)
{
    namespace DU = util::directory;

    m_worksGroupBox = new QGroupBox(tr("Select &works to uninstall:"), this);
    m_worksGroupBox->setFlat(true);
    QVBoxLayout *wLayout = new QVBoxLayout;
    wLayout->setContentsMargins(0, 0, 0, 0);
    m_worksGroupBox->setLayout(wLayout);

    BtRemovePageTreeModel *treeModel = new BtRemovePageTreeModel(groupingOrderKey,
                                                                 this);
    connect(treeModel, SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
            this,      SLOT(slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping&)));

    m_bookshelfWidget = new BtBookshelfWidget(this);
    m_bookshelfWidget->postFilterModel()->setShowHidden(true);
    m_bookshelfWidget->setTreeModel(treeModel);
    m_bookshelfWidget->setSourceModel(CSwordBackend::instance()->model());
    m_bookshelfWidget->showHideAction()->setVisible(false);
    m_bookshelfWidget->showHideButton()->hide();
    m_bookshelfWidget->treeView()->header()->show();
    m_bookshelfWidget->treeView()->header()->setResizeMode(QHeaderView::ResizeToContents);
    wLayout->addWidget(m_bookshelfWidget);

    m_uninstallGroupBox = new QGroupBox(this);
    m_uninstallGroupBox->setFlat(true);
    retranslateUninstallGroupBox();
    QHBoxLayout *uLayout = new QHBoxLayout;
    uLayout->setContentsMargins(0, 0, 0, 0);
    m_uninstallGroupBox->setLayout(uLayout);
    uLayout->addStretch(1);

    m_removeButton = new QPushButton(tr("&Remove..."), this);
    m_removeButton->setToolTip(tr("Remove the selected works"));
    m_removeButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::removepage::remove_icon));
    m_removeButton->setEnabled(false);
    uLayout->addWidget(m_removeButton, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_worksGroupBox, 1);
    mainLayout->addWidget(m_uninstallGroupBox);

    connect(m_removeButton, SIGNAL(clicked()),
            this, SLOT(slotRemoveModules()));
    connect(m_bookshelfWidget->treeModel(), SIGNAL(moduleChecked(CSwordModuleInfo*,bool)),
            this,                           SLOT(slotResetRemoveButton()));
    connect(m_bookshelfWidget->treeModel(), SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
            this,                           SLOT(slotResetRemoveButton()));
}

const QIcon &BtRemovePage::icon() const {
    return util::directory::getIcon(CResMgr::bookshelfmgr::removepage::icon);
}

QString BtRemovePage::header() const {
    return tr("Remove");
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

    QStringList moduleNames;
    const int textHeight = fontMetrics().height();
    /// \bug <nobr> is not working, Qt bug
    const QString moduleString("<nobr><img src=\"%1\" width=\"%2\" height=\"%3\"/>&nbsp;%4</nobr>");
    const QString iconDir = util::directory::getIconDir().canonicalPath() + '/';
    Q_FOREACH(const CSwordModuleInfo *m,
              m_bookshelfWidget->treeModel()->checkedModules())
    {
        const QIcon icon = CSwordModuleInfo::moduleIcon(m);
        const QSize iconSize = icon.actualSize(QSize(textHeight, textHeight));
        moduleNames.append(moduleString
                           .arg(iconDir + CSwordModuleInfo::moduleIconFilename(m))
                           .arg(iconSize.width())
                           .arg(iconSize.height())
                           .arg(m->name()));
    }
    const QString message = tr("You selected the following work(s): ")
                            .append("<br/><br/>&nbsp;&nbsp;&nbsp;&nbsp;")
                            .append(moduleNames.join(",&nbsp; "))
                            .append("<br/><br/>")
                            .append(tr("Do you really want to remove them from your system?"));

    if ((util::showQuestion(this, tr("Remove Works?"), message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)) {  //Yes was pressed.

        // Update the module list before really removing. Remember deleting the pointers later.
        QList<CSwordModuleInfo*> toBeDeleted = CSwordBackend::instance()->takeModulesFromList(moduleNames);

        sword::InstallMgr installMgr;
        QMap<QString, sword::SWMgr*> mgrDict; //maps config paths to SWMgr objects
        foreach ( CSwordModuleInfo* mInfo, toBeDeleted ) {
            Q_ASSERT(mInfo); // Only installed modules could have been selected and returned by takeModulesFromList
            // Find the install path for the sword manager
            QString prefixPath = mInfo->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
            QString dataPath = mInfo->config(CSwordModuleInfo::DataPath);
            if (dataPath.left(2) == "./") {
                dataPath = dataPath.mid(2);
            }
            if (prefixPath.contains(dataPath)) { //remove module part to get the prefix path
                prefixPath = prefixPath.remove( prefixPath.indexOf(dataPath), dataPath.length() );
            }
            else { //This is an error, should not happen
                qWarning() << "Removing" << mInfo->name() << "didn't succeed because the absolute path" << prefixPath << "didn't contain the data path" << dataPath;
                continue; // don't remove this, go to next of the for loop
            }

            // Create the sword manager and remove the module
            sword::SWMgr* mgr = mgrDict[ prefixPath ];
            if (!mgr) { //create new mgr if it's not yet available
                mgrDict.insert(prefixPath, new sword::SWMgr(prefixPath.toLocal8Bit()));
                mgr = mgrDict[ prefixPath ];
            }
            qDebug() << "Removing the module" << mInfo->name() << "...";
            installMgr.removeModule(mgr, mInfo->module()->Name());
        }
        //delete the removed moduleinfo pointers
        qDeleteAll(toBeDeleted);
        //delete all mgrs which were created above
        qDeleteAll(mgrDict);
        mgrDict.clear();
    }
}

void BtRemovePage::slotGroupingOrderChanged(const BtBookshelfTreeModel::Grouping &g) {
    g.saveTo(groupingOrderKey);
}
