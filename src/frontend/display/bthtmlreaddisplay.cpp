/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/bthtmlreaddisplay.h"

#include <QSharedPointer>
#include <QMenu>
#include <QString>
#include "backend/keys/cswordkey.h"
#include "backend/managers/referencemanager.h"
#include "bibletime.h"
#include "frontend/cdragdrop.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/bthtmljsobject.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cdisplaywindowfactory.h"
#include "frontend/displaywindow/creadwindow.h"
#include "util/directory.h"


using namespace InfoDisplay;

static QString javascript; // Initialized from file bthtml.js

BtHtmlReadDisplay::BtHtmlReadDisplay(CReadWindow* readWindow, QWidget* parentWidget)
        : QWebPage(parentWidget), CReadDisplay(readWindow), m_magTimerId(0), m_view(0), m_jsObject(0)

{
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    m_view = new BtHtmlReadDisplayView(this, parentWidget ? parentWidget : readWindow, readWindow);
    m_view->setAcceptDrops(true);
    m_view->setPage(this);
    setParent(m_view);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_view->setHtml("");
    initJavascript();
    bool ok = connect(this, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool)));
    Q_ASSERT(ok);
}

BtHtmlReadDisplay::~BtHtmlReadDisplay() {
    setView(0);
}

// Read javascript into memory once and create the c++ javascript object
void BtHtmlReadDisplay::initJavascript() {
    namespace DU = util::directory;

    // read bthtml.js javascript file once
    if (javascript.isEmpty()) {
        QString jsFile = DU::getJavascriptDir().canonicalPath() + "/bthtml.js";
        QFile file(jsFile);
        if (file.open(QFile::ReadOnly)) {
            while (!file.atEnd()) {
                QByteArray line = file.readLine();
                javascript = javascript + line;
            }
            file.close();
        }
    }

    // Setup BtHtmlJsObject which will be called from javascript
    m_jsObject = new BtHtmlJsObject(this);
    m_jsObject->setObjectName("btHtmlJsObject");
}

// When the QWebFrame is cleared, this function is called to install the
// javascript object (BtHtmlJsObject class) into the Javascript model
void BtHtmlReadDisplay::loadJSObject() {
    // Starting with Qt 4.7.4 with QtWebKit 2.2 stronger security checking occurs.
    // The BtHtmlJsObject that is associated with a given load of a page is rejected
    // as causing a cross site security problem when a new page is loaded. Deleting
    // the object and creating it new for each page loaded allows the object to access
    // javascript variables without this security issue.
    if (m_jsObject != 0)
        delete m_jsObject;
    m_jsObject = new BtHtmlJsObject(this);
    m_jsObject->setObjectName("btHtmlJsObject");

    mainFrame()->addToJavaScriptWindowObject(m_jsObject->objectName(), m_jsObject);
}

