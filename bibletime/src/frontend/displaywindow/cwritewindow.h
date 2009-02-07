/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CWRITEWINDOW_H
#define CWRITEWINDOW_H

//BibleTime includes
#include "cdisplaywindow.h"


class CWriteDisplay;
class QString;
class KActionCollection;

/**The base class for all write-only display windows.
  *@author The BibleTime team
  */

class CWriteWindow : public CDisplayWindow  {
	Q_OBJECT
public:
	static void insertKeyboardActions( KActionCollection* const a );

	CWriteWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);
	virtual ~CWriteWindow();
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void storeProfileSettings(Profile::CProfileWindow * const settings);
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void applyProfileSettings(Profile::CProfileWindow * const settings);
	virtual void initConnections();
	virtual void initActions();

public slots:
	/**
	* Look up the given key and display the text. In our case we offer to edit the text.
	*/
	virtual void lookupSwordKey( CSwordKey* key );


protected: // Protected methods
	/**
	* Saves the given text as text of the given key. Use this function
	* as backend in each write window implementation.
	*/
	void setDisplayWidget( CDisplay* display );
	virtual CDisplayWindow::WriteWindowType writeWindowType() = 0;
	virtual bool queryClose();
	virtual void saveCurrentText( const QString& key ) = 0;

protected slots:
	/** Save text to the module
	*/
	void saveCurrentText();
	/**
	 */
	virtual void beforeKeyChange(const QString&);

private:
	CWriteDisplay* m_writeDisplay;
};

#endif
