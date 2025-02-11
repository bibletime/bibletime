/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cbookreadwindow.h"

#include <QAction>
#include <QDockWidget>
#include <Qt>
#include <QToolBar>
#include "../../backend/config/btconfigcore.h"
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletime.h"
#include "../display/btmodelviewreaddisplay.h"
#include "../keychooser/bthistory.h"
#include "../keychooser/cbooktreechooser.h"
#include "../keychooser/ckeychooser.h"
#include "btactioncollection.h"
#include "btdisplaysettingsbutton.h"
#include "btmodulechooserbar.h"
#include "bttoolbarpopupaction.h"


CBookReadWindow::ActionCollection::ActionCollection(QObject * const parent)
    : CDisplayWindow::ActionCollection(parent)
{
    auto * const qaction = new QAction(tr("Toggle tree view"), this);
    qaction->setCheckable(true);
    addAction(QStringLiteral("toggleTree"), qaction);
}

CBookReadWindow::CBookReadWindow(QList<CSwordModuleInfo *> const & modules,
                                 QString const & key,
                                 CMDIArea * parent)
    : CDisplayWindow(modules, key, false, new ActionCollection(), parent)
{
    auto * const h = history();

    m_treeChooserDock = new QDockWidget(this);
    m_treeChooserDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_treeChooserDock->setAllowedAreas(
                Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    // "Remove" title bar:
    m_treeChooserDock->setTitleBarWidget(new QWidget(m_treeChooserDock));
        m_treeChooser =
                new CBookTreeChooser(constModules(),
                                     swordKey(),
                                     m_treeChooserDock);
        BT_CONNECT(m_treeChooser, &CKeyChooser::keyChanged,
                   h, &BTHistory::add);
        BT_CONNECT(h, &BTHistory::historyMoved,
                   m_treeChooser, &CKeyChooser::handleHistoryMoved);
    m_treeChooserDock->setWidget(m_treeChooser);
    m_treeChooserDock->hide();
    addDockWidget(Qt::LeftDockWidgetArea, m_treeChooserDock);

    init();
}

void CBookReadWindow::applyProfileSettings(BtConfigCore const & conf) {
    CDisplayWindow::applyProfileSettings(conf);

    BT_ASSERT(m_treeAction);
    if (conf.value<bool>(QStringLiteral("treeShown"), true)
        != m_treeAction->isChecked())
        m_treeAction->activate(QAction::Trigger);
}

void CBookReadWindow::storeProfileSettings(BtConfigCore & conf) const {
    CDisplayWindow::storeProfileSettings(conf);
    conf.setValue(QStringLiteral("treeShown"), m_treeAction->isChecked());
}

void CBookReadWindow::initActions() {
    BtActionCollection* ac = actionCollection();

    m_treeAction = &ac->action(QStringLiteral("toggleTree"));
    BT_ASSERT(m_treeAction);
    BT_CONNECT(m_treeAction, &QAction::triggered,
               m_treeChooserDock, &QDockWidget::setVisible);
    addAction(m_treeAction);

    ac->readShortcuts(QStringLiteral("Book shortcuts"));
}

/** No descriptions */
void CBookReadWindow::initConnections() {
    CDisplayWindow::initConnections();

    BT_CONNECT(m_treeChooser, &CBookTreeChooser::keyChanged,
               this,          &CBookReadWindow::lookupSwordKey);
    BT_CONNECT(m_treeChooser, &CBookTreeChooser::keyChanged,
               keyChooser(),  &CKeyChooser::updateKey);
    BT_CONNECT(keyChooser(),  &CKeyChooser::keyChanged,
               m_treeChooser, &CBookTreeChooser::updateKey);
}

void CBookReadWindow::initToolbars() {
    BT_ASSERT(m_treeAction);
    BT_ASSERT(m_actions.backInHistory);

    mainToolBar()->addAction(m_actions.backInHistory);
    mainToolBar()->addAction(m_actions.forwardInHistory);
    mainToolBar()->addWidget(keyChooser());

    // Tools toolbar
    buttonsToolBar()->addAction(m_treeAction);  // Tree
    m_treeAction->setChecked(false);
    BtDisplaySettingsButton* button = new BtDisplaySettingsButton(buttonsToolBar());
    setDisplaySettingsButton(button);
    buttonsToolBar()->addWidget(button);  // Display settings
    // Search:
    buttonsToolBar()->addAction(
                &actionCollection()->action(
                    CResMgr::displaywindows::general::search::actionName));
}

void CBookReadWindow::setupMainWindowToolBars() {
    auto const constMods = constModules();

    // Navigation toolbar
    btMainWindow()->navToolBar()->addAction(m_actions.backInHistory); //1st button
    btMainWindow()->navToolBar()->addAction(m_actions.forwardInHistory); //2nd button
    CKeyChooser * const keyChooser =
            CKeyChooser::createInstance(constMods,
                                        swordKey(),
                                        btMainWindow()->navToolBar());
    auto * const h = history();
    BT_CONNECT(keyChooser, &CKeyChooser::keyChanged,
               h, &BTHistory::add);
    BT_CONNECT(h, &BTHistory::historyMoved,
               keyChooser, &CKeyChooser::handleHistoryMoved);
    btMainWindow()->navToolBar()->addWidget(keyChooser);
    BT_CONNECT(keyChooser, &CKeyChooser::keyChanged,
               this,       &CBookReadWindow::lookupSwordKey);
    BT_CONNECT(this,       &CBookReadWindow::sigKeyChanged,
               keyChooser, &CKeyChooser::updateKey);

    // Works toolbar
    btMainWindow()->worksToolBar()->associateWithWindow(this);

    // Tools toolbar
    btMainWindow()->toolsToolBar()->addAction(m_treeAction);  // Tree
    BtDisplaySettingsButton* button = new BtDisplaySettingsButton(buttonsToolBar());
    setDisplaySettingsButton(button);
    btMainWindow()->toolsToolBar()->addWidget(button);  // Display settings
    // Search:
    btMainWindow()->toolsToolBar()->addAction(
                &actionCollection()->action(
                    CResMgr::displaywindows::general::search::actionName));
}

/** Reimplementation to take care of the tree chooser. */
void CBookReadWindow::modulesChanged() {
    CDisplayWindow::modulesChanged();
    m_treeChooser->setModules(constModules());
}
