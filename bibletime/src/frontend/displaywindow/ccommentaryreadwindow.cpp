/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "ccommentaryreadwindow.h"
#include "ccommentaryreadwindow.moc"

#include "frontend/profile/cprofilewindow.h"
#include "frontend/cbtconfig.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"

#include "backend/keys/cswordversekey.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

#include <QMenu>

#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <ktoolbar.h>

using namespace Profile;

CCommentaryReadWindow::CCommentaryReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent) : CLexiconReadWindow(modules, parent) {}

void CCommentaryReadWindow::insertKeyboardActions( KActionCollection* const a ) {
	
	KAction* kaction;
	
	kaction = new KAction(tr("Next book"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
	a->addAction("nextBook", kaction);
	
	kaction = new KAction(tr("Previous book"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousBook::accel);
	a->addAction( "previousBook", kaction);
	
	kaction = new KAction(tr("Next chapter"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
	a->addAction("nextChapter", kaction);
	
	kaction = new KAction(tr("Previous chapter"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
	a->addAction("previousChapter", kaction);
	
	kaction = new KAction(tr("Next verse"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
	a->addAction("nextVerse", kaction);
	
	kaction = new KAction(tr("Previous verse"), a);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
	a->addAction("previousVerse", kaction);
	
}

void CCommentaryReadWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
	CLexiconReadWindow::applyProfileSettings(profileWindow);
	if (profileWindow->windowSettings()) {
		m_syncButton->setChecked(true);
	}
}

void CCommentaryReadWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
	CLexiconReadWindow::storeProfileSettings(profileWindow);
	profileWindow->setWindowSettings( m_syncButton->isChecked() );
}

void CCommentaryReadWindow::initToolbars() {
	CLexiconReadWindow::initToolbars();

	m_syncButton = new KToggleAction(
			KIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon),
			tr("Sync with active Bible"),
			actionCollection()
			);
	m_syncButton->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
	m_syncButton->setToolTip(CResMgr::displaywindows::commentaryWindow::syncWindow::tooltip);	
	actionCollection()->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, m_syncButton);
	buttonsToolBar()->addAction(m_syncButton);
}

/** Reimplementation to handle the keychooser refresh. */
void CCommentaryReadWindow::reload() {
	CLexiconReadWindow::reload();

	//refresh the book lists
	verseKey()->setLocale( backend()->booknameLanguage().toLatin1() );
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

	KActionCollection* ac = actionCollection();

	//cleanup, not a clean oo-solution
	ac->action("nextEntry")->setEnabled(false);
	ac->action("previousEntry")->setEnabled(false);

	KAction* kaction;

	kaction = new KAction(tr("Next book"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(nextBook()) );
	ac->addAction("nextBook", kaction);

	kaction = new KAction(tr("Previous book"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousBook::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(previousBook()) );
	ac->addAction("previousBook", kaction);

	kaction = new KAction(tr("Next chapter"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(nextChapter()) );
	ac->addAction("nextChapter", kaction);

	kaction = new KAction(tr("Previous chapter"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(previousChapter()) );
	ac->addAction("previousChapter", kaction);

	kaction = new KAction(tr("Next verse"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(nextVerse()) );
	ac->addAction("nextVerse", kaction);

	kaction = new KAction(tr("Previous verse"), ac);
	kaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(previousVerse()) );
	ac->addAction("previousVerse", kaction);

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

	//popup()->changeTitle(-1, CToolClass::getIconForModule(modules().first()), tr("Commentary window"));
	popup()->actions().at(0)->setText(tr("Commentary window"));
	popup()->actions().at(0)->setIcon(CToolClass::getIconForModule(modules().first()));
}
