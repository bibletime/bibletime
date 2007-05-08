/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CCOMMENTARYREADWINDOW_H
#define CCOMMENTARYREADWINDOW_H

//BibleTime includes
#include "cbiblereadwindow.h"

//Qt includes
#include <qwidget.h>

/**
  *@author The BibleTime team
  */

class CCommentaryReadWindow : public CLexiconReadWindow  {
	Q_OBJECT
public:
	/**
	* Reimplementation.
	*/
	static void insertKeyboardActions( KActionCollection* const a );

	CCommentaryReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name=0);
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void storeProfileSettings( Profile::CProfileWindow* profileWindow );
	/**
	* Store the settings of this window in the given profile window.
	*/
	virtual void applyProfileSettings( Profile::CProfileWindow* profileWindow );
	virtual const bool syncAllowed() const;

public slots: // Public slots
	void nextBook();
	void previousBook();
	void nextChapter();
	void previousChapter();
	void nextVerse();
	void previousVerse();
	/**
	* Reimplementation to handle the keychooser refresh.
	*/
	virtual void reload();

protected:
	virtual void initActions();
	virtual void initToolbars();

private:
	KToggleAction* m_syncButton;
	CSwordVerseKey* verseKey();
protected:
    virtual void setupPopupMenu();
};

#endif
