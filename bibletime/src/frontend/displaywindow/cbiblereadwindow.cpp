/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cbiblereadwindow.h"
#include "cbiblereadwindow.moc"
#include "btactioncollection.h"

#include "ccommentaryreadwindow.h"
#include "cbuttons.h"

#include "backend/keys/cswordversekey.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"

#include "frontend/profile/cprofilewindow.h"
#include "frontend/cexportmanager.h"
#include "backend/config/cbtconfig.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/keychooser/ckeychooser.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <math.h>

//Qt includes
#include <QWidget>
#include <QTimer>
#include <QEvent>
#include <QApplication>
#include <QMenu>
#include <QMdiSubWindow>
#include <QAction>
#include <QMenu>
#include <QMenu>

using namespace Profile;

CBibleReadWindow::CBibleReadWindow(QList<CSwordModuleInfo*> moduleList, CMDIArea* parent)
	: CLexiconReadWindow(moduleList, parent)
{
	qDebug("CBibleReadWindow::CBibleReadWindow");
}

CBibleReadWindow::~CBibleReadWindow() {}

void CBibleReadWindow::applyProfileSettings( CProfileWindow* const settings ) {
	CLexiconReadWindow::applyProfileSettings(settings);

	const int count = displaySettingsButton()->menuItemCount();
	int result = settings->windowSettings();
	for (int i = count-1; i>=1; i--) {
		if (result-(int)pow((double)2,i-1)>= 0) { //2^i was added before, so item with index i is set
			result -= (int)pow((double)2,i-1);
			displaySettingsButton()->setItemStatus(i,true);
		}
		else {
			displaySettingsButton()->setItemStatus(i,false);
		}
	}
	displaySettingsButton()->setChanged();
}

void CBibleReadWindow::storeProfileSettings( CProfileWindow* const settings ) {
	CLexiconReadWindow::storeProfileSettings(settings);

	const int count = displaySettingsButton()->menuItemCount();
	int result = 0;
	//now check every item
	for (int i = 1; i < count; i++) { //first item is a title
		if (displaySettingsButton()->itemStatus(i)) //item is checked
			result += (int)pow((double)2,i-1);//add 2^i (the i. digit in binary)
	}
	settings->setWindowSettings(result);
}


/** Reimplementation. */
void CBibleReadWindow::insertKeyboardActions( BtActionCollection* const a ) {

	QAction* qaction;

	qaction = new QAction(tr("Next book"), a);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
	a->addAction("nextBook", qaction);
	
	qaction = new QAction(tr("Previous book"), a);
	qaction->setShortcut( CResMgr::displaywindows::bibleWindow::previousBook::accel);
	a->addAction("previousBook", qaction);
	
	qaction = new QAction(tr("Next chapter"), a);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
	a->addAction("nextChapter", qaction);
	
	qaction = new QAction(tr("Previous chapter"), a);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
	a->addAction("previousChapter", qaction);
	
	qaction = new QAction(tr("Next verse"), a);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
	a->addAction("nextVerse", qaction);
	
	qaction = new QAction(tr("Previous verse"), a);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
	a->addAction("previousVerse", qaction);
	

	//popup menu items
	//  new KAction(tr("Select all"), KStdAccel::selectAll(), a, "selectAll");

	//copy menu items
	//  new KAction(tr("Copy reference only"), KShortcut(0), a, "copyReferenceOnly");
	//  new KAction(tr("Text of reference"), KShortcut(0), a, "copyTextOfReference");
	//   new KAction(tr("Reference with text"), KShortcut(0), a, "copyReferenceWithText");
	qaction = new QAction(tr("Copy chapter"), a);
	a->addAction("copyChapter", qaction);
	//   new KAction(tr("Copy selected text"), KStdAccel::copy(), a, "copySelectedText");

	//save menu
	//   new KAction(tr("Reference with text"), KShortcut(0), a, "saveReferenceWithText");
	qaction = new QAction(tr("Save chapter as plain text"), a);
	a->addAction("saveChapterAsPlainText", qaction);

	qaction = new QAction(tr("Save chapter as HTML"), a);
	a->addAction("saveChapterAsHTML", qaction);
	//   new KAction(tr("Reference with text"), KShortcut(0), a, "saveReferenceWithText");

	//print
	qaction = new QAction(tr("Print chapter"), a);
	qaction->setShortcut(QKeySequence::Print);
	a->addAction("printChapter", qaction);
}

