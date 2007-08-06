/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "clexiconreadwindow.h"
#include "clexiconreadwindow.moc"

#include "cmodulechooserbar.h"
#include "cbuttons.h"

#include "backend/keys/cswordkey.h"
#include "backend/keys/cswordldkey.h"

#include "frontend/cbtconfig.h"
#include "frontend/cexportmanager.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/keychooser/ckeychooser.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

//Qt includes

//KDE includes
#include <ktoolbar.h>
#include <klocale.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoolbarpopupaction.h>
#include <kactionmenu.h>
#include <kmenu.h>

#include <QMenu>
// #include <kactionclasses.h>


CLexiconReadWindow::CLexiconReadWindow(ListCSwordModuleInfo moduleList, CMDIArea* parent)
	: CReadWindow(moduleList, parent)
{
	qDebug("CLexiconReadWindow::CLexiconReadWindow");
	moduleList.first();
	qDebug("CLexiconReadWindow::CLexiconReadWindow 2");
	setKey( CSwordKey::createInstance(moduleList.first()) );
	qDebug("CLexiconReadWindow::CLexiconReadWindow 3");
}

CLexiconReadWindow::~CLexiconReadWindow() {}

void CLexiconReadWindow::insertKeyboardActions( KActionCollection* const a ) {
	KAction* kaction;
	kaction = new KAction( i18n("Next entry"), a);
	kaction->setShortcut(CResMgr::displaywindows::lexiconWindow::nextEntry::accel);
	a->addAction("nextEntry", kaction);	

	kaction = new KAction( i18n("Previous entry"), a);
	kaction->setShortcut( CResMgr::displaywindows::lexiconWindow::previousEntry::accel);
	a->addAction("previousEntry", kaction);
	
	//  new KAction(i18n("Copy reference only"), KShortcut(0), a, "copyReferenceOnly");
	kaction = new KAction(i18n("Copy entry with text"), a);
	a->addAction("copyReferenceOnly", kaction);

	//  new KAction(i18n("Copy selected text"), KShortcut(0), a, "copySelectedText");
	kaction = new KAction(i18n("Save entry as plain text"), a);
	a->addAction("copySelectedText", kaction);

	kaction = new KAction(i18n("Save entry as HTML"), a);
	a->addAction("saveHtml", kaction);

	//   new KAction(i18n("Print reference only"), KShortcut(0), a, "printReferenceOnly");
	kaction = new KAction(i18n("Print entry with text"), a);
	a->addAction("printReferenceOnly", kaction);
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

	KActionCollection* ac = actionCollection();

	KAction* kaction = new KAction(i18n("Next entry"), ac );
	kaction->setShortcut( CResMgr::displaywindows::lexiconWindow::nextEntry::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT( nextEntry() ) );
	ac->addAction("nextEntry", kaction);

	kaction = new KAction(i18n("Previous entry"), ac );
	kaction->setShortcut( CResMgr::displaywindows::lexiconWindow::previousEntry::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT( previousEntry() ) );
	ac->addAction("previousEntry", kaction);

	m_actions.selectAll = qobject_cast<KAction*>(ac->action("selectAll"));
	Q_ASSERT(m_actions.selectAll);

	m_actions.findText = qobject_cast<KAction*>(ac->action("findText"));
	Q_ASSERT(m_actions.findText);
	
	//m_actions.findStrongs = new KAction(i18n("Strong's Search"), KShortcut(0),this, SLOT(openSearchStrongsDialog()), actionCollection(), "findStrongs");
	m_actions.findStrongs = new KAction(
		KIcon(CResMgr::displaywindows::general::findStrongs::icon),
		i18n("Strong's Search"),
		ac
		);
	m_actions.findStrongs->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
	QObject::connect(m_actions.findStrongs, SIGNAL(triggered()), this, SLOT(openSearchStrongsDialog()) );
	ac->addAction(CResMgr::displaywindows::general::findStrongs::actionName, kaction);

	m_actions.copy.reference = new KAction(i18n("Reference only"), ac );
	QObject::connect(m_actions.copy.reference, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorOnly()) );
	ac->addAction("copyReferenceOnly", kaction);

	m_actions.copy.entry = new KAction(i18n("Entry with text"), ac );
	QObject::connect(m_actions.copy.entry, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorWithText()) );
	ac->addAction("copyEntryWithText", kaction);

	m_actions.copy.selectedText = qobject_cast<KAction*>(ac->action("copySelectedText"));
	Q_ASSERT(m_actions.copy.selectedText);

	m_actions.save.entryAsPlain = new KAction(i18n("Entry as plain text"), ac );
	QObject::connect(m_actions.save.entryAsPlain, SIGNAL(triggered()), this, SLOT(saveAsPlain()) );
	ac->addAction("saveEntryAsPlain", m_actions.save.entryAsPlain);

	m_actions.save.entryAsHTML = new KAction(i18n("Entry as HTML"), ac );
	QObject::connect(m_actions.save.entryAsHTML, SIGNAL(triggered()), this, SLOT(saveAsHTML()));
	ac->addAction("saveEntryAsHTML", m_actions.save.entryAsHTML);


	m_actions.print.reference = new KAction(i18n("Reference only"), ac);
	QObject::connect(m_actions.print.reference, SIGNAL(triggered()), this, SLOT(printAnchorWithText()));
	ac->addAction("printReferenceOnly", m_actions.print.reference);

	m_actions.print.entry = new KAction(i18n("Entry with text"), ac);
	QObject::connect(m_actions.print.entry, SIGNAL(triggered()), this, SLOT(printAll()));
	ac->addAction("printEntryWithText", m_actions.print.entry);

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

