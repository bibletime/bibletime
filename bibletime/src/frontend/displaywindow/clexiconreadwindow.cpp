/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
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
#include "frontend/display/chtmlreaddisplay.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/keychooser/bthistory.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"


//KDE includes
#include <ktoolbar.h>

#include <kaction.h>
#include <kactioncollection.h>
#include <ktoolbarpopupaction.h>
#include <kactionmenu.h>
#include <kmenu.h>


#include <QMenu>
#include <QApplication>
#include <QFile>
#include <QFileDialog>



CLexiconReadWindow::CLexiconReadWindow(ListCSwordModuleInfo moduleList, CMDIArea* parent)
	: CReadWindow(moduleList, parent)
{
	qDebug("CLexiconReadWindow::CLexiconReadWindow");
	moduleList.first();
	setKey( CSwordKey::createInstance(moduleList.first()) );
}

CLexiconReadWindow::~CLexiconReadWindow() {}

void CLexiconReadWindow::insertKeyboardActions( KActionCollection* const a )
{
	qDebug("CLexiconReadWindow::insertKeyboardActions");
	KAction* kaction;
	kaction = new KAction( tr("Next entry"), a);
	kaction->setShortcut(CResMgr::displaywindows::lexiconWindow::nextEntry::accel);
	a->addAction("nextEntry", kaction);

	kaction = new KAction( tr("Previous entry"), a);
	kaction->setShortcut( CResMgr::displaywindows::lexiconWindow::previousEntry::accel);
	a->addAction("previousEntry", kaction);
	
	kaction = new KAction(tr("Copy reference only"), a);
	a->addAction("copyReferenceOnly", kaction);

	kaction = new KAction(tr("Copy selected text"), a);
	a->addAction("copySelectedText", kaction);

	kaction = new KAction(tr("Save entry as HTML"), a);
	a->addAction("saveHtml", kaction);

	kaction = new KAction(tr("Print reference only"), a);
	a->addAction("printReferenceOnly", kaction);
}

void CLexiconReadWindow::initActions()
{
	qDebug("CLexiconReadWindow::initActions");

	KActionCollection* ac = actionCollection();
	CLexiconReadWindow::insertKeyboardActions(ac);
	CReadWindow::initActions();

	
	m_actions.backInHistory = dynamic_cast<KToolBarPopupAction*>(
		ac->action(CResMgr::displaywindows::general::backInHistory::actionName) );
	Q_ASSERT(m_actions.backInHistory);

	m_actions.forwardInHistory = dynamic_cast<KToolBarPopupAction*>(
		 ac->action(CResMgr::displaywindows::general::forwardInHistory::actionName) );
	Q_ASSERT(m_actions.forwardInHistory);


	KAction* kaction;

	kaction = new KAction(tr("Next entry"), ac );
	kaction->setShortcut( CResMgr::displaywindows::lexiconWindow::nextEntry::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT( nextEntry() ) );
	ac->addAction("nextEntry", kaction);

	kaction = new KAction(tr("Previous entry"), ac );
	kaction->setShortcut( CResMgr::displaywindows::lexiconWindow::previousEntry::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT( previousEntry() ) );
	ac->addAction("previousEntry", kaction);


	m_actions.selectAll = qobject_cast<KAction*>(ac->action("selectAll"));
	//TODO: Q_ASSERT(m_actions.selectAll);

	m_actions.findText = qobject_cast<KAction*>(ac->action("findText"));
	//TODO: Q_ASSERT(m_actions.findText);
	

	m_actions.findStrongs = new KAction(
		KIcon(CResMgr::displaywindows::general::findStrongs::icon),
		tr("Strong's Search"),
		ac
		);
	m_actions.findStrongs->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
	QObject::connect(m_actions.findStrongs, SIGNAL(triggered()), this, SLOT(openSearchStrongsDialog()) );
	ac->addAction(CResMgr::displaywindows::general::findStrongs::actionName, m_actions.findStrongs);

	m_actions.copy.reference = new KAction(tr("Reference only"), ac );
	QObject::connect(m_actions.copy.reference, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorOnly()) );
	ac->addAction("copyReferenceOnly", m_actions.copy.reference);

	m_actions.copy.entry = new KAction(tr("Entry with text"), ac );
	QObject::connect(m_actions.copy.entry, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAll()) );
	ac->addAction("copyEntryWithText", m_actions.copy.entry);

	Q_ASSERT(ac->action("copySelectedText"));
	m_actions.copy.selectedText = qobject_cast<KAction*>(ac->action("copySelectedText"));
	
	m_actions.save.entryAsPlain = new KAction(tr("Entry as plain text"), ac );
	QObject::connect(m_actions.save.entryAsPlain, SIGNAL(triggered()), this, SLOT(saveAsPlain()) );
	ac->addAction("saveEntryAsPlain", m_actions.save.entryAsPlain);

	m_actions.save.entryAsHTML = new KAction(tr("Entry as HTML"), ac );
	QObject::connect(m_actions.save.entryAsHTML, SIGNAL(triggered()), this, SLOT(saveAsHTML()));
	ac->addAction("saveEntryAsHTML", m_actions.save.entryAsHTML);

	m_actions.print.reference = new KAction(tr("Reference only"), ac);
	QObject::connect(m_actions.print.reference, SIGNAL(triggered()), this, SLOT(printAnchorWithText()));
	ac->addAction("printReferenceOnly", m_actions.print.reference);

	m_actions.print.entry = new KAction(tr("Entry with text"), ac);
	QObject::connect(m_actions.print.entry, SIGNAL(triggered()), this, SLOT(printAll()));
	ac->addAction("printEntryWithText", m_actions.print.entry);

	// init with the user defined settings
	qDebug("call CBTConfig::setupAccelSettings(CBTConfig::lexiconWindow, ac); and end CLexiconReadWindow::initActions");
	CBTConfig::setupAccelSettings(CBTConfig::lexiconWindow, ac);
};

