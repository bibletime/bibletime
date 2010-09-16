/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btsourcearea.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QSpacerItem>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include "backend/managers/cswordbackend.h"
#include "backend/btinstallbackend.h"
#include "frontend/bookshelfmanager/installpage/btsourceareamodel.h"
#include "frontend/bookshelfmanager/installpage/btsourcewidget.h"
#include "frontend/btaboutmoduledialog.h"
#include "frontend/btbookshelfview.h"
#include "frontend/btbookshelfwidget.h"
#include "util/directory.h"
#include "util/cresmgr.h"
#include "util/tool.h"

// Sword includes:
#include <installmgr.h>


namespace {

/** Filters out already installed modules which can't be updated right now. */
bool filter(CSwordModuleInfo *mInfo) {
    typedef CSwordModuleInfo CSMI;
    typedef sword::SWVersion SV;

    const CSMI *installedModule = CSwordBackend::instance()->findModuleByName(mInfo->name());
    if (installedModule) {
        // Already installed, check if it's an update:
        const SV curVersion(installedModule->config(CSMI::ModuleVersion).toLatin1());
        const SV newVersion(mInfo->config(CSMI::ModuleVersion).toLatin1());
        if (curVersion >= newVersion) {
            return false;
        }
    }
    return true;
}

} // anonymous namespace

// ****************************************************************
// ******** Installation source and module list widget ************
// ****************************************************************

BtSourceArea::BtSourceArea(const QString &sourceName, BtSourceWidget *parent)
        : QWidget(parent),
        m_sourceName(sourceName),
        m_treeAlreadyInitialized(false),
        m_remoteBackend(0), //important!
        m_model(0),
        m_parent(parent)
{
    setObjectName(sourceName);

    m_treeModel = new BtSourceAreaModel(BtBookshelfTreeModel::Grouping(true), this);

    initView();

    connect(m_treeModel, SIGNAL(groupingOrderChanged(BtBookshelfTreeModel::Grouping)),
            this,        SLOT(slotViewGroupingOrderChanged()));
    connect(m_view->treeView()->header(), SIGNAL(geometriesChanged()),
            this,                         SLOT(slotHeaderChanged()));
}

BtSourceArea::~BtSourceArea() {
    delete m_remoteBackend;
}

void BtSourceArea::initView() {
    namespace DU = util::directory;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // source related button row
    QHBoxLayout *sourceLayout = new QHBoxLayout();
    m_refreshButton = new QPushButton(tr("Refresh..."));
    m_refreshButton->setToolTip(tr("Refresh the list of works from this source"));
    m_refreshButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::refresh_icon));
    //m_refreshButton->setEnabled(false);
    QSpacerItem *sourceSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_deleteButton = new QPushButton(tr("Delete..."));
    m_deleteButton->setToolTip(tr("Delete this source"));
    m_deleteButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::delete_icon));
    //m_deleteButton->setEnabled(false);
    m_addButton = new QPushButton(tr("Add..."));
    m_addButton->setToolTip(tr("Add new source"));
    m_addButton->setIcon(DU::getIcon(CResMgr::bookshelfmgr::installpage::add_icon));

    sourceLayout->addWidget(m_refreshButton);
    sourceLayout->addItem(sourceSpacer);
    sourceLayout->addWidget(m_deleteButton);
    sourceLayout->addWidget(m_addButton);

    mainLayout->addLayout(sourceLayout);
    // There are no views for the stack yet, see initSources
    m_view = new BtBookshelfWidget(this);
    m_view->treeView()->setHeaderHidden(false);
    m_view->showHideButton()->hide();
    m_view->setTreeModel(m_treeModel);
    mainLayout->addWidget(m_view);

    connect(m_view->treeView(), SIGNAL(doubleClicked(QModelIndex)),
            this,   SLOT(slotItemDoubleClicked(const QModelIndex&)));
    connect(this, SIGNAL(signalCreateTree()),
            this, SLOT(slotCreateTree()), Qt::QueuedConnection);
}

