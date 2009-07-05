/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CLEXICONREADWINDOW_H
#define CLEXICONREADWINDOW_H

//BibleTime includes
#include "creadwindow.h"
#include "frontend/keychooser/ckeychooser.h"
class BtToolBarPopupAction;

class CSwordKey;
class CSwordLDKey;

class KToolBarPopupAction;
class BtActionCollection;
class QAction;
class QMenu;

/**
  *@author The BibleTime team
  */

class CLexiconReadWindow : public CReadWindow  {
	Q_OBJECT
public:
	CLexiconReadWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent);
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
	static void insertKeyboardActions( BtActionCollection* const a );

protected:
	virtual void initActions();
	virtual void initToolbars();
	virtual void initConnections();
	virtual void initView();
	virtual void updatePopupMenu();
	virtual void setupPopupMenu();


	struct ActionsStruct {
		BtToolBarPopupAction* backInHistory;
		BtToolBarPopupAction* forwardInHistory;

		QAction* selectAll;
		QAction* findText;
		QAction* findStrongs;

		QMenu* copyMenu;
		struct {
			QAction* reference;
			QAction* entry;
			QAction* selectedText;
		}
		copy;

		QMenu* saveMenu;
		struct {
			QAction* reference;
			QAction* entryAsPlain;
			QAction* entryAsHTML;
		}
		save;

		QMenu* printMenu;
		struct {
			QAction* reference;
			QAction* entry;
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
	void saveRawHTML();

	void slotFillBackHistory();
	void slotFillForwardHistory();

	void slotUpdateHistoryButtons(bool backEnabled, bool fwEnabled);
};

#endif
