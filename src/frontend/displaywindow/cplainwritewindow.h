/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CPLAINWRITEWINDOW_H
#define CPLAINWRITEWINDOW_H

#include "frontend/displaywindow/cdisplaywindow.h"


class BtActionCollection;
class CPlainWriteDisplay;

/** The write window class which offers a plain text editor for creating a personal commentary.
  *
  * Inherits CWriteWindow.
  *
  * Inherited by CHTMLWriteWindow.
  *
  * @author The BibleTime team
  */
class CPlainWriteWindow : public CDisplayWindow {
        Q_OBJECT
    public:

        enum WriteWindowType {
            HTMLWindow = 1,
            PlainTextWindow = 2
        };

        CPlainWriteWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);

        /**
        * Set the displayWidget which is a subclass of QWebPage.
        */
        void setDisplayWidget( CDisplay* display ) override;

        virtual void storeProfileSettings(QString const & windowGroup) const override;
        virtual void applyProfileSettings(const QString & windowGroup) override;

        /**
         * Setups the popup menu of this display widget.
         */
        virtual void setupPopupMenu() override;

        /**
         * Returns true if the sync toolbar is enabled.
         */
        virtual bool syncAllowed() const override;

    public slots:

        /**
          Look up the given key and display the text. In our case we offer to edit the text.
        */
        virtual void lookupSwordKey(CSwordKey * key) override;

    protected: // Protected methods
        /**
        * Initialize the state of this widget.
        */
        virtual void initView() override;
        virtual void initConnections() override;
        virtual void initToolbars() override;
        virtual WriteWindowType writeWindowType() const {
            return PlainTextWindow;
        }

        /** Called to add actions to mainWindow toolbars */
        virtual void setupMainWindowToolBars() override;
        /**
         * Initializes the intern keyboard actions.
         */
        virtual void initActions() override;
        /**
        * Insert the keyboard accelerators of this window into the given KAccel object.
        */
        static void insertKeyboardActions( BtActionCollection* const a );

        /** \returns whether the window may be closed.*/
        virtual bool queryClose() override;

    protected slots: // Protected slots

        void saveCurrentText();

        /**
        * Saves the text for the current key. Directly writes the changed text into the module.
        */
        virtual void saveCurrentText( const QString& );
        /**
        * Is called when the current text was changed.
        */
        virtual void textChanged();
        /**
        * Loads the original text from the module.
        */
        virtual void restoreText();
        /**
        * Deletes the module entry and clears the edit widget.
        */
        virtual void deleteEntry();

        virtual void beforeKeyChange();

    protected: /* Fields: */

        CPlainWriteDisplay * m_writeDisplay;

};

#endif
