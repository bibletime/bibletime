/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cbookreadwindow.h"
#include "cbookreadwindow.moc"

#include "cmodulechooserbar.h"
#include "cbuttons.h"

#include "backend/keys/cswordtreekey.h"

#include "frontend/display/cdisplay.h"
#include "frontend/keychooser/cbooktreechooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "backend/config/cbtconfig.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

#include <QSplitter>
#include <QToolBar>
#include <QMenu>

#include <kaction.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <ktoolbarpopupaction.h>

using namespace Profile;

CBookReadWindow::CBookReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent) : CLexiconReadWindow(modules, parent) {}

CBookReadWindow::~CBookReadWindow() {}

void CBookReadWindow::applyProfileSettings( CProfileWindow* profileWindow )
{
	CLexiconReadWindow::applyProfileSettings(profileWindow);

	const bool enable = static_cast<bool>( profileWindow->windowSettings() );
	if (enable) {
		m_treeAction->activate(QAction::Trigger);
	}
}

void CBookReadWindow::storeProfileSettings( CProfileWindow* profileWindow )
{
	CLexiconReadWindow::storeProfileSettings(profileWindow);

	//store information about our show tree structure button
	profileWindow->setWindowSettings( static_cast<int>( m_treeAction->isChecked() ) );
}

void CBookReadWindow::initActions()
{
	CLexiconReadWindow::initActions();
	KActionCollection* ac = actionCollection();
	//cleanup, not a clean oo-solution
	Q_ASSERT(ac->action("nextEntry"));
	Q_ASSERT(ac->action("previousEntry"));
	ac->action("nextEntry")->setEnabled(false);
	ac->action("previousEntry")->setEnabled(false);

	m_treeAction = new KToggleAction(
			KIcon(CResMgr::displaywindows::bookWindow::toggleTree::icon),
			tr("Toggle tree view"),
			ac
			);
	m_treeAction->setShortcut(CResMgr::displaywindows::bookWindow::toggleTree::accel);
	QObject::connect(m_treeAction, SIGNAL(triggered()), this, SLOT(treeToggled()) );
	ac->addAction("toggleTree", m_treeAction);

//	CBTConfig::setupAccelSettings(CBTConfig::bookWindow, ac);
}

void CBookReadWindow::insertKeyboardActions( KActionCollection* const a )
{
	KAction* kaction;

	kaction = new KToggleAction(
			KIcon(CResMgr::displaywindows::bookWindow::toggleTree::icon),
			tr("Toggle tree view"),
			a
			);
	kaction->setShortcut(CResMgr::displaywindows::bookWindow::toggleTree::accel);
	a->addAction("toggleTree", kaction);
		
		//  new KAction(tr("Copy reference only"), KShortcut(0), a, "copyReferenceOnly");
	kaction = new KAction(tr("Copy entry with text"), a);
	a->addAction("copyEntryWithText", kaction);
	//  new KAction(tr("Copy selected text"), KShortcut(0), a, "copySelectedText");
	kaction = new KAction(tr("Save entry as plain text"), a);
	a->addAction("saveEntryAsPlainText", kaction);
	kaction = new KAction(tr("Save entry as HTML"),a);
	a->addAction("saveEntryAsHTML", kaction);
	//   new KAction(tr("Print reference only"), KShortcut(0), a, "printReferenceOnly");
	kaction = new KAction(tr("Print entry with text"), a);
	a->addAction("printEntryWithText", kaction);

}

/** No descriptions */
void CBookReadWindow::initConnections()
{
	CLexiconReadWindow::initConnections();

	connect(m_treeChooser, SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookupSwordKey(CSwordKey*)));
	connect(m_treeChooser, SIGNAL(keyChanged(CSwordKey*)), keyChooser(), SLOT(updateKey(CSwordKey*)));
	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)), m_treeChooser, SLOT(updateKey(CSwordKey*)));
}

/** Init the view */
void CBookReadWindow::initView()
{
	QSplitter* splitter = new QSplitter(this);

	setMainToolBar( new QToolBar(this) );
	addToolBar(mainToolBar());
	//addDockWindow(mainToolBar());

	m_treeChooser = new CBookTreeChooser(modules(), key(), splitter);
	setDisplayWidget( CDisplay::createReadInstance(this, splitter) );

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );

	setModuleChooserBar( new CModuleChooserBar(modules(), modules().first()->type(), this) );
	moduleChooserBar()->setButtonLimit(1);
	addToolBar(moduleChooserBar());
	//addDockWindow( moduleChooserBar() );

	setButtonsToolBar( new QToolBar(this) );
	//addDockWindow( buttonsToolBar() );
	setDisplaySettingsButton( new CDisplaySettingsButton( &displayOptions(), &filterOptions(), modules(), buttonsToolBar()) );
	addToolBar(buttonsToolBar());
	m_treeChooser->hide();

	//splitter->setResizeMode(m_treeChooser, QSplitter::Stretch);
	setCentralWidget( splitter );
	setWindowIcon(CToolClass::getIconForModule(modules().first()));
}

void CBookReadWindow::initToolbars()
{
	Q_ASSERT(m_treeAction);
	Q_ASSERT(m_actions.backInHistory);

	mainToolBar()->addAction(m_actions.backInHistory);
	mainToolBar()->addAction(m_actions.forwardInHistory);

	mainToolBar()->addWidget(keyChooser());

	buttonsToolBar()->addAction(m_treeAction);
	m_treeAction->setChecked(false);

	buttonsToolBar()->addWidget(displaySettingsButton());

	KAction* action = qobject_cast<KAction*>(actionCollection()->action(
						  CResMgr::displaywindows::general::search::actionName ));
	if (action) {
		buttonsToolBar()->addAction(action);
	}

	//#if KDE_VERSION_MINOR < 1
	//action->plugAccel( accel() );
	//#endif
}

/** Is called when the action was executed to toggle the tree view. */
void CBookReadWindow::treeToggled()
{
	if (m_treeAction->isChecked()) {
		m_treeChooser->show();
	}
	else {
		m_treeChooser->hide();
	}
}

/** Reimplementation to take care of the tree chooser. */
void CBookReadWindow::modulesChanged()
{
	CLexiconReadWindow::modulesChanged();
	m_treeChooser->setModules(modules());
}

void CBookReadWindow::setupPopupMenu()
{
	CLexiconReadWindow::setupPopupMenu();
}
