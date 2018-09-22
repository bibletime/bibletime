/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/display/bthtmlreaddisplay.h"

#include <memory>
#include <QMenu>
#include <QDebug>
#include <QString>
#include <QTimer>
#include "backend/keys/cswordkey.h"
#include "backend/managers/referencemanager.h"
#include "frontend/bibletime.h"
#include "frontend/BtMimeData.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/bthtmljsobject.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/creadwindow.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/directory.h"

#ifdef USEWEBENGINE
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#endif

using namespace InfoDisplay;

#ifdef USEWEBENGINE
static QString javascriptFile = "btwebengine.js";
#else
static QString javascriptFile = "bthtml.js";
#endif

static QString s_javascript; // Initialized from javascript file

// This is s work around for Qt bug 51565
// It is also documented in BibleTime bug #53
static void clearChildFocusWidget(QWidget * widget) {
    QWidget * childFocusedWidget = widget->focusWidget();
    if (childFocusedWidget)
        childFocusedWidget->clearFocus();
}

BtHtmlReadDisplay::BtHtmlReadDisplay(CReadWindow* readWindow, QWidget* parentWidget)
    : BtWebEnginePage(parentWidget), CReadDisplay(readWindow), m_magTimerId(0), m_view(nullptr), m_jsObject(nullptr)

{
    m_view = new BtHtmlReadDisplayView(this, parentWidget ? parentWidget : readWindow, readWindow);
    m_view->setAcceptDrops(true);
    m_view->setPage(this);
    setParent(m_view);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    loadJSObject();
    loadScripts();
    m_view->setHtml("");    // This sets focus on a child widget
    clearChildFocusWidget(m_view);

    BT_CONNECT(this, SIGNAL(loadFinished(bool)),
               this, SLOT(slotLoadFinished(bool)));
}

BtHtmlReadDisplay::~BtHtmlReadDisplay() {
    setView(nullptr);
}

void BtHtmlReadDisplay::loadScripts() {
    namespace DU = util::directory;

    QString jScript;
#ifdef USEWEBENGINE
     jScript = readJavascript(":/qtwebchannel/qwebchannel.js");
#endif
    QString jsFile = DU::getJavascriptDir().canonicalPath() + "/" + javascriptFile;
    jScript += readJavascript(jsFile);

#ifdef USEWEBENGINE
    // Directly load javascript into QWebEngine
    QWebEngineScript script;
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setWorldId(QWebEngineScript::MainWorld);
    script.setSourceCode(jScript);
    script.setName("script1");
    scripts().insert(script);
#else
    // Save javascript and load each time the document is loaded (setHtml)
    s_javascript = jScript;
#endif
}

QString BtHtmlReadDisplay::readJavascript(const QString& jsFileName) {
    QString javascript;
    QFile file(jsFileName);
    if (file.open(QFile::ReadOnly)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            javascript = javascript + line;
        }
        file.close();
    } else {
        qWarning() << objectName() << ": Missing " +jsFileName;
    }
    return javascript;
}

// When the QWebFrame is cleared, this function is called to install the
// javascript object (BtHtmlJsObject class) into the Javascript model.
// It is called only once with QWebEngine.
void BtHtmlReadDisplay::loadJSObject() {
    // Starting with Qt 4.7.4 with QtWebKit 2.2 stronger security checking occurs.
    // The BtHtmlJsObject that is associated with a given load of a page is rejected
    // as causing a cross site security problem when a new page is loaded. Deleting
    // the object and creating it new for each page loaded allows the object to access
    // javascript variables without this security issue.
    if (m_jsObject != nullptr)
        delete m_jsObject;
    m_jsObject = new BtHtmlJsObject(this);
    addJavaScriptObject("btHtmlJsObject", m_jsObject);
}

