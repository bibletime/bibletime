/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cbiblereadwindow.h"
#include "ccommentaryreadwindow.h"
#include "cbuttons.h"

#include "backend/cswordversekey.h"
#include "backend/cswordbiblemoduleinfo.h"

#include "frontend/cprofilewindow.h"
#include "frontend/cexportmanager.h"
#include "frontend/cbtconfig.h"
#include "frontend/cmdiarea.h"
#include "util/cresmgr.h"

#include "frontend/display/creaddisplay.h"
#include "frontend/keychooser/ckeychooser.h"

#include "util/ctoolclass.h"

#include <math.h>

//Qt includes
#include <qwidgetlist.h>
#include <qtimer.h>

//KDE includes
#include <kaccel.h>
#include <klocale.h>
#include <kpopupmenu.h>

using namespace Profile;

CBibleReadWindow::CBibleReadWindow(ListCSwordModuleInfo moduleList, CMDIArea* parent, const char *name ) : CLexiconReadWindow(moduleList, parent,name) {
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
	new KAction(
		i18n("Next book"), CResMgr::displaywindows::bibleWindow::nextBook::accel,
		a, "nextBook"
	);
	new KAction(
		i18n("Previous book"), CResMgr::displaywindows::bibleWindow::previousBook::accel,
		a, "previousBook"
	);
	new KAction(
		i18n("Next chapter"), CResMgr::displaywindows::bibleWindow::nextChapter::accel,
		a, "nextChapter"
	);
	new KAction(
		i18n("Previous chapter"), CResMgr::displaywindows::bibleWindow::previousChapter::accel,
		a, "previousChapter"
	);
	new KAction(
		i18n("Next verse"), CResMgr::displaywindows::bibleWindow::nextVerse::accel,
		a, "nextVerse"
	);
	new KAction(
		i18n("Previous verse"), CResMgr::displaywindows::bibleWindow::previousVerse::accel,
		a, "previousVerse"
	);

	//popup menu items
	//  new KAction(i18n("Select all"), KStdAccel::selectAll(), a, "selectAll");

	//copy menu items
	//  new KAction(i18n("Copy reference only"), KShortcut(0), a, "copyReferenceOnly");
	//  new KAction(i18n("Text of reference"), KShortcut(0), a, "copyTextOfReference");
	//   new KAction(i18n("Reference with text"), KShortcut(0), a, "copyReferenceWithText");
	new KAction(i18n("Copy chapter"), KShortcut(0), a, "copyChapter");
	//   new KAction(i18n("Copy selected text"), KStdAccel::copy(), a, "copySelectedText");

	//save menu
	//   new KAction(i18n("Reference with text"), KShortcut(0), a, "saveReferenceWithText");
	new KAction(i18n("Save chapter as plain text"), KShortcut(0), a, "saveChapterAsPlainText");
	new KAction(i18n("Save chapter as HTML"), KShortcut(0), a, "saveChapterAsHTML");
	//   new KAction(i18n("Reference with text"), KShortcut(0), a, "saveReferenceWithText");

	//print
	new KAction(i18n("Print chapter"), KStdAccel::print(), a, "printChapter");
}