/** No descriptions */
void CLexiconReadWindow::initConnections()
{
	qDebug("CLexiconReadWindow::initConnections");
	Q_ASSERT(keyChooser());

	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)),
			this, SLOT(lookup(CSwordKey*)));
	connect(keyChooser()->history(), SIGNAL(historyChanged(bool, bool)),
			this, SLOT(slotUpdateHistoryButtons(bool, bool)));

	//connect the history actions to the right slots
	connect(
		m_actions.backInHistory->popupMenu(), SIGNAL(aboutToShow()),
		this, SLOT(slotFillBackHistory())
	);
	connect(
		m_actions.backInHistory->popupMenu(), SIGNAL(triggered(QAction*)),
		keyChooser()->history(), SLOT(move(QAction*))
	);
	connect(
		m_actions.forwardInHistory->popupMenu(), SIGNAL(aboutToShow()),
		this, SLOT(slotFillForwardHistory())
	);
	connect(
		m_actions.forwardInHistory->popupMenu(), SIGNAL(triggered(QAction*)),
		keyChooser()->history(), SLOT(move(QAction*))
	);


}

void CLexiconReadWindow::initView()
{
	qDebug("CLexiconReadWindow::initView");
	setDisplayWidget( CDisplay::createReadInstance(this) );
	setMainToolBar( new QToolBar(this) );
	addToolBar(mainToolBar());
	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	mainToolBar()->addWidget(keyChooser());
	setModuleChooserBar( new CModuleChooserBar(modules(), modules().first()->type(), this) );
	moduleChooserBar()->adjustSize();
	addToolBar(moduleChooserBar());
	setButtonsToolBar( new QToolBar(this) );
	addToolBar(buttonsToolBar());
	setWindowIcon(CToolClass::getIconForModule(modules().first()));
	setCentralWidget( displayWidget()->view() );
}

void CLexiconReadWindow::initToolbars()
{
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

	setDisplaySettingsButton( new CDisplaySettingsButton( &displayOptions(), &filterOptions(), modules(), buttonsToolBar()) );

	//TODO: find the right place for the button
	buttonsToolBar()->addWidget(displaySettingsButton());
}

void CLexiconReadWindow::setupPopupMenu()
{
	popup()->setTitle(tr("Lexicon window"));
	popup()->setIcon(CToolClass::getIconForModule(modules().first()));
	//   m_actions.selectAll = new KAction(tr("Select all"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(selectAll()), actionCollection());
	popup()->addAction(m_actions.findText);
	popup()->addAction(m_actions.findStrongs);
	popup()->addAction(m_actions.selectAll);
	popup()->addSeparator();

	m_actions.copyMenu = new KActionMenu(KIcon(CResMgr::displaywindows::lexiconWindow::copyMenu::icon), tr("Copy..."), actionCollection());
	m_actions.copyMenu->setDelayed(false);

	m_actions.copyMenu->addAction(m_actions.copy.reference);
	m_actions.copyMenu->addAction(m_actions.copy.entry);
	m_actions.copyMenu->addSeparator();
	m_actions.copyMenu->addAction(m_actions.copy.selectedText);
	popup()->addAction(m_actions.copyMenu);

	m_actions.saveMenu = new KActionMenu(
			KIcon(CResMgr::displaywindows::lexiconWindow::saveMenu::icon),
			tr("Save..."),
			actionCollection()
			);
	m_actions.saveMenu->setDelayed(false);
	m_actions.saveMenu->addAction(m_actions.save.entryAsPlain);
	m_actions.saveMenu->addAction(m_actions.save.entryAsHTML);

	// Save raw HTML action for debugging purposes
	if (qApp->property("--debug").toBool()) {
		KAction* debugAction = new KAction("Raw HTML", this);
		QObject::connect(debugAction, SIGNAL(triggered()), this, SLOT(saveRawHTML()));
		m_actions.saveMenu->addAction(debugAction);
	} // end of Save Raw HTML

	popup()->addAction(m_actions.saveMenu);

	m_actions.printMenu = new KActionMenu(
			KIcon(CResMgr::displaywindows::lexiconWindow::printMenu::icon),
			tr("Print..."),
			actionCollection()
			);
	m_actions.printMenu->setDelayed(false);
	m_actions.printMenu->addAction(m_actions.print.reference);
	m_actions.printMenu->addAction(m_actions.print.entry);
	popup()->addAction(m_actions.printMenu);
}

