/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/removepage/btremovepage.h"

#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QMultiMap>
#include <QPushButton>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "backend/btmoduletreeitem.h"
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "util/directory.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/ctoolclass.h"

// Sword includes:
#include <swmgr.h>
#include <installmgr.h>


BtRemovePage::BtRemovePage()
        : BtConfigPage()
{
    namespace DU = util::directory;

    QGridLayout* layout = new QGridLayout(this);
    layout->setMargin(5);

    layout->setSpacing(10);
    layout->setColumnStretch(1, 1);
    layout->setRowStretch(2, 1);

    m_view = new QTreeWidget(this);
    m_view->setHeaderLabels(QStringList() << tr("Work") << tr("Install path"));
    m_view->setColumnWidth(0, CToolClass::mWidth(m_view, 20));

    layout->addWidget( m_view, 2, 0, 1, 2);

    m_removeButton = new QPushButton(tr("Remove..."), this);
    m_removeButton->setToolTip(tr("Remove the selected works"));
    m_removeButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::removepage::remove_icon));
    m_removeButton->setEnabled(false);
    layout->addWidget(m_removeButton, 3, 1, Qt::AlignRight);

    connect(m_view, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(slotItemDoubleClicked(QTreeWidgetItem*, int)));
    connect(m_removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveModules()));
    connect(CPointers::backend(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)), SLOT(slotSwordSetupChanged()));
    populateModuleList();
}

QString BtRemovePage::label() {
    return tr("Remove installed works. Select the works and click Remove button.");
}
QString BtRemovePage::iconName() {
    return CResMgr::bookshelfmgr::removepage::icon;
}
QString BtRemovePage::header() {
    return tr("Remove");
}


void BtRemovePage::populateModuleList() {

    m_view->clear();
    m_selectedModules.clear();

    // disconnect the signal so that we don't have to run functions for every module
    disconnect(m_view, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotSelectionChanged(QTreeWidgetItem*, int)) );

    QList<BTModuleTreeItem::Filter*> empty;
    BTModuleTreeItem rootItem(empty, (BTModuleTreeItem::Grouping)CBTConfig::get(CBTConfig::bookshelfGrouping));
    addToTree(&rootItem, m_view->invisibleRootItem());

    // receive signal when user checks modules
    connect(m_view, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotSelectionChanged(QTreeWidgetItem*, int)) );
    qDebug("BtSourceArea::createModuleTree end");
}

void BtRemovePage::addToTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem) {
    //qDebug()<<"BtRemovePage::addToTree "<<item->text();
    //qDebug() << "BTMTItem type: " << item->type();

    foreach (BTModuleTreeItem* i, item->children()) {
        addToTree(i, new QTreeWidgetItem(widgetItem));
    }
    if (item->type() != BTModuleTreeItem::Root) {
        CSwordModuleInfo* mInfo = item->moduleInfo();
        widgetItem->setText(0, item->text());
        if (item->type() == BTModuleTreeItem::Category || item->type() == BTModuleTreeItem::Language) {
            //qDebug() << "item"<<item->text()<< "was cat or lang";
            widgetItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
        }
        if (item->type() == BTModuleTreeItem::Module) {
            //qDebug() << "item"<<item->text()<< "was a module";
            widgetItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            widgetItem->setCheckState(0, Qt::Unchecked);

            //TODO: add the relevant information in to item or tooltip
            // (install path, is still available from some source)

            QString descr(mInfo->config(CSwordModuleInfo::AbsoluteDataPath));
            QString toolTipText = CToolClass::moduleToolTip(mInfo);
            widgetItem->setText(1, descr);
            widgetItem->setToolTip(0, toolTipText);
            widgetItem->setToolTip(1, descr);
        }
    }
}



void BtRemovePage::slotRemoveModules() {
    if ( m_selectedModules.empty() ) {
        return; //no message, just do nothing
    }

    QStringList moduleNames;
    foreach (CSwordModuleInfo* m, m_selectedModules) {
        moduleNames.append(m->name());
    }
    const QString message = tr("You selected the following work(s): ")
                            .append(moduleNames.join(", "))
                            .append("\n\n")
                            .append(tr("Do you really want to remove them from your system?"));

    if ((QMessageBox::question(this, tr("Remove Works?"), message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)) {  //Yes was pressed.

        // Update the module list before really removing. Remember deleting the pointers later.
        QList<CSwordModuleInfo*> toBeDeleted = CPointers::backend()->takeModulesFromList(moduleNames);

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


void BtRemovePage::slotSelectionChanged(QTreeWidgetItem* item, int column) {
    //qDebug("BtRemovePage::slotSelectionChanged");
    // modify the internal list of checked modules
    // if() leaves groups away
    if (!item->childCount() && column == 0) {
        CSwordModuleInfo* mInfo = 0;
        //qDebug("BtRemovePage::slotSelectionChanged");
        foreach (CSwordModuleInfo* module, CPointers::backend()->moduleList()) {
            if (module->name() == item->text(0) && module->config(CSwordModuleInfo::AbsoluteDataPath) == item->text(1)) {
                mInfo = module;
                break;
            }
        }
        Q_ASSERT(mInfo); // this should have been found
        if (item->checkState(0) == Qt::Checked) {
            //qDebug() << item->text(0) << "in" << item->text(1) << "was checked";
            m_selectedModules.append(mInfo);
        }
        else {
            //qDebug() << mInfo->name() << "was unchecked";
            m_selectedModules.removeAll(mInfo); // there is only one, it's a pointer
        }

        if (m_selectedModules.count() > 0) {
            m_removeButton->setEnabled(true);
        }
        else {
            m_removeButton->setEnabled(false);
        }
    }
}

void BtRemovePage::slotItemDoubleClicked(QTreeWidgetItem* /*item*/, int /*column*/) {
    // TODO: Open the About dialog.
}

void BtRemovePage::slotSwordSetupChanged() {
    populateModuleList();
}
