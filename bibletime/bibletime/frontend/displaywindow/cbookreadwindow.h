/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CBOOKREADWINDOW_H
#define CBOOKREADWINDOW_H

//BibleTime includes
#include "clexiconreadwindow.h"
#include "backend/cswordkey.h"
#include "backend/cswordtreekey.h"
#include "backend/cswordversekey.h"

class KToggleAction;
class CBookTreeChooser;

/**
  * @author The BibleTime team
  */
class CBookReadWindow : public CLexiconReadWindow  {
	Q_OBJECT
public:
	static void insertKeyboardActions( KActionCollection* const a );

	CBookReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name=0);
	
	virtual ~CBookReadWindow();
	/**
	* Store the settings of this window in the given CProfileWindow object.
	*/
	virtual void storeProfileSettings( Profile::CProfileWindow* profileWindow );
	/**
	* Store the settings of this window in the given profile window.
	*/
	virtual void applyProfileSettings( Profile::CProfileWindow* profileWindow );

protected:
	virtual void initActions();
	virtual void initToolbars();
	virtual void initConnections();
	virtual void initView();

	virtual void setupPopupMenu();

	protected slots: // Protected slots
	/**
	 * Reimplementation to take care of the tree chooser.
	 */
	virtual void modulesChanged();

private:
	KToggleAction* m_treeAction;
	CBookTreeChooser* m_treeChooser;

private slots: // Private slots
	/**
	* Is called when the action was executed to toggle the tree view.
	*/
	void treeToggled();
};

#endif
