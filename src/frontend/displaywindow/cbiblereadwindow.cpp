/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/cbiblereadwindow.h"

#include <QAction>
#include <QEvent>
#include <QMdiSubWindow>
#include <QMenu>
#include <QTimer>
#include <QWidget>
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "bibletimeapp.h"
#include "frontend/cexportmanager.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/ccommentaryreadwindow.h"
#include "frontend/displaywindow/btdisplaysettingsbutton.h"
#include "frontend/keychooser/ckeychooser.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/directory.h"
#include "util/cresmgr.h"
#include "util/tool.h"


void CBibleReadWindow::applyProfileSettings(const QString & windowGroup) {
    CLexiconReadWindow::applyProfileSettings(windowGroup);

    BtConfig & conf = btConfig();
    conf.beginGroup(windowGroup);
    filterOptions() = conf.getFilterOptions();
    displayOptions() = conf.getDisplayOptions();
    conf.endGroup();

    emit sigFilterOptionsChanged(filterOptions());
    emit sigDisplayOptionsChanged(displayOptions());

    // Apply settings to display:
    lookup();
}

void CBibleReadWindow::storeProfileSettings(QString const & windowGroup) const {
    BtConfig & conf = btConfig();
    conf.beginGroup(windowGroup);
    conf.setFilterOptions(filterOptions());
    conf.setDisplayOptions(displayOptions());
    conf.endGroup();

    CLexiconReadWindow::storeProfileSettings(windowGroup);
}


/** Reimplementation. */
void CBibleReadWindow::insertKeyboardActions( BtActionCollection* const a ) {
    QAction* qaction;

    qaction = new QAction(tr("Next book"), a);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
    a->addAction("nextBook", qaction);

    qaction = new QAction(tr("Previous book"), a);
    qaction->setShortcut( CResMgr::displaywindows::bibleWindow::previousBook::accel);
    a->addAction("previousBook", qaction);

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

    //popup menu items
    qaction = new QAction(tr("Copy chapter"), a);
    a->addAction("copyChapter", qaction);

    qaction = new QAction(tr("Save chapter as plain text"), a);
    a->addAction("saveChapterAsPlainText", qaction);

    qaction = new QAction(tr("Save chapter as HTML"), a);
    a->addAction("saveChapterAsHTML", qaction);

    qaction = new QAction(tr("Print chapter"), a);
    qaction->setShortcut(QKeySequence::Print);
    a->addAction("printChapter", qaction);

//    qaction = new QAction( /* QIcon(CResMgr::displaywindows::general::findStrongs::icon), */ tr("Strong's search"), a);
//    qaction->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
//    qaction->setToolTip(tr("Find all occurences of the Strong number currently under the mouse cursor"));
//    a->addAction(CResMgr::displaywindows::general::findStrongs::actionName, qaction);

//    qaction = new QAction(tr("Reference only"), a );
//    a->addAction("copyReferenceOnly", qaction);

    qaction = new QAction(tr("Text of reference"), a);
    a->addAction("copyTextOfReference", qaction);

    qaction = new QAction(tr("Reference with text"), a);
    a->addAction( "copyReferenceWithText", qaction);

    qaction = new QAction(tr("Reference with text"), a);
    a->addAction("printReferenceWithText", qaction);

    qaction = new QAction(tr("Reference with text"), a);
    a->addAction("saveReferenceWithText", qaction);
}

