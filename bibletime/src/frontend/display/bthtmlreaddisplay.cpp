/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bthtmlreaddisplay.h"
#include "bthtmlreaddisplay.moc"
#include "bthtmljsobject.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/cdragdrop.h"
#include "frontend/cinfodisplay.h"
#include "backend/managers/creferencemanager.h"
#include "backend/keys/cswordkey.h"
#include "backend/config/cbtconfig.h"
#include "util/ctoolclass.h"
#include "util/cpointers.h"
#include "util/directoryutil.h"
#include <QString>

using namespace InfoDisplay;

const static QString body = "</body>";
const static QString jsBegin = "<script  type=\"text/javascript\">";
const static QString jsEnd = "</script>";
static QString javascript; // Initialized from file bthtml.js


BtHtmlReadDisplay::BtHtmlReadDisplay(CReadWindow* readWindow, QWidget* parentWidget)
	: QWebPage(parentWidget),CReadDisplay(readWindow), m_magTimerId(0), m_view(0), m_jsObject(0)

{
	settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
	m_view = new BtHtmlReadDisplayView(this, parentWidget ? parentWidget : readWindow);
	m_view->setPage(this);
	m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_view->setHtml("");
	initJavascript();
	bool ok = connect(this, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool)));
	Q_ASSERT(ok);
}

BtHtmlReadDisplay::~BtHtmlReadDisplay() {}

