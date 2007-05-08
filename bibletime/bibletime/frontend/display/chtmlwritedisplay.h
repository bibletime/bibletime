/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CHTMLWRITEDISPLAY_H
#define CHTMLWRITEDISPLAY_H

//BibleTime includes
#include "cplainwritedisplay.h"

//Qt includes
#include <qwidget.h>
#include <qtextedit.h>

class CWriteWindow;

class QPopupMenu;

class KToggleAction;
class KFontAction;
class KFontSizeAction;

class KColorButton;

/** The WYSIWYG implementation of the write display interface.
  * @author The BibleTime team
  */
class CHTMLWriteDisplay : public CPlainWriteDisplay  {
	Q_OBJECT
public:
	/**
	* Sets the new text for this display widget.
	*/
	virtual void setText( const QString& newText );
	/**
	* Returns the text of this edit widget.
	*/
	virtual const QString plainText();

	/**
	* Creates the necessary action objects and puts them on the toolbar.
	*/
	virtual void setupToolbar(KToolBar * bar, KActionCollection * actionCollection);

protected:
	friend class CDisplay;
	CHTMLWriteDisplay(CWriteWindow* parentWindow, QWidget* parent);
	~CHTMLWriteDisplay();
	/**
	* Reimplementation to show a popup menu if the right mouse butoon was clicked.
	*/
	virtual QPopupMenu* createPopupMenu( const QPoint& pos );

protected slots:
	void toggleBold();
	void toggleItalic();
	void toggleUnderline();

	void alignLeft();
	void alignCenter();
	void alignRight();

	void slotFontChanged( const QFont& );
	/**
	* The text's alignment changed. Enable the right buttons.
	*/
	void slotAlignmentChanged( int );
	/**
	* Is called when a new color was selected.
	*/
	void slotColorSelected( const QColor& );
	/**
	* Is called when a text with another color was selected.
	*/
	void slotColorChanged( const QColor& );

private:
	struct {
		KToggleAction* bold;
		KToggleAction* italic;
		KToggleAction* underline;

		KToggleAction* alignLeft;
		KToggleAction* alignCenter;
		KToggleAction* alignRight;

		KFontAction* fontChooser;
		KFontSizeAction* fontSizeChooser;

		//popup menu
		KAction* selectAll;
	}
	m_actions;
	KColorButton* m_colorButton;
};

#endif