void CBibleReadWindow::initActions() {
	qDebug("CBibleReadWindow::initActions");

	BtActionCollection* ac = actionCollection();
	CBibleReadWindow::insertKeyboardActions(ac);
	CLexiconReadWindow::initActions(); //make sure the predefined actions are available

	//cleanup, not a clean oo-solution
	ac->action("nextEntry")->setEnabled(false);
	ac->action("previousEntry")->setEnabled(false);

	QAction* qaction;

	qaction = new QAction(tr("Next book"), ac);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
	QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(nextBook()) );
	ac->addAction("nextBook", qaction);

	qaction = new QAction(tr("Previous book"), ac);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousBook::accel);
	QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(previousBook()) );
	ac->addAction("previousBook", qaction);

	qaction = new QAction(tr("Next chapter"), ac);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
	QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(nextChapter()) );
	ac->addAction("nextChapter", qaction);

	qaction = new QAction(tr("Previous chapter"),ac);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
	QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(previousChapter()) );
	ac->addAction("previousChapter", qaction);

	qaction = new QAction(tr("Next verse"), ac);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
	QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(nextVerse()) );
	ac->addAction("nextVerse", qaction);

	qaction = new QAction(tr("Previous verse"), ac);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
	QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(previousVerse()) );
	ac->addAction("previousVerse", qaction);

	m_actions.selectAll = qobject_cast<QAction*>(ac->action("selectAll"));
	Q_ASSERT(m_actions.selectAll);

	m_actions.findText = qobject_cast<QAction*>(ac->action("findText"));
	Q_ASSERT(m_actions.findText);

	m_actions.findStrongs = new QAction(
		QIcon(CResMgr::displaywindows::general::findStrongs::icon),
		tr("Strong's search"),
		ac
		);
	m_actions.findStrongs->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
	m_actions.findStrongs->setToolTip(tr("Find all occurences of the Strong number currently under the mouse cursor"));
	QObject::connect(m_actions.findStrongs, SIGNAL(triggered()), this, SLOT(openSearchStrongsDialog()) );
	ac->addAction(CResMgr::displaywindows::general::findStrongs::actionName, m_actions.findStrongs);

	m_actions.copy.referenceOnly = new QAction(tr("Reference only"), ac );
	QObject::connect(m_actions.copy.referenceOnly, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorOnly()));
	ac->addAction("copyReferenceOnly", m_actions.copy.referenceOnly);

	m_actions.copy.referenceTextOnly = new QAction(tr("Text of reference"), ac);
	QObject::connect(m_actions.copy.referenceTextOnly, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorTextOnly()) );
	ac->addAction("copyTextOfReference", m_actions.copy.referenceTextOnly);

	m_actions.copy.referenceAndText = new QAction(tr("Reference with text"), ac);
	QObject::connect(m_actions.copy.referenceAndText, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorWithText()) );
	ac->addAction( "copyReferenceWithText", m_actions.copy.referenceAndText);

	m_actions.copy.chapter = new QAction(tr("Chapter"), ac);
	QObject::connect(m_actions.copy.chapter, SIGNAL(triggered()), this, SLOT(copyDisplayedText()) );
	ac->addAction("copyChapter", m_actions.copy.chapter);

	m_actions.copy.selectedText = qobject_cast<QAction*>(ac->action("copySelectedText"));
	Q_ASSERT(m_actions.copy.selectedText);

	m_actions.save.referenceAndText = new QAction(tr("Reference with text"), ac );
	QObject::connect(m_actions.save.referenceAndText, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(saveAnchorWithText()) );
	ac->addAction("saveReferenceWithText", m_actions.save.referenceAndText);

	m_actions.save.chapterAsPlain = new QAction(tr("Chapter as plain text"), ac);
	QObject::connect(m_actions.save.chapterAsPlain, SIGNAL(triggered()), this, SLOT(saveChapterPlain()) );
	ac->addAction("saveChapterAsPlainText", m_actions.save.chapterAsPlain);

	m_actions.save.chapterAsHTML = new QAction(tr("Chapter as HTML"), ac);
	QObject::connect(m_actions.save.chapterAsHTML, SIGNAL(triggered()), this, SLOT(saveChapterHTML()) );
	ac->addAction("saveChapterAsHTML", m_actions.save.chapterAsHTML);

	m_actions.print.reference = new QAction(tr("Reference with text"), ac);
	QObject::connect(m_actions.print.reference, SIGNAL(triggered()), this, SLOT(printAnchorWithText()) );
	ac->addAction("saveReferenceWithText", m_actions.print.reference);

	m_actions.print.chapter = new QAction(tr("Chapter"), ac);
	QObject::connect(m_actions.print.chapter, SIGNAL(triggered()), this, SLOT(printAll()) );
	ac->addAction("printChapter", m_actions.print.chapter);

