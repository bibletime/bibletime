/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "ccommentaryreadwindow.h"

#include "frontend/cprofilewindow.h"
#include "frontend/cbtconfig.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

//Qt includes

//KDE includes
#include <kaction.h>
#include <kaccel.h>
#include <klocale.h>
#include <kpopupmenu.h>

using namespace Profile;

CCommentaryReadWindow::CCommentaryReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name) : CLexiconReadWindow(modules, parent,name) {}

void CCommentaryReadWindow::insertKeyboardActions( KActionCollection* const a ) {
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
}

void CCommentaryReadWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
	CLexiconReadWindow::applyProfileSettings(profileWindow);
	if (profileWindow->windowSettings()) {
		m_syncButton->setChecked(true);
	}
};

void CCommentaryReadWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
	CLexiconReadWindow::storeProfileSettings(profileWindow);
	profileWindow->setWindowSettings( m_syncButton->isChecked() );
};

void CCommentaryReadWindow::initToolbars() {
	CLexiconReadWindow::initToolbars();

	m_syncButton = new KToggleAction(i18n("Sync with active Bible"),
									 CResMgr::displaywindows::commentaryWindow::syncWindow::icon,
									 CResMgr::displaywindows::commentaryWindow::syncWindow::accel,
									 actionCollection(),
									 CResMgr::displaywindows::commentaryWindow::syncWindow::actionName
									);
	m_syncButton->setToolTip(CResMgr::displaywindows::commentaryWindow::syncWindow::tooltip);
	m_syncButton->plug(buttonsToolBar());
}

/** Reimplementation to handle the keychooser refresh. */
void CCommentaryReadWindow::reload() {
	CLexiconReadWindow::reload();

	//refresh the book lists
	verseKey()->setLocale( backend()->booknameLanguage().latin1() );
	keyChooser()->refreshContent();
}

/** rapper around key() to return the right type of key. */
CSwordVerseKey* CCommentaryReadWindow::verseKey() {
	CSwordVerseKey* k = dynamic_cast<CSwordVerseKey*>(CDisplayWindow::key());
	Q_ASSERT(k);
	return k;
}

void CCommentaryReadWindow::initActions() {
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

	CBTConfig::setupAccelSettings(CBTConfig::commentaryWindow, actionCollection());
}

/** Moves to the next book. */
void CCommentaryReadWindow::nextBook() {
	if (verseKey()->next(CSwordVerseKey::UseBook))
		keyChooser()->setKey(key());
}

/** Moves one book behind. */
void CCommentaryReadWindow::previousBook() {
	if (verseKey()->previous(CSwordVerseKey::UseBook))
		keyChooser()->setKey(key());
}

/** Moves to the next book. */
void CCommentaryReadWindow::nextChapter() {
	if (verseKey()->next(CSwordVerseKey::UseChapter))
		keyChooser()->setKey(key());
}

/** Moves one book behind. */
void CCommentaryReadWindow::previousChapter() {
	if (verseKey()->previous(CSwordVerseKey::UseChapter))
		keyChooser()->setKey(key());
}

/** Moves to the next book. */
void CCommentaryReadWindow::nextVerse() {
	if (verseKey()->next(CSwordVerseKey::UseVerse))
		keyChooser()->setKey(key());
}

/** Moves one book behind. */
void CCommentaryReadWindow::previousVerse() {
	if (verseKey()->previous(CSwordVerseKey::UseVerse))
		keyChooser()->setKey(key());
}

const bool CCommentaryReadWindow::syncAllowed() const {
	return m_syncButton->isChecked();
}


/*!
    \fn CCommentaryReadWindow::setupPopupMenu()
 */
void CCommentaryReadWindow::setupPopupMenu() {
	CLexiconReadWindow::setupPopupMenu();

	popup()->changeTitle(-1, CToolClass::getIconForModule(modules().first()), i18n("Commentary window"));
}