const QString BtHtmlReadDisplay::text( const CDisplay::TextType format, const CDisplay::TextPart part) {
    switch (part) {
        case Document: {
            if (format == HTMLText) {
                return mainFrame()->toHtml();
            }
            else {
                CDisplayWindow* window = parentWindow();
                CSwordKey* const key = window->key();
                const CSwordModuleInfo *module = key->module();
                //This is never used for Bibles, so it is not implemented for
                //them.  If it should be, see CReadDisplay::print() for example
                //code.
                Q_ASSERT(module->type() == CSwordModuleInfo::Lexicon ||
                         module->type() == CSwordModuleInfo::Commentary ||
                         module->type() == CSwordModuleInfo::GenericBook);
                if (module->type() == CSwordModuleInfo::Lexicon ||
                        module->type() == CSwordModuleInfo::Commentary ||
                        module->type() == CSwordModuleInfo::GenericBook) {
                    /// \todo This is a BAD HACK, we have to fnd a better solution to manage the settings now
                    FilterOptions filterOptions;
                    filterOptions.footnotes = false;
                    filterOptions.strongNumbers = false;
                    filterOptions.morphTags = false;
                    filterOptions.lemmas = false;
                    filterOptions.scriptureReferences = false;
                    filterOptions.textualVariants = false;

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
                QSharedPointer<CSwordKey> key( CSwordKey::createInstance(module) );
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
                QSharedPointer<CSwordKey> key( CSwordKey::createInstance(module) );
                key->setKey(keyName);

                /// \todo This is a BAD HACK, we have to fnd a better solution to manage the settings now
                FilterOptions filterOptions;
                filterOptions.footnotes = false;
                filterOptions.strongNumbers = false;
                filterOptions.morphTags = false;
                filterOptions.lemmas = false;
                filterOptions.scriptureReferences = false;
                filterOptions.textualVariants = false;

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

// Puts html text and javascript into QWebView
void BtHtmlReadDisplay::setText( const QString& newText ) {

    QString jsText = newText;

    jsText.replace(
        QString("</body>"),
        QString("<script  type=\"text/javascript\">").append(javascript).append("</script></body>")
    );

    // Disconnect any previous connections and connect to slot that loads the javascript object
    QWebFrame* frame = mainFrame();
    disconnect(frame, SIGNAL(javaScriptWindowObjectCleared()), 0, 0);
    bool ok = connect(frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(loadJSObject()));
    Q_ASSERT(ok);

    // Send text to the html viewer
    m_view->setHtml(jsText);

    this->currentSource = jsText;
}

QString BtHtmlReadDisplay::getCurrentSource( ) {
    return this->currentSource;
}

// See if any text is selected
bool BtHtmlReadDisplay::hasSelection() {
    if (selectedText().isEmpty())
        return false;
    return true;
}

// Reimplementation
// Returns the BtHtmlReadDisplayView object
QWidget* BtHtmlReadDisplay::view() {
    return m_view;
}

// Select all text in the viewer
void BtHtmlReadDisplay::selectAll() {
    m_view->triggerPageAction( QWebPage::SelectAll, true );
}

// Scroll QWebView to the correct location as specified by the anchor
void BtHtmlReadDisplay::moveToAnchor( const QString& anchor ) {
#if QT_VERSION >= 0x040700
    mainFrame()->scrollToAnchor(anchor);
#else
    slotGoToAnchor(anchor);
#endif
}

// Scroll the QWebView to the correct location specified by anchor
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
        : QWebView(parent), m_display(displayWidget), m_readWindow(readWindow) {
}

BtHtmlReadDisplayView::~BtHtmlReadDisplayView() {
    setPage(0);
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
        if (mimedata != 0) {
            const BTMimeData* btmimedata = qobject_cast<const BTMimeData*>(mimedata);
            if (btmimedata != 0) {
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

// Reimplementation from QWebView
void BtHtmlReadDisplayView::dragEnterEvent( QDragEnterEvent* e ) {
    if ( ! e->mimeData()->hasFormat("BibleTime/Bookmark"))
        return;

    const QMimeData* mimedata = e->mimeData();
    if (mimedata == 0)
        return;

    const BTMimeData* btmimedata = qobject_cast<const BTMimeData*>(mimedata);
    if (btmimedata == 0)
        return;

    BookmarkItem item = (qobject_cast<const BTMimeData*>(e->mimeData()))->bookmark();
    QString moduleName = item.module();
    CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(moduleName);
    Q_ASSERT(m);
    if (m == 0)
        return;

    CSwordModuleInfo::ModuleType bookmarkType = m->type();
    CSwordModuleInfo::ModuleType windowType = CDisplayWindowFactory::getModuleType(m_readWindow);

    // Is bible reference bookmark compatible with the window type?
    if ((bookmarkType == CSwordModuleInfo::Bible ||
        bookmarkType == CSwordModuleInfo::Commentary)) {
            if (windowType == CSwordModuleInfo::Bible ||
                windowType == CSwordModuleInfo::Commentary)
                e->acceptProposedAction();
            return;
    }

    // Is reference type compatible with window type
    if (bookmarkType == windowType) {
            e->acceptProposedAction();
            return;
    }

    return;
}

// Reimplementation from QWebView
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
