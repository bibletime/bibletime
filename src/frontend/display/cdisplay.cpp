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

#include "frontend/display/cdisplay.h"

#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMenu>
#include <QTimer>
#include "backend/managers/referencemanager.h"
#include "frontend/btcopybyreferencesdialog.h"
#include "frontend/display/chtmlwritedisplay.h"
#include "frontend/display/cplainwritedisplay.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "util/tool.h"


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
    m_display->copy(CDisplay::PlainText, CDisplay::SelectedText);
}

void CDisplayConnections::copyByReferences() {
   m_display->copyByReferences();
}

void CDisplayConnections::printAll(const DisplayOptions &displayOptions,
                                   const FilterOptions &filterOptions)
{
    m_display->print(CDisplay::Document, displayOptions, filterOptions);
}

void CDisplayConnections::printAnchorWithText(const DisplayOptions &displayOptions,
                                              const FilterOptions &filterOptions)
{
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

void CDisplayConnections::openFindTextDialog() {
    m_display->openFindTextDialog();
}


/*----------------------*/


CDisplay::CDisplay(CDisplayWindow* parent) :
    m_parentWindow(parent),
    m_connections( new CDisplayConnections( this ) ),
    m_popup(nullptr) {}

CDisplay::~CDisplay() {
    delete m_connections;
}

void CDisplay::scrollToKey(CSwordKey* /* key */ ) {
    // Implemented for some subclases
}

void CDisplay::setModules(const QStringList& /* modules */) {
    // Implemented for some subclases
}

bool CDisplay::copy( const CDisplay::TextType format, const CDisplay::TextPart part  ) {
    QApplication::clipboard()->setText( this->text(format, part) );
    return true;
}

void CDisplay::copyByReferences() {
    parentWindow()->copyByReferences();
}

bool CDisplay::save( const CDisplay::TextType format, const CDisplay::TextPart part ) {
    //  qWarning("CDisplay::save( const CDisplay::TextType format, const CDisplay::TextPart part  )");
    const QString content = text(format, part);
    QString filter = QString::null;

    switch (format) {
    case HTMLText:
        filter = QObject::tr("HTML files") + QString(" (*.html *.htm);;") + QObject::tr("All files") + QString(" (*.*)");
        break;
    case PlainText:
        filter = QObject::tr("Text files") + QString(" (*.txt);;") + QObject::tr("All files") + QString(" (*.*)");
        break;
    }

    const QString filename = QFileDialog::getSaveFileName(nullptr, QObject::tr("Save document ..."), "", filter);

    if (!filename.isEmpty()) {
        util::tool::savePlainFile(filename, content);
    }
    return true;
}

/** Emits the signal which used when a reference was clicked. */
void CDisplay::emitReferenceClicked( const QString& reference ) {
    QString module, key;
    ReferenceManager::Type type;
    ReferenceManager::decodeHyperlink(reference, module, key, type);
    if (module.isEmpty()) {
        module = ReferenceManager::preferredModule( type );
    }
    m_connections->emitReferenceClicked(module, key);
}

/** Used when a reference was dropped onto the widget. */
void CDisplay::emitReferenceDropped( const QString& reference ) {
    QString module;
    QString key;
    ReferenceManager::Type type;
    ReferenceManager::decodeHyperlink(reference, module, key, type);
    m_connections->emitReferenceDropped(key);
}

/** Returns the connections object used for signals and slots. */
CDisplayConnections* CDisplay::connectionsProxy() const {
    return m_connections;
}

CDisplayWindow* CDisplay::parentWindow() const {
    return m_parentWindow;
}

/** Installs the popup which should be opened when the right mouse button was pressed. */
void CDisplay::installPopup( QMenu* popup ) {
    m_popup = popup;
}

/** Returns the popup menu which was set by installPopupMenu() */
QMenu* CDisplay::installedPopup() {
    return m_popup;
}

