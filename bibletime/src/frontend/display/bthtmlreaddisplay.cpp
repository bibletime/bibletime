/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bthtmlreaddisplay.h"
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
#include <boost/scoped_ptr.hpp>
#include <QString>
#include <QMenu>

using namespace InfoDisplay;

void showBtHtmlFindText(CMDIArea*);

static const QString body = "</body>";
static const QString jsBegin = "<script  type=\"text/javascript\">";
static const QString jsEnd = "</script>";
static QString javascript; // Initialized from file bthtml.js


BtHtmlReadDisplay::BtHtmlReadDisplay(CReadWindow* readWindow, QWidget* parentWidget)
	: QWebPage(parentWidget),CReadDisplay(readWindow), m_magTimerId(0), m_view(0), m_jsObject(0)

{
	settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
	m_view = new BtHtmlReadDisplayView(this, parentWidget ? parentWidget : readWindow);
	m_view->setAcceptDrops(true);
	m_view->setPage(this);
	setParent(m_view);
	m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_view->setHtml("");
	initJavascript();
	bool ok = connect(this, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinished(bool)));
	Q_ASSERT(ok);
}

BtHtmlReadDisplay::~BtHtmlReadDisplay() 
{
	setView(0);
}

// Read javascript into memory once and create the c++ javascript object
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

// When the QWebFrame is cleared, this function is called to install the 
// javascript object (BtHtmlJsObject class) into the Javascript model
void BtHtmlReadDisplay::loadJSObject()
{
	mainFrame()->addToJavaScriptWindowObject(m_jsObject->objectName(), m_jsObject);
}

const QString BtHtmlReadDisplay::text( const CDisplay::TextType format, const CDisplay::TextPart part) 
{
	switch (part) 
	{
		case Document: 
		{
			if (format == HTMLText) 
			{
				return mainFrame()->toHtml();
			}
			else 
			{
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

		case SelectedText: 
		{
			if (!hasSelection()) 
			{
				return QString::null;
			}
			else if (format == HTMLText) 
			{
				//	TODO: It does not appear this is ever called
			}
			else 
			{ //plain text requested
				return selectedText();
			}
		}

		case AnchorOnly: 
		{
			QString moduleName;
			QString keyName;
			CReferenceManager::Type type;
			CReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

			return keyName;
		}

		case AnchorTextOnly: 
		{
			QString moduleName;
			QString keyName;
			CReferenceManager::Type type;
			CReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

			if (CSwordModuleInfo* module = backend()->findModuleByName(moduleName)) 
			{
				boost::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
				key->key( keyName );

				return key->strippedText();
			}
			return QString::null;
		}

		case AnchorWithText: 
		{
			QString moduleName;
			QString keyName;
			CReferenceManager::Type type;
			CReferenceManager::decodeHyperlink(activeAnchor(), moduleName, keyName, type);

			if (CSwordModuleInfo* module = backend()->findModuleByName(moduleName)) 
			{
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
	return QString();
}

// Puts html text and javascript into QWebView
void BtHtmlReadDisplay::setText( const QString& newText ) 
{
	QString jsText = newText;
	jsText.replace(body,jsBegin+javascript+jsEnd+body);

	// Disconnect any previous connect and connect to slot that loads the javascript object
	QWebFrame* frame = mainFrame();
	disconnect(frame,SIGNAL(javaScriptWindowObjectCleared()),0,0);
	bool ok = connect(frame,SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(loadJSObject()));
	Q_ASSERT(ok);

	// Send text to the html viewer
	m_view->setHtml(jsText);
}

// See if any text is selected
bool BtHtmlReadDisplay::hasSelection() 
{
	if (selectedText().isEmpty())
		return false;
	return true;
}

// Reimplementation
// Returns the BtHtmlReadDisplayView object
QWidget* BtHtmlReadDisplay::view()
{
	return m_view;
}

// Select all text in the viewer
void BtHtmlReadDisplay::selectAll() 
{
	m_jsObject->setBodyEditable(true);
    m_view->triggerPageAction( QWebPage::MoveToStartOfDocument, true );
    m_view->triggerPageAction( QWebPage::SelectEndOfDocument,   true );
	m_jsObject->setBodyEditable(false);
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

// Save the Lemma (Strongs number) attribute 
void BtHtmlReadDisplay::setLemma(const QString& lemma)
{
	m_nodeInfo[ CDisplay::Lemma ] = lemma;
}

// Open the Find text dialog
void BtHtmlReadDisplay::openFindTextDialog() 
{
	CMDIArea* mdiArea = parentWindow()->mdi();
	showBtHtmlFindText(mdiArea);
}

// Send "completed" signal when the text is finished loading into the viewer
void BtHtmlReadDisplay::slotLoadFinished(bool)
{
	emit completed();
}

// For debugging javascript
#if 0
void BtHtmlReadDisplay::javaScriptConsoleMessage (const QString& message, int lineNumber, const QString& sourceID )
{
}
#endif




// ----------------- BtHtmlReadDisplayView -------------------------------------

BtHtmlReadDisplayView::BtHtmlReadDisplayView(BtHtmlReadDisplay* displayWidget, QWidget* parent) 
	: QWebView(parent), m_display(displayWidget) 
{
}

BtHtmlReadDisplayView::~BtHtmlReadDisplayView()
	{
		setPage(0);
	}

// Create the right mouse context menus
void BtHtmlReadDisplayView::contextMenuEvent(QContextMenuEvent* event)
{
	if (QMenu* popup = m_display->installedPopup()) 
	{
		popup->exec(event->globalPos());
	}
}

// Reimplementation from QWidget
void BtHtmlReadDisplayView::dropEvent( QDropEvent* e ) 
{
	if (e->mimeData()->hasFormat("BibleTime/Bookmark")) 
	{
		//see docs for BTMimeData and QMimeData
		const QMimeData* mimedata = e->mimeData();
		if (mimedata != 0)
		{
			const BTMimeData* btmimedata = qobject_cast<const BTMimeData*>(mimedata);
			if (btmimedata != 0)
			{
				BookmarkItem item = (qobject_cast<const BTMimeData*>(e->mimeData()))->bookmark();
				m_display->connectionsProxy()->emitReferenceDropped(item.key());
				e->acceptProposedAction();
				return;
			}
		}
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

// Reimplementation from QWebView
void BtHtmlReadDisplayView::dragMoveEvent( QDragMoveEvent* e ) 
{
	if (e->mimeData()->hasFormat("BibleTime/Bookmark")) 
	{
		e->acceptProposedAction();
		return;
	}
	//don't accept the action!
	e->ignore();
}

