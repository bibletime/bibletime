/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CLEXICONREADWINDOW_H
#define CLEXICONREADWINDOW_H

//BibleTime includes
#include "creadwindow.h"

//Qt includes
#include <qwidget.h>

//KDE includes
#include <kaction.h>

class CSwordKey;
class CSwordLDKey;
//KDE
class KToolBarPopupAction;

/**
  *@author The BibleTime team
  */

class CLexiconReadWindow : public CReadWindow  {
	Q_OBJECT
public:
	CLexiconReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name=0);
	virtual ~CLexiconReadWindow();
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	//   virtual void storeProfileSettings( CProfileWindow* profileWindow );
	/**
	* Store the settings of this window in the given profile window.
	*/
	//   virtual void applyProfileSettings( CProfileWindow* profileWindow );
	/**
	* Reimplementation.
	*/
	//   static void insertKeyboardActions( KAccel* a );
	static void insertKeyboardActions( KActionCollection* const a );

protected:
	virtual void initActions();
	virtual void initToolbars();
	virtual void initConnections();
	virtual void initView();
	virtual void updatePopupMenu();
	virtual void setupPopupMenu();


	struct ActionsStruct {
		KToolBarPopupAction* backInHistory;
		KToolBarPopupAction* forwardInHistory;

		KAction* selectAll;
		KAction* findText;
		KAction* findStrongs;

		KActionMenu* copyMenu;
		struct {
			KAction* reference;
			KAction* entry;
			KAction* selectedText;
		}
		copy;

		KActionMenu* saveMenu;
		struct {
			KAction* reference;
			KAction* entryAsPlain;
			KAction* entryAsHTML;
		}
		save;

		KActionMenu* printMenu;
		struct {
			KAction* reference;
			KAction* entry;
		}
		print;
	}
	m_actions;

private:
	/**
	* Reimplementation to return the right key.
	*/
	CSwordLDKey* ldKey();

protected slots: // Protected slots
	void previousEntry();
	void nextEntry();
	/**
	* This function saves the entry as html using the CExportMgr class.
	*/
	void saveAsHTML();
	/**
	* This function saves the entry as plain text using the CExportMgr class.
	*/
	void saveAsPlain();
	void slotFillBackHistory();
	void slotFillForwardHistory();

	void slotUpdateHistoryButtons();
};

#endif