void CBibleReadWindow::initActions() {
    BtActionCollection* ac = actionCollection();

    CLexiconReadWindow::initActions(); //make sure the predefined actions are available

    CBibleReadWindow::insertKeyboardActions(ac);

    QAction * qaction;

    //cleanup, not a clean oo-solution
    qaction = ac->action("nextEntry");
    BT_ASSERT(qaction);
    qaction->setEnabled(false);
    qaction = ac->action("previousEntry");
    BT_ASSERT(qaction);
    qaction->setEnabled(false);


    qaction = m_actionCollection->action("nextBook");
    BT_ASSERT(qaction);
    BT_CONNECT(qaction, SIGNAL(triggered()), this, SLOT(nextBook()));
    addAction(qaction);

    qaction = m_actionCollection->action("previousBook");
    BT_ASSERT(qaction);
    BT_CONNECT(qaction, SIGNAL(triggered()), this, SLOT(previousBook()));
    addAction(qaction);

    qaction = m_actionCollection->action("nextChapter");
    BT_ASSERT(qaction);
    BT_CONNECT(qaction, SIGNAL(triggered()), this, SLOT(nextChapter()));
    addAction(qaction);

    qaction = m_actionCollection->action("previousChapter");
    BT_ASSERT(qaction);
    BT_CONNECT(qaction, SIGNAL(triggered()), this, SLOT(previousChapter()) );
    addAction(qaction);

    qaction = m_actionCollection->action("nextVerse");
    BT_ASSERT(qaction);
    BT_CONNECT(qaction, SIGNAL(triggered()), this, SLOT(nextVerse()) );
    addAction(qaction);

    qaction = m_actionCollection->action("previousVerse");
    BT_ASSERT(qaction);
    BT_CONNECT(qaction, SIGNAL(triggered()), this, SLOT(previousVerse()) );
    addAction(qaction);

    m_actions.selectAll = ac->action("selectAll");
    BT_ASSERT(m_actions.selectAll);

    m_actions.findText = ac->action("findText");
    BT_ASSERT(m_actions.findText);

    m_actions.findStrongs = m_actionCollection->action(CResMgr::displaywindows::general::findStrongs::actionName);
    BT_ASSERT(m_actions.findStrongs);

    m_actions.copy.referenceOnly = m_actionCollection->action("copyReferenceOnly");
    BT_ASSERT(m_actions.copy.referenceOnly);

    m_actions.copy.referenceTextOnly = m_actionCollection->action("copyTextOfReference");
    BT_ASSERT(m_actions.copy.referenceTextOnly);
    BT_CONNECT(m_actions.copy.referenceTextOnly,    SIGNAL(triggered()),
               displayWidget()->connectionsProxy(), SLOT(copyAnchorTextOnly()));
    addAction(m_actions.copy.referenceTextOnly);

    m_actions.copy.referenceAndText = m_actionCollection->action("copyReferenceWithText");
    BT_ASSERT(m_actions.copy.referenceAndText);
    BT_CONNECT(m_actions.copy.referenceAndText,     SIGNAL(triggered()),
               displayWidget()->connectionsProxy(), SLOT(copyAnchorWithText()));
    addAction(m_actions.copy.referenceAndText);

    m_actions.copy.chapter = m_actionCollection->action("copyChapter");
    BT_ASSERT(m_actions.copy.chapter);
    BT_CONNECT(m_actions.copy.chapter, SIGNAL(triggered()),
               this,                   SLOT(copyDisplayedText()));
    addAction(m_actions.copy.chapter);

    m_actions.copy.selectedText = ac->action("copySelectedText");
    BT_ASSERT(m_actions.copy.selectedText);

    m_actions.save.referenceAndText = m_actionCollection->action("saveReferenceWithText");
    BT_ASSERT(m_actions.save.referenceAndText);
    BT_CONNECT(m_actions.save.referenceAndText,     SIGNAL(triggered()),
               displayWidget()->connectionsProxy(), SLOT(saveAnchorWithText()));
    addAction(m_actions.copy.chapter);

    m_actions.save.chapterAsPlain = m_actionCollection->action("saveChapterAsPlainText");
    BT_ASSERT(m_actions.save.chapterAsPlain);
    BT_CONNECT(m_actions.save.chapterAsPlain, SIGNAL(triggered()),
               this,                          SLOT(saveChapterPlain()));
    addAction(m_actions.save.referenceAndText);

    m_actions.save.chapterAsHTML = m_actionCollection->action("saveChapterAsHTML");
    BT_ASSERT(m_actions.save.chapterAsHTML);
    BT_CONNECT(m_actions.save.chapterAsHTML, SIGNAL(triggered()),
               this,                         SLOT(saveChapterHTML()));
    addAction(m_actions.save.chapterAsHTML);

    m_actions.print.reference = m_actionCollection->action("printReferenceWithText");
    BT_ASSERT(m_actions.print.reference);
    BT_CONNECT(m_actions.print.reference, SIGNAL(triggered()),
               this,                      SLOT(printAnchorWithText()));
    addAction(m_actions.print.reference);

    m_actions.print.chapter = m_actionCollection->action("printChapter");
    BT_ASSERT(m_actions.print.chapter);
    BT_CONNECT(m_actions.print.chapter, SIGNAL(triggered()),
               this,                    SLOT(printAll()));
    addAction(m_actions.print.chapter);

    ac->readShortcuts("Bible shortcuts");
}

