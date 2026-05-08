/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cbiblereadwindow.h"

#include <QAction>
#include <QClipboard>
#include <QEvent>
#include <QGuiApplication>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QMenu>
#include <QRegularExpression>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QWidget>
#include "../../backend/drivers/cswordbiblemoduleinfo.h"
#include "../../backend/keys/cswordversekey.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/cresmgr.h"
#include "../cexportmanager.h"
#include "../cmdiarea.h"
#include "../bttexteditorwindow.h"
#include "../display/btmodelviewreaddisplay.h"
#include "../display/modelview/btqmlinterface.h"
#include "../keychooser/ckeychooser.h"

namespace {

QString bibleTextForEditor(QString text) {
    text.remove(
                QRegularExpression(
                    QStringLiteral("\\s*\\[[^\\]]*(?:crossrefs\\s*=|sword://Bible/)[^\\]]*\\]"),
                    QRegularExpression::CaseInsensitiveOption));
    text.remove(
                QRegularExpression(
                    QStringLiteral("\\s*\\[[^\\]]*\\]")));
    text.remove(
                QRegularExpression(
                    QStringLiteral("\\s*<[GH][0-9]+>"),
                    QRegularExpression::CaseInsensitiveOption));

    auto plainText = QTextDocumentFragment::fromHtml(text).toPlainText();
    plainText.remove(
                QRegularExpression(
                    QStringLiteral("\\s*<[GH][0-9]+>"),
                    QRegularExpression::CaseInsensitiveOption));
    plainText.remove(QRegularExpression(QStringLiteral("\\s*\\[[^\\]]*\\]")));
    plainText.replace(QRegularExpression(QStringLiteral("[ \\t]+\\n")),
                      QStringLiteral("\n"));
    plainText.replace(QRegularExpression(QStringLiteral("\\n[ \\t]+")),
                      QStringLiteral("\n"));
    plainText.replace(QRegularExpression(QStringLiteral("[ \\t]{2,}")),
                      QStringLiteral(" "));
    return plainText.trimmed();
}

} // anonymous namespace


CBibleReadWindow::ActionCollection::ActionCollection(QObject * const parent)
    : CDisplayWindow::ActionCollection(parent)
{
    QAction* qaction;

    qaction = new QAction(tr("Next book"), this);
    qaction->setShortcut(CResMgr::displaywindows::bibleWindow::nextBook::accel);
    addAction(QStringLiteral("nextBook"), qaction);

    qaction = new QAction(tr("Previous book"), this);
    qaction->setShortcut( CResMgr::displaywindows::bibleWindow::previousBook::accel);
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

    //popup menu items
    qaction = new QAction(tr("Copy chapter"), this);
    addAction(QStringLiteral("copyChapter"), qaction);

    qaction = new QAction(tr("Save chapter as plain text"), this);
    addAction(QStringLiteral("saveChapterAsPlainText"), qaction);

    qaction = new QAction(tr("Save chapter as HTML"), this);
    addAction(QStringLiteral("saveChapterAsHTML"), qaction);

    qaction = new QAction(tr("Print chapter"), this);
    qaction->setShortcut(QKeySequence::Print);
    addAction(QStringLiteral("printChapter"), qaction);

    qaction = new QAction(tr("Text of reference"), this);
    addAction(QStringLiteral("copyTextOfReference"), qaction);

    qaction = new QAction(tr("Reference with text"), this);
    addAction(QStringLiteral("copyReferenceWithText"), qaction);

    qaction = new QAction(tr("Reference with text"), this);
    addAction(QStringLiteral("printReferenceWithText"), qaction);

    qaction = new QAction(tr("Reference with text"), this);
    addAction(QStringLiteral("saveReferenceWithText"), qaction);

    qaction = new QAction(tr("Selected text"), this);
    addAction(QStringLiteral("sendBibleSelectedTextToEditor"), qaction);

    qaction = new QAction(tr("Reference with text"), this);
    addAction(QStringLiteral("sendReferenceWithTextToEditor"), qaction);

    qaction = new QAction(tr("Chapter"), this);
    addAction(QStringLiteral("sendChapterToEditor"), qaction);
}

CBibleReadWindow::CBibleReadWindow(QList<CSwordModuleInfo *> const & modules,
                                   QString const & key,
                                   CMDIArea * parent)
    : CDisplayWindow(modules, key, true, new ActionCollection(), parent)
{ init(); }

