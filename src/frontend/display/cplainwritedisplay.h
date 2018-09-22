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

#ifndef CPLAINWRITEDISPLAY_H
#define CPLAINWRITEDISPLAY_H

#include <QTextEdit>
#include "frontend/display/cdisplay.h"


class BtActionCollection;
class CPlainWriteWindow;
class QToolBar;

/** The write display implementation for plain source code editing.
  * @author The BibleTime team
  */
class CPlainWriteDisplay : public QTextEdit, public CDisplay  {
    public:

        CPlainWriteDisplay(CPlainWriteWindow * parentWindow, QWidget * parent = nullptr);

        void selectAll() override;

        void setText(const QString & newText) override;

        bool hasSelection() const override;

        QWidget* view() override;

        const QString text(const CDisplay::TextType format = CDisplay::HTMLText,
                           const CDisplay::TextPart part = CDisplay::Document )
                override;

        inline void print(const CDisplay::TextPart,
                          const DisplayOptions &,
                          const FilterOptions &) override {}

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

        void dropEvent(QDropEvent * e) override;

        void dragEnterEvent(QDragEnterEvent * e) override;

        void dragMoveEvent(QDragMoveEvent * e) override;

};

#endif
