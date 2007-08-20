/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cbiblereadwindow.h"
#include "cbiblereadwindow.moc"

#include "ccommentaryreadwindow.h"
#include "cbuttons.h"

#include "backend/keys/cswordversekey.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"

#include "frontend/profile/cprofilewindow.h"
#include "frontend/cexportmanager.h"
#include "frontend/cbtconfig.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/keychooser/ckeychooser.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

#include <math.h>

//Qt includes
#include <QWidget>
#include <QTimer>
#include <QEvent>

//KDE includes
#include <kstandardshortcut.h>
#include <klocale.h>
#include <kmenu.h>
#include <kaction.h>
#include <kactionmenu.h>
#include <kactioncollection.h>

using namespace Profile;

CBibleReadWindow::CBibleReadWindow(ListCSwordModuleInfo moduleList, CMDIArea* parent)
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
};

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
};


/** Reimplementation. */
void CBibleReadWindow::insertKeyboardActions( KActionCollection* const a ) {

	KAction* kaction;

	kaction = new KAction(i18n("Next book"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
	a->addAction("nextBook", kaction);
	
	kaction = new KAction(i18n("Previous book"), a);
	kaction->setShortcut( CResMgr::displaywindows::bibleWindow::previousBook::accel);
	a->addAction("previousBook", kaction);
	
	kaction = new KAction(i18n("Next chapter"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
	a->addAction("nextChapter", kaction);
	
	kaction = new KAction(i18n("Previous chapter"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
	a->addAction("previousChapter", kaction);
	
	kaction = new KAction(i18n("Next verse"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
	a->addAction("nextVerse", kaction);
	
	kaction = new KAction(i18n("Previous verse"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
	a->addAction("previousVerse", kaction);
	

	//popup menu items
	//  new KAction(i18n("Select all"), KStdAccel::selectAll(), a, "selectAll");

	//copy menu items
	//  new KAction(i18n("Copy reference only"), KShortcut(0), a, "copyReferenceOnly");
	//  new KAction(i18n("Text of reference"), KShortcut(0), a, "copyTextOfReference");
	//   new KAction(i18n("Reference with text"), KShortcut(0), a, "copyReferenceWithText");
	kaction = new KAction(i18n("Copy chapter"), a);
	a->addAction("copyChapter", kaction);
	//   new KAction(i18n("Copy selected text"), KStdAccel::copy(), a, "copySelectedText");

	//save menu
	//   new KAction(i18n("Reference with text"), KShortcut(0), a, "saveReferenceWithText");
	kaction = new KAction(i18n("Save chapter as plain text"), a);
	a->addAction("saveChapterAsPlainText", kaction);

	kaction = new KAction(i18n("Save chapter as HTML"), a);
	a->addAction("saveChapterAsHTML", kaction);
	//   new KAction(i18n("Reference with text"), KShortcut(0), a, "saveReferenceWithText");

	//print
	kaction = new KAction(i18n("Print chapter"), a);
	kaction->setShortcut( KStandardShortcut::print());
	a->addAction("printChapter", kaction);
}

void CBibleReadWindow::initActions() {
	qDebug("CBibleReadWindow::initActions");

	KActionCollection* ac = actionCollection();
	CBibleReadWindow::insertKeyboardActions(ac);
	CLexiconReadWindow::initActions(); //make sure the predefined actions are available

	//cleanup, not a clean oo-solution
	ac->action("nextEntry")->setEnabled(false);
	ac->action("previousEntry")->setEnabled(false);

	KAction* kaction;

	kaction = new KAction(i18n("Next book"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(nextBook()) );
	ac->addAction("nextBook", kaction);

	kaction = new KAction(i18n("Previous book"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousBook::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(previousBook()) );
	ac->addAction("previousBook", kaction);

	kaction = new KAction(i18n("Next chapter"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(nextChapter()) );
	ac->addAction("nextChapter", kaction);

	kaction = new KAction(i18n("Previous chapter"),ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(previousChapter()) );
	ac->addAction("previousChapter", kaction);

	kaction = new KAction(i18n("Next verse"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(nextVerse()) );
	ac->addAction("nextVerse", kaction);

	kaction = new KAction(i18n("Previous verse"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(previousVerse()) );
	ac->addAction("previousVerse", kaction);


	m_actions.selectAll = qobject_cast<KAction*>(ac->action("selectAll"));
	Q_ASSERT(m_actions.selectAll);

	m_actions.findText = qobject_cast<KAction*>(ac->action("findText"));
	Q_ASSERT(m_actions.findText);

	m_actions.findStrongs = new KAction(
		KIcon(CResMgr::displaywindows::general::findStrongs::icon),
		i18n("Strong's Search"),
		ac
		);
	m_actions.findStrongs->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
	QObject::connect(m_actions.findStrongs, SIGNAL(triggered()), this, SLOT(openSearchStrongsDialog()) );
	ac->addAction(CResMgr::displaywindows::general::findStrongs::actionName, m_actions.findStrongs);

	m_actions.copy.referenceOnly = new KAction(i18n("Reference only"), ac );
	QObject::connect(m_actions.copy.referenceOnly, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorOnly()));
	ac->addAction("copyReferenceOnly", m_actions.copy.referenceOnly);

	m_actions.copy.referenceTextOnly = new KAction(i18n("Text of reference"), ac);
	QObject::connect(m_actions.copy.referenceTextOnly, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorTextOnly()) );
	ac->addAction("copyTextOfReference", m_actions.copy.referenceTextOnly);

	m_actions.copy.referenceAndText = new KAction(i18n("Reference with text"), ac);
	QObject::connect(m_actions.copy.referenceAndText, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorWithText()) );
	ac->addAction( "copyReferenceWithText", m_actions.copy.referenceAndText);

	m_actions.copy.chapter = new KAction(i18n("Chapter"), ac);
	QObject::connect(m_actions.copy.chapter, SIGNAL(triggered()), this, SLOT(copyDisplayedText()) );
	ac->addAction("copyChapter", m_actions.copy.chapter);

	m_actions.copy.selectedText = qobject_cast<KAction*>(ac->action("copySelectedText"));
	Q_ASSERT(m_actions.copy.selectedText);

	m_actions.save.referenceAndText = new KAction(i18n("Reference with text"), ac );
	QObject::connect(m_actions.save.referenceAndText, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(saveAnchorWithText()) );
	ac->addAction("saveReferenceWithText", m_actions.save.referenceAndText);

	m_actions.save.chapterAsPlain = new KAction(i18n("Chapter as plain text"), ac);
	QObject::connect(m_actions.save.chapterAsPlain, SIGNAL(triggered()), this, SLOT(saveChapterPlain()) );
	ac->addAction("saveChapterAsPlainText", m_actions.save.chapterAsPlain);

	m_actions.save.chapterAsHTML = new KAction(i18n("Chapter as HTML"), ac);
	QObject::connect(m_actions.save.chapterAsHTML, SIGNAL(triggered()), this, SLOT(saveChapterHTML()) );
	ac->addAction("saveChapterAsHTML", m_actions.save.chapterAsHTML);

	m_actions.print.reference = new KAction(i18n("Reference with text"), ac);
	QObject::connect(m_actions.print.reference, SIGNAL(triggered()), this, SLOT(printAnchorWithText()) );
	ac->addAction("saveReferenceWithText", m_actions.print.reference);

	m_actions.print.chapter = new KAction(i18n("Chapter"), ac);
	QObject::connect(m_actions.print.chapter, SIGNAL(triggered()), this, SLOT(printAll()) );
	ac->addAction("printChapter", m_actions.print.chapter);

	CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, ac);
}

void CBibleReadWindow::initConnections() {
	CLexiconReadWindow::initConnections();

	/*  if (m_transliterationButton) { // Transliteration is not always available
	    connect(m_transliterationButton, SIGNAL(sigChanged()), SLOT(lookup()));
	  }*/
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
	popup()->setTitle(i18n("Bible window"));
	popup()->setIcon(CToolClass::getIconForModule(modules().first()) );
	popup()->addAction(m_actions.findText);
	popup()->addAction(m_actions.findStrongs);
	popup()->addAction(m_actions.selectAll);
		
	popup()->addSeparator();

	m_actions.copyMenu = new KActionMenu(KIcon(CResMgr::displaywindows::bibleWindow::copyMenu::icon), i18n("Copy..."), popup());
	m_actions.copyMenu->setDelayed( false );
	m_actions.copyMenu->addAction(m_actions.copy.referenceOnly);
	m_actions.copyMenu->addAction(m_actions.copy.referenceTextOnly);
	m_actions.copyMenu->addAction(m_actions.copy.referenceAndText);
	m_actions.copyMenu->addAction(m_actions.copy.chapter);

	m_actions.copyMenu->addSeparator();

	m_actions.copyMenu->addAction(m_actions.copy.selectedText);
	popup()->addAction(m_actions.copyMenu);

	m_actions.saveMenu = new KActionMenu(KIcon(CResMgr::displaywindows::bibleWindow::saveMenu::icon), i18n("Save..."), popup());
	m_actions.saveMenu->setDelayed( false );
	m_actions.saveMenu->addAction(m_actions.save.referenceAndText);
	m_actions.saveMenu->addAction(m_actions.save.chapterAsPlain);
	m_actions.saveMenu->addAction(m_actions.save.chapterAsHTML);
	popup()->addAction(m_actions.saveMenu);

	m_actions.printMenu = new KActionMenu(KIcon(CResMgr::displaywindows::bibleWindow::printMenu::icon), i18n("Print..."), popup());
	m_actions.printMenu->setDelayed(false);
	m_actions.printMenu->addAction(m_actions.print.reference);
	m_actions.printMenu->addAction(m_actions.print.chapter);
	popup()->addAction(m_actions.printMenu);
}

/** Reimplemented. */
void CBibleReadWindow::updatePopupMenu() {
	qWarning("CBibleReadWindow::updatePopupMenu()");

	//enable the action depending on the supported module features
// 	bool hasStrongs = false;
// 	ListCSwordModuleInfo mods = modules();
// 	for (ListCSwordModuleInfo::iterator it = mods.begin(); it != mods.end(); ++it) {
// 		if ( (*it)->has( CSwordModuleInfo::strongNumbers ) ) {
// 			hasStrongs = true;
// 			break;
// 		}
// 	}
// 	
// 	m_actions.findStrongs->setEnabled( hasStrongs );
	m_actions.findStrongs->setEnabled( displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null );

	
	m_actions.copy.referenceOnly->setEnabled( displayWidget()->hasActiveAnchor() );
	m_actions.copy.referenceTextOnly->setEnabled( displayWidget()->hasActiveAnchor() );
	m_actions.copy.referenceAndText->setEnabled( displayWidget()->hasActiveAnchor() );
	m_actions.copy.selectedText->setEnabled( displayWidget()->hasSelection() );

	m_actions.save.referenceAndText->setEnabled( displayWidget()->hasActiveAnchor() );

	m_actions.print.reference->setEnabled( displayWidget()->hasActiveAnchor() );
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

	CExportManager mgr(i18n("Copy chapter to clipboard ..."), false, i18n("Copying"), filterOptions(), displayOptions());
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

	CExportManager mgr(i18n("Saving chapter ..."), true, i18n("Saving"), filterOptions(), displayOptions());
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

	CExportManager mgr(i18n("Saving chapter ..."), true, i18n("Saving"), filterOptions(),displayOptions());
	mgr.saveKey(&vk, CExportManager::Text, true);
}

void CBibleReadWindow::reload() {
	CLexiconReadWindow::reload();

	if (m_modules.count() == 0) {
		close();
		return;
	}

	//refresh the book lists
// 	qDebug("lang is %s",backend()->booknameLanguage().latin1());
	verseKey()->setLocale( backend()->booknameLanguage().toLatin1() );
	keyChooser()->refreshContent();

	CBTConfig::setupAccelSettings(CBTConfig::readWindow, actionCollection()); //setup the predefined actions
	CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, actionCollection());
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

void CBibleReadWindow::lookup( CSwordKey* newKey ) {
	qDebug("CBibleReadWindow::lookup( CSwordKey* newKey )");
	CLexiconReadWindow::lookup(newKey);

	syncWindows();
}

void CBibleReadWindow::syncWindows() {
	//  qWarning("syncing windows");
	QList<QWidget*> windows = mdi()->windowList();
	//  Q_ASSERT(windows.count());
	if (!windows.count()) {
		return;
	}

	for (int i = 0; i < windows.size(); i++) {
		CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(windows.at(i));
		//   Q_ASSERT(w && w->syncAllowed());

		if (w && w->syncAllowed()) {
			w->lookup( key()->key() );
		}
		/*  else {
		   qWarning("class sync: %s", w->className());
		  }*/
	}
}
