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

#include "frontend/display/cplainwritedisplay.h"

#include <memory>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMenu>
#include "backend/keys/cswordkey.h"
#include "frontend/BtMimeData.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cplainwritewindow.h"
#include "util/btconnect.h"


CPlainWriteDisplay::CPlainWriteDisplay(CPlainWriteWindow * parentWindow, QWidget * parent)
    : QTextEdit(parentWindow ? parentWindow : parent)
    , CDisplay(parentWindow)
{
    setAcceptRichText(false);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    BT_CONNECT(this, SIGNAL(textChanged()),
               connectionsProxy(), SLOT(emitTextChanged()));
}

/** Reimplementation. */
void CPlainWriteDisplay::selectAll() {
    QTextEdit::selectAll();
}

void CPlainWriteDisplay::setText( const QString& newText ) {
    //make sure the text has been converted to show \n instead of <br/>
    QString text = newText;
//     text.replace("\n<br /><!-- BT newline -->\n", "\n");
    text.replace("<br />", "\n"); //inserted by BT or the Qt textedit widget

    QTextEdit::setText(text);
}

bool CPlainWriteDisplay::hasSelection() const {
    /// \todo test this
    return textCursor().hasSelection();
}

QWidget *CPlainWriteDisplay::view() {
    return this;
}

const QString CPlainWriteDisplay::text( const CDisplay::TextType /*format*/, const CDisplay::TextPart /*part*/) {
    return QString::null;
}

/** Sets the current status of the edit widget. */
void CPlainWriteDisplay::setModified( const bool modified ) {
    document()->setModified(modified);
}

/** Reimplementation. */
bool CPlainWriteDisplay::isModified() const {
    return document()->isModified();
}


/** Returns the text of this edit widget. */
const QString CPlainWriteDisplay::plainText() {
    QString ret = QTextEdit::toPlainText();

    //in plain text mode the text just contains newlines, convert them into <br/> before we return the text for display in a HTML widget
    ret.replace("\n", "<br />");

    return ret;
}

/** Creates the necessary action objects and puts them on the toolbar. */
void CPlainWriteDisplay::setupToolbar(QToolBar*, BtActionCollection*) {}

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

    if ( mimedata && mimedata->hasFormat("BibleTime/Bookmark") ) {
        e->acceptProposedAction();

        BTMimeData::ItemList items = mimedata->bookmarks();
        BTMimeData::ItemList::iterator it;
        for (it = items.begin(); it != items.end(); ++it) {

            CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName((*it).module());
            std::unique_ptr<CSwordKey> key(CSwordKey::createInstance(module));
            key->setKey((*it).key());
            QString moduleText = key->strippedText();

            const QString text = QString::fromLatin1("%1\n(%2, %3)\n").arg(moduleText).arg((*it).key()).arg((*it).module());

            setTextCursor(cursorForPosition(e->pos()));
            textCursor().insertText( text );
        }
    }
    else if ( e->mimeData()->hasFormat("text/plain")) {
        e->acceptProposedAction();
        setTextCursor(cursorForPosition(e->pos()));
        textCursor().insertText( e->mimeData()->text() );
    }
}
