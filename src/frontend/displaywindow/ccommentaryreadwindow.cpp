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

CCommentaryReadWindow::ActionCollection::ActionCollection(QObject * const parent)
    : CDisplayWindow::ActionCollection(parent)
{
    QAction* qaction;

    qaction = new QAction(tr("Next book"), this);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
    addAction(QStringLiteral("nextBook"), qaction);

    qaction = new QAction(tr("Previous book"), this);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousBook::accel);
    addAction(QStringLiteral("previousBook"), qaction);

    qaction = new QAction(tr("Next chapter"), this);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextChapter::accel);
    addAction(QStringLiteral("nextChapter"), qaction);

    qaction = new QAction(tr("Previous chapter"), this);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousChapter::accel);
    addAction(QStringLiteral("previousChapter"), qaction);

    qaction = new QAction(tr("Next verse"), this);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextVerse::accel);
    addAction(QStringLiteral("nextVerse"), qaction);

    qaction = new QAction(tr("Previous verse"), this);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::previousVerse::accel);
    addAction(QStringLiteral("previousVerse"), qaction);

    qaction = new QAction(CResMgr::displaywindows::commentaryWindow::syncWindow::icon(),
        tr("Synchronize"), this);
    qaction->setCheckable(true);
    qaction->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
    qaction->setToolTip(tr("Synchronize the displayed entry of this work with the active Bible window"));
    addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, qaction);
}

CCommentaryReadWindow::CCommentaryReadWindow(
        QList<CSwordModuleInfo *> const & modules,
        CMDIArea * parent)
    : CDisplayWindow(modules, true, new ActionCollection(), parent)
{ init(); }

void CCommentaryReadWindow::initActions() {
    BtActionCollection* ac = actionCollection();

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
    CSwordVerseKey * const k =
            dynamic_cast<CSwordVerseKey*>(CDisplayWindow::swordKey());
    BT_ASSERT(k);
    return k;
}

/** Moves to the next book. */
void CCommentaryReadWindow::nextBook() {
    if (verseKey()->next(CSwordVerseKey::UseBook))
        keyChooser()->setKey(swordKey());
}

/** Moves one book behind. */
void CCommentaryReadWindow::previousBook() {
    if (verseKey()->previous(CSwordVerseKey::UseBook))
        keyChooser()->setKey(swordKey());
}

/** Moves to the next book. */
void CCommentaryReadWindow::nextChapter() {
    if (verseKey()->next(CSwordVerseKey::UseChapter))
        keyChooser()->setKey(swordKey());
}

/** Moves one book behind. */
void CCommentaryReadWindow::previousChapter() {
    if (verseKey()->previous(CSwordVerseKey::UseChapter))
        keyChooser()->setKey(swordKey());
}

/** Moves to the next book. */
void CCommentaryReadWindow::nextVerse() {
    if (verseKey()->next(CSwordVerseKey::UseVerse))
        keyChooser()->setKey(swordKey());
}

/** Moves one book behind. */
void CCommentaryReadWindow::previousVerse() {
    if (verseKey()->previous(CSwordVerseKey::UseVerse))
        keyChooser()->setKey(swordKey());
}

bool CCommentaryReadWindow::syncAllowed() const noexcept {
    return m_syncButton->isChecked();
}
