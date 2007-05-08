/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cbiblekeychooser.h"
#include "ckeyreferencewidget.h"
#include "cscrollbutton.h"

#include "backend/cswordversekey.h"
#include "backend/cswordbiblemoduleinfo.h"
#include "backend/cswordmoduleinfo.h"

#include "util/cresmgr.h"

//Qt includes
#include <qcombobox.h>
#include <qlistbox.h>
#include <qpushbutton.h>

//KDE includes
#include <klocale.h>

CBibleKeyChooser::CBibleKeyChooser(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent, const char *name )
: CKeyChooser(modules, key, parent, name),
m_key(dynamic_cast<CSwordVerseKey*>(key)) {
	w_ref = 0;
	setModules(modules, false);
	if (!m_modules.count()) {
		qWarning("CBibleKeyChooser: module is not a Bible or commentary!");
		m_key = 0;
		return;
	}
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setDirection( QBoxLayout::LeftToRight );

	w_ref = new CKeyReferenceWidget(dynamic_cast<CSwordBibleModuleInfo*>(m_modules.first()), m_key, this);
	layout->addWidget(w_ref);

	connect(w_ref,SIGNAL(changed(CSwordVerseKey *)),SLOT(refChanged(CSwordVerseKey *)));

	setKey(m_key); //set the key without changing it, setKey(key()) would change it
}

CSwordKey* const CBibleKeyChooser::key() {
	return m_key;
}

void CBibleKeyChooser::setKey(CSwordKey* key) {
	Q_ASSERT(dynamic_cast<CSwordVerseKey*>(key));
	if (dynamic_cast<CSwordVerseKey*>(key) == 0) {
		return;
	}

	m_key = dynamic_cast<CSwordVerseKey*>(key);
	emit (beforeKeyChange(m_key->key())); //required to make direct setKey calls work from the outside

	w_ref->setKey(m_key);

	emit keyChanged(m_key);
}

void CBibleKeyChooser::refChanged(CSwordVerseKey* key) {
	Q_ASSERT(m_key);
	Q_ASSERT(key);

	if (!isUpdatesEnabled())
		return;

	setUpdatesEnabled(false);
	if (m_key)
		emit beforeKeyChange(m_key->key());

	m_key = key;
	emit keyChanged(m_key);

	setUpdatesEnabled(true);
}

/** Sets te module and refreshes the combos */
void CBibleKeyChooser::setModules(const ListCSwordModuleInfo& modules, const bool refresh) {
	m_modules.clear();

	//   for (modules.first(); modules.current(); modules.next()) {
	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for (ListCSwordModuleInfo::const_iterator it(modules.begin()); it != end_it; ++it) {
		if ((*it)->type() == CSwordModuleInfo::Bible || (*it)->type() == CSwordModuleInfo::Commentary) {
			if (CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(*it)) {
				m_modules.append(bible);
			}
		}
	}

	// First time this is called we havnt set up w_ref.
	if (w_ref) w_ref->setModule(dynamic_cast<CSwordBibleModuleInfo*>(m_modules.first()));
	if (refresh) refreshContent();
}

void CBibleKeyChooser::refreshContent() {
	setKey (m_key);
}


/** No descriptions */
void CBibleKeyChooser::updateKey(CSwordKey* /*key*/) {}
void CBibleKeyChooser::adjustFont() {}