void CBibleReadWindow::applyProfileSettings(BtConfigCore const & conf) {
    CDisplayWindow::applyProfileSettings(conf);

    setFilterOptions(BtConfig::loadFilterOptionsFromGroup(conf));
    setDisplayOptions(BtConfig::loadDisplayOptionsFromGroup(conf));

    Q_EMIT sigFilterOptionsChanged(filterOptions());
    Q_EMIT sigDisplayOptionsChanged(displayOptions());

    // Apply settings to display:
    lookup();
}

void CBibleReadWindow::storeProfileSettings(BtConfigCore & conf) const {
    BtConfig::storeFilterOptionsToGroup(filterOptions(), conf);
    BtConfig::storeDisplayOptionsToGroup(displayOptions(), conf);

    CDisplayWindow::storeProfileSettings(conf);
}

void CBibleReadWindow::initActions() {
    initAddAction(QStringLiteral("nextBook"),
                  this,
                  &CBibleReadWindow::nextBook);
    initAddAction(QStringLiteral("previousBook"),
                  this,
                  &CBibleReadWindow::previousBook);
    initAddAction(QStringLiteral("nextChapter"),
                  this,
                  &CBibleReadWindow::nextChapter);
    initAddAction(QStringLiteral("previousChapter"),
                  this,
                  &CBibleReadWindow::previousChapter);
    initAddAction(QStringLiteral("nextVerse"),
                  this,
                  &CBibleReadWindow::nextVerse);
    initAddAction(QStringLiteral("previousVerse"),
                  this,
                  &CBibleReadWindow::previousVerse);

    auto * const ac = actionCollection();
    m_actions.findText = &ac->action(QStringLiteral("findText"));
    m_actions.findStrongs = &ac->action(CResMgr::displaywindows::general::findStrongs::actionName);
    m_actions.copy.referenceOnly =
            &ac->action(QStringLiteral("copyReferenceOnly"));

    m_actions.copy.referenceTextOnly =
            &initAddAction(QStringLiteral("copyTextOfReference"),
                           displayWidget(),
                           &BtModelViewReadDisplay::copyAnchorTextOnly);

    m_actions.copy.referenceAndText =
            &initAddAction(QStringLiteral("copyReferenceWithText"),
                           displayWidget(),
                           &BtModelViewReadDisplay::copyAnchorWithText);

    m_actions.copy.chapter =
            &initAddAction(QStringLiteral("copyChapter"),
                           this,
                           &CBibleReadWindow::copyDisplayedText);

    m_actions.copy.selectedText =
            &ac->action(QStringLiteral("copySelectedText"));

    m_actions.copy.byReferences =
            &ac->action(QStringLiteral("copyByReferences"));

    m_actions.save.referenceAndText =
            &initAddAction(QStringLiteral("saveReferenceWithText"),
                           displayWidget(),
                           &BtModelViewReadDisplay::saveAnchorWithText);

    m_actions.save.chapterAsPlain =
            &initAddAction(QStringLiteral("saveChapterAsPlainText"),
                           this,
                           &CBibleReadWindow::saveChapterPlain);

    m_actions.save.chapterAsHTML =
            &initAddAction(QStringLiteral("saveChapterAsHTML"),
                           this,
                           &CBibleReadWindow::saveChapterHTML);

    m_actions.print.reference =
            &initAddAction(QStringLiteral("printReferenceWithText"),
                           this,
                           &CBibleReadWindow::printAnchorWithText);

    m_actions.print.chapter =
            &initAddAction(QStringLiteral("printChapter"),
                           this,
                           &CBibleReadWindow::printAll);

    m_actions.send.selectedText =
            &initAddAction(QStringLiteral("sendBibleSelectedTextToEditor"),
                           this,
                           &CBibleReadWindow::sendSelectedTextToEditor);
    m_actions.send.selectedText->setEnabled(
                displayWidget()->qmlInterface()->selection().has_value());

    m_actions.send.referenceAndText =
            &initAddAction(QStringLiteral("sendReferenceWithTextToEditor"),
                           this,
                           &CBibleReadWindow::sendReferenceToEditor);

    m_actions.send.chapter =
            &initAddAction(QStringLiteral("sendChapterToEditor"),
                           this,
                           &CBibleReadWindow::sendChapterToEditor);

    #ifdef BUILD_TEXT_TO_SPEECH
    m_actions.speakSelectedText = &ac->action("speakSelectedText");
    #endif

    BT_CONNECT(displayWidget(), &BtModelViewReadDisplay::activeAnchorChanged,
               this,
               [this](QString const & newActiveAnchor) {
                bool const hasActiveAnchor = !newActiveAnchor.isEmpty();
                m_actions.copy.referenceTextOnly->setEnabled(
                            hasActiveAnchor);
                m_actions.copy.referenceAndText->setEnabled(
                            hasActiveAnchor);
                m_actions.save.referenceAndText->setEnabled(
                            hasActiveAnchor);
               });

    BT_CONNECT(displayWidget()->qmlInterface(),
               &BtQmlInterface::selectionChanged,
               this,
               [this](
                 std::optional<BtQmlInterface::Selection> const & newSelection)
               { m_actions.send.selectedText->setEnabled(newSelection.has_value()); });

    ac->readShortcuts(QStringLiteral("Bible shortcuts"));
}