void CBibleReadWindow::initConnections() {
    CLexiconReadWindow::initConnections();
}

void CBibleReadWindow::initToolbars() {
    CLexiconReadWindow::initToolbars();
}

void CBibleReadWindow::initView() {
    CLexiconReadWindow::initView();

    parentWidget()->installEventFilter(this);
}

/** Reimplementation. */
void CBibleReadWindow::setupPopupMenu() {
    popup()->setTitle(tr("Bible window"));
    popup()->setIcon(util::tool::getIconForModule(modules().first()) );
    popup()->addAction(m_actions.findText);
    QKeySequence ks = m_actions.findText->shortcut();
    QString keys = ks.toString();
    popup()->addAction(m_actions.findStrongs);
    popup()->addAction(m_actions.selectAll);

    popup()->addSeparator();

    m_actions.copyMenu = new QMenu(tr("Copy..."), popup());
    m_actions.copyMenu->addAction(m_actions.copy.referenceOnly);
    m_actions.copyMenu->addAction(m_actions.copy.referenceTextOnly);
    m_actions.copyMenu->addAction(m_actions.copy.referenceAndText);
    m_actions.copyMenu->addAction(m_actions.copy.chapter);

    m_actions.copyMenu->addSeparator();

    m_actions.copyMenu->addAction(m_actions.copy.selectedText);
    popup()->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(tr("Save..."), popup());
    m_actions.saveMenu->addAction(m_actions.save.referenceAndText);
    m_actions.saveMenu->addAction(m_actions.save.chapterAsPlain);
    m_actions.saveMenu->addAction(m_actions.save.chapterAsHTML);

    // Save raw HTML action for debugging purposes
    if (btApp->debugMode()) {
        QAction* debugAction = new QAction("Raw HTML", this);
        BT_CONNECT(debugAction, SIGNAL(triggered()), this, SLOT(saveRawHTML()));
        m_actions.saveMenu->addAction(debugAction);
    } // end of Save Raw HTML
    popup()->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(tr("Print..."), popup());
    m_actions.printMenu->addAction(m_actions.print.reference);
    m_actions.printMenu->addAction(m_actions.print.chapter);
    popup()->addMenu(m_actions.printMenu);
}

