/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/cbiblereadwindow.h"

#include <QAction>
#include <QApplication>
#include <QEvent>
#include <QMdiSubWindow>
#include <QMenu>
#include <QTimer>
#include <QWidget>
#include <QDebug>

#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/cexportmanager.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/ccommentaryreadwindow.h"
#include "frontend/displaywindow/btdisplaysettingsbutton.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "util/directory.h"
#include "util/cresmgr.h"
#include "util/tool.h"


using namespace Profile;

CBibleReadWindow::CBibleReadWindow(QList<CSwordModuleInfo*> moduleList, CMDIArea* parent)
        : CLexiconReadWindow(moduleList, parent) {
    qDebug() << "CBibleReadWindow::CBibleReadWindow";
}

CBibleReadWindow::~CBibleReadWindow() {
}

void CBibleReadWindow::applyProfileSettings(CProfileWindow* const settings) {
    /**
      \todo Make \ref CProfileWindow properly handle these things so we wouldn't have to mess
            around with bits.
    */
    CLexiconReadWindow::applyProfileSettings(settings);

    int result = settings->windowSettings();

    filterOptions().footnotes           = (result & 0x0001) != 0;
    filterOptions().strongNumbers       = (result & 0x0002) != 0;
    filterOptions().headings            = (result & 0x0004) != 0;
    filterOptions().morphTags           = (result & 0x0008) != 0;
    filterOptions().lemmas              = (result & 0x0010) != 0;
    filterOptions().hebrewPoints        = (result & 0x0020) != 0;
    filterOptions().hebrewCantillation  = (result & 0x0040) != 0;
    filterOptions().greekAccents        = (result & 0x0080) != 0;
    filterOptions().textualVariants     = (result & 0x0100) != 0;
    filterOptions().redLetterWords      = (result & 0x0200) != 0;
    filterOptions().scriptureReferences = (result & 0x0400) != 0;
    filterOptions().morphSegmentation   = (result & 0x0800) != 0;
    displayOptions().lineBreaks         = (result & 0x1000) != 0;
    displayOptions().verseNumbers       = (result & 0x2000) != 0;

    displaySettingsButton()->setFilterOptions(filterOptions(), false);
    displaySettingsButton()->setDisplayOptions(displayOptions());
}