/** Reimplemented. */
void CLexiconReadWindow::updatePopupMenu()
{
	//enable the action depending on the supported module features

	m_actions.findStrongs->setEnabled( displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null );
	
	m_actions.copy.reference->setEnabled( displayWidget()->hasActiveAnchor() );
	m_actions.copy.selectedText->setEnabled( displayWidget()->hasSelection() );

	m_actions.print.reference->setEnabled( displayWidget()->hasActiveAnchor() );
}

/** No descriptions */
void CLexiconReadWindow::nextEntry()
{
	keyChooser()->setKey(ldKey()->NextEntry());
}

/** No descriptions */
void CLexiconReadWindow::previousEntry()
{
	keyChooser()->setKey(ldKey()->PreviousEntry());
}

/** Reimplementation to return the right key. */
CSwordLDKey* CLexiconReadWindow::ldKey()
{
	return dynamic_cast<CSwordLDKey*>(CDisplayWindow::key());
}

/** This function saves the entry as html using the CExportMgr class. */
void CLexiconReadWindow::saveAsHTML() {
	CExportManager mgr(tr("Saving entry ..."), true, tr("Saving"), filterOptions(), displayOptions());
	mgr.saveKey(key(), CExportManager::HTML, true);
}

/** Saving the raw HTML for debugging purposes */
void CLexiconReadWindow::saveRawHTML()
{
	//qDebug("CLexiconReadWindow::saveRawHTML");
	QString savefilename = QFileDialog::getSaveFileName();
	if (savefilename.isEmpty()) return;
	QFile file(savefilename);
	CHTMLReadDisplay* disp = dynamic_cast<CHTMLReadDisplay*>(displayWidget());
	if (disp) {
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug("could not open file");
			return;
		}
		QString source = disp->text();
		int bytes = file.write(source.toUtf8());
		//qDebug() << "wrote" << bytes << "bytes";
		file.close();
		file.flush();
	} else {
		qDebug("No htmlreaddisplay widget!");
	}
	
}

/** This function saves the entry as html using the CExportMgr class. */
void CLexiconReadWindow::saveAsPlain()
{
	CExportManager mgr(tr("Saving entry ..."), true, tr("Saving"), filterOptions(), displayOptions());
	mgr.saveKey(key(), CExportManager::Text, true);
}

void CLexiconReadWindow::slotFillBackHistory()
{
	qDebug("CLexiconReadWindow::slotFillBackHistory");

	QMenu* menu = m_actions.backInHistory->popupMenu();
	menu->clear();

	//TODO: take the history list and fill the menu
	QListIterator<QAction*> it(keyChooser()->history()->getBackList());
	while (it.hasNext()) {
		menu->addAction(it.next());
	}
}

void CLexiconReadWindow::slotFillForwardHistory()
{
	qDebug("CLexiconReadWindow::slotFillForwardHistory");

	QMenu* menu = m_actions.forwardInHistory->popupMenu();
	menu->clear();
	//TODO: take the history list and fill the menu using addAction
	QListIterator<QAction*> it(keyChooser()->history()->getFwList());
	while (it.hasNext()) {
		menu->addAction(it.next());
	}
}


void CLexiconReadWindow::slotUpdateHistoryButtons(bool backEnabled, bool fwEnabled)
{
	qDebug("CLexiconReadWindow::slotUpdateHistoryButtons");
	Q_ASSERT(m_actions.backInHistory);
	Q_ASSERT(keyChooser());

	m_actions.backInHistory->setEnabled( backEnabled );
	m_actions.forwardInHistory->setEnabled( fwEnabled );
}
