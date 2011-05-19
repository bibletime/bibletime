/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/ccommentaryreadwindow.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QToolBar>
#include "backend/config/cbtconfig.h"
#include "backend/keys/cswordversekey.h"
#include "bibletime.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "util/directory.h"
#include "util/cresmgr.h"

using namespace Profile;

CCommentaryReadWindow::CCommentaryReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent) : CLexiconReadWindow(modules, parent) {
}

void CCommentaryReadWindow::insertKeyboardActions(BtActionCollection* const a) {
    namespace DU = util::directory;
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

    qaction = new QAction(QIcon(DU::getIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon)),
        tr("Synchronize"), a);
    qaction->setCheckable(true);
    qaction->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
    qaction->setToolTip(tr("Synchronize the displayed entry of this work with the active Bible window"));
    a->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, qaction);
}

void CCommentaryReadWindow::initActions() {
    CLexiconReadWindow::initActions(); //make sure the predefined actions are available
    BtActionCollection* ac = actionCollection();
    insertKeyboardActions(ac);

    //cleanup, not a clean oo-solution
    QAction *qaction = ac->action("nextEntry");
    Q_ASSERT(qaction != 0);
    qaction->setEnabled(false);
    qaction = ac->action("previousEntry");
    Q_ASSERT(qaction != 0);
    qaction->setEnabled(false);

    qaction = ac->action("nextBook");
    Q_ASSERT(qaction != 0);
    QObject::connect(qaction, SIGNAL(triggered()),
                     this,    SLOT(nextBook()));
    addAction(qaction);

    qaction = ac->action("previousBook");
    Q_ASSERT(qaction != 0);
    QObject::connect(qaction, SIGNAL(triggered()),
                     this,    SLOT(previousBook()));
    addAction(qaction);

    qaction = ac->action("nextChapter");
    Q_ASSERT(qaction != 0);
    QObject::connect(qaction, SIGNAL(triggered()),
                     this,    SLOT(nextChapter()));
    addAction(qaction);

    qaction = ac->action("previousChapter");
    Q_ASSERT(qaction != 0);
    QObject::connect(qaction, SIGNAL(triggered()),
                     this,    SLOT(previousChapter()));
    addAction(qaction);

    qaction = ac->action("nextVerse");
    Q_ASSERT(qaction != 0);
    QObject::connect(qaction, SIGNAL(triggered()),
                     this,    SLOT(nextVerse()));
    addAction(qaction);

    qaction = ac->action("previousVerse");
    Q_ASSERT(qaction != 0);
    QObject::connect(qaction, SIGNAL(triggered()),
                     this,    SLOT(previousVerse()));
    addAction(qaction);

    qaction = ac->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    Q_ASSERT(qaction != 0);
    m_syncButton = qaction;
    addAction(qaction);

    CBTConfig::setupAccelSettings(CBTConfig::commentaryWindow, actionCollection());
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
    buttonsToolBar()->addAction(m_syncButton);
}

void CCommentaryReadWindow::setupMainWindowToolBars() {
    CLexiconReadWindow::setupMainWindowToolBars();
    btMainWindow()->toolsToolBar()->addAction(m_syncButton);
}

/** Reimplementation to handle the keychooser refresh. */
void CCommentaryReadWindow::reload(CSwordBackend::SetupChangedReason reason) {
    CLexiconReadWindow::reload(reason);

    //refresh the book lists
    verseKey()->setLocale( CSwordBackend::instance()->booknameLanguage().toLatin1() );
    keyChooser()->refreshContent();

    CBTConfig::setupAccelSettings(CBTConfig::commentaryWindow, actionCollection());
}

/** rapper around key() to return the right type of key. */
CSwordVerseKey* CCommentaryReadWindow::verseKey() {
    CSwordVerseKey* k = dynamic_cast<CSwordVerseKey*>(CDisplayWindow::key());
    Q_ASSERT(k);
    return k;
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