const QString BtHtmlReadDisplay::text( const CDisplay::TextType format, const CDisplay::TextPart part) {
    switch (part) {
        case Document: {
            if (format == HTMLText) {
                return getCurrentSource();
            }
            else {
                CDisplayWindow* window = parentWindow();
                CSwordKey* const key = window->key();
                const CSwordModuleInfo *module = key->module();
                //This is never used for Bibles, so it is not implemented for
                //them.  If it should be, see CReadDisplay::print() for example
                //code.
                BT_ASSERT(module->type() == CSwordModuleInfo::Lexicon ||
                         module->type() == CSwordModuleInfo::Commentary ||
                         module->type() == CSwordModuleInfo::GenericBook);
                if (module->type() == CSwordModuleInfo::Lexicon ||
                        module->type() == CSwordModuleInfo::Commentary ||
                        module->type() == CSwordModuleInfo::GenericBook) {

                    FilterOptions filterOptions;
                    CSwordBackend::instance()->setFilterOptions(filterOptions);

                    return QString(key->strippedText()).append("\n(")
                           .append(key->key())
                           .append(", ")
                           .append(key->module()->name())
                           .append(")");
                }
            }
        }

        case SelectedText: {
            if (!hasSelection()) {
                return QString::null;
            }
            else if (format == HTMLText) {
                //    \todo It does not appear this is ever called
            }
            else { //plain text requested
                return selectedText();
            }
        }

        case AnchorOnly: {
            QString moduleName;
            QString keyName;
            ReferenceManager::Type type;
            ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

            return keyName;
        }

        case AnchorTextOnly: {
            QString moduleName;
            QString keyName;
            ReferenceManager::Type type;
            ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

            if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
                std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
                key->setKey(keyName);

                return key->strippedText();
            }
            return QString::null;
        }

        case AnchorWithText: {
            QString moduleName;
            QString keyName;
            ReferenceManager::Type type;
            ReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

            if (CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName)) {
                std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
                key->setKey(keyName);

                FilterOptions filterOptions;
                CSwordBackend::instance()->setFilterOptions(filterOptions);

                return QString(key->strippedText()).append("\n(")
                       .append(key->key())
                       .append(", ")
                       .append(key->module()->name())
                       .append(")");
                /*    ("%1\n(%2, %3)")
                    .arg()
                    .arg(key->key())
                    .arg(key->module()->name());*/
            }
            return QString::null;
        }
        default:
            return QString::null;
    }
    return QString::null;
}

// Puts html text and javascript into BtWebEngineView
void BtHtmlReadDisplay::setText( const QString& newText ) {
    QString jsText = newText;

#ifndef USEWEBENGINE
    // Inject javascript into the document
    jsText.replace(
        QString("</body>"),
        QString("<script  type=\"text/javascript\">").append(s_javascript).append("</script></body>")
    );

    // Disconnect any previous connections and connect to slot that loads the javascript object
    QWebFrame* frame = mainFrame();
    disconnect(frame, SIGNAL(javaScriptWindowObjectCleared()), nullptr, nullptr);
    BT_CONNECT(frame, SIGNAL(javaScriptWindowObjectCleared()),
               this, SLOT(loadJSObject()));
#endif

    // Send text to the html viewer
    m_view->setHtml(jsText, QUrl("file://"));

    this->currentSource = jsText;
}

QString BtHtmlReadDisplay::getCurrentSource( ) {
    return this->currentSource;
}

// See if any text is selected
bool BtHtmlReadDisplay::hasSelection() const {
    return !selectedText().isEmpty();
}

// Reimplementation
// Returns the BtHtmlReadDisplayView object
QWidget* BtHtmlReadDisplay::view() {
    return m_view;
}

// Select all text in the viewer
void BtHtmlReadDisplay::selectAll() {
    BtWebEnginePage::selectAll();
}

// Scroll BtWebEngineView to the correct location as specified by the anchor
void BtHtmlReadDisplay::moveToAnchor( const QString& anchor ) {
#ifdef USEWEBENGINE
    // Rendering in QWebEngine is asynchronous, must delay before scroll to anchor
    // TODO - find a better solution
    m_currentAnchorCache = anchor;
    QTimer::singleShot(180, this, SLOT(slotDelayedMoveToAnchor()));
#else
    mainFrame()->scrollToAnchor(anchor);
#endif
}

void BtHtmlReadDisplay::slotDelayedMoveToAnchor() {
    m_jsObject->moveToAnchor(m_currentAnchorCache);
}

// Scroll the BtWebEngineView to the correct location specified by anchor
void BtHtmlReadDisplay::slotGoToAnchor(const QString& anchor) {
    m_jsObject->moveToAnchor(anchor);
}

// Save the Lemma (Strongs number) attribute
void BtHtmlReadDisplay::setLemma(const QString& lemma) {
    m_nodeInfo = lemma;
}

// Open the Find text dialog
void BtHtmlReadDisplay::openFindTextDialog() {
    BibleTime* bibleTime = parentWindow()->mdi()->bibleTimeWindow();
    bibleTime->openFindWidget();
}