QMenu * CBibleReadWindow::newDisplayWidgetPopupMenu() {
    auto * const popupMenu = new QMenu(this);
    popupMenu->addAction(m_actions.findText);
    popupMenu->addAction(m_actions.findStrongs);
    popupMenu->addSeparator();

    m_actions.copyMenu = new QMenu(tr("Copy"), popupMenu);

    m_actions.copyMenu->addSeparator();

    m_actions.copyMenu->addAction(m_actions.copy.selectedText);
    m_actions.copyMenu->addAction(m_actions.copy.byReferences);
    m_actions.copyMenu->addAction(m_actions.copy.referenceOnly);
    m_actions.copyMenu->addAction(m_actions.copy.referenceTextOnly);
    m_actions.copyMenu->addAction(m_actions.copy.referenceAndText);
    m_actions.copyMenu->addAction(m_actions.copy.chapter);


    popupMenu->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(tr("Save..."), popupMenu);
    m_actions.saveMenu->addAction(m_actions.save.referenceAndText);
    m_actions.saveMenu->addAction(m_actions.save.chapterAsPlain);
    m_actions.saveMenu->addAction(m_actions.save.chapterAsHTML);
    popupMenu->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(tr("Print..."), popupMenu);
    m_actions.printMenu->addAction(m_actions.print.reference);
    m_actions.printMenu->addAction(m_actions.print.chapter);
    popupMenu->addMenu(m_actions.printMenu);

    m_actions.sendMenu = new QMenu(tr("Send to Text Editor..."), popupMenu);
    m_actions.sendMenu->addAction(m_actions.send.selectedText);
    m_actions.sendMenu->addAction(m_actions.send.referenceAndText);
    m_actions.sendMenu->addAction(m_actions.send.chapter);
    popupMenu->addMenu(m_actions.sendMenu);

    #ifdef BUILD_TEXT_TO_SPEECH
    popupMenu->addAction(m_actions.speakSelectedText);
    #endif

    return addDebugActions(popupMenu);
}

/** Moves to the next book. */
void CBibleReadWindow::nextBook() {
    if (verseKey()->next(CSwordVerseKey::UseBook)) {
        keyChooser()->setKey(swordKey());
    }
}

/** Moves one book behind. */
void CBibleReadWindow::previousBook() {
    if (verseKey()->previous(CSwordVerseKey::UseBook)) {
        keyChooser()->setKey(swordKey());
    }
}

/** Moves to the next book. */
void CBibleReadWindow::nextChapter() {
    if (verseKey()->next(CSwordVerseKey::UseChapter)) {
        keyChooser()->setKey(swordKey());
    }
}

/** Moves one book behind. */
void CBibleReadWindow::previousChapter() {
    if (verseKey()->previous(CSwordVerseKey::UseChapter)) {
        keyChooser()->setKey(swordKey());
    }
}

/** Moves to the next book. */
void CBibleReadWindow::nextVerse() {
    if (verseKey()->next(CSwordVerseKey::UseVerse)) {
        keyChooser()->setKey(swordKey());
    }
}

/** Moves one book behind. */
void CBibleReadWindow::previousVerse() {
    if (verseKey()->previous(CSwordVerseKey::UseVerse)) {
        keyChooser()->setKey(swordKey());
    }
}

/** wrapper around key() to return the right type of key. */
CSwordVerseKey* CBibleReadWindow::verseKey() {
    CSwordVerseKey * const k =
            dynamic_cast<CSwordVerseKey*>(CDisplayWindow::swordKey());
    BT_ASSERT(k);
    return k;
}

