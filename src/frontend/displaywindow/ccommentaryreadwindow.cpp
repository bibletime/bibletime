/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/ccommentaryreadwindow.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QToolBar>
#include "backend/keys/cswordversekey.h"
#include "bibletime.h"
#include "bibletimeapp.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/ckeychooser.h"
#include "util/cresmgr.h"
#include "util/geticon.h"


void CCommentaryReadWindow::insertKeyboardActions(BtActionCollection* const a) {
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

    qaction = new QAction(QIcon(util::getIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon)),
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

    actionCollection()->readShortcuts("Commentary shortcuts");
}

void CCommentaryReadWindow::applyProfileSettings(const QString & windowGroup) {
    CLexiconReadWindow::applyProfileSettings(windowGroup);

    Q_ASSERT(windowGroup.endsWith('/'));
    Q_ASSERT(m_syncButton);
    m_syncButton->setChecked(btConfig().sessionValue<bool>(windowGroup + "syncEnabled", false));
}

void CCommentaryReadWindow::storeProfileSettings(const QString & windowGroup) {
    CLexiconReadWindow::storeProfileSettings(windowGroup);

    Q_ASSERT(windowGroup.endsWith('/'));
    Q_ASSERT(m_syncButton);
    btConfig().setSessionValue(windowGroup + "syncEnabled", m_syncButton->isChecked());
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

    actionCollection()->readShortcuts("Commentary shortcuts");
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
