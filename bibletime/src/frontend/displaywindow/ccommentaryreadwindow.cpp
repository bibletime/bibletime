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
#include "btactioncollection.h"

#include "frontend/profile/cprofilewindow.h"
#include "backend/config/cbtconfig.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"

#include "backend/keys/cswordversekey.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QToolBar>

using namespace Profile;

CCommentaryReadWindow::CCommentaryReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent) : CLexiconReadWindow(modules, parent) {}

void CCommentaryReadWindow::insertKeyboardActions( BtActionCollection* const a ) {

	QAction* qaction;

	qaction = new QAction(tr("Next book"), a);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
	a->addAction("nextBook", qaction);

	qaction = new QAction(tr("Previous book"), a);
	qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousBook::accel);
	a->addAction( "previousBook", qaction);

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

	m_syncButton = new QAction(
			QIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon)),
			tr("Synchronize"),
			actionCollection()
			);
	m_syncButton->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
	m_syncButton->setToolTip(tr("Synchronize the displayed entry of this work with the active Bible window"));
	actionCollection()->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, m_syncButton);
	buttonsToolBar()->addAction(m_syncButton);
}

/** Reimplementation to handle the keychooser refresh. */
void CCommentaryReadWindow::reload(CSwordBackend::SetupChangedReason reason) {
	CLexiconReadWindow::reload(reason);

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

	BtActionCollection* ac = actionCollection();

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

	qaction = new QAction(tr("Previous chapter"), ac);
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

//	CBTConfig::setupAccelSettings(CBTConfig::commentaryWindow, actionCollection());
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

bool CCommentaryReadWindow::syncAllowed() const {
	return m_syncButton->isChecked();
}


/*!
    \fn CCommentaryReadWindow::setupPopupMenu()
 */
void CCommentaryReadWindow::setupPopupMenu() {
	CLexiconReadWindow::setupPopupMenu();
}
