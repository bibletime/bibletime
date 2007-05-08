/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cbookreadwindow.h"
#include "cmodulechooserbar.h"
#include "cbuttons.h"

#include "backend/cswordtreekey.h"

#include "frontend/display/cdisplay.h"
#include "frontend/keychooser/cbooktreechooser.h"
#include "frontend/cprofilewindow.h"
#include "frontend/cbtconfig.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

//Qt includes
#include <qsplitter.h>

//KDE includes
#include <kaction.h>
#include <klocale.h>
#include <kdeversion.h>

using namespace Profile;

CBookReadWindow::CBookReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name) : CLexiconReadWindow(modules, parent, name) {}

CBookReadWindow::~CBookReadWindow() {}

void CBookReadWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
	CLexiconReadWindow::applyProfileSettings(profileWindow);

	const bool enable = static_cast<bool>( profileWindow->windowSettings() );
	if (enable) {
		m_treeAction->activate();
	}
};

void CBookReadWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
	CLexiconReadWindow::storeProfileSettings(profileWindow);

	//store information about our show tree structure button
	profileWindow->setWindowSettings( static_cast<int>( m_treeAction->isChecked() ) );
};

void CBookReadWindow::initActions() {
	CLexiconReadWindow::initActions();
	//cleanup, not a clean oo-solution
	Q_ASSERT(actionCollection()->action("nextEntry"));
	Q_ASSERT(actionCollection()->action("previousEntry"));
	actionCollection()->action("nextEntry")->setEnabled(false);
	actionCollection()->action("previousEntry")->setEnabled(false);

	m_treeAction = new KToggleAction(
					   i18n("Toggle tree view"),
					   CResMgr::displaywindows::bookWindow::toggleTree::icon,
					   CResMgr::displaywindows::bookWindow::toggleTree::accel,
					   this, SLOT(treeToggled()),
					   actionCollection(), "toggleTree");

	CBTConfig::setupAccelSettings(CBTConfig::bookWindow, actionCollection());
};

void CBookReadWindow::insertKeyboardActions( KActionCollection* const a ) {
	new KToggleAction(
		i18n("Toggle tree view"),
		CResMgr::displaywindows::bookWindow::toggleTree::icon,
		CResMgr::displaywindows::bookWindow::toggleTree::accel,
		a, "toggleTree");
		
		//  new KAction(i18n("Copy reference only"), KShortcut(0), a, "copyReferenceOnly");
	new KAction(i18n("Copy entry with text"), KShortcut(0), a, "copyEntryWithText");
	//  new KAction(i18n("Copy selected text"), KShortcut(0), a, "copySelectedText");
	new KAction(i18n("Save entry as plain text"), KShortcut(0), a, "saveEntryAsPlainText");
	new KAction(i18n("Save entry as HTML"), KShortcut(0), a, "saveEntryAsHTML");
	//   new KAction(i18n("Print reference only"), KShortcut(0), a, "printReferenceOnly");
	new KAction(i18n("Print entry with text"), KShortcut(0), a, "printEntryWithText");

}

/** No descriptions */
void CBookReadWindow::initConnections() {
	CLexiconReadWindow::initConnections();

	connect(m_treeChooser, SIGNAL(keyChanged(CSwordKey*)),
			this, SLOT(lookup(CSwordKey*)));
	connect(m_treeChooser, SIGNAL(keyChanged(CSwordKey*)),
			keyChooser(), SLOT(updateKey(CSwordKey*)));
	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)),
			m_treeChooser, SLOT(updateKey(CSwordKey*)));
}

/** Init the view */
void CBookReadWindow::initView() {
	QSplitter* splitter = new QSplitter(this);

	setMainToolBar( new KToolBar(this) );
	addDockWindow(mainToolBar());

	m_treeChooser = new CBookTreeChooser(modules(), key(), splitter);
	setDisplayWidget( CDisplay::createReadInstance(this, splitter) );

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );

	setModuleChooserBar( new CModuleChooserBar(modules(), modules().first()->type(), this) );
	moduleChooserBar()->setButtonLimit(1);
	addDockWindow( moduleChooserBar() );

	setButtonsToolBar( new KToolBar(this) );
	addDockWindow( buttonsToolBar() );
	setDisplaySettingsButton( new CDisplaySettingsButton( &displayOptions(), &filterOptions(), modules(), buttonsToolBar()) );

	m_treeChooser->hide();

	splitter->setResizeMode(m_treeChooser, QSplitter::Stretch);
	setCentralWidget( splitter );
	setIcon(CToolClass::getIconForModule(modules().first()));
}

void CBookReadWindow::initToolbars() {
	Q_ASSERT(m_treeAction);
	Q_ASSERT(m_actions.backInHistory);

	m_actions.backInHistory->plug( mainToolBar(), 0 );
	m_actions.forwardInHistory->plug( mainToolBar(), 1 );

	mainToolBar()->insertWidget(0,keyChooser()->sizeHint().width(),keyChooser());

	m_treeAction->plug(buttonsToolBar());
	m_treeAction->setChecked(false);

	buttonsToolBar()->insertWidget(2,displaySettingsButton()->size().width(),displaySettingsButton());

	KAction* action = actionCollection()->action(
						  CResMgr::displaywindows::general::search::actionName );
	if (action) {
		action->plug(buttonsToolBar());
	}

	#if KDE_VERSION_MINOR < 1
	action->plugAccel( accel() );
	#endif
}

/** Is called when the action was executed to toggle the tree view. */
void CBookReadWindow::treeToggled() {
	if (m_treeAction->isChecked()) {
		m_treeChooser->show();
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

	popup()->changeTitle(-1, CToolClass::getIconForModule(modules().first()), i18n("Book window"));
}
