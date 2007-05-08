/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "clexiconreadwindow.h"
#include "cmodulechooserbar.h"
#include "cbuttons.h"

#include "backend/cswordkey.h"
#include "backend/cswordldkey.h"

#include "frontend/cbtconfig.h"
#include "frontend/cexportmanager.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/keychooser/ckeychooser.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

//Qt includes

//KDE includes
#include <kaccel.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kdeversion.h>
// #include <kactionclasses.h>

CLexiconReadWindow::CLexiconReadWindow(ListCSwordModuleInfo moduleList, CMDIArea* parent, const char *name) : CReadWindow(moduleList, parent,name) {
	setKey( CSwordKey::createInstance(moduleList.first()) );
}

CLexiconReadWindow::~CLexiconReadWindow() {}

void CLexiconReadWindow::insertKeyboardActions( KActionCollection* const a ) {
	new KAction(
		i18n("Next entry"), CResMgr::displaywindows::lexiconWindow::nextEntry::accel,
		a, "nextEntry"
	);
	new KAction(
		i18n("Previous entry"), CResMgr::displaywindows::lexiconWindow::previousEntry::accel,
		a, "previousEntry"
	);

	//  new KAction(i18n("Copy reference only"), KShortcut(0), a, "copyReferenceOnly");
	new KAction(i18n("Copy entry with text"), KShortcut(0), a, "copyEntryWithText");
	//  new KAction(i18n("Copy selected text"), KShortcut(0), a, "copySelectedText");
	new KAction(i18n("Save entry as plain text"), KShortcut(0), a, "saveEntryAsPlainText");
	new KAction(i18n("Save entry as HTML"), KShortcut(0), a, "saveEntryAsHTML");
	//   new KAction(i18n("Print reference only"), KShortcut(0), a, "printReferenceOnly");
	new KAction(i18n("Print entry with text"), KShortcut(0), a, "printEntryWithText");
}

void CLexiconReadWindow::initActions() {
	CReadWindow::initActions();

	m_actions.backInHistory = dynamic_cast<KToolBarPopupAction*>(
								  actionCollection()->action(
									  CResMgr::displaywindows::general::backInHistory::actionName
								  )
							  );
	Q_ASSERT(m_actions.backInHistory);

	m_actions.forwardInHistory = dynamic_cast<KToolBarPopupAction*>(
									 actionCollection()->action(
										 CResMgr::displaywindows::general::forwardInHistory::actionName
									 )
								 );

	Q_ASSERT(m_actions.forwardInHistory);

	new KAction(
		i18n("Next entry"), CResMgr::displaywindows::lexiconWindow::nextEntry::accel,
		this, SLOT( nextEntry() ),
		actionCollection(), "nextEntry"
	);
	new KAction(
		i18n("Previous entry"), CResMgr::displaywindows::lexiconWindow::previousEntry::accel,
		this, SLOT( previousEntry() ),
		actionCollection(), "previousEntry"
	);

	m_actions.selectAll = actionCollection()->action("selectAll");
	Q_ASSERT(m_actions.selectAll);

	m_actions.findText = actionCollection()->action("findText");
	Q_ASSERT(m_actions.findText);
	
	//m_actions.findStrongs = new KAction(i18n("Strong's Search"), KShortcut(0),this, SLOT(openSearchStrongsDialog()), actionCollection(), "findStrongs");
	m_actions.findStrongs = new KAction(
		i18n("Strong's Search"),
		CResMgr::displaywindows::general::findStrongs::icon,
		CResMgr::displaywindows::general::findStrongs::accel,
		this, SLOT(openSearchStrongsDialog()),
		actionCollection(),
		CResMgr::displaywindows::general::findStrongs::actionName);

	m_actions.copy.reference = new KAction(i18n("Reference only"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(copyAnchorOnly()), actionCollection(), "copyReferenceOnly");

	m_actions.copy.entry = new KAction(i18n("Entry with text"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(copyAnchorWithText()), actionCollection(), "copyEntryWithText");

	m_actions.copy.selectedText = actionCollection()->action("copySelectedText");
	Q_ASSERT(m_actions.copy.selectedText);

	m_actions.save.entryAsPlain = new KAction(i18n("Entry as plain text"), KShortcut(0), this, SLOT(saveAsPlain()),actionCollection(), "saveEntryAsPlain");

	m_actions.save.entryAsHTML = new KAction(i18n("Entry as HTML"), KShortcut(0), this, SLOT(saveAsHTML()), actionCollection(), "saveEntryAsHTML");

	m_actions.print.reference = new KAction(i18n("Reference only"), KShortcut(0), this, SLOT(printAnchorWithText()), actionCollection(), "printReferenceOnly");

	m_actions.print.entry = new KAction(i18n("Entry with text"), KShortcut(0), this, SLOT(printAll()), actionCollection(), "printEntryWithText");

	// init with the user defined settings
	CBTConfig::setupAccelSettings(CBTConfig::lexiconWindow, actionCollection());
};

/** No descriptions */
void CLexiconReadWindow::initConnections() {
	Q_ASSERT(keyChooser());

	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)),
			this, SLOT(lookup(CSwordKey*)));
	connect(keyChooser(), SIGNAL(historyChanged()),
			this, SLOT(slotUpdateHistoryButtons()));

	//connect the history actions to the right slots
	connect(
		m_actions.backInHistory->popupMenu(), SIGNAL(aboutToShow()),
		this, SLOT(slotFillBackHistory())
	);
	connect(
		m_actions.backInHistory->popupMenu(), SIGNAL(activated(int)),
		keyChooser(), SLOT(backInHistory(int))
	);
	connect(
		m_actions.forwardInHistory->popupMenu(), SIGNAL(aboutToShow()),
		this, SLOT(slotFillForwardHistory())
	);
	connect(
		m_actions.forwardInHistory->popupMenu(), SIGNAL(activated(int)),
		keyChooser(), SLOT(forwardInHistory(int))
	);


}

