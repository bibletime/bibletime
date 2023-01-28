/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "ccommentaryreadwindow.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QToolBar>
#include "../../backend/config/btconfigcore.h"
#include "../../backend/keys/cswordversekey.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/cresmgr.h"
#include "../bibletime.h"
#include "../keychooser/ckeychooser.h"
#include "btactioncollection.h"


void CCommentaryReadWindow::insertKeyboardActions(BtActionCollection* const a) {
    QAction* qaction;

    qaction = new QAction(tr("Next book"), a);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
    a->addAction(QStringLiteral("nextBook"), qaction);

    qaction = new QAction(tr("Previous book"), a);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousBook::accel);
    a->addAction(QStringLiteral("previousBook"), qaction);

    qaction = new QAction(tr("Next chapter"), a);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
    a->addAction(QStringLiteral("nextChapter"), qaction);

    qaction = new QAction(tr("Previous chapter"), a);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
    a->addAction(QStringLiteral("previousChapter"), qaction);

    qaction = new QAction(tr("Next verse"), a);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
    a->addAction(QStringLiteral("nextVerse"), qaction);

    qaction = new QAction(tr("Previous verse"), a);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
    a->addAction(QStringLiteral("previousVerse"), qaction);

    qaction = new QAction(CResMgr::displaywindows::commentaryWindow::syncWindow::icon(),
        tr("Synchronize"), a);
    qaction->setCheckable(true);
    qaction->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
    qaction->setToolTip(tr("Synchronize the displayed entry of this work with the active Bible window"));
    a->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, qaction);
}

void CCommentaryReadWindow::initActions() {
    CDisplayWindow::initActions(); //make sure the predefined actions are available
    BtActionCollection* ac = actionCollection();
    insertKeyboardActions(ac);

    initAddAction(QStringLiteral("nextBook"),
                  this,
                  &CCommentaryReadWindow::nextBook);
    initAddAction(QStringLiteral("previousBook"),
                  this,
                  &CCommentaryReadWindow::previousBook);
    initAddAction(QStringLiteral("nextChapter"),
                  this,
                  &CCommentaryReadWindow::nextChapter);
    initAddAction(QStringLiteral("previousChapter"),
                  this,
                  &CCommentaryReadWindow::previousChapter);
    initAddAction(QStringLiteral("nextVerse"),
                  this,
                  &CCommentaryReadWindow::nextVerse);
    initAddAction(QStringLiteral("previousVerse"),
                  this,
                  &CCommentaryReadWindow::previousVerse);

    QAction & qaction = ac->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    m_syncButton = &qaction;
    addAction(&qaction);

    actionCollection()->readShortcuts(QStringLiteral("Commentary shortcuts"));
}

void CCommentaryReadWindow::applyProfileSettings(BtConfigCore const & conf) {
    CDisplayWindow::applyProfileSettings(conf);
    BT_ASSERT(m_syncButton);
    m_syncButton->setChecked(
                conf.value<bool>(QStringLiteral("syncEnabled"), false));
}

void CCommentaryReadWindow::storeProfileSettings(BtConfigCore & conf)
        const
{
    CDisplayWindow::storeProfileSettings(conf);
    BT_ASSERT(m_syncButton);
    conf.setValue(QStringLiteral("syncEnabled"), m_syncButton->isChecked());
}

void CCommentaryReadWindow::initToolbars() {
    CDisplayWindow::initToolbars();
    buttonsToolBar()->addAction(m_syncButton);
}

void CCommentaryReadWindow::setupMainWindowToolBars() {
    CDisplayWindow::setupMainWindowToolBars();
    btMainWindow()->toolsToolBar()->addAction(m_syncButton);
}

/** Reimplementation to handle the keychooser refresh. */
void CCommentaryReadWindow::reload() {
    CDisplayWindow::reload();

    //refresh the book lists
    verseKey()->setLocale(
                CSwordBackend::instance().booknameLanguage().toLatin1());
    keyChooser()->refreshContent();

    actionCollection()->readShortcuts(QStringLiteral("Commentary shortcuts"));
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
