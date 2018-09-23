/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/displaywindow/cbookreadwindow.h"

#include <QAction>
#include <QMenu>
#include <QSplitter>
#include <QToolBar>
#include "backend/keys/cswordtreekey.h"
#include "frontend/bibletime.h"
#include "frontend/display/btmodelviewreaddisplay.h"
#include "frontend/displaywindow/bttoolbarpopupaction.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/displaywindow/btdisplaysettingsbutton.h"
#include "frontend/keychooser/cbooktreechooser.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"
#include "util/tool.h"


void CBookReadWindow::applyProfileSettings(const QString & windowGroup) {
    CLexiconReadWindow::applyProfileSettings(windowGroup);

    BT_ASSERT(m_treeAction);
    BT_ASSERT(windowGroup.endsWith('/'));
    if (btConfig().sessionValue<bool>(windowGroup + "treeShown", true) != m_treeAction->isChecked())
        m_treeAction->activate(QAction::Trigger);
}

void CBookReadWindow::storeProfileSettings(QString const & windowGroup) const {
    CLexiconReadWindow::storeProfileSettings(windowGroup);

    BT_ASSERT(windowGroup.endsWith('/'));
    btConfig().setSessionValue(windowGroup + "treeShown", m_treeAction->isChecked());
}

void CBookReadWindow::initActions() {
    CLexiconReadWindow::initActions();
    BtActionCollection* ac = actionCollection();
    insertKeyboardActions(ac);

    //cleanup, not a clean oo-solution
    ac->action("nextEntry").setEnabled(false);
    ac->action("previousEntry").setEnabled(false);

    m_treeAction = &ac->action("toggleTree");
    BT_ASSERT(m_treeAction);
    BT_CONNECT(m_treeAction, SIGNAL(triggered()), this, SLOT(treeToggled()));
    addAction(m_treeAction);

    ac->readShortcuts("Book shortcuts");
}

void CBookReadWindow::insertKeyboardActions( BtActionCollection* const a ) {
    QAction* qaction;

    qaction = new QAction( /* QIcon(CResMgr::displaywindows::bookWindow::toggleTree::icon), */
                           tr("Toggle tree view"), a);
    qaction->setCheckable(true);
    // qaction->setShortcut(CResMgr::displaywindows::bookWindow::toggleTree::accel);
    a->addAction("toggleTree", qaction);
}

/** No descriptions */
void CBookReadWindow::initConnections() {
    CLexiconReadWindow::initConnections();

    BT_CONNECT(m_treeChooser, SIGNAL(keyChanged(CSwordKey *)),
               this,          SLOT(lookupSwordKey(CSwordKey *)));
    BT_CONNECT(m_treeChooser, SIGNAL(keyChanged(CSwordKey *)),
               keyChooser(),  SLOT(updateKey(CSwordKey *)));
    BT_CONNECT(keyChooser(),  SIGNAL(keyChanged(CSwordKey *)),
               m_treeChooser, SLOT(updateKey(CSwordKey *)));
}

/** Init the view */
void CBookReadWindow::initView() {
    QSplitter* splitter = new QSplitter(this);
    m_treeChooser = new CBookTreeChooser(modules(), history(), key(), splitter);
    setDisplayWidget(new BtModelViewReadDisplay(this, splitter));
    m_treeChooser->hide();
    splitter->setStretchFactor(1,3);

    BtModelViewReadDisplay* disp = dynamic_cast<BtModelViewReadDisplay*>(displayWidget());
    if (disp)
        disp->setModules(getModuleList());

    // Create Navigation toolbar
    setMainToolBar( new QToolBar(this) );
    mainToolBar()->setAllowedAreas(Qt::TopToolBarArea);
    mainToolBar()->setFloatable(false);
    addToolBar(mainToolBar());
    setKeyChooser( CKeyChooser::createInstance(modules(), history(), key(), mainToolBar()) );

    // Create the Works toolbar
    setModuleChooserBar( new BtModuleChooserBar(this));
    moduleChooserBar()->setModules(getModuleList(), modules().first()->type(), this);
    addToolBar(moduleChooserBar());

    // Create the Tools toolbar
    setButtonsToolBar( new QToolBar(this) );
    buttonsToolBar()->setAllowedAreas(Qt::TopToolBarArea);
    buttonsToolBar()->setFloatable(false);
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
    BT_CONNECT(keyChooser, SIGNAL(keyChanged(CSwordKey *)),
               this,       SLOT(lookupSwordKey(CSwordKey *)));
    BT_CONNECT(this,       SIGNAL(sigKeyChanged(CSwordKey *)),
               keyChooser, SLOT(updateKey(CSwordKey *)));

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
    CLexiconReadWindow::modulesChanged();
    m_treeChooser->setModules(modules());
}

void CBookReadWindow::setupPopupMenu() {
    CLexiconReadWindow::setupPopupMenu();
}

void CBookReadWindow::reload(CSwordBackend::SetupChangedReason reason) {
    CLexiconReadWindow::reload(reason);
}