void CBibleReadWindow::storeProfileSettings( CProfileWindow * const settings) {
    /**
      \todo Make \ref CProfileWindow properly handle these things so we wouldn't have to mess
            around with bits.
    */

    int result = 0x0000;
    if (filterOptions().footnotes)           result |= 0x0001;
    if (filterOptions().strongNumbers)       result |= 0x0002;
    if (filterOptions().headings)            result |= 0x0004;
    if (filterOptions().morphTags)           result |= 0x0008;
    if (filterOptions().lemmas)              result |= 0x0010;
    if (filterOptions().hebrewPoints)        result |= 0x0020;
    if (filterOptions().hebrewCantillation)  result |= 0x0040;
    if (filterOptions().greekAccents)        result |= 0x0080;
    if (filterOptions().textualVariants)     result |= 0x0100;
    if (filterOptions().redLetterWords)      result |= 0x0200;
    if (filterOptions().scriptureReferences) result |= 0x0400;
    if (filterOptions().morphSegmentation)   result |= 0x0800;
    if (displayOptions().lineBreaks)         result |= 0x1000;
    if (displayOptions().verseNumbers)       result |= 0x2000;

    settings->setWindowSettings(result);

    CLexiconReadWindow::storeProfileSettings(settings);
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

//	qaction = new QAction( /* QIcon(CResMgr::displaywindows::general::findStrongs::icon), */ tr("Strong's search"), a);
//	qaction->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
//	qaction->setToolTip(tr("Find all occurences of the Strong number currently under the mouse cursor"));
//	a->addAction(CResMgr::displaywindows::general::findStrongs::actionName, qaction);

//	qaction = new QAction(tr("Reference only"), a );
//	a->addAction("copyReferenceOnly", qaction);

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
    qDebug() << "CBibleReadWindow::initActions";

    BtActionCollection* ac = actionCollection();

    CLexiconReadWindow::initActions(); //make sure the predefined actions are available

    CBibleReadWindow::insertKeyboardActions(ac);

    //cleanup, not a clean oo-solution
    ac->action("nextEntry")->setEnabled(false);
    ac->action("previousEntry")->setEnabled(false);

    QAction* qaction;

    qaction = m_actionCollection->action("nextBook");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(nextBook()) );
    addAction(qaction);

    qaction = m_actionCollection->action("previousBook");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(previousBook()) );
    addAction(qaction);

    qaction = m_actionCollection->action("nextChapter");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(nextChapter()) );
    addAction(qaction);

    qaction = m_actionCollection->action("previousChapter");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(previousChapter()) );
    addAction(qaction);

    qaction = m_actionCollection->action("nextVerse");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(nextVerse()) );
    addAction(qaction);

    qaction = m_actionCollection->action("previousVerse");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT(previousVerse()) );
    addAction(qaction);

    m_actions.selectAll = ac->action("selectAll");
    Q_ASSERT(m_actions.selectAll);

    m_actions.findText = ac->action("findText");
    Q_ASSERT(m_actions.findText);

    m_actions.findStrongs = m_actionCollection->action(CResMgr::displaywindows::general::findStrongs::actionName);

    m_actions.copy.referenceOnly = m_actionCollection->action("copyReferenceOnly");

    m_actions.copy.referenceTextOnly = m_actionCollection->action("copyTextOfReference");
    QObject::connect(m_actions.copy.referenceTextOnly, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorTextOnly()) );
    addAction(m_actions.copy.referenceTextOnly);

    m_actions.copy.referenceAndText = m_actionCollection->action("copyReferenceWithText");
    QObject::connect(m_actions.copy.referenceAndText, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorWithText()) );
    addAction(m_actions.copy.referenceAndText);

    m_actions.copy.chapter = m_actionCollection->action("copyChapter");
    QObject::connect(m_actions.copy.chapter, SIGNAL(triggered()), this, SLOT(copyDisplayedText()) );
    addAction(m_actions.copy.chapter);

    m_actions.copy.selectedText = ac->action("copySelectedText");
    Q_ASSERT(m_actions.copy.selectedText);

    m_actions.save.referenceAndText = m_actionCollection->action("saveReferenceWithText");
    QObject::connect(m_actions.save.referenceAndText, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(saveAnchorWithText()) );
    addAction(m_actions.copy.chapter);

    m_actions.save.chapterAsPlain = m_actionCollection->action("saveChapterAsPlainText");
    QObject::connect(m_actions.save.chapterAsPlain, SIGNAL(triggered()), this, SLOT(saveChapterPlain()) );
    addAction(m_actions.save.referenceAndText);

    m_actions.save.chapterAsHTML = m_actionCollection->action("saveChapterAsHTML");
    QObject::connect(m_actions.save.chapterAsHTML, SIGNAL(triggered()), this, SLOT(saveChapterHTML()) );
    addAction(m_actions.save.chapterAsHTML);

    m_actions.print.reference = m_actionCollection->action("printReferenceWithText");
    QObject::connect(m_actions.print.reference, SIGNAL(triggered()), this, SLOT(printAnchorWithText()) );
    addAction(m_actions.print.reference);

    m_actions.print.chapter = m_actionCollection->action("printChapter");
    QObject::connect(m_actions.print.chapter, SIGNAL(triggered()), this, SLOT(printAll()) );
    addAction(m_actions.print.chapter);

    CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, ac);
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
    if (qApp->property("--debug").toBool()) {
        QAction* debugAction = new QAction("Raw HTML", this);
        QObject::connect(debugAction, SIGNAL(triggered()), this, SLOT(saveRawHTML()));
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

    m_actions.findStrongs->setEnabled( displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null );

    m_actions.copy.referenceOnly->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
    m_actions.copy.referenceTextOnly->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
    m_actions.copy.referenceAndText->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
    m_actions.copy.selectedText->setEnabled( ((CReadDisplay*)displayWidget())->hasSelection() );

    m_actions.save.referenceAndText->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );

    m_actions.print.reference->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
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

    CExportManager mgr(tr("Copy chapter to clipboard ..."), false, tr("Copying"), filterOptions(), displayOptions());
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

    CExportManager mgr(tr("Saving chapter ..."), true, tr("Saving"), filterOptions(), displayOptions());
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

    CExportManager mgr(tr("Saving chapter ..."), true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(&vk, CExportManager::Text, true);
}

void CBibleReadWindow::reload(CSwordBackend::SetupChangedReason reason) {
    CLexiconReadWindow::reload(reason);

    if (m_modules.count() == 0) {
        close();
        return;
    }

    //refresh the book lists
    verseKey()->setLocale( backend()->booknameLanguage().toLatin1() );
    keyChooser()->refreshContent();

    CBTConfig::setupAccelSettings(CBTConfig::bibleWindow, actionCollection());
}

/** No descriptions */
bool CBibleReadWindow::eventFilter( QObject* o, QEvent* e) {
    const bool ret = CLexiconReadWindow::eventFilter(o, e);

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

void CBibleReadWindow::lookupSwordKey( CSwordKey* newKey ) {
    CLexiconReadWindow::lookupSwordKey(newKey);
    syncWindows();
}

void CBibleReadWindow::syncWindows() {
    foreach (QMdiSubWindow* subWindow, mdi()->subWindowList()) {
        CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
        if (w && w->syncAllowed()) {
            w->lookupKey( key()->key() );
        }
    }
}
