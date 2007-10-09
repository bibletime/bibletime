/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "cplainwritedisplay.h"

#include "frontend/cdragdrop.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cwritewindow.h"

#include "util/scoped_resource.h"

//Qt includes
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QMenu>

//KDE includes
#include <kaction.h>
#include <klocale.h>


CPlainWriteDisplay::CPlainWriteDisplay(CWriteWindow* parentWindow, QWidget* parent) : QTextEdit(parentWindow ? parentWindow : parent), CWriteDisplay(parentWindow) {
	setAcceptRichText(false);
	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);

	connect(this, SIGNAL(textChanged()),
			connectionsProxy(), SLOT(emitTextChanged()));
}

CPlainWriteDisplay::~CPlainWriteDisplay() {}

/** Reimplementation. */
void CPlainWriteDisplay::selectAll() {
	QTextEdit::selectAll();
}

void CPlainWriteDisplay::setText( const QString& newText ) {
	//make sure the text has been converted to show \n instead of <br/>
	QString text = newText;
// 	text.replace("\n<br /><!-- BT newline -->\n", "\n");
	text.replace("<br />", "\n"); //inserted by BT or the Qt textedit widget

	QTextEdit::setText(text);
}

const bool CPlainWriteDisplay::hasSelection() {
	//TODO: test this
	return textCursor().hasSelection();
}

QWidget* CPlainWriteDisplay::view() {
	qDebug("CPlainWriteDisplay::view()");
	return this;
}

const QString CPlainWriteDisplay::text( const CDisplay::TextType /*format*/, const CDisplay::TextPart /*part*/) {
	return QString::null;
}

void CPlainWriteDisplay::print( const CDisplay::TextPart, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions ) {
}

/** Sets the current status of the edit widget. */
void CPlainWriteDisplay::setModified( const bool modified ) {
	document()->setModified(modified);
}

/** Reimplementation. */
const bool CPlainWriteDisplay::isModified() const {
	return document()->isModified();
}


/** Returns the text of this edit widget. */
const QString CPlainWriteDisplay::plainText() {
	QString ret = QTextEdit::toPlainText();

	//in plain text mode the text just contains newlines, convert them into <br/> before we return the text for display in a HTML widget
	ret.replace("\n", "<br />");

	return ret;
}

/** Reimplementation from QTextEdit. Provides an popup menu for the given position. */
QMenu* CPlainWriteDisplay::createPopupMenu( const QPoint& /*pos*/ ) {
	return installedPopup();
}

/** Reimplementation from QTextEdit. Provides an popup menu for the given position. */
QMenu* CPlainWriteDisplay::createPopupMenu( ) {
	return installedPopup();
}

/** Creates the necessary action objects and puts them on the toolbar. */
void CPlainWriteDisplay::setupToolbar(QToolBar*, KActionCollection*) {}

/** Reimplementation to insert the text of a dragged reference into the edit view. */
void CPlainWriteDisplay::dragEnterEvent( QDragEnterEvent* e ) {
	//if (CDragDropMgr::canDecode(e)) {
	if (e->mimeData()->hasFormat("BibleTime/Bookmark") || e->mimeData()->hasFormat("text/plain")) {
		e->acceptProposedAction();
	}
	else {
		//e->accept(false);
		e->ignore();
	}
}

/** Reimplementation to insert the text of a dragged reference into the edit view. */
void CPlainWriteDisplay::dragMoveEvent( QDragMoveEvent* e ) {
	if (e->mimeData()->hasFormat("BibleTime/Bookmark") || e->mimeData()->hasFormat("text/plain")) {
		//placeCursor(e->pos());
		setTextCursor(cursorForPosition(e->pos()));
		ensureCursorVisible();
		e->acceptProposedAction();
	}
	else {
		//e->accept(false);
		e->ignore();
	}
}

/** Reimplementation to manage drops of our drag and drop objects. */
void CPlainWriteDisplay::dropEvent( QDropEvent* e ) {

	const BTMimeData* mimedata = qobject_cast<const BTMimeData*>(e->mimeData());

	if ( mimedata->hasFormat("BibleTime/Bookmark") ) {
		e->acceptProposedAction();

		BTMimeData::ItemList items = mimedata->bookmarks();
		BTMimeData::ItemList::iterator it;
		for (it = items.begin(); it != items.end(); ++it) {
			
			CSwordModuleInfo* module = backend()->findModuleByName((*it).module());
			util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
			key->key( (*it).key() );
			QString moduleText = key->strippedText();

			const QString text = QString::fromLatin1("%1\n(%2, %3)\n").arg(moduleText).arg((*it).key()).arg((*it).module());

			setTextCursor(cursorForPosition(e->pos()));
			textCursor().insertText( text );
		}
	}
	else if ( mimedata->hasFormat("text/plain")) {
		e->acceptProposedAction();
		setTextCursor(cursorForPosition(e->pos()));
		textCursor().insertText( mimedata->text() );
	}
}
