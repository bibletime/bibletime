/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CMODULECHOOSERBUTTON_H
#define CMODULECHOOSERBUTTON_H

//BibleTime includes
#include "backend/cswordmoduleinfo.h"
#include "util/cpointers.h"

//Qt includes
#include <qdict.h>
#include <qtoolbutton.h>

//KDE includes
#include <ktoolbarbutton.h>
#include <kpopupmenu.h>

class KPopupMenu;
class CModuleChooserBar;

/** The CModuleChooserButton displays a list of submenus sorted by language which contain the possible modules
  * which can be displayed together with the first one.
  * @author The BibleTime team
  */
class CModuleChooserButton : public KToolBarButton, public CPointers  {
	Q_OBJECT
public:
	CModuleChooserButton(CSwordModuleInfo* useModule, CSwordModuleInfo::ModuleType type, const int id, CModuleChooserBar *parent, const char *name = 0 );
	~CModuleChooserButton();

	CSwordModuleInfo* module();
	/**
	* Returns the id used for this button.
	*/
	int getId() const;
	void updateMenuItems();

protected: // Protected methods
	void populateMenu();

private:
	/**
	* Returns the icon used for the current status.
	*/
	const QString iconName();

	bool m_hasModule;
	int m_id;
	int m_noneId;
	int m_titleId;
	CSwordModuleInfo::ModuleType m_moduleType;
	CSwordModuleInfo* m_module;

	KPopupMenu* m_popup;
	QPtrList<KPopupMenu> m_submenus;

	CModuleChooserBar* m_moduleChooserBar;


private slots:
	void moduleChosen(int ID );

signals:
	void sigRemoveButton(const int ID);
	void sigAddButton();
	void sigChanged();
};
#endif
