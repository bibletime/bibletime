/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cdisplay.h"
#include "chtmlreaddisplay.h"
#include "cplainwritedisplay.h"
#include "chtmlwritedisplay.h"

#include "backend/creferencemanager.h"

#include "util/ctoolclass.h"


//Qt includes
#include <qclipboard.h>
#include <qpopupmenu.h>
#include <qtimer.h>

//KDE includes
#include <kapplication.h>
#include <kfiledialog.h>
#include <klocale.h>


CDisplayConnections::CDisplayConnections( CDisplay* display ) : m_display(display) {}

void CDisplayConnections::selectAll() {
	m_display->selectAll();
}

void CDisplayConnections::saveAsHTML() {
	m_display->save(CDisplay::HTMLText, CDisplay::Document);
}

void CDisplayConnections::saveAsPlain() {
	m_display->save(CDisplay::PlainText, CDisplay::Document);
}

/** Emits the signal. */
void CDisplayConnections::emitReferenceClicked( const QString& module, const QString& key) {
	emit referenceClicked( module, key );
}

/** Emits the signal. */
void CDisplayConnections::emitReferenceDropped( const QString& key) {
	emit referenceDropped(key);
}

/** Emits the signal. */
void CDisplayConnections::emitTextChanged() {
	emit textChanged();
}

void CDisplayConnections::copyAll() {
	m_display->copy(CDisplay::PlainText, CDisplay::Document);
}

/** No descriptions */
void CDisplayConnections::copySelection() {
	qWarning("copyign the selected text");
	m_display->copy(CDisplay::PlainText, CDisplay::SelectedText);
}

void CDisplayConnections::printAll(CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) {
	m_display->print(CDisplay::Document, displayOptions, filterOptions);
}

void CDisplayConnections::printAnchorWithText(CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) {
	m_display->print(CDisplay::AnchorWithText, displayOptions, filterOptions);
}

void CDisplayConnections::copyAnchorOnly() {
	m_display->copy(CDisplay::PlainText, CDisplay::AnchorOnly);
}

void CDisplayConnections::copyAnchorTextOnly() {
	m_display->copy(CDisplay::PlainText, CDisplay::AnchorTextOnly);
}

void CDisplayConnections::copyAnchorWithText() {
	m_display->copy(CDisplay::PlainText, CDisplay::AnchorWithText);
}

void CDisplayConnections::saveAnchorWithText() {
	m_display->save(CDisplay::PlainText, CDisplay::AnchorWithText);
}

void CDisplayConnections::clear() {
	m_display->setText(QString::null);
}

void CDisplayConnections::zoomIn() {
	m_display->zoomIn();
}

void CDisplayConnections::zoomOut() {
	m_display->zoomOut();
}

void CDisplayConnections::openFindTextDialog() {
	m_display->openFindTextDialog();
}


/*----------------------*/

CReadDisplay* CDisplay::createReadInstance( CReadWindow* readWindow, QWidget* parent ) {
	return new CHTMLReadDisplay(readWindow, parent);
}

CWriteDisplay* CDisplay::createWriteInstance( CWriteWindow* writeWindow, const CWriteDisplay::WriteDisplayType& type, QWidget* parent ) {
	//  qWarning("CDisplay::createWriteInstance");
	if (type == PlainTextDisplay) {
		return new CPlainWriteDisplay(writeWindow, parent);
	}
	else {
		return new CHTMLWriteDisplay(writeWindow, parent);
	};
}


CDisplay::CDisplay(CDisplayWindow* parent) :
m_parentWindow(parent),
m_connections( new CDisplayConnections( this ) ),
m_popup(0) {}

CDisplay::~CDisplay() {
	delete m_connections;
}

const bool CDisplay::copy( const CDisplay::TextType format, const CDisplay::TextPart part  ) {
	const QString content = text(format, part);

	QClipboard* cb = KApplication::clipboard();
	cb->setText(content);
	return true;
}

const bool CDisplay::save( const CDisplay::TextType format, const CDisplay::TextPart part ) {
	//  qWarning("CDisplay::save( const CDisplay::TextType format, const CDisplay::TextPart part  )");
	const QString content = text(format, part);
	QString filter = QString::null;

	switch (format) {
		case HTMLText:
		filter = QString("*.html *.htm | ") + i18n("HTML files") + QString("\n *.* | All files (*.*)");
		break;
		case PlainText:
		filter = QString("*.txt | ") + i18n("Text files") + QString("\n *.* | All files (*.*)");
		break;
	};

	const QString filename = KFileDialog::getSaveFileName(QString::null, filter, 0, i18n("Save document ..."));

	if (!filename.isEmpty()) {
		CToolClass::savePlainFile(filename, content);
	}
	return true;
}

/** Emits the signal which used when a reference was clicked. */
void CDisplay::emitReferenceClicked( const QString& reference ) {
	qWarning("reference clicked %s", reference.latin1());
	QString module;
	QString key;
	CReferenceManager::Type type;
	/*const bool ok = */
	CReferenceManager::decodeHyperlink(reference, module, key, type);
	if (module.isEmpty()) {
		module = CReferenceManager::preferredModule( type );
	}
	m_connections->emitReferenceClicked(module, key);
}

/** Used when a reference was dropped onto the widget. */
void CDisplay::emitReferenceDropped( const QString& reference ) {
	QString module;
	QString key;
	CReferenceManager::Type type;
	/*const bool ok = */
	CReferenceManager::decodeHyperlink(reference, module, key, type);
	//  if (module.isEmpty()) {
	//    module = CReferenceManager::preferredModule( type );
	//  }

	m_connections->emitReferenceDropped(key);
}

/** Returns the connections obect used for signas and slots. */
CDisplayConnections* const CDisplay::connectionsProxy() const {
	return m_connections;
}

CDisplayWindow* const CDisplay::parentWindow() const {
	return m_parentWindow;
}

/** Installs the popup which should be opened when the right mouse button was pressed. */
void CDisplay::installPopup( QPopupMenu* popup ) {
	m_popup = popup;
}

/** Returns the popup menu which was set by installPopupMenu() */
QPopupMenu* const CDisplay::installedPopup() {
	Q_ASSERT(m_popup);
	return m_popup;
}

