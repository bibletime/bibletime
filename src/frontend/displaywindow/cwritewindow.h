/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CWRITEWINDOW_H
#define CWRITEWINDOW_H

#include "frontend/displaywindow/cdisplaywindow.h"


class BtActionCollection;
class CWriteDisplay;
class QString;

/** The base class for all write-only display windows.
  *
  * Inherits CDisplayWindow.
  *
  * Inherited by CPlainWriteWindow.
  *
  *@author The BibleTime team
  */
class CWriteWindow : public CDisplayWindow  {
        Q_OBJECT
    public:
        enum WriteWindowType {
            HTMLWindow = 1,
            PlainTextWindow = 2
        };

        /** Insert the keyboard accelerators of this window into the given actioncollection.*/
        static void insertKeyboardActions( BtActionCollection* const a );

        CWriteWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);

        virtual void storeProfileSettings(const QString & windowGroup);

        /** Initializes the signal / slot connections of this display window.*/
        virtual void initConnections();

        /** Initializes the internel keyboard actions.*/
        virtual void initActions();

    public slots:
        /**
        * Look up the given key and display the text. In our case we offer to edit the text.
        */
        virtual void lookupSwordKey( CSwordKey* key );


    protected: // Protected methods
        /**
        * Set the displayWidget which is a subclass of QWebPage.
        */
        void setDisplayWidget( CDisplay* display );

        /** Returns the type of the write window.*/
        virtual CWriteWindow::WriteWindowType writeWindowType() = 0;

        /** Returns true if the window may be closed.*/
        virtual bool queryClose();

        /** Saves the text for the current key. Directly writes the changed text into the module. */
        virtual void saveCurrentText( const QString& key ) = 0;

        /** Called to add actions to mainWindow toolbars */
        virtual void setupMainWindowToolBars() = 0;

    protected slots:
        /** Save text to the module
        */
        void saveCurrentText();
        /**
         */
        virtual void beforeKeyChange();

    private:
        CWriteDisplay* m_writeDisplay;
};

#endif
