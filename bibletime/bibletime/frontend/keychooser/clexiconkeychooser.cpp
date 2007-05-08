/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "clexiconkeychooser.h"
#include "ckeychooserwidget.h"
#include "cscrollbutton.h"

#include "backend/cswordlexiconmoduleinfo.h"
#include "frontend/cbtconfig.h"

#include "util/cresmgr.h"

//STL headers
#include <algorithm>
#include <iterator>
#include <map>

//Qt includes
#include <qcombobox.h>
#include <qlayout.h>
#include <qlistbox.h>

//KDE includes
#include <klocale.h>

CLexiconKeyChooser::CLexiconKeyChooser(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent, const char *name )
: CKeyChooser(modules, key, parent, name),
m_key(dynamic_cast<CSwordLDKey*>(key)) {

	setModules(modules, false);

	//we use a layout because the key chooser should be resized to full size
	m_layout = new QHBoxLayout(this, QBoxLayout::LeftToRight);
	m_layout->setResizeMode(QLayout::FreeResize);

	m_widget = new CKeyChooserWidget(0, false, this);

	//don't allow a too high width, try to keep as narrow as possible
	//to aid users with smaller screen resolutions
	m_widget->comboBox()->setMaximumWidth(200);

	m_widget->setToolTips(
		CResMgr::displaywindows::lexiconWindow::entryList::tooltip,
		CResMgr::displaywindows::lexiconWindow::nextEntry::tooltip,
		CResMgr::displaywindows::general::scrollButton::tooltip,
		CResMgr::displaywindows::lexiconWindow::previousEntry::tooltip
	);

	m_layout->addWidget(m_widget,0,Qt::AlignLeft);

	connect(m_widget,SIGNAL(changed(int)),SLOT(activated(int)));
	connect(m_widget,SIGNAL(focusOut(int)),SLOT(activated(int)));

	setModules(modules, true);
	setKey(key);
}

CSwordKey* const CLexiconKeyChooser::key() {
	//  qWarning("key");
	return m_key;
}

void CLexiconKeyChooser::setKey(CSwordKey* key) {
	if (!(m_key = dynamic_cast<CSwordLDKey*>(key))) {
		return;
	}

	//  qWarning("setKey start");
	QString newKey = m_key->key();
	const int index = m_widget->comboBox()->listBox()->index(m_widget->comboBox()->listBox()->findItem( newKey ));
	m_widget->comboBox()->setCurrentItem(index);

	//   qWarning("setKey end");
	emit keyChanged( m_key );
}

void CLexiconKeyChooser::activated(int index) {
	//  qWarning("activated");
	const QString text = m_widget->comboBox()->text(index);

	// To prevent from eternal loop, because activated() is emitted again
	if (m_key && m_key->key() != text) {
		m_key->key(text);
		setKey(m_key);
	}
	//  qWarning("activated end");
}

inline const bool my_cmpEntries(const QString& a, const QString& b) {
	return a < b;
};

/** Reimplementation. */
void CLexiconKeyChooser::refreshContent() {
	if (m_modules.count() == 1) {
		m_widget->reset(m_modules.first()->entries(), 0, true);
		//     qWarning("resetted");
	}
	else {
		typedef std::multimap<unsigned int, QStringList*> EntryMap;
		EntryMap entryMap;
		QStringList* entries = 0;
		for (m_modules.first(); m_modules.current(); m_modules.next()) {
			entries = m_modules.current()->entries();
			entryMap.insert( std::make_pair(entries->count(), entries) );
		}

		QStringList goodEntries; //The string list which contains the entries which are available in all modules

		EntryMap::iterator it = entryMap.begin(); //iterator to go thoigh all selected modules
		QStringList refEntries = *(it->second); //copy the items for the first time
		QStringList* cmpEntries = ( ++it )->second; //list for comparision, starts with the second module in the map

		while(it != entryMap.end()) {
			std::set_intersection(
				refEntries.begin(), --(refEntries.end()), //--end() is the last valid entry
				cmpEntries->begin(), --(cmpEntries->end()),
				std::back_inserter(goodEntries), //append valid entries to the end of goodEntries
				my_cmpEntries  //ci_cmpEntries is the comparision function
			);

			cmpEntries = ( ++it )->second; //this is a pointer to the string list of a new module

			/*
			* use the good entries for next comparision,
			* because the final list can only have the entries of goodEntries as maxiumum
			*/
			refEntries = goodEntries;
		};

		m_widget->reset(goodEntries, 0, true); //write down the entries
	} //end of ELSE

	//make sure the list sorted
	/* This is not the best solution, module()->entries() should be sorted already */
	//   Q_ASSERT(m_widget->comboBox()->listBox());
	//   m_widget->comboBox()->listBox()->sort();
}

/** No descriptions */
void CLexiconKeyChooser::adjustFont() {
	//  //Make sure the entries are displayed correctly.
	//   m_widget->comboBox()->setFont( CBTConfig::get( m_modules.first()->language() ).second );
}

/** Sets the module and refreshes the combo boxes */
void CLexiconKeyChooser::setModules( const ListCSwordModuleInfo& modules, const bool refresh ) {
	Q_ASSERT(!m_modules.autoDelete());
	m_modules.clear();
	Q_ASSERT(!m_modules.autoDelete());

	//   for (modules.first(); modules.current(); modules.next()) {
	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for (ListCSwordModuleInfo::const_iterator it(modules.begin()); it != end_it; ++it) {
		CSwordLexiconModuleInfo* lexicon = dynamic_cast<CSwordLexiconModuleInfo*>(*it);
		if (lexicon) {
			m_modules.append(lexicon);
		}
	}

	if (refresh) {
		refreshContent();
		//   adjustFont();
	}
}

/** No descriptions */
void CLexiconKeyChooser::updateKey(CSwordKey*) {}