/** Copies the current chapter into the clipboard. */
void CBibleReadWindow::copyDisplayedText() {
    CSwordVerseKey dummy(*verseKey());
    dummy.setVerse(1);

    CSwordVerseKey vk(*verseKey());
    vk.setLowerBound(dummy);

    auto const bible =
            dynamic_cast<CSwordBibleModuleInfo const *>(firstModule());
    dummy.setVerse(bible->verseCount(dummy.bookName(), dummy.chapter()));
    vk.setUpperBound(dummy);

    CExportManager mgr(false, tr("Copying"), filterOptions(), displayOptions());
    mgr.copyKey(&vk, CExportManager::Text, true);
}

/** Saves the chapter as valid HTML page. */
void CBibleReadWindow::saveChapterHTML() {
    saveChapter(CExportManager::HTML);
}

/** Saves the chapter as valid HTML page. */
void CBibleReadWindow::saveChapterPlain() {
    saveChapter(CExportManager::Text);
}

void CBibleReadWindow::sendSelectedTextToEditor() {
    auto const text =
            bibleTextForEditor(displayWidget()->qmlInterface()->getSelectedText());
    if (text.isEmpty())
        return;

    if (!BtTextEditorWindow::appendToActiveDocument(
            text,
            tr("Selected Bible text from %1").arg(firstModule()->name())))
    {
        QMessageBox::information(
                    this,
                    tr("Text Editor"),
                    tr("Open the Text Editor first, then try sending text again."));
    }
}

void CBibleReadWindow::sendReferenceToEditor() {
    std::unique_ptr<CSwordKey> key(getMouseClickedKey());
    if (!key)
        return;

    auto const text =
            QStringLiteral("%1\n(%2, %3)")
            .arg(bibleTextForEditor(key->strippedText()),
                 key->key(),
                 key->module()->name());
    if (text.isEmpty())
        return;

    if (!BtTextEditorWindow::appendToActiveDocument(
            text,
            tr("Bible reference from %1").arg(firstModule()->name())))
    {
        QMessageBox::information(
                    this,
                    tr("Text Editor"),
                    tr("Open the Text Editor first, then try sending text again."));
    }
}

void CBibleReadWindow::sendChapterToEditor() {
    auto * const clipboard = QGuiApplication::clipboard();
    auto const previousText = clipboard->text();
    copyDisplayedText();
    auto const text = bibleTextForEditor(clipboard->text());
    clipboard->setText(previousText);

    if (text.isEmpty())
        return;

    if (!BtTextEditorWindow::appendToActiveDocument(
            text,
            tr("Bible chapter from %1").arg(firstModule()->name())))
    {
        QMessageBox::information(
                    this,
                    tr("Text Editor"),
                    tr("Open the Text Editor first, then try sending text again."));
    }
}

void CBibleReadWindow::saveChapter(CExportManager::Format const format) {
    CSwordKey * key = getMouseClickedKey();
    if (! key)
        return;
    CSwordVerseKey* verseKey = dynamic_cast<CSwordVerseKey*>(key);
    if (! verseKey)
        return;

    CSwordVerseKey vk(*verseKey);
    CSwordVerseKey dummy(*verseKey);
    dummy.setVerse(1);
    vk.setLowerBound(dummy);

    auto const constMods = constModules();

    auto const bible =
            dynamic_cast<CSwordBibleModuleInfo const *>(constMods.first());
    /// \bug Check if bible is nullptr!
    dummy.setVerse(bible->verseCount(dummy.book(), dummy.chapter()));
    vk.setUpperBound(dummy);

    CExportManager mgr(true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(&vk, format, true, constMods);
}

void CBibleReadWindow::reload() {
    CDisplayWindow::reload();

    if (m_modules.isEmpty()) {
        close();
        return;
    }

    //refresh the book lists
    verseKey()->setLocale(
                CSwordBackend::instance().booknameLanguage().toLatin1());
    keyChooser()->refreshContent();

    actionCollection()->readShortcuts(QStringLiteral("Bible shortcuts"));
}

void CBibleReadWindow::lookupSwordKey( CSwordKey* newKey ) {
    CDisplayWindow::lookupSwordKey(newKey);
    syncWindows();
}

void CBibleReadWindow::syncWindows() {
    for (auto * const subWindow : mdi()->subWindowList()) {
        CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
        if (w && w->syncAllowed()) {
            w->lookupKey(swordKey()->key());
        }
    }
}
