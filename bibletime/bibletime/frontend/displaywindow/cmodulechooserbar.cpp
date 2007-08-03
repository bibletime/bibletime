/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmodulechooserbar.h"
#include "cmodulechooserbar.moc"

#include "cmodulechooserbutton.h"

//#include <>
#include <QList>

#include <ktoolbar.h>


//TODO: test if this works with qt4.
// m_buttonList had "current" item with qt3. I don't see why it is used here. Only once at() is used
// and even then only to save the current index. current() is used only when iterating and every time
// first() and next() are called first so that previously saved current index is not valid anymore.
// Therefore I just use an iterator and leave off the idea of permanent "current item". If it's needed an
// extra iterator should be used anyways - we can see here (in qt3 version) how complicated the idea of
// integrated permanent current item of qt3 is.

CModuleChooserBar::CModuleChooserBar(ListCSwordModuleInfo useModules, CSwordModuleInfo::ModuleType type, QWidget *parent)
: KToolBar(parent),
m_moduleType(type),
m_idCounter(0),
m_buttonLimit(-1) //-1 means no limit
{
	//insert buttons if useModules != 0
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

//change current with append
/** Adds a button to the toolbar */
CModuleChooserButton* const CModuleChooserBar::addButton( CSwordModuleInfo* const module ) {
	CModuleChooserButton* b = new CModuleChooserButton(module, m_moduleType, ++m_idCounter, this);
	//insertWidget( m_idCounter, b->size().width(), b );
	QAction* a = addWidget(b);
	m_buttonList.append(b);

	connect( b, SIGNAL(sigAddButton()), this, SLOT(addButton()) );

	connect( b, SIGNAL(sigRemoveButton(const int)), this, SLOT(removeButton(const int)) );

	connect( b, SIGNAL(sigChanged()), SIGNAL(sigChanged()) );
	connect( b, SIGNAL(sigChanged()), SLOT(updateMenuItems()) );

	//b->show(); //according to qt4 docs this does not work: "You should use QAction::setVisible()"
	a->setVisible(true);

	updateMenuItems(); //make sure the items are up to date with the newest module list

	return b;
}

void CModuleChooserBar::addButton( ) {
	addButton(0);
}

//change current with next and remove
/** Removes a button from the toolbar */
void CModuleChooserBar::removeButton( const int ID ) {
	QMutableListIterator<CModuleChooserButton*> it(m_buttonList);
	//for (m_buttonList.first(); m_buttonList.current(); m_buttonList.next()) {
	while (it.hasNext()) {
		if (it.peekNext()->getId() == ID) { //found the right button to remove
			CModuleChooserButton* b = it.next();
			it.remove();
			//TODO: hide does not work; see e.g. QToolBar::addWidget
			//b->hide();
			b->defaultAction()->setVisible(false);
			b->deleteLater();
			break;
		}
	}

	emit sigChanged();

	updateMenuItems(); //make sure the items are up to date with the newest module list
}

/** Returns a list of selected modules. */
ListCSwordModuleInfo CModuleChooserBar::getModuleList() {
	//qt3: const int currentItemIndex = m_buttonList.at(); //make sure we don't change the current list item
	ListCSwordModuleInfo list;
	CSwordModuleInfo* m = 0;

	QListIterator<CModuleChooserButton*> it(m_buttonList);
	while (it.hasNext()) {
		m = it.next()->module();
		if ( m ) {
			list.append( m );
		}
	}
	//qt3: m_buttonList.at( currentItemIndex ); //change current back
	return list;
}

//change current with remove
/** Sets the number of the maximum count of buttons. */
void CModuleChooserBar::setButtonLimit(const int limit) {
	m_buttonLimit = limit;
	if (limit == -1) //no need to delete buttons
		return;

	const int tooMuch = m_buttonList.count() - limit;
	for (int i = 0; i < tooMuch; ++i) {
		//CModuleChooserButton* b = m_buttonList.getLast();
		//m_buttonList.remove(b); //change current to after removed (or new last)
		CModuleChooserButton* b = m_buttonList.takeLast();
		//b->hide();
		b->defaultAction()->setVisible(false);	

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
	//QList<CModuleChooserButton*> buttons = m_buttonList ;
	QListIterator<CModuleChooserButton*> it(m_buttonList);
	while (it.hasNext()) {
		it.next()->updateMenuItems();
	}
}
