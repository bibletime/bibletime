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

#include "cbookreadwindow.h"

#include <QAction>
#include <QSplitter>
#include <Qt>
#include <QToolBar>
#include "../../backend/config/btconfigcore.h"
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../../util/tool.h"
#include "../bibletime.h"
#include "../display/btmodelviewreaddisplay.h"
#include "../keychooser/cbooktreechooser.h"
#include "../keychooser/ckeychooser.h"
#include "btactioncollection.h"
#include "btdisplaysettingsbutton.h"
#include "btmodulechooserbar.h"
#include "bttoolbarpopupaction.h"


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
    CDisplayWindow::initActions();
    BtActionCollection* ac = actionCollection();
    insertKeyboardActions(ac);

    m_treeAction = &ac->action(QStringLiteral("toggleTree"));
    BT_ASSERT(m_treeAction);
    BT_CONNECT(m_treeAction, &QAction::triggered,
               this, &CBookReadWindow::treeToggled);
    addAction(m_treeAction);

    ac->readShortcuts(QStringLiteral("Book shortcuts"));
}

void CBookReadWindow::insertKeyboardActions( BtActionCollection* const a ) {
    QAction* qaction;

    qaction = new QAction( /* QIcon(CResMgr::displaywindows::bookWindow::toggleTree::icon), */
                           tr("Toggle tree view"), a);
    qaction->setCheckable(true);
    // qaction->setShortcut(CResMgr::displaywindows::bookWindow::toggleTree::accel);
    a->addAction(QStringLiteral("toggleTree"), qaction);
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

/** Init the view */
void CBookReadWindow::initView() {
    QSplitter* splitter = new QSplitter(this);
    m_treeChooser = new CBookTreeChooser(modules(), history(), key(), splitter);
    setDisplayWidget(new BtModelViewReadDisplay(this, splitter));
    m_treeChooser->hide();
    splitter->setStretchFactor(1,3);

    if (auto * const disp = displayWidget())
        disp->setModules(getModuleList());

    // Add the Navigation toolbar
    auto * const navigationToolBar = mainToolBar();
    addToolBar(navigationToolBar);
    setKeyChooser( CKeyChooser::createInstance(modules(), history(), key(), mainToolBar()) );

    // Add the Works toolbar
    auto * const worksToolbar = moduleChooserBar();
    worksToolbar->setModules(getModuleList(), modules().first()->type(), this);
    addToolBar(worksToolbar);

    // Add the Tools toolbar
    addToolBar(buttonsToolBar());

    setCentralWidget( splitter );
    setWindowIcon(util::tool::getIconForModule(modules().first()));
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
    // Navigation toolbar
    btMainWindow()->navToolBar()->addAction(m_actions.backInHistory); //1st button
    btMainWindow()->navToolBar()->addAction(m_actions.forwardInHistory); //2nd button
    CKeyChooser* keyChooser = CKeyChooser::createInstance(modules(), history(), key(), btMainWindow()->navToolBar() );
    btMainWindow()->navToolBar()->addWidget(keyChooser);
    BT_CONNECT(keyChooser, &CKeyChooser::keyChanged,
               this,       &CBookReadWindow::lookupSwordKey);
    BT_CONNECT(this,       &CBookReadWindow::sigKeyChanged,
               keyChooser, &CKeyChooser::updateKey);

    // Works toolbar
    btMainWindow()->worksToolBar()->setModules(getModuleList(), modules().first()->type(), this);

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

/** Is called when the action was executed to toggle the tree view. */
void CBookReadWindow::treeToggled() {
    if (m_treeAction->isChecked()) {
        m_treeChooser->doShow();
    }
    else {
        m_treeChooser->hide();
    }
}

/** Reimplementation to take care of the tree chooser. */
void CBookReadWindow::modulesChanged() {
    CDisplayWindow::modulesChanged();
    m_treeChooser->setModules(modules());
}
