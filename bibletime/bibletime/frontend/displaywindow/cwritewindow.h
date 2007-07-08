/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CWRITEWINDOW_H
#define CWRITEWINDOW_H

//BibleTime includes
#include "frontend/display/cwritedisplay.h"
#include "cdisplaywindow.h"

//Qt includes
#include <qwidget.h>

/**The base class for all write-only display windows.
  *@author The BibleTime team
  */

class CWriteWindow : public CDisplayWindow  {
	Q_OBJECT
public:
	static void insertKeyboardActions( KActionCollection* const a );

	CWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name=0);
	virtual ~CWriteWindow();
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void storeProfileSettings(Profile::CProfileWindow * const settings);
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void applyProfileSettings(Profile::CProfileWindow * const settings);
	/**
	* Returns the write display widget used by this window.
	*/
	CWriteDisplay* const displayWidget();
	virtual void initConnections();
	virtual void initActions();

public slots:
	/**
	* Look up the given key and display the text. In our case we offer to edit the text.
	*/
	virtual void lookup( CSwordKey* key );


protected: // Protected methods
	/**
	* Saves the given text as text of the given key. Use this function
	* as backend in each write window implementation.
	*/
	void setDisplayWidget( CWriteDisplay* display );
	virtual const CDisplayWindow::WriteWindowType writeWindowType() = 0;
	virtual bool queryClose();
	virtual void saveCurrentText( const QString& key ) = 0;

protected slots:
	/** Save text to the module
	*/
	void saveCurrentText() {
		if(key()) {
			saveCurrentText(key()->key());
		};
	};
	virtual void beforeKeyChange(const QString&);

private:
	CWriteDisplay* m_writeDisplay;
};

#endif