void BtSourceArea::prepareRemove() {
    // don't create tree anymore, this will be removed
    disconnect(this, SIGNAL(signalCreateTree()),
               this, SLOT(slotCreateTree()));
}

QSize BtSourceArea::sizeHint() const {
    return QSize(100, m_refreshButton->height() + (m_view->treeView()->header()->height() * 5));
}

void BtSourceArea::initTreeFirstTime() {
    if (!m_treeAlreadyInitialized) {
        createModuleTree();
        m_treeAlreadyInitialized = true;
    }
}

void BtSourceArea::createModuleTree() {
    // Start creating tree with a queued connection.
    // This makes showing the dialog possible even before the tree is initialized.
    emit signalCreateTree();
}
void BtSourceArea::slotCreateTree() {
    //let the dialog become visible
    QCoreApplication::processEvents();
    // Creating the view and populating list may take time
    QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

    // disconnect the signal so that we don't have to run functions for every module
    // (note: what to do if we want to restore the item selection when rebuilding?
    disconnect(m_treeModel, SIGNAL(moduleChecked(CSwordModuleInfo*,bool)),
               this,        SIGNAL(signalInstallablesChanged()));

    sword::InstallSource is = BtInstallBackend::source(m_sourceName);
    delete m_remoteBackend; // the old one can be deleted
    m_remoteBackend = BtInstallBackend::backend(is);
    Q_ASSERT(m_remoteBackend);

    // give the list to BTModuleTreeItem, create filter to remove
    // those modules which are installed already

    BtBookshelfModel *model = new BtBookshelfModel(this);
    Q_FOREACH (CSwordModuleInfo *module, m_remoteBackend->moduleList()) {
        if (filter(module)) model->addModule(module);
    }
    m_treeModel->setSourceModel(model);
    delete m_model;
    m_model = model;

    QCoreApplication::processEvents();
    // receive signal when user checks modules
    connect(m_treeModel, SIGNAL(moduleChecked(CSwordModuleInfo*,bool)),
            this,        SIGNAL(signalInstallablesChanged()));
    QApplication::restoreOverrideCursor();
}

// return the selected modules
const QSet<CSwordModuleInfo*> &BtSourceArea::selectedModules() const {
    return m_treeModel->checkedModules();
}

void BtSourceArea::slotItemDoubleClicked(const QModelIndex &i) {
    QModelIndex itemIndex = m_view->treeView()->model()->index(i.row(), 0, i.parent());
    CSwordModuleInfo *mInfo = m_view->treeView()->getModule(itemIndex);
    if (mInfo) {
        BTAboutModuleDialog *dialog = new BTAboutModuleDialog(mInfo, this);
        dialog->setAttribute(Qt::WA_DeleteOnClose); // Destroy dialog when closed
        dialog->show();
        dialog->raise();
    }
}

void BtSourceArea::slotViewGroupingOrderChanged() {
    m_parent->setGroupingOrder(m_treeModel->groupingOrder());
}

void BtSourceArea::slotHeaderChanged() {
    m_parent->setHeaderState(m_view->treeView()->header()->saveState());
}

void BtSourceArea::syncGroupingOrder(const BtBookshelfTreeModel::Grouping &groupingOrder) {
    m_treeModel->setGroupingOrder(groupingOrder, false);
    m_view->treeView()->setRootIsDecorated(!groupingOrder.empty());
}

void BtSourceArea::syncHeaderState(const QByteArray &state) {
    disconnect(m_view->treeView()->header(), SIGNAL(geometriesChanged()),
               this,                         SLOT(slotHeaderChanged()));
    m_view->treeView()->header()->restoreState(state);
    connect(m_view->treeView()->header(), SIGNAL(geometriesChanged()),
            this,                         SLOT(slotHeaderChanged()));
}