/** Reimplemented. */
void CBibleReadWindow::updatePopupMenu() {
    qWarning("CBibleReadWindow::updatePopupMenu()");

    CReadDisplay const & display =
            *static_cast<CReadDisplay *>(displayWidget());
    m_actions.findStrongs->setEnabled(!display.getCurrentNodeInfo().isNull());

    bool const hasActiveAnchor = display.hasActiveAnchor();
    m_actions.copy.referenceOnly->setEnabled(hasActiveAnchor);
    m_actions.copy.referenceTextOnly->setEnabled(hasActiveAnchor);
    m_actions.copy.referenceAndText->setEnabled(hasActiveAnchor);
    m_actions.copy.selectedText->setEnabled(display.hasSelection());

    m_actions.save.referenceAndText->setEnabled(hasActiveAnchor);

    m_actions.print.reference->setEnabled(hasActiveAnchor);
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

/** wrapper around key() to return the right type of key. */
CSwordVerseKey* CBibleReadWindow::verseKey() {
    CSwordVerseKey* k = dynamic_cast<CSwordVerseKey*>(CDisplayWindow::key());
    BT_ASSERT(k);

    return k;
}

/** Copies the current chapter into the clipboard. */
void CBibleReadWindow::copyDisplayedText() {
    CSwordVerseKey dummy(*verseKey());
    dummy.setVerse(1);

    CSwordVerseKey vk(*verseKey());
    vk.setLowerBound(dummy);

    const CSwordBibleModuleInfo* bible = dynamic_cast<const CSwordBibleModuleInfo*>(modules().first());
    dummy.setVerse(bible->verseCount(dummy.book(), dummy.getChapter()));
    vk.setUpperBound(dummy);

    CExportManager mgr(false, tr("Copying"), filterOptions(), displayOptions());
    mgr.copyKey(&vk, CExportManager::Text, true);
}

/** Saves the chapter as valid HTML page. */
void CBibleReadWindow::saveChapterHTML() {
    //saves the complete chapter to disk
    BT_ASSERT(dynamic_cast<CSwordBibleModuleInfo const *>(modules().first()));
    const CSwordBibleModuleInfo *bible = static_cast<const CSwordBibleModuleInfo*>(modules().first());

    CSwordVerseKey dummy(*verseKey());
    dummy.setVerse(1);

    CSwordVerseKey vk(*verseKey());
    vk.setLowerBound(dummy);

    dummy.setVerse(bible->verseCount(dummy.book(), dummy.getChapter()));
    vk.setUpperBound(dummy);

    CExportManager mgr(true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(&vk, CExportManager::HTML, true);
}

/** Saves the chapter as valid HTML page. */
void CBibleReadWindow::saveChapterPlain() {
    //saves the complete chapter to disk

    CSwordVerseKey vk(*verseKey());
    CSwordVerseKey dummy(*verseKey());

    dummy.setVerse(1);
    vk.setLowerBound(dummy);

    const CSwordBibleModuleInfo* bible = dynamic_cast<const CSwordBibleModuleInfo*>(modules().first());
    dummy.setVerse(bible->verseCount(dummy.book(), dummy.getChapter()));
    vk.setUpperBound(dummy);

    CExportManager mgr(true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(&vk, CExportManager::Text, true);
}

void CBibleReadWindow::reload(CSwordBackend::SetupChangedReason reason) {
    CLexiconReadWindow::reload(reason);

    if (m_modules.isEmpty()) {
        close();
        return;
    }

    //refresh the book lists
    verseKey()->setLocale( CSwordBackend::instance()->booknameLanguage().toLatin1() );
    keyChooser()->refreshContent();

    actionCollection()->readShortcuts("Bible shortcuts");
}

/** No descriptions */
bool CBibleReadWindow::eventFilter( QObject* o, QEvent* e) {
    const bool ret = CLexiconReadWindow::eventFilter(o, e);

    //   BT_ASSERT(o->inherits("CDisplayWindow"));
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

void CBibleReadWindow::lookupSwordKey( CSwordKey* newKey ) {
    CLexiconReadWindow::lookupSwordKey(newKey);
    syncWindows();
}

void CBibleReadWindow::syncWindows() {
    Q_FOREACH(QMdiSubWindow * const subWindow, mdi()->subWindowList()) {
        CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
        if (w && w->syncAllowed()) {
            w->lookupKey( key()->key() );
        }
    }
}

void CBibleReadWindow::setupMainWindowToolBars() {
    CLexiconReadWindow::setupMainWindowToolBars();
}
