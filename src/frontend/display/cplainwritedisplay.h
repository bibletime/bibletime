/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CPLAINWRITEDISPLAY_H
#define CPLAINWRITEDISPLAY_H

#include <QTextEdit>
#include "frontend/display/cdisplay.h"


class BtActionCollection;
class CWriteWindow;
class QToolBar;

/** The write display implementation for plain source code editing.
  * @author The BibleTime team
  */
class CPlainWriteDisplay : public QTextEdit, public CDisplay  {
    public:

        CPlainWriteDisplay(CWriteWindow * parentWindow, QWidget * parent = 0);

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
        virtual bool hasSelection();
        /**
        * Returns the view of this display widget.
        */
        virtual QWidget* view();
        virtual const QString text( const CDisplay::TextType format = CDisplay::HTMLText, const CDisplay::TextPart part = CDisplay::Document );

        /**
          Reimplemented from CDisplay.
        */
        virtual inline void print(const CDisplay::TextPart,
                                  const DisplayOptions &,
                                  const FilterOptions &) {}

        virtual bool isModified() const;
        /**
        * Sets the current status of the edit widget.
        */
        virtual void setModified( const bool modified );
        /**
        * Returns the text of this edit widget.
        */
        virtual const QString plainText();
        /**
        * Creates the necessary action objects and puts them on the toolbar.
        */
        virtual void setupToolbar(QToolBar*, BtActionCollection*);

    protected:

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