// Send "completed" signal when the text is finished loading into the viewer
void BtHtmlReadDisplay::slotLoadFinished(bool) {
    emit completed();
}

// For debugging javascript - set breakpoint in this function to catch javascript error messages
#ifdef DEBUG_JS
void BtHtmlReadDisplay::javaScriptConsoleMessage (const QString& message, int lineNumber, const QString& sourceID ) {
}
#endif




// ----------------- BtHtmlReadDisplayView -------------------------------------

BtHtmlReadDisplayView::BtHtmlReadDisplayView(BtHtmlReadDisplay* displayWidget, QWidget* parent, CReadWindow* readWindow)
    : BtWebEngineView(parent), m_display(displayWidget), m_readWindow(readWindow) {
}

BtHtmlReadDisplayView::~BtHtmlReadDisplayView() {
    setPage(nullptr);
}

// Create the right mouse context menus
void BtHtmlReadDisplayView::contextMenuEvent(QContextMenuEvent* event) {
    if (QMenu* popup = m_display->installedPopup()) {
        popup->exec(event->globalPos());
    }
}

// Reimplementation from QWidget
void BtHtmlReadDisplayView::dropEvent( QDropEvent* e ) {
    if (e->mimeData()->hasFormat("BibleTime/Bookmark")) {
        //see docs for BTMimeData and QMimeData
        const QMimeData* mimedata = e->mimeData();
        if (mimedata != nullptr) {
            const BTMimeData* btmimedata = qobject_cast<const BTMimeData*>(mimedata);
            if (btmimedata != nullptr) {
                BookmarkItem item = (qobject_cast<const BTMimeData*>(e->mimeData()))->bookmark();
                m_display->connectionsProxy()->emitReferenceDropped(item.key());
                e->acceptProposedAction();
                return;
            }
        }
    };
    //don't accept the action!
//    e->ignore();
}

// Reimplementation from BtWebEngineView
void BtHtmlReadDisplayView::dragEnterEvent( QDragEnterEvent* e ) {
    if ( ! e->mimeData()->hasFormat("BibleTime/Bookmark"))
        return;

    const QMimeData* mimedata = e->mimeData();
    if (mimedata == nullptr)
        return;

    const BTMimeData* btmimedata = qobject_cast<const BTMimeData*>(mimedata);
    if (btmimedata == nullptr)
        return;

    BookmarkItem item = (qobject_cast<const BTMimeData*>(e->mimeData()))->bookmark();
    QString moduleName = item.module();
    CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(moduleName);
    BT_ASSERT(m);

    CSwordModuleInfo::ModuleType bookmarkType = m->type();
    CSwordModuleInfo::ModuleType windowType = CSwordModuleInfo::Unknown;
    if (m_readWindow)
        windowType = m_readWindow->moduleType();

    // Is bible reference bookmark compatible with the window type?
    if ((bookmarkType == CSwordModuleInfo::Bible ||
        bookmarkType == CSwordModuleInfo::Commentary)) {
            if (windowType == CSwordModuleInfo::Bible ||
                windowType == CSwordModuleInfo::Commentary)
                e->acceptProposedAction();
#ifdef USEWEBENGINE
            BtWebEngineView::dragEnterEvent(e);  // Fix crash, QTBUG-54896, BT bug #70
#endif
            return;
    }

    // Is reference type compatible with window type
    if (bookmarkType == windowType) {
            e->acceptProposedAction();
#ifdef USEWEBENGINE
            BtWebEngineView::dragEnterEvent(e);  // Fix crash, QTBUG-54896, BT bug #70
#endif
            return;
    }

    return;
}

// Reimplementation from BtWebEngineView
void BtHtmlReadDisplayView::dragMoveEvent( QDragMoveEvent* e ) {
    if (e->mimeData()->hasFormat("BibleTime/Bookmark")) {
        e->acceptProposedAction();
        return;
    }
    //don't accept the action!
    e->ignore();
}

bool BtHtmlReadDisplayView::event(QEvent* e) {
    // If the mouse leaves the widget clear the previous attribute
    // in bthtmljsobject. This cancels any time out event that
    // is in progress.
    if ( e->type() == QEvent::Leave )
        m_display->m_jsObject->clearPrevAttribute();
    return QWidget::event(e);
}
