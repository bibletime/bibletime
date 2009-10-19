/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btsourcearea.h"

#include <QString>
#include <QWidget>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QDebug>
#include <QTime>
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/instbackend.h"
#include "frontend/btaboutmoduledialog.h"
#include "util/directory.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/tool.h"

// Sword includes:
#include <installmgr.h>


// ****************************************************************
// ******** Installation source and module list widget ************
// ****************************************************************

BtSourceArea::BtSourceArea(const QString& sourceName)
        : QWidget(),
        m_sourceName(sourceName),
        m_treeAlreadyInitialized(false),
        m_remoteBackend(0) { //important!
    m_checkedModules = QMap<QString, bool>();
    qDebug() << "BtSourceArea::BtSourceArea, " << m_sourceName;
    initView();
}

BtSourceArea::~BtSourceArea() {
    delete m_remoteBackend;
}

void BtSourceArea::initView() {
    namespace DU = util::directory;

    qDebug("BtSourceArea::initView");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    //QHBoxLayout *refreshLabelLayout = new QHBoxLayout();
    //QLabel *refreshLabel = new QLabel(tr("Last refreshed:"));
    //m_refreshTimeLabel = new QLabel();
    //QSpacerItem *refreshLabelSpacer = new QSpacerItem(201, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    //refreshLabelLayout->addWidget(refreshLabel);
    //refreshLabelLayout->addWidget(m_refreshTimeLabel);
    //refreshLabelLayout->addItem(refreshLabelSpacer);
    // TODO: or would it be better to integrate this information into the tooltip
    // of the source tab?
    //mainLayout->addLayout(refreshLabelLayout);

    // source related button row
    QHBoxLayout *sourceLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton(tr("Refresh..."));
    m_refreshButton->setToolTip(tr("Refresh the list of works from this source"));
    m_refreshButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::refresh_icon));
    //m_refreshButton->setEnabled(false);
    QSpacerItem *sourceSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    //m_editButton = new QPushButton(tr("Edit..."));
    //m_editButton->setEnabled(false); // TODO after writing the edit widget
    m_deleteButton = new QPushButton(tr("Delete..."));
    m_deleteButton->setToolTip(tr("Delete this source"));
    m_deleteButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::delete_icon));
    //m_deleteButton->setEnabled(false);
    m_addButton = new QPushButton(tr("Add..."));
    m_addButton->setToolTip(tr("Add new source"));
    m_addButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::add_icon));

    sourceLayout->addWidget(m_refreshButton);
    sourceLayout->addItem(sourceSpacer);
    //sourceLayout->addWidget(m_editButton);
    sourceLayout->addWidget(m_deleteButton);
    sourceLayout->addWidget(m_addButton);

    mainLayout->addLayout(sourceLayout);
    // There are no views for the stack yet, see initSources
    m_view = new QTreeWidget(this);
    m_view->setHeaderLabels(QStringList() << tr("Work") << tr("Description"));
    m_view->setColumnWidth(0, util::tool::mWidth(m_view, 20));
    mainLayout->addWidget(m_view);

    connect(m_view, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(slotItemDoubleClicked(QTreeWidgetItem*, int)));
    connect(CPointers::backend(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)), SLOT(slotSwordSetupChanged()));
    connect(this, SIGNAL(signalCreateTree()), SLOT(slotCreateTree()), Qt::QueuedConnection);
}

QSize BtSourceArea::sizeHint() const {
    return QSize(100, m_refreshButton->height() + (m_view->header()->height() * 5));
}

void BtSourceArea::initTreeFirstTime() {
    if (!m_treeAlreadyInitialized) {
        createModuleTree();
        m_treeAlreadyInitialized = true;
    }
}

void BtSourceArea::createModuleTree() {
    qDebug("BtSourceArea::createModuleTree start");
    // Start creating tree with a queued connection.
    // This makes showing the dialog possible even before the tree is initialized.
    emit signalCreateTree();
}
void BtSourceArea::slotCreateTree() {
    qDebug() << "BtSourceArea::slotCreateTree" << QTime::currentTime ();
    //let the dialog become visible
    QCoreApplication::processEvents();
    // Creating the view and populating list may take time
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    // disconnect the signal so that we don't have to run functions for every module
    // (note: what to do if we want to restore the item selection when rebuilding?
    disconnect(m_view, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotSelectionChanged(QTreeWidgetItem*, int)) );
    m_view->clear();

    // TODO: if the tree already exists for this source,
    // maybe the selections should be preserved
    m_checkedModules.clear();

    sword::InstallSource is = instbackend::source(m_sourceName);
    delete m_remoteBackend; // the old one can be deleted
    m_remoteBackend = instbackend::backend(is);
    Q_ASSERT(m_remoteBackend);
    m_moduleList = m_remoteBackend->moduleList();

    // give the list to BTModuleTreeItem, create filter to remove
    // those modules which are installed already
    InstalledFilter alreadyInstalledFilter(m_sourceName);
    QList<BTModuleTreeItem::Filter*> filterList;
    filterList.append(&alreadyInstalledFilter);
    BTModuleTreeItem rootItem(filterList, BTModuleTreeItem::CatLangMod, &m_moduleList);

    addToTree(&rootItem, m_view->invisibleRootItem());
    QCoreApplication::processEvents();
    // receive signal when user checks modules
    connect(m_view, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotSelectionChanged(QTreeWidgetItem*, int)) );
    QApplication::restoreOverrideCursor();
    qDebug() << "BtSourceArea::createModuleTree end" << QTime::currentTime ();
}

