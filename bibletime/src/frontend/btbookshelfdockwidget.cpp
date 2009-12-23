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

#include "frontend/btbookshelfdockwidget.h"

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/btbookshelfview.h"
#include "frontend/btbookshelfwidget.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directory.h"

BtBookshelfDockWidget::BtBookshelfDockWidget(QWidget *parent, Qt::WindowFlags f)
        : QDockWidget(parent, f) {
    setObjectName("BookshelfDock");

    // Setup actions and menus:
    initMenus();

    // Setup widgets:
    m_bookshelfWidget = new BtBookshelfWidget(BtBookshelfWidget::HintBookshelfDock, this);
    m_bookshelfWidget->setSourceModel(CPointers::backend()->model());
    m_bookshelfWidget->setItemContextMenu(m_itemContextMenu);
    setWidget(m_bookshelfWidget);


    connect(m_bookshelfWidget->treeView(), SIGNAL(moduleActivated(CSwordModuleInfo*)),
            this,                 SIGNAL(moduleOpenTriggered(CSwordModuleInfo*)));
    connect(m_bookshelfWidget->treeModel(), SIGNAL(moduleChecked(CSwordModuleInfo*, bool)),
            this,                  SLOT(slotModuleChecked(CSwordModuleInfo*, bool)));
    connect(m_bookshelfWidget->showHideAction(), SIGNAL(toggled(bool)),
            this,                       SLOT(slotEditHiddenModules(bool)));

    retranslateUi();
}

void BtBookshelfDockWidget::initMenus() {
    namespace DU = util::directory;
    namespace RM = CResMgr::mainIndex;

    m_itemContextMenu = new QMenu(this);
    m_itemActionGroup = new QActionGroup(this);
    connect(m_itemActionGroup, SIGNAL(triggered(QAction*)),
            this,              SLOT(slotItemActionTriggered(QAction*)));

    m_itemOpenAction = new QAction(this);
    m_itemActionGroup->addAction(m_itemOpenAction);
    m_itemContextMenu->addAction(m_itemOpenAction);

    m_itemSearchAction = new QAction(this);
    m_itemSearchAction->setIcon(DU::getIcon(RM::search::icon));
    m_itemActionGroup->addAction(m_itemSearchAction);
    m_itemContextMenu->addAction(m_itemSearchAction);

    m_itemEditMenu = new QMenu(this);
    m_itemEditMenu->setIcon(DU::getIcon(RM::editModuleMenu::icon));
    m_itemContextMenu->addMenu(m_itemEditMenu);
    m_itemEditPlainAction = new QAction(this);
    m_itemEditPlainAction->setIcon(DU::getIcon(RM::editModulePlain::icon));
    m_itemActionGroup->addAction(m_itemEditPlainAction);
    m_itemEditMenu->addAction(m_itemEditPlainAction);

    m_itemEditHtmlAction = new QAction(this);
    m_itemEditHtmlAction->setIcon(DU::getIcon(RM::editModuleHTML::icon));
    m_itemActionGroup->addAction(m_itemEditHtmlAction);
    m_itemEditMenu->addAction(m_itemEditHtmlAction);

    m_itemUnlockAction = new QAction(this);
    m_itemUnlockAction->setIcon(DU::getIcon(RM::unlockModule::icon));
    m_itemActionGroup->addAction(m_itemUnlockAction);
    m_itemContextMenu->addAction(m_itemUnlockAction);

    m_itemAboutAction = new QAction(this);
    m_itemAboutAction->setIcon(DU::getIcon(RM::aboutModule::icon));
    m_itemActionGroup->addAction(m_itemAboutAction);
    m_itemContextMenu->addAction(m_itemAboutAction);

    connect(m_itemContextMenu, SIGNAL(aboutToShow()),
            this,              SLOT(slotPrepareItemContextMenu()));
}

void BtBookshelfDockWidget::retranslateUi() {
    setWindowTitle(tr("Bookshelf"));

    m_itemOpenAction->setText(tr("&Open"));
    m_itemEditMenu->setTitle(tr("&Edit"));
    m_itemEditPlainAction->setText(tr("&Plain text"));
    m_itemEditHtmlAction->setText(tr("&HTML"));
    m_itemUnlockAction->setText(tr("&Unlock..."));
    m_itemAboutAction->setText(tr("&About..."));
}

void BtBookshelfDockWidget::slotModuleChecked(CSwordModuleInfo *module, bool c) {
    module->setHidden(!c);
}

void BtBookshelfDockWidget::slotItemActionTriggered(QAction *action) {
    CSwordModuleInfo *module((CSwordModuleInfo*) m_itemContextMenu->property("BtModule").value<void*>());
    if (module == 0) return;

    if (action == m_itemOpenAction) {
        emit moduleOpenTriggered(module);
    }
    else if (action == m_itemSearchAction) {
        emit moduleSearchTriggered(module);
    }
    else if (action == m_itemEditPlainAction) {
        emit moduleEditPlainTriggered(module);
    }
    else if (action == m_itemEditHtmlAction) {
        emit moduleEditHtmlTriggered(module);
    }
    else if (action == m_itemUnlockAction) {
        emit moduleUnlockTriggered(module);
    }
    else if (action == m_itemAboutAction) {
        emit moduleAboutTriggered(module);
    }
}

void BtBookshelfDockWidget::slotPrepareItemContextMenu() {
    void *v = m_itemContextMenu->property("BtModule").value<void*>();
    CSwordModuleInfo *module = static_cast<CSwordModuleInfo*>(v);
    m_itemSearchAction->setText(tr("&Search in %1...").arg(module->name()));
    m_itemOpenAction->setEnabled(!module->isLocked());
    m_itemEditMenu->setEnabled(module->isWritable());
    m_itemUnlockAction->setEnabled(module->isLocked());
}

void BtBookshelfDockWidget::slotEditHiddenModules(bool enable) {
    if (enable) {
        m_bookshelfWidget->treeModel()->setCheckable(true);
    }
    else {
        m_bookshelfWidget->treeModel()->setCheckable(false);
    }
}
