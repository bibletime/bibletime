/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CPLAINWRITEDISPLAY_H
#define CPLAINWRITEDISPLAY_H

//Bibletime include files
#include "cwritedisplay.h"

//Qt includes
#include <QTextEdit>


class CHTMLWriteDisplay;

class KAction;
class QWidget;
class QMenu;
class QDragMoveEvent;
class QDropEvent;
class QDragEnterEvent;

/** The write display implementation for plain source code editing.
  * @author The BibleTime team
  */
class CPlainWriteDisplay : public QTextEdit, public CWriteDisplay  {
public:
	/**
	* Reimplementation.
	*/
	virtual void selectAll();
	/**
	* Sets the new text for this display widget.
	*/
	virtual void setText( const QString& newText );
	/**
	* Returns true if the display widget has a selection. Otherwise false.
	*/
	virtual const bool hasSelection();
	/**
	* Returns the view of this display widget.
	*/
	virtual QWidget* view();
	virtual const QString text( const CDisplay::TextType format = CDisplay::HTMLText, const CDisplay::TextPart part = CDisplay::Document );
	virtual void print( const CDisplay::TextPart, CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions );
	/**
	* Reimplementation (CWriteDisplay).
	*/
	virtual const bool isModified() const;
	/**
	* Sets the current status of the edit widget (CWriteDisplay).
	*/
	virtual void setModified( const bool modified );
	/**
	* Returns the text of this edit widget (CWriteDisplay).
	*/
	virtual const QString plainText();
	/**
	* Creates the necessary action objects and puts them on the toolbar (CWriteDisplay).
	*/
	virtual void setupToolbar(QToolBar*, KActionCollection*);

protected:
	friend class CDisplay;
	friend class CHTMLWriteDisplay;

	CPlainWriteDisplay(CWriteWindow* parentWindow, QWidget* parent);
	virtual ~CPlainWriteDisplay();
	/**
	* Reimplementation from QTextEdit. Provides an popup menu for the given position.
	*/
	virtual QMenu* createPopupMenu( const QPoint& pos );
	/**
	* Reimplementation from QTextEdit. Provides an popup menu.
	*/
	virtual QMenu* createPopupMenu();
	/**
	* Reimplementation from QTextEdit to manage drops of our drag and drop objects.
	*/
	virtual void dropEvent( QDropEvent* e );
	/**
	* Reimplementation from QTextEdit to insert the text of a dragged reference into the edit view.
	*/
	virtual void dragEnterEvent( QDragEnterEvent* e );
	/**
	* Reimplementation from QTextEdit to insert the text of a dragged reference into the edit view.
	*/
	virtual void dragMoveEvent( QDragMoveEvent* e );

};

#endif