void CLexiconReadWindow::initView()
{
	qDebug("CLexiconReadWindow::initView");
	setDisplayWidget( CDisplay::createReadInstance(this) );
	qDebug("CLexiconReadWindow::initView 2");
	setMainToolBar( new KToolBar(this) );
	//addDockWindow(mainToolBar()); //TODO: does this work? see write windows
	qDebug("CLexiconReadWindow::initView 3");
	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	qDebug("CLexiconReadWindow::initView 4");
	mainToolBar()->addWidget(keyChooser());
	//mainToolBar()->setFullSize(false);
	qDebug("CLexiconReadWindow::initView 5");
	setModuleChooserBar( new CModuleChooserBar(modules(), modules().first()->type(), this) );
	//addDockWindow(moduleChooserBar());
	qDebug("CLexiconReadWindow::initView 6");
	setButtonsToolBar( new KToolBar(this) );
	//addDockWindow(buttonsToolBar());
	qDebug("CLexiconReadWindow::initView 7");
	setWindowIcon(CToolClass::getIconForModule(modules().first()));
	qDebug("CLexiconReadWindow::initView 8");
	setCentralWidget( displayWidget()->view() );
}

void CLexiconReadWindow::initToolbars() {
	//main toolbar
	Q_ASSERT(m_actions.backInHistory);
	mainToolBar()->addAction(m_actions.backInHistory); //1st button
	mainToolBar()->addAction(m_actions.forwardInHistory); //2nd button

	//buttons toolbar
	KAction* action = qobject_cast<KAction*>(actionCollection()->action(
		CResMgr::displaywindows::general::search::actionName));
	Q_ASSERT( action );
	if (action) {
		buttonsToolBar()->addAction(action);
	}
	//#if KDE_VERSION_MINOR < 1
	//action->plugAccel( accel() );
	//#endif

	setDisplaySettingsButton( new CDisplaySettingsButton( &displayOptions(), &filterOptions(), modules(), buttonsToolBar()) );
	//buttonsToolBar()->insertWidget(2,displaySettingsButton()->size().width(), displaySettingsButton());
	//TODO: find the right place for the button
	buttonsToolBar()->addWidget(displaySettingsButton());
}

void CLexiconReadWindow::setupPopupMenu() {
	popup()->addTitle(CToolClass::getIconForModule(modules().first()), i18n("Lexicon window"));

	//   m_actions.selectAll = new KAction(i18n("Select all"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(selectAll()), actionCollection());
	popup()->addAction(m_actions.findText);
	popup()->addAction(m_actions.findStrongs);
	
	popup()->addAction(m_actions.selectAll);

	
	//(new KActionSeparator(this))->plug( popup() );
	popup()->addSeparator();

	m_actions.copyMenu = new KActionMenu(KIcon(CResMgr::displaywindows::lexiconWindow::copyMenu::icon), i18n("Copy..."), actionCollection());
	m_actions.copyMenu->setDelayed(false);

	m_actions.copyMenu->addAction(m_actions.copy.reference);
	m_actions.copyMenu->addAction(m_actions.copy.entry);
	m_actions.copyMenu->addSeparator();
	m_actions.copyMenu->addAction(m_actions.copy.selectedText);
	popup()->addAction(m_actions.copyMenu);

	m_actions.saveMenu = new KActionMenu(
			KIcon(CResMgr::displaywindows::lexiconWindow::saveMenu::icon),
			i18n("Save..."),
			actionCollection()
			);
	m_actions.saveMenu->setDelayed(false);
	m_actions.saveMenu->addAction(m_actions.save.entryAsPlain);
	m_actions.saveMenu->addAction(m_actions.save.entryAsHTML);
	popup()->addAction(m_actions.saveMenu);

	m_actions.printMenu = new KActionMenu(
			KIcon(CResMgr::displaywindows::lexiconWindow::printMenu::icon),
			i18n("Print..."),
			actionCollection()
			);
	m_actions.printMenu->setDelayed(false);
	m_actions.printMenu->addAction(m_actions.print.reference);
	m_actions.printMenu->addAction(m_actions.print.entry);
	popup()->addAction(m_actions.printMenu);
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
	QMenu* menu = m_actions.backInHistory->popupMenu();
	menu->clear();

	QStringList::iterator it;
	int index = 1;
	for (it = keyList.begin(); it != keyList.end(); ++it) {
		menu->addAction(*it);
		++index;
	}
}

void CLexiconReadWindow::slotFillForwardHistory() {
	//  qWarning("fill forward history");
	QStringList keyList = keyChooser()->getNextKeys();
	QMenu* menu = m_actions.forwardInHistory->popupMenu();
	menu->clear();

	QStringList::iterator it;
	int index = 1;
	for (it = keyList.begin(); it != keyList.end(); ++it) {
		menu->addAction(*it);
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