//	CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, ac);
}

void CBibleReadWindow::initConnections() {
	CLexiconReadWindow::initConnections();
}

void CBibleReadWindow::initToolbars() {
	CLexiconReadWindow::initToolbars();
}

void CBibleReadWindow::initView() {
	CLexiconReadWindow::initView();

	parentWidget()->installEventFilter( this );
}

/** Reimplementation. */
void CBibleReadWindow::setupPopupMenu() {
	popup()->setTitle(tr("Bible window"));
	popup()->setIcon(CToolClass::getIconForModule(modules().first()) );
	popup()->addAction(m_actions.findText);
	popup()->addAction(m_actions.findStrongs);
	popup()->addAction(m_actions.selectAll);
		
	popup()->addSeparator();

	m_actions.copyMenu = new QMenu(tr("Copy..."), popup());
	m_actions.copyMenu->addAction(m_actions.copy.referenceOnly);
	m_actions.copyMenu->addAction(m_actions.copy.referenceTextOnly);
	m_actions.copyMenu->addAction(m_actions.copy.referenceAndText);
	m_actions.copyMenu->addAction(m_actions.copy.chapter);

	m_actions.copyMenu->addSeparator();

	m_actions.copyMenu->addAction(m_actions.copy.selectedText);
	popup()->addMenu(m_actions.copyMenu);

	m_actions.saveMenu = new QMenu(tr("Save..."), popup());
	m_actions.saveMenu->addAction(m_actions.save.referenceAndText);
	m_actions.saveMenu->addAction(m_actions.save.chapterAsPlain);
	m_actions.saveMenu->addAction(m_actions.save.chapterAsHTML);
	// Save raw HTML action for debugging purposes
	if (qApp->property("--debug").toBool()) {
		QAction* debugAction = new QAction("Raw HTML", this);
		QObject::connect(debugAction, SIGNAL(triggered()), this, SLOT(saveRawHTML()));
		m_actions.saveMenu->addAction(debugAction);
	} // end of Save Raw HTML
	popup()->addMenu(m_actions.saveMenu);

	m_actions.printMenu = new QMenu(tr("Print..."), popup());
	m_actions.printMenu->addAction(m_actions.print.reference);
	m_actions.printMenu->addAction(m_actions.print.chapter);
	popup()->addMenu(m_actions.printMenu);
}

/** Reimplemented. */
void CBibleReadWindow::updatePopupMenu() {
	qWarning("CBibleReadWindow::updatePopupMenu()");

	//enable the action depending on the supported module features
// 	bool hasStrongs = false;
// 	QList<CSwordModuleInfo*> mods = modules();
// 	for (QList<CSwordModuleInfo*>::iterator it = mods.begin(); it != mods.end(); ++it) {
// 		if ( (*it)->has( CSwordModuleInfo::strongNumbers ) ) {
// 			hasStrongs = true;
// 			break;
// 		}
// 	}
// 	
// 	m_actions.findStrongs->setEnabled( hasStrongs );
	m_actions.findStrongs->setEnabled( displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null );

	
	m_actions.copy.referenceOnly->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
	m_actions.copy.referenceTextOnly->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
	m_actions.copy.referenceAndText->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
	m_actions.copy.selectedText->setEnabled( ((CReadDisplay*)displayWidget())->hasSelection() );

	m_actions.save.referenceAndText->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );

	m_actions.print.reference->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
}

/** Moves to the next book. */
void CBibleReadWindow::nextBook() {
	if (verseKey()->next(CSwordVerseKey::UseBook)) {
		keyChooser()->setKey(key());
	}
}

/** Moves one book behind. */
void CBibleReadWindow::previousBook() {
	if (verseKey()->previous(CSwordVerseKey::UseBook)) {
		keyChooser()->setKey(key());
	}
}

/** Moves to the next book. */
void CBibleReadWindow::nextChapter() {
	if (verseKey()->next(CSwordVerseKey::UseChapter)) {
		keyChooser()->setKey(key());
	}
}

/** Moves one book behind. */
void CBibleReadWindow::previousChapter() {
	if (verseKey()->previous(CSwordVerseKey::UseChapter)) {
		keyChooser()->setKey(key());
	}
}

