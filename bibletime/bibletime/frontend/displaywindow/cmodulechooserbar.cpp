/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmodulechooserbar.h"
#include "cmodulechooserbutton.h"

#include <qtimer.h>

CModuleChooserBar::CModuleChooserBar(ListCSwordModuleInfo useModules, CSwordModuleInfo::ModuleType type, QWidget *parent, const char *name )
: KToolBar(parent,name),
m_moduleType(type),
m_idCounter(0),
m_buttonLimit(-1) //-1 means no limit
{
	//insert buttons if useModules != 0
	//  for (useModules.first(); useModules.current(); useModules.next())   {
	ListCSwordModuleInfo::iterator end_it = useModules.end();
	for (ListCSwordModuleInfo::iterator it(useModules.begin()); it != end_it; ++it) {

		if ((m_buttonLimit != -1) && ( m_buttonLimit <= (int)m_buttonList.count()) ) { //we reached the button limit
			break;
		};

		addButton( *it );
	}

	// We can add a button to choose an additional module
	if ( (m_buttonLimit == -1) || (m_buttonLimit > (int)m_buttonList.count()) ) {
		addButton(0); //add a button without module set
	}
}

/** Adds a button to the toolbar */
CModuleChooserButton* const CModuleChooserBar::addButton( CSwordModuleInfo* const module ) {
	CModuleChooserButton* b = new CModuleChooserButton(module, m_moduleType, ++m_idCounter, this);
	insertWidget( m_idCounter, b->size().width(), b );
	m_buttonList.append(b);

	connect( b, SIGNAL(sigAddButton()), this, SLOT(addButton()) );

	connect( b, SIGNAL(sigRemoveButton(const int)), this, SLOT(removeButton(const int)) );

	connect( b, SIGNAL(sigChanged()), SIGNAL(sigChanged()) );
	connect( b, SIGNAL(sigChanged()), SLOT(updateMenuItems()) );

	b->show();

	updateMenuItems(); //make sure the items are up to date with the newest module list

	return b;
}

void CModuleChooserBar::addButton( ) {
	addButton(0);
}

/** Removes a button from the toolbar */
void CModuleChooserBar::removeButton( const int ID ) {
	for (m_buttonList.first(); m_buttonList.current(); m_buttonList.next()) {
		if (m_buttonList.current()->getId() == ID) { //found the right button to remove
			CModuleChooserButton* b = m_buttonList.current();
			m_buttonList.remove(b);

			b->hide();
			b->deleteLater();
			break;
		}
	}

	emit sigChanged();

	updateMenuItems(); //make sure the items are up to date with the newest module list
}

/** Returns a list of selected modules. */
ListCSwordModuleInfo CModuleChooserBar::getModuleList() {
	const int currentItemIndex = m_buttonList.at(); //make sure we don't change the current list item

	ListCSwordModuleInfo list;
	CSwordModuleInfo* m = 0;
	for ( m_buttonList.first(); m_buttonList.current(); m_buttonList.next()) {
		m = m_buttonList.current()->module();
		if ( m ) {
			list.append( m );
		}
	}

	m_buttonList.at( currentItemIndex );
	return list;
}

/** Sets the number of the maximum count of buttons. */
void CModuleChooserBar::setButtonLimit(const int limit) {
	m_buttonLimit = limit;
	if (limit == -1) //no need to delete buttons
		return;

	const int tooMuch = m_buttonList.count() - limit;
	for (int i = 0; i < tooMuch; ++i) {
		CModuleChooserButton* b = m_buttonList.getLast();
		m_buttonList.remove(b);
		b->hide();
		delete b;
	}

	updateMenuItems();
}

/** Sets the modules which are chosen in this module chooser bar. */
void CModuleChooserBar::setModules( ListCSwordModuleInfo useModules ) {
	setButtonLimit(0);
	setButtonLimit(-1);  //these two lines clear the bar

	if (!useModules.count()) {
		return;
	}

	//  for (useModules.first(); useModules.current(); useModules.next())   {
	ListCSwordModuleInfo::iterator end_it = useModules.end();
	for (ListCSwordModuleInfo::iterator it(useModules.begin()); it != end_it; ++it) {
		if ( (m_buttonLimit != -1) && (m_buttonLimit <= (int)m_buttonList.count()) ) {
			break;
		}

		addButton( *it );
	}
	if ( (m_buttonLimit == -1) || (m_buttonLimit > (int)m_buttonList.count()) ) {
		addButton(0);//add button without module set
	}

	updateMenuItems();
}


/*!
    \fn CModuleChooserBar::updateMenuItems()
 */
void CModuleChooserBar::updateMenuItems() {
	QPtrList<CModuleChooserButton> buttons = m_buttonList ;

	for (buttons.first(); buttons.current(); buttons.next()) {
		buttons.current()->updateMenuItems();
	}
}
