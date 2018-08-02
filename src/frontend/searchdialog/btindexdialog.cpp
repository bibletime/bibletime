/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/btindexdialog.h"

#include <QCheckBox>
#include <QDir>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include "backend/config/btconfig.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/btmoduleindexdialog.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"
#include "util/tool.h"


BtIndexDialog::BtIndexDialog(QDialog *parent)
    : QDialog(parent)
{
    setWindowIcon(CResMgr::searchdialog::icon());

    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    QHBoxLayout *hboxLayout = new QHBoxLayout();

    m_autoDeleteOrphanedIndicesBox = new QCheckBox(this);
    vboxLayout->addWidget(m_autoDeleteOrphanedIndicesBox);

    m_moduleList = new QTreeWidget(this);
    vboxLayout->addWidget(m_moduleList);

    QSpacerItem* spacerItem = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hboxLayout->addItem(spacerItem);

    m_deleteButton = new QPushButton(this);
    m_deleteButton->setIcon(CResMgr::bookshelfmgr::indexpage::icon_delete());
    hboxLayout->addWidget(m_deleteButton);

    m_createButton = new QPushButton(this);
    m_createButton->setIcon(CResMgr::bookshelfmgr::indexpage::icon_create());
    hboxLayout->addWidget(m_createButton);

    m_closeButton = new QPushButton(this);
    m_closeButton->setIcon(CResMgr::searchdialog::icon_close());
    hboxLayout->addWidget(m_closeButton);

    vboxLayout->addLayout(hboxLayout);

    // configure the list view
    m_moduleList->setRootIsDecorated(true);
    m_moduleList->setColumnWidth(0, util::tool::mWidth(m_moduleList, 20) );
    m_moduleList->setSortingEnabled(false);

    m_autoDeleteOrphanedIndicesBox->setChecked( btConfig().value<bool>("settings/behaviour/autoDeleteOrphanedIndices", true) );

    // connect our signals/slots
    BT_CONNECT(m_createButton, SIGNAL(clicked()),
               this,           SLOT(createIndices()));
    BT_CONNECT(m_deleteButton, SIGNAL(clicked()),
               this,           SLOT(deleteIndices()));
    BT_CONNECT(m_closeButton, SIGNAL(clicked()),
               this,           SLOT(close()));
    BT_CONNECT(CSwordBackend::instance(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)),
               this,                      SLOT(slotSwordSetupChanged()));
    BT_CONNECT(m_autoDeleteOrphanedIndicesBox, SIGNAL(stateChanged(int)),
               this,                           SLOT(autoDeleteOrphanedIndicesChanged(int)));

    retranslateUi(); // also calls populateModuleList();
}

void BtIndexDialog::autoDeleteOrphanedIndicesChanged(int newState) {
    btConfig().setValue("settings/behaviour/autoDeleteOrphanedIndices",
                        newState == Qt::Checked);
}

/** Populates the module list with installed modules and orphaned indices */
void BtIndexDialog::populateModuleList() {
    typedef QList<CSwordModuleInfo*>::const_iterator MLCI;

    m_moduleList->clear();

    // populate installed modules
    m_modsWithIndices = new QTreeWidgetItem(m_moduleList);
    m_modsWithIndices->setText(0, tr("Indexed Works"));
    m_modsWithIndices->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
    m_modsWithIndices->setExpanded(true);

    m_modsWithoutIndices = new QTreeWidgetItem(m_moduleList);
    m_modsWithoutIndices->setText(0, tr("Unindexed Works"));
    m_modsWithoutIndices->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
    m_modsWithoutIndices->setExpanded(true);

    const QList<CSwordModuleInfo*> &modules(CSwordBackend::instance()->moduleList());
    for (MLCI it(modules.begin()); it != modules.end(); ++it) {
        QTreeWidgetItem* item = nullptr;

        if ((*it)->hasIndex()) {
            item = new QTreeWidgetItem(m_modsWithIndices);
            item->setText(0, (*it)->name());
            item->setText(1, tr("%1 KiB").arg((*it)->indexSize() / 1024));
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Unchecked);
        }
        else {
            item = new QTreeWidgetItem(m_modsWithoutIndices);
            item->setText(0, (*it)->name());
            item->setText(1, tr("0 KiB"));
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
        }
    }
}

void BtIndexDialog::retranslateUi() {

    setWindowTitle(tr("Manage Search Indexes"));

    m_autoDeleteOrphanedIndicesBox->setToolTip(tr("If selected, those indexes which have no corresponding work will be deleted when BibleTime starts"));
    m_autoDeleteOrphanedIndicesBox->setText(tr("Automatically delete orphaned indexes when BibleTime starts"));

    m_deleteButton->setToolTip(tr("Delete the selected indexes"));
    m_deleteButton->setText(tr("Delete"));

    m_closeButton->setText(tr("&Close"));

    m_createButton->setToolTip(tr("Create new indexes for the selected works"));
    m_createButton->setText(tr("Create..."));

    m_moduleList->setHeaderLabels( (QStringList(tr("Work")) << tr("Index size")) );

    populateModuleList();
}

/** Creates indices for selected modules if no index currently exists */
void BtIndexDialog::createIndices() {
    QList<CSwordModuleInfo*> moduleList;

    auto & backend = *CSwordBackend::instance();
    for (int i = 0; i < m_modsWithoutIndices->childCount(); ++i) {
        if (m_modsWithoutIndices->child(i)->checkState(0) == Qt::Checked) {
            if (auto * module = backend.findModuleByName(
                    m_modsWithoutIndices->child(i)->text(0).toUtf8()))
                moduleList.append(module);
        }
    }

    //Shows the progress dialog
    if (!moduleList.isEmpty()) {
        BtModuleIndexDialog::indexAllModules(moduleList);
        populateModuleList();
    }
}

/** Deletes indices for selected modules */
void BtIndexDialog::deleteIndices() {
    bool indicesDeleted = false;

    auto & backend = *CSwordBackend::instance();
    for (int i = 0; i < m_modsWithIndices->childCount(); ++i) {
        if (m_modsWithIndices->child(i)->checkState(0) == Qt::Checked) {
            if (auto * module = backend.findModuleByName(
                                m_modsWithIndices->child(i)->text(0).toUtf8()))
            {
                module->deleteIndex();
                indicesDeleted = true;
            }
        }
    }

    // repopulate the list if an action was taken
    if (indicesDeleted)
        populateModuleList();
}

void BtIndexDialog::slotSwordSetupChanged() {
    populateModuleList();
}