/** Moves to the next book. */
void CBibleReadWindow::nextVerse() {
	if (verseKey()->next(CSwordVerseKey::UseVerse)) {
		keyChooser()->setKey(key());
	}
}

/** Moves one book behind. */
void CBibleReadWindow::previousVerse() {
	if (verseKey()->previous(CSwordVerseKey::UseVerse)) {
		keyChooser()->setKey(key());
	}
}

/** rapper around key() to return the right type of key. */
CSwordVerseKey* CBibleReadWindow::verseKey() {
	CSwordVerseKey* k = dynamic_cast<CSwordVerseKey*>(CDisplayWindow::key());
	Q_ASSERT(k);
	
	return k;
}

/** Copies the current chapter into the clipboard. */
void CBibleReadWindow::copyDisplayedText() {
	CSwordVerseKey dummy(*verseKey());
	dummy.Verse(1);

	CSwordVerseKey vk(*verseKey());
	vk.LowerBound(dummy);

	CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(modules().first());
	dummy.Verse(bible->verseCount(dummy.book(), dummy.Chapter()));
	vk.UpperBound(dummy);

	CExportManager mgr(tr("Copy chapter to clipboard ..."), false, tr("Copying"), filterOptions(), displayOptions());
	mgr.copyKey(&vk, CExportManager::Text, true);
}

/** Saves the chapter as valid HTML page. */
void CBibleReadWindow::saveChapterHTML() {
	//saves the complete chapter to disk
	CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(modules().first());
	Q_ASSERT(bible);
	if (!bible) //shouldn't happen
		return;

	CSwordVerseKey dummy(*verseKey());
	dummy.Verse(1);

	CSwordVerseKey vk(*verseKey());
	vk.LowerBound(dummy);

	dummy.Verse(bible->verseCount(dummy.book(), dummy.Chapter()));
	vk.UpperBound(dummy);

	CExportManager mgr(tr("Saving chapter ..."), true, tr("Saving"), filterOptions(), displayOptions());
	mgr.saveKey(&vk, CExportManager::HTML, true);
}

/** Saves the chapter as valid HTML page. */
void CBibleReadWindow::saveChapterPlain() {
	//saves the complete chapter to disk

	CSwordVerseKey vk(*verseKey());
	CSwordVerseKey dummy(*verseKey());

	dummy.Verse(1);
	vk.LowerBound(dummy);

	CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(modules().first());
	dummy.Verse(bible->verseCount(dummy.book(), dummy.Chapter()));
	vk.UpperBound(dummy);

	CExportManager mgr(tr("Saving chapter ..."), true, tr("Saving"), filterOptions(),displayOptions());
	mgr.saveKey(&vk, CExportManager::Text, true);
}

void CBibleReadWindow::reload(CSwordBackend::SetupChangedReason reason) {
	CLexiconReadWindow::reload(reason);

	if (m_modules.count() == 0) {
		close();
		return;
	}

	//refresh the book lists
// 	qDebug("lang is %s",backend()->booknameLanguage().latin1());
	verseKey()->setLocale( backend()->booknameLanguage().toLatin1() );
	keyChooser()->refreshContent();

//	CBTConfig::setupAccelSettings(CBTConfig::readWindow, actionCollection()); //setup the predefined actions
//	CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, actionCollection());
}

/** No descriptions */
bool CBibleReadWindow::eventFilter( QObject* o, QEvent* e) {
	const bool ret = CLexiconReadWindow::eventFilter(o,e);

	//   Q_ASSERT(o->inherits("CDisplayWindow"));
	//   qWarning("class: %s", o->className());
	if (e && (e->type() == QEvent::FocusIn)) { //sync other windows to this active

		/* This is a hack to work around a KHTML problem (similair to the Drag&Drop problem we had):
		* If new HTML content is loaded from inside a  kHTML event handler
		* the widget's state will be confused, i.e. it's scrolling without having
		* the mousebutton clicked.
		*
		* This is not really in a KHTML event handler but works anyway.
		* Sometime KDE/Qt is hard to use ...
		*/
		QTimer::singleShot(0, this, SLOT(syncWindows()));
	}

	return ret;
}

void CBibleReadWindow::lookupSwordKey( CSwordKey* newKey ) {
	CLexiconReadWindow::lookupSwordKey(newKey);
	syncWindows();
}

void CBibleReadWindow::syncWindows() {
	foreach (QMdiSubWindow* subWindow, mdi()->subWindowList()) {
		CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
		if (w && w->syncAllowed()) {
			w->lookupKey( key()->key() );
		}
	}
}