void CBibleReadWindow::initActions() {
	CLexiconReadWindow::initActions(); //make sure the predefined actions are available

	//cleanup, not a clean oo-solution
	actionCollection()->action("nextEntry")->setEnabled(false);
	actionCollection()->action("previousEntry")->setEnabled(false);

	new KAction(
		i18n("Next book"),
		CResMgr::displaywindows::bibleWindow::nextBook::accel,
		this, SLOT(nextBook()),
		actionCollection(), "nextBook"
	);
	new KAction(
		i18n("Previous book"),
		CResMgr::displaywindows::bibleWindow::previousBook::accel,
		this, SLOT(previousBook()),
		actionCollection(), "previousBook"
	);
	new KAction(
		i18n("Next chapter"),
		CResMgr::displaywindows::bibleWindow::nextChapter::accel,
		this, SLOT(nextChapter()),
		actionCollection(), "nextChapter"
	);
	new KAction(
		i18n("Previous chapter"),
		CResMgr::displaywindows::bibleWindow::previousChapter::accel,
		this, SLOT(previousChapter()),
		actionCollection(), "previousChapter"
	);
	new KAction(
		i18n("Next verse"),
		CResMgr::displaywindows::bibleWindow::nextVerse::accel,
		this, SLOT(nextVerse()),
		actionCollection(), "nextVerse"
	);
	new KAction(
		i18n("Previous verse"),
		CResMgr::displaywindows::bibleWindow::previousVerse::accel,
		this, SLOT(previousVerse()),
		actionCollection(), "previousVerse"
	);

	m_actions.selectAll = actionCollection()->action("selectAll");
	Q_ASSERT(m_actions.selectAll);

	m_actions.findText = actionCollection()->action("findText");
	Q_ASSERT(m_actions.findText);

	m_actions.findStrongs = new KAction(
		i18n("Strong's Search"),
		CResMgr::displaywindows::general::findStrongs::icon,
		CResMgr::displaywindows::general::findStrongs::accel,
		this, SLOT(openSearchStrongsDialog()),
		actionCollection(),
		CResMgr::displaywindows::general::findStrongs::actionName);


	m_actions.copy.referenceOnly = new KAction(i18n("Reference only"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(copyAnchorOnly()), actionCollection(), "copyReferenceOnly");

	m_actions.copy.referenceTextOnly = new KAction(i18n("Text of reference"), KShortcut(0),displayWidget()->connectionsProxy(), SLOT(copyAnchorTextOnly()), actionCollection(), "copyTextOfReference");

	m_actions.copy.referenceAndText = new KAction(i18n("Reference with text"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(copyAnchorWithText()), actionCollection(), "copyReferenceWithText");

	m_actions.copy.chapter = new KAction(i18n("Chapter"), KShortcut(0), this, SLOT(copyDisplayedText()), actionCollection(), "copyChapter");

	m_actions.copy.selectedText = actionCollection()->action("copySelectedText");
	Q_ASSERT(m_actions.copy.selectedText);

	m_actions.save.referenceAndText = new KAction(i18n("Reference with text"), KShortcut(0), displayWidget()->connectionsProxy(), SLOT(saveAnchorWithText()), actionCollection(), "saveReferenceWithText");

	m_actions.save.chapterAsPlain = new KAction(i18n("Chapter as plain text"), KShortcut(0), this, SLOT(saveChapterPlain()), actionCollection(), "saveChapterAsPlainText");

	m_actions.save.chapterAsHTML = new KAction(i18n("Chapter as HTML"), KShortcut(0), this, SLOT(saveChapterHTML()), actionCollection(), "saveChapterAsHTML");

	m_actions.print.reference = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(printAnchorWithText()), actionCollection(), "saveReferenceWithText");

	m_actions.print.chapter = new KAction(i18n("Chapter"), KShortcut(0), this, SLOT(printAll()), actionCollection(), "printChapter");

	CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, actionCollection());
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
	popup()->insertTitle(CToolClass::getIconForModule(modules().first()), i18n("Bible window"));

	m_actions.findText->plug(popup());
	m_actions.findStrongs->plug(popup());
	m_actions.selectAll->plug(popup());
		
	(new KActionSeparator(this))->plug( popup() );

	m_actions.copyMenu = new KActionMenu(i18n("Copy..."), CResMgr::displaywindows::bibleWindow::copyMenu::icon, popup());
	m_actions.copyMenu->setDelayed( false );
	m_actions.copyMenu->insert(m_actions.copy.referenceOnly);
	m_actions.copyMenu->insert(m_actions.copy.referenceTextOnly);
	m_actions.copyMenu->insert(m_actions.copy.referenceAndText);
	m_actions.copyMenu->insert(m_actions.copy.chapter);
	m_actions.copyMenu->insert(new KActionSeparator(this));
	m_actions.copyMenu->insert(m_actions.copy.selectedText);
	m_actions.copyMenu->plug(popup());

	m_actions.saveMenu = new KActionMenu(i18n("Save..."),CResMgr::displaywindows::bibleWindow::saveMenu::icon, popup());
	m_actions.saveMenu->setDelayed( false );
	m_actions.saveMenu->insert(m_actions.save.referenceAndText);
	m_actions.saveMenu->insert(m_actions.save.chapterAsPlain);
	m_actions.saveMenu->insert(m_actions.save.chapterAsHTML);
	m_actions.saveMenu->plug(popup());

	m_actions.printMenu = new KActionMenu(i18n("Print..."),CResMgr::displaywindows::bibleWindow::printMenu::icon, popup());
	m_actions.printMenu->setDelayed(false);
	m_actions.printMenu->insert(m_actions.print.reference);
	m_actions.printMenu->insert(m_actions.print.chapter);
	m_actions.printMenu->plug(popup());
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
	verseKey()->setLocale( backend()->booknameLanguage().latin1() );
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
	CLexiconReadWindow::lookup(newKey);

	syncWindows();
}

void CBibleReadWindow::syncWindows() {
	//  qWarning("syncing windows");
	QWidgetList windows = mdi()->windowList();
	//  Q_ASSERT(windows.count());
	if (!windows.count()) {
		return;
	}

	for (windows.first(); windows.current(); windows.next()) {
		CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(windows.current());
		//   Q_ASSERT(w && w->syncAllowed());

		if (w && w->syncAllowed()) {
			w->lookup( key()->key() );
		}
		/*  else {
		   qWarning("class sync: %s", w->className());
		  }*/
	}
}
