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

#ifndef CREADWINDOW_H
#define CREADWINDOW_H

#include "frontend/displaywindow/cdisplaywindow.h"

#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"


class BtActionCollection;
class QResizeEvent;

/** \brief The base class for all read-only display windows. */
class CReadWindow: public CDisplayWindow {

    Q_OBJECT

public: /* Methods: */

    CReadWindow(QList<CSwordModuleInfo *> modules, CMDIArea * parent);

    /**
       Inserts the keyboard accelerators of this window into the given
       collection.
    */
    static void insertKeyboardActions(BtActionCollection * const a);

    virtual CSwordModuleInfo::ModuleType moduleType() const = 0;

protected: /* Methods: */

    /** Sets the display widget of this display window. */
    virtual void setDisplayWidget(CDisplay * newDisplay);

    virtual void resizeEvent(QResizeEvent * e);

    /** Adds actions to mainWindow toolbars. */
    virtual void setupMainWindowToolBars() = 0;

protected slots:

    /** Loads the text using the key. */
    virtual void lookupSwordKey(CSwordKey *);

    /**
      Catches the signal when the KHTMLPart has finished the layout (anchors are
      not ready before that).
    */
    virtual void slotMoveToAnchor();

    /** Updates the status of the popup menu entries. */
    virtual void copyDisplayedText();

    /** Opens the search dialog with the strong info of the last clicked word.*/
    void openSearchStrongsDialog();

private: /* Fields: */

    CReadDisplay * m_readDisplayWidget;

};

#endif /* CREADWINDOW_H */
