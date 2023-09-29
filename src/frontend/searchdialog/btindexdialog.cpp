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

#include "btindexdialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QFlags>
#include <QList>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../btmoduleindexdialog.h"


BtIndexDialog::BtIndexDialog(QWidget * parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setWindowIcon(CResMgr::searchdialog::icon());

    auto vboxLayout = new QVBoxLayout(this);
    auto hboxLayout = new QHBoxLayout();

    m_autoDeleteOrphanedIndicesBox = new QCheckBox(this);
    vboxLayout->addWidget(m_autoDeleteOrphanedIndicesBox);

    m_moduleList = new QTreeWidget(this);
    vboxLayout->addWidget(m_moduleList);

    auto spacerItem =
            new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
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
    m_moduleList->setColumnWidth(0, util::tool::mWidth(*m_moduleList, 20));
    m_moduleList->setSortingEnabled(false);

    m_autoDeleteOrphanedIndicesBox->setChecked(
                btConfig().value<bool>(
                    QStringLiteral(
                        "settings/behaviour/autoDeleteOrphanedIndices"),
                    true));

    // connect our signals/slots
    BT_CONNECT(m_createButton, &QPushButton::clicked,
               this,           &BtIndexDialog::createIndices);
    BT_CONNECT(m_deleteButton, &QPushButton::clicked,
               this,           &BtIndexDialog::deleteIndices);
    BT_CONNECT(m_closeButton, &QPushButton::clicked,
               this,          &BtIndexDialog::close);
    BT_CONNECT(&CSwordBackend::instance(), &CSwordBackend::sigSwordSetupChanged,
               this,  &BtIndexDialog::slotSwordSetupChanged);
    BT_CONNECT(m_autoDeleteOrphanedIndicesBox, &QCheckBox::stateChanged,
               this, &BtIndexDialog::autoDeleteOrphanedIndicesChanged);

    retranslateUi(); // also calls populateModuleList();
}

/** Populates the module list with installed modules and orphaned indices */
void BtIndexDialog::populateModuleList() {
    m_moduleList->clear();

    // populate installed modules
    m_modsWithIndices = new QTreeWidgetItem(m_moduleList);
    m_modsWithIndices->setText(0, tr("Indexed Works"));
    m_modsWithIndices->setFlags(Qt::ItemIsUserCheckable
                                | Qt::ItemIsEnabled
                                | Qt::ItemIsAutoTristate);
    m_modsWithIndices->setExpanded(true);

    m_modsWithoutIndices = new QTreeWidgetItem(m_moduleList);
    m_modsWithoutIndices->setText(0, tr("Unindexed Works"));
    m_modsWithoutIndices->setFlags(Qt::ItemIsUserCheckable
                                   | Qt::ItemIsEnabled
                                   | Qt::ItemIsAutoTristate);
    m_modsWithoutIndices->setExpanded(true);

    for (auto const & modulePtr : CSwordBackend::instance().moduleList()) {
        if (modulePtr->hasIndex()) {
            auto item = new QTreeWidgetItem(m_modsWithIndices);
            item->setText(0, modulePtr->name());
            item->setText(1, tr("%1 KiB").arg(modulePtr->indexSize() / 1024));
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Unchecked);
        } else {
            auto item = new QTreeWidgetItem(m_modsWithoutIndices);
            item->setText(0, modulePtr->name());
            item->setText(1, tr("0 KiB"));
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setCheckState(0, Qt::Checked);
        }
    }
}

void BtIndexDialog::retranslateUi() {
    setWindowTitle(tr("Manage Search Indexes"));

    m_autoDeleteOrphanedIndicesBox->setToolTip(
                tr("If selected, those indexes which have no corresponding "
                   "work will be deleted when BibleTime starts"));
    m_autoDeleteOrphanedIndicesBox->setText(
                tr("Automatically delete orphaned indexes when BibleTime "
                   "starts"));

    m_deleteButton->setToolTip(tr("Delete the selected indexes"));
    m_deleteButton->setText(tr("Delete"));

    m_closeButton->setText(tr("&Close"));

    m_createButton->setToolTip(tr("Create new indexes for the selected works"));
    m_createButton->setText(tr("Create..."));

    m_moduleList->setHeaderLabels(QStringList(tr("Work")) << tr("Index size"));

    populateModuleList();
}

void BtIndexDialog::autoDeleteOrphanedIndicesChanged(int newState) {
    btConfig().setValue(
                QStringLiteral("settings/behaviour/autoDeleteOrphanedIndices"),
                newState == Qt::Checked);
}

/** Creates indices for selected modules if no index currently exists */
void BtIndexDialog::createIndices() {
    QList<CSwordModuleInfo *> moduleList;

    auto & backend = CSwordBackend::instance();
    for (int i = 0; i < m_modsWithoutIndices->childCount(); ++i) {
        if (m_modsWithoutIndices->child(i)->checkState(0) == Qt::Checked) {
            if (auto * module = backend.findModuleByName(
                    m_modsWithoutIndices->child(i)->text(0).toUtf8()))
                moduleList.append(module);
        }
    }

    if (!moduleList.isEmpty()) { // This also shows the progress dialog
        BtModuleIndexDialog::indexAllModules(moduleList);
        populateModuleList();
    }
}

/** Deletes indices for selected modules */
void BtIndexDialog::deleteIndices() {
    bool indicesDeleted = false;

    auto & backend = CSwordBackend::instance();
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

void BtIndexDialog::slotSwordSetupChanged() { populateModuleList(); }