void CLexiconReadWindow::initView() {
	setDisplayWidget( CDisplay::createReadInstance(this) );

	setMainToolBar( new KToolBar(this) );
	addDockWindow(mainToolBar());

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );

	mainToolBar()->insertWidget(0, keyChooser()->sizeHint().width(), keyChooser());
	mainToolBar()->setFullSize(false);

	setModuleChooserBar( new CModuleChooserBar(modules(), modules().first()->type(), this) );
	addDockWindow(moduleChooserBar());

	setButtonsToolBar( new KToolBar(this) );
	addDockWindow(buttonsToolBar());
	
	setIcon(CToolClass::getIconForModule(modules().first()));
	setCentralWidget( displayWidget()->view() );
}

void CLexiconReadWindow::initToolbars() {
	//main toolbar
	Q_ASSERT(m_actions.backInHistory);
	m_actions.backInHistory->plug( mainToolBar(),0 ); //1st button
	m_actions.forwardInHistory->plug( mainToolBar(),1 ); //2nd button

	//buttons toolbar
	KAction* action = actionCollection()->action(
		CResMgr::displaywindows::general::search::actionName);
	Q_ASSERT( action );
	if (action) {
		action->plug(buttonsToolBar());
	}
	#if KDE_VERSION_MINOR < 1
	action->plugAccel( accel() );
	#endif

	setDisplaySettingsButton( new CDisplaySettingsButton( &displayOptions(), &filterOptions(), modules(), buttonsToolBar()) );
	buttonsToolBar()->insertWidget(2,displaySettingsButton()->size().width(), displaySettingsButton());
}