void BtSourceArea::addToTree(BTModuleTreeItem* item, QTreeWidgetItem* widgetItem) {
    //qDebug()<<"BtSourceArea::addToTree "<<item->text();
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

            CSwordModuleInfo* const installedModule = CPointers::backend()->findModuleByName(mInfo->name());
            QString installedV;

            if (!installedModule) {
                // possible TODO: save the module list of a source before refreshing,
                // compare after refreshing, mark the newly added modules
                //if not newly added:
                //state: installable (no indicator)
                //else: status: newly added, color yellow
            } else { // the module is already installed
                QBrush bg(QColor(255, 153, 153)); /// \bug Possible color conflict
                widgetItem->setBackground(0, bg);
                widgetItem->setBackground(1, bg);
                installedV = QString(installedModule->config(CSwordModuleInfo::ModuleVersion).toLatin1());
                // set the color for the parent items
                QTreeWidgetItem* parent1 = widgetItem->parent();
                if (parent1) {
                    parent1->setBackground(0, bg);
                    parent1->setBackground(1, bg);
                    QTreeWidgetItem* parent2 = parent1->parent();
                    if (parent2) {
                        parent2->setBackground(0, bg);
                        parent2->setBackground(1, bg);
                    }
                }
            }


            QString descr(mInfo->config(CSwordModuleInfo::Description));
            QString toolTipText = util::tool::remoteModuleToolTip(mInfo, installedV);

            widgetItem->setText(1, descr);
            widgetItem->setToolTip(0, toolTipText);
            widgetItem->setToolTip(1, toolTipText);
        }
    }
}

QTreeWidget* BtSourceArea::treeWidget() {
    return m_view;
}

// return the selected modules
QMap<QString, bool>* BtSourceArea::selectedModules() {
    return &m_checkedModules;
}

// when a module is checked/unchecked
void BtSourceArea::slotSelectionChanged(QTreeWidgetItem* item, int column) {
    //qDebug("BtSourceArea::slotSelectionChanged");
    // modify the internal list of selected (actually checked) modules
    // if() leaves groups away
    if (!item->childCount() && column == 0) {
        foreach (CSwordModuleInfo* module, m_moduleList) {
            if (module->name() == item->text(0)) {
                if (item->checkState(0) == Qt::Checked) {
                    qDebug() << module->name() << "was checked";
                    m_checkedModules.insert(module->name(), true);
                }
                else {
                    qDebug() << module->name() << "was unchecked";
                    m_checkedModules.remove(module->name());
                }
                emit signalSelectionChanged(m_sourceName, m_checkedModules.count());
                break;
            }
        }
    }
}

void BtSourceArea::slotItemDoubleClicked(QTreeWidgetItem* item, int /*column*/) {
    CSwordModuleInfo* mInfo = m_remoteBackend->findModuleByName(item->text(0));
    if (mInfo) {
        BTAboutModuleDialog* dialog = new BTAboutModuleDialog(this, mInfo);
        dialog->show();
        dialog->raise();
    }
}

BtSourceArea::InstalledFilter::InstalledFilter(QString sourceName)
        : BTModuleTreeItem::Filter(),
        m_source(instbackend::source(sourceName)),
        m_swordBackend(instbackend::backend(m_source)) {
    // these are set once to optimize away repeated calls
    // m_source, m_swordBackend

}
//filter out already installed, not updateable modules
bool BtSourceArea::InstalledFilter::filter(CSwordModuleInfo* mInfo) {
    //qDebug() << "BtSourceArea::InstalledFilter::filter, module " << mInfo->name();
    CSwordModuleInfo* const installedModule = CPointers::backend()->findModuleByName(mInfo->name());
    if (installedModule) {
        //qDebug("already installed, check if it's an update...");
        const sword::SWVersion installedVersion(installedModule->config(CSwordModuleInfo::ModuleVersion).toLatin1());
        const sword::SWVersion newVersion(mInfo->config(CSwordModuleInfo::ModuleVersion).toLatin1());
        if (installedVersion >= newVersion) {
            return false;
        }
    }
    return true;
}

void BtSourceArea::slotSwordSetupChanged() {
    createModuleTree();
}
