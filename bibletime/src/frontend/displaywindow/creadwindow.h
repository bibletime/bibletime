/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CREADWINDOW_H
#define CREADWINDOW_H

#include "frontend/displaywindow/cdisplaywindow.h"

#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"


class BtActionCollection;
class QResizeEvent;

/** The base class for all read-only display windows.
  * @author The BibleTime team
  */
class CReadWindow : public CDisplayWindow  {
        Q_OBJECT
    public:

        static void insertKeyboardActions( BtActionCollection* const a );

        CReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);
        virtual ~CReadWindow();
        /**
        * Store the settings of this window in the given CProfileWindow object.
        */
        virtual void storeProfileSettings(Profile::CProfileWindow * const settings);
        /**
        * Store the settings of this window in the given CProfileWindow object.
        */
        virtual void applyProfileSettings(Profile::CProfileWindow * const settings);

    protected:
        /**
        * Sets the display widget of this display window.
        */
        virtual void setDisplayWidget( CDisplay* newDisplay );
        /**
         */
        virtual void resizeEvent(QResizeEvent* e);
        ///** Called to add actions to mainWindow toolbars */
        virtual void setupMainWindowToolBars() = 0;

    protected slots:
        /**
        * Load the text using the key
        */
        virtual void lookupSwordKey( CSwordKey* );
        /**
        * Catch the signal when the KHTMLPart has finished the layout (anchors are not ready before that).
        */
        virtual void slotMoveToAnchor();

        /**
        * Update the status of the popup menu entries.
        */
        virtual void copyDisplayedText();
        /** Open the search dialog with the strong info of the last clicked word.
         *
        */
        void openSearchStrongsDialog();

    private:
        CReadDisplay* m_readDisplayWidget;
};

#endif