void CLexiconReadWindow::setupPopupMenu() {
	popup()->insertTitle(CToolClass::getIconForModule(modules().first()), i18n("Lexicon window"));

	//   m_actions.selectAll = new KAction(i18n("Select all"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(selectAll()), actionCollection());
	m_actions.findText->plug(popup());
	m_actions.findStrongs->plug(popup());
	
	m_actions.selectAll->plug(popup());

	
	(new KActionSeparator(this))->plug( popup() );

	m_actions.copyMenu = new KActionMenu(i18n("Copy..."), CResMgr::displaywindows::lexiconWindow::copyMenu::icon, actionCollection());
	m_actions.copyMenu->setDelayed(false);

	m_actions.copyMenu->insert(m_actions.copy.reference);
	m_actions.copyMenu->insert(m_actions.copy.entry);
	m_actions.copyMenu->insert(new KActionSeparator(this));
	m_actions.copyMenu->insert(m_actions.copy.selectedText);
	m_actions.copyMenu->plug(popup());

	m_actions.saveMenu = new KActionMenu(i18n("Save..."), CResMgr::displaywindows::lexiconWindow::saveMenu::icon, actionCollection());
	m_actions.saveMenu->setDelayed(false);
	m_actions.saveMenu->insert(m_actions.save.entryAsPlain);
	m_actions.saveMenu->insert(m_actions.save.entryAsHTML);
	m_actions.saveMenu->plug(popup());

	m_actions.printMenu = new KActionMenu(i18n("Print..."), CResMgr::displaywindows::lexiconWindow::printMenu::icon, actionCollection());
	m_actions.printMenu->setDelayed(false);
	m_actions.printMenu->insert(m_actions.print.reference);
	m_actions.printMenu->insert(m_actions.print.entry);
	m_actions.printMenu->plug(popup());
}

/** Reimplemented. */
void CLexiconReadWindow::updatePopupMenu() {
	//enable the action depending on the supported module features
/*	bool hasStrongs = false;
	ListCSwordModuleInfo mods = modules();
	for (ListCSwordModuleInfo::iterator it = mods.begin(); it != mods.end(); ++it) {
		if ( (*it)->has( CSwordModuleInfo::strongNumbers ) ) {
			hasStrongs = true;
			break;
		}
	}
	m_actions.findStrongs->setEnabled( hasStrongs );*/
	m_actions.findStrongs->setEnabled( displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null );
	
	m_actions.copy.reference->setEnabled( displayWidget()->hasActiveAnchor() );
	m_actions.copy.selectedText->setEnabled( displayWidget()->hasSelection() );

	m_actions.print.reference->setEnabled( displayWidget()->hasActiveAnchor() );
}

/** No descriptions */
void CLexiconReadWindow::nextEntry() {
	keyChooser()->setKey(ldKey()->NextEntry());
}

/** No descriptions */
void CLexiconReadWindow::previousEntry() {
	keyChooser()->setKey(ldKey()->PreviousEntry());
}

/** Reimplementation to return the right key. */
CSwordLDKey* CLexiconReadWindow::ldKey() {
	return dynamic_cast<CSwordLDKey*>(CDisplayWindow::key());
}

/** This function saves the entry as html using the CExportMgr class. */
void CLexiconReadWindow::saveAsHTML() {
	CExportManager mgr(i18n("Saving entry ..."), true, i18n("Saving"), filterOptions(), displayOptions());
	mgr.saveKey(key(), CExportManager::HTML, true);
}

/** This function saves the entry as html using the CExportMgr class. */
void CLexiconReadWindow::saveAsPlain() {
	CExportManager mgr(i18n("Saving entry ..."), true, i18n("Saving"), filterOptions(), displayOptions());
	mgr.saveKey(key(), CExportManager::Text, true);
}

void CLexiconReadWindow::slotFillBackHistory() {
	//  qWarning("fill back history");
	QStringList keyList = keyChooser()->getPreviousKeys();
	QPopupMenu* menu = m_actions.backInHistory->popupMenu();
	menu->clear();

	QStringList::iterator it;
	int index = 1;
	for (it = keyList.begin(); it != keyList.end(); ++it) {
		menu->insertItem(*it,index, index);
		++index;
	}
}

void CLexiconReadWindow::slotFillForwardHistory() {
	//  qWarning("fill forward history");
	QStringList keyList = keyChooser()->getNextKeys();
	QPopupMenu* menu = m_actions.forwardInHistory->popupMenu();
	menu->clear();

	QStringList::iterator it;
	int index = 1;
	for (it = keyList.begin(); it != keyList.end(); ++it) {
		menu->insertItem(*it,index, index);
		++index;
	}
}


void CLexiconReadWindow::slotUpdateHistoryButtons() {
	//  qWarning("updating history buttons");
	Q_ASSERT(m_actions.backInHistory);
	Q_ASSERT(keyChooser());

	m_actions.backInHistory->setEnabled( keyChooser()->getPreviousKeys().size() > 0 );
	m_actions.forwardInHistory->setEnabled( keyChooser()->getNextKeys().size() > 0 );
}