void BtHtmlReadDisplay::initJavascript()
{
	// read bthtml.js javascript file once
	if (javascript.size() == 0)
	{
		QString jsFile = util::filesystem::DirectoryUtil::getJavascriptDir().canonicalPath() + "/bthtml.js";
		QFile file(jsFile);	
 		if (file.open(QFile::ReadOnly)) 
		{
			while (!file.atEnd()) 
			{
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

// When the QWebFrame is cleared, this function is called to install the BtHtmlJsObject
// into the Javascript model
void BtHtmlReadDisplay::loadJSObject()
{
	mainFrame()->addToJavaScriptWindowObject(m_jsObject->objectName(), m_jsObject);
}

const QString BtHtmlReadDisplay::text( const CDisplay::TextType /*format*/, const CDisplay::TextPart /*part*/) 
{
// TODO
#if 0
	switch (part) {
		case Document: {
			if (format == HTMLText) {
				return document().toHTML();
			}
			else {
				//return htmlDocument().body().innerText().string().toLatin1();
				CDisplayWindow* window = parentWindow();
				CSwordKey* const key = window->key();
				CSwordModuleInfo* module = key->module();
				//This is never used for Bibles, so it is not implemented for
				//them.  If it should be, see CReadDisplay::print() for example
				//code.
				Q_ASSERT(module->type() == CSwordModuleInfo::Lexicon ||
						module->type() == CSwordModuleInfo::Commentary ||
						module->type() == CSwordModuleInfo::GenericBook);
				if (module->type() == CSwordModuleInfo::Lexicon ||
						module->type() == CSwordModuleInfo::Commentary ||
						module->type() == CSwordModuleInfo::GenericBook){
					//TODO: This is a BAD HACK, we have to fnd a better solution to manage the settings now
					CSwordBackend::FilterOptions filterOptions;
					filterOptions.footnotes = false;
					filterOptions.strongNumbers = false;
					filterOptions.morphTags = false;
					filterOptions.lemmas = false;
					filterOptions.scriptureReferences = false;
					filterOptions.textualVariants = false;

					CPointers::backend()->setFilterOptions(filterOptions);

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
				DOM::Range range = selection();
				return range.toHTML().string();
			}
			else { //plain text requested
				return selectedText();
			}
		}

		case AnchorOnly: {
			QString moduleName;
			QString keyName;
			CReferenceManager::Type type;
			CReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

			return keyName;
		}

		case AnchorTextOnly: {
			QString moduleName;
			QString keyName;
			CReferenceManager::Type type;
			CReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

			if (CSwordModuleInfo* module = backend()->findModuleByName(moduleName)) {
				boost::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
				key->key( keyName );

				return key->strippedText();
			}
			return QString::null;
		}

		case AnchorWithText: {
			QString moduleName;
			QString keyName;
			CReferenceManager::Type type;
			CReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

			if (CSwordModuleInfo* module = backend()->findModuleByName(moduleName)) {
				boost::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
				key->key( keyName );

				//TODO: This is a BAD HACK, we have to fnd a better solution to manage the settings now
				CSwordBackend::FilterOptions filterOptions;
				filterOptions.footnotes = false;
				filterOptions.strongNumbers = false;
				filterOptions.morphTags = false;
				filterOptions.lemmas = false;
				filterOptions.scriptureReferences = false;
				filterOptions.textualVariants = false;

				CPointers::backend()->setFilterOptions(filterOptions);

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
#endif
}

// Puts html text and javascript into QWebView
void BtHtmlReadDisplay::setText( const QString& newText ) 
{
	QString jsText = newText;
	jsText.replace(body,jsBegin+javascript+jsEnd+body);

	QWebFrame* frame = mainFrame();
	m_view->setHtml(jsText);
	frame = mainFrame();
	bool ok = connect(frame,SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(loadJSObject()));
	Q_ASSERT(ok);
}

bool BtHtmlReadDisplay::hasSelection() 
{
// TODO
//	return KHTMLPart::hasSelection();
	return false;
}

// Reimplementation
QWidget* BtHtmlReadDisplay::view()
{
	return m_view;
}

void BtHtmlReadDisplay::selectAll() 
{
// TODO
//	KHTMLPart::selectAll();
}

// Scroll QWebView to the correct location as specified by the anchor
void BtHtmlReadDisplay::moveToAnchor( const QString& anchor ) 
{
	slotGoToAnchor(anchor);
}

// Scroll the QWebView to the correct location specified by anchor
void BtHtmlReadDisplay::slotGoToAnchor(const QString& anchor) 
{
	m_jsObject->moveToAnchor(anchor);
}

void BtHtmlReadDisplay::openFindTextDialog() {
//	findText();
}

void BtHtmlReadDisplay::javaScriptConsoleMessage (const QString& /*message*/, int /*lineNumber*/, const QString& /*sourceID*/ )
{
}

void BtHtmlReadDisplay::slotLoadFinished(bool)
{
	emit completed();
}




// ----------------- BtHtmlReadDisplayView -------------------------------------

BtHtmlReadDisplayView::BtHtmlReadDisplayView(BtHtmlReadDisplay* displayWidget, QWidget* parent) : QWebView(parent), m_display(displayWidget) 
{
}

void BtHtmlReadDisplayView::mousePressEvent(QMouseEvent * event)
{
	QWebView::mousePressEvent(event);
}

void BtHtmlReadDisplayView::contextMenuEvent(QContextMenuEvent* event)
{
	QWebView::contextMenuEvent(event);
}

#if 0
/** Opens the popupmenu at the given position. */
void BtHtmlReadDisplayView::popupMenu( const QString& url, const QPoint& pos) 
{
	if (!url.isEmpty()) 
	{
		m_display->setActiveAnchor(url);
	}
	if (QMenu* popup = m_display->installedPopup()) 
	{
		popup->exec(pos);
	}
}

/** Reimplementation from QScrollArea. Sets the right slots */
	bool BtHtmlReadDisplayView::event(QEvent* e) 
{
	if (e->type() == QEvent::Polish) 
	{
		connect( this, SIGNAL(popupMenu(const QString&, const QPoint&)), // TODO
			this, SLOT(popupMenu(const QString&, const QPoint&)));
	}
	return QWebView::event(e);
	return false;
}
#endif

// Reimplementation from QWebView
void BtHtmlReadDisplayView::dropEvent( QDropEvent* e ) 
{
	if (e->mimeData()->hasFormat("BibleTime/Bookmark")) 
	{
		//see docs for BTMimeData and QMimeData
		BookmarkItem item = (qobject_cast<const BTMimeData*>(e->mimeData()))->bookmark();
		e->acceptProposedAction();
		m_display->connectionsProxy()->emitReferenceDropped(item.key());
		return;
	};
	//don't accept the action!
	e->ignore();
}

// Reimplementation from QWebView
void BtHtmlReadDisplayView::dragEnterEvent( QDragEnterEvent* e ) 
{
	if (e->mimeData()->hasFormat("BibleTime/Bookmark")) 
	{
		e->acceptProposedAction();
		return;
	}
	//don't accept the action!
	e->ignore();
}


