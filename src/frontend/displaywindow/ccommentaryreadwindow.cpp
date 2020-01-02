/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "ccommentaryreadwindow.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QToolBar>
#include "../../backend/keys/cswordversekey.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../bibletime.h"
#include "../bibletimeapp.h"
#include "../display/cdisplay.h"
#include "../display/creaddisplay.h"
#include "../keychooser/ckeychooser.h"
#include "btactioncollection.h"
#include "btmodulechooserbar.h"


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

    qaction = new QAction(CResMgr::displaywindows::commentaryWindow::syncWindow::icon(),
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

    ac->action("nextEntry").setEnabled(false);
    ac->action("previousEntry").setEnabled(false);

    auto const initAction = [this, ac](QString actionName,
                                       void (CCommentaryReadWindow::* slot)())
    {
        QAction & action = ac->action(std::move(actionName));
        BT_CONNECT(&action, &QAction::triggered, this, slot);
        addAction(&action);
    };

    initAction("nextBook", &CCommentaryReadWindow::nextBook);
    initAction("previousBook", &CCommentaryReadWindow::previousBook);
    initAction("nextChapter", &CCommentaryReadWindow::nextChapter);
    initAction("previousChapter", &CCommentaryReadWindow::previousChapter);
    initAction("nextVerse", &CCommentaryReadWindow::nextVerse);
    initAction("previousVerse", &CCommentaryReadWindow::previousVerse);

    QAction & qaction = ac->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    m_syncButton = &qaction;
    addAction(&qaction);

    actionCollection()->readShortcuts("Commentary shortcuts");
}

void CCommentaryReadWindow::applyProfileSettings(const QString & windowGroup) {
    CLexiconReadWindow::applyProfileSettings(windowGroup);

    BT_ASSERT(windowGroup.endsWith('/'));
    BT_ASSERT(m_syncButton);
    m_syncButton->setChecked(btConfig().sessionValue<bool>(windowGroup + "syncEnabled", false));
}

void CCommentaryReadWindow::storeProfileSettings(QString const & windowGroup)
        const
{
    CLexiconReadWindow::storeProfileSettings(windowGroup);

    BT_ASSERT(windowGroup.endsWith('/'));
    BT_ASSERT(m_syncButton);
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
    BT_ASSERT(k);
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
