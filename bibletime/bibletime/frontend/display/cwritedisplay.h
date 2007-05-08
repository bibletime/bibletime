/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CWRITEDISPLAY_H
#define CWRITEDISPLAY_H

#include "cdisplay.h"

class KActionCollection;
class KToolBar;

/**The base class for all read/write-display classes.
  *@author The BibleTime team
  */

class CWriteDisplay : public CDisplay  {
protected:
	friend class CDisplay;
	friend class CPlainWriteDisplay;
	CWriteDisplay( CWriteWindow* writeWindow );
	~CWriteDisplay();

public: // Public methods
	/**
	* Sets the current modified status of the widget.
	*/
	virtual void setModified( const bool modified ) = 0;
	/**
	* Returns true if the current text was modified.
	*/
	virtual const bool isModified() const = 0;
	/**
	* Returns the text of this edit widget.
	*/
	virtual const QString plainText() = 0;
	/**
	* Creates the necessary action objects and puts them on the toolbar.
	*/
	virtual void setupToolbar( KToolBar* bar, KActionCollection* actionCollection ) = 0;
};

#endif
