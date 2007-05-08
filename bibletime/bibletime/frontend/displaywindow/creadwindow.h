/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CREADWINDOW_H
#define CREADWINDOW_H

//BibleTime includes
#include "cdisplaywindow.h"

#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"

//Qt includes
#include <qwidget.h>
#include <qpopupmenu.h>

class KPopupMenu;

/** The base class for all read-only display windows.
  * @author The BibleTime team
  */

class CReadWindow : public CDisplayWindow  {
	Q_OBJECT
public:
	//  static void insertKeyboardActions( KAccel* const a );
	static void insertKeyboardActions( KActionCollection* const a );

	CReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name=0);
	virtual ~CReadWindow();
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void storeProfileSettings(Profile::CProfileWindow * const settings);
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void applyProfileSettings(Profile::CProfileWindow * const settings);

protected: // Protected methods
	/**
	* Sets the display widget of this display window.
	*/
	virtual void setDisplayWidget( CReadDisplay* newDisplay );
	/**
	* Returns the display widget of this window.
	*/
	virtual CReadDisplay* const displayWidget();
	virtual void resizeEvent(QResizeEvent* e);

protected slots:
	virtual void lookup( CSwordKey* );
	/**
	* Update the status of the popup menu entries.
	*/
	virtual void copyDisplayedText();
	/** Open the search dialog with the strong info of the last clicked word.
	 * 
	*/
	void openSearchStrongsDialog();

private:
	CReadDisplay* m_displayWidget;
};

#endif
