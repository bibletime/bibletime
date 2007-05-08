/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cbookkeychooser.h"
#include "backend/cswordtreekey.h"
#include "backend/cswordbookmoduleinfo.h"
#include "frontend/cbtconfig.h"

//Qt includes
#include <qlayout.h>
#include <qmap.h>

QMap<QObject*, int> boxes;

CBookKeyChooser::CBookKeyChooser(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent, const char *name)
: CKeyChooser(modules, key, parent,name), m_layout(0) {

	setModules(modules, false);
	m_key = dynamic_cast<CSwordTreeKey*>(key);
	if (!m_modules.count()) {
		m_key = 0;
	}

	setModules(modules, true);
	setKey(key);

	adjustFont();
}

CBookKeyChooser::~CBookKeyChooser() {}

void CBookKeyChooser::setKey(CSwordKey* newKey) {
	setKey(newKey, true);
}

/** Sets a new key to this keychooser */
void CBookKeyChooser::setKey(CSwordKey* newKey, const bool emitSignal) {
	if (m_key != newKey) {
		m_key = dynamic_cast<CSwordTreeKey*>(newKey);
	}

	/*const */QString oldKey = m_key->key();

	if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
		m_key->firstChild();
		oldKey = m_key->key();
	}
	const int oldOffset = m_key->getOffset();

	QStringList siblings;
	if (m_key && !oldKey.isEmpty()) {
		siblings = QStringList::split("/", oldKey, false);
	}

	int depth = 0;
	int index = 0;

	m_key->root();

	while( m_key->firstChild() && (depth <= int(siblings.count())) ) {
		const QString key = m_key->key();
		index = (depth == 0) ? -1 : 0;
		const QString sibling = siblings[depth];

		if (!sibling.isEmpty()) { //found it
			bool found = false;

			do {
				++index;
				found = (m_key->getLocalName() == sibling);
			}
			while (!found && m_key->nextSibling());

			if (!found) {
				m_key->key( key );
			}
		}

		setupCombo(key, depth, index);
		depth++;
	}

	//clear the combos which were not filled
	for (; depth < m_modules.first()->depth(); ++depth)  {
		CKeyChooserWidget* chooser = m_chooserWidgets.at(depth);
		if (chooser) {
			chooser->reset(0,0,false);
		}
	}

	if (oldKey.isEmpty()) {
		m_key->root();
	}
	else {
		//m_key->key(oldKey);
		m_key->setOffset(oldOffset);
	}

	if (emitSignal) {
		emit keyChanged(m_key);
	}
}

/** Returns the key of this kechooser. */
CSwordKey* const CBookKeyChooser::key() {
	return m_key;
}

/** Sets another module to this keychooser */
void CBookKeyChooser::setModules(const ListCSwordModuleInfo& modules, const bool refresh) {
	m_modules.clear();

	//   for (modules.first(); modules.current(); modules.next()) {
	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for (ListCSwordModuleInfo::const_iterator it(modules.begin()); it != end_it; ++it) {
		if ( (*it)->type() == CSwordModuleInfo::GenericBook ) {
			if (CSwordBookModuleInfo* book = dynamic_cast<CSwordBookModuleInfo*>(*it)) {
				m_modules.append(book);
			}
		}
	}

	//refresh the number of combos
	if (refresh && m_modules.count() && m_key) {
		if (!m_layout) {
			m_layout = new QHBoxLayout(this);
		}

		//delete old widgets
		m_chooserWidgets.setAutoDelete(true);
		m_chooserWidgets.clear();
		m_chooserWidgets.setAutoDelete(false);

		for (int i = 0; i < m_modules.first()->depth(); ++i) {
			// Create an empty keychooser, don't handle next/prev signals
			CKeyChooserWidget* w = new CKeyChooserWidget(0, false, this);
			m_chooserWidgets.append( w );

			//don't allow a too high width, try to keep as narrow as possible
			//to aid users with smaller screen resolutions
			int totalWidth = 200; //only 1 level
			if (m_modules.first()->depth() > 1) {
				if (m_modules.first()->depth() > 3)
					totalWidth = 400; //4+ levels
				else
					totalWidth = 300; //2-3 levels
			}

			int maxWidth = (int) ((float) totalWidth / (float) m_modules.first()->depth());

			w->comboBox()->setMaximumWidth(maxWidth);
			w->comboBox()->setCurrentItem(0);

			connect(w, SIGNAL(changed(int)), SLOT(keyChooserChanged(int)));
			connect(w, SIGNAL(focusOut(int)), SLOT(keyChooserChanged(int)));

			m_layout->addWidget(w);
			boxes[w] = i;

			w->show();
		}

		//set the tab order of the key chooser widgets

		CKeyChooserWidget* chooser = 0;
		CKeyChooserWidget* chooser_prev = 0;
		const int count = m_chooserWidgets.count();
		for (int i = 0; i < count; ++i) {
			chooser = m_chooserWidgets.at(i);
			Q_ASSERT(chooser);

			if (chooser && chooser_prev) {
				QWidget::setTabOrder(chooser_prev, chooser);
			}

			chooser_prev = chooser;
		}
		QWidget::setTabOrder(chooser, 0);

		updateKey(m_key);
		adjustFont(); // only when refresh is set.
	}
}

/** No descriptions */
void CBookKeyChooser::adjustFont() {

	//Make sure the entries are displayed correctly.
	for ( CKeyChooserWidget* idx = m_chooserWidgets.first(); idx; idx = m_chooserWidgets.next() ) {
		idx->comboBox()->setFont( CBTConfig::get
									  ( m_modules.first()->language() ).second );
	}
}

/** Refreshes the content. */
void CBookKeyChooser::refreshContent() {
	if (m_key) {
		updateKey( m_key ); //refresh with current key
	}
}

void CBookKeyChooser::setupCombo(const QString key, const int depth, const int currentItem) {
	CKeyChooserWidget* chooserWidget = m_chooserWidgets.at(depth);

	const unsigned long oldOffset = m_key->getOffset();
	m_key->key(key);

	if ((depth == 0) && chooserWidget && chooserWidget->comboBox()->count()) { //has already items
		//set now the right item
		if (CKeyChooserWidget* chooserWidget = m_chooserWidgets.at(depth)) {
			chooserWidget->setItem( chooserWidget->comboBox()->text(currentItem) );
		}

		m_key->setOffset(oldOffset);
		return;
	}


	//insert an empty item at the top
	QStringList items;
	if (depth > 0) {
		items << QString::null;
	}

	do {
		items << QString::fromLocal8Bit(m_key->getLocalName());
	}
	while (m_key->nextSibling());

	if (chooserWidget) {
		chooserWidget->reset(items,currentItem,false);
	}

	//restore old key
	//  m_key->key(oldKey);
	m_key->setOffset( oldOffset );
}

/** A keychooser changed. Update and emit a signal if necessary. */
void CBookKeyChooser::keyChooserChanged(int /*newIndex*/) {
	const int activeID = boxes[const_cast<QObject*>(sender())]; //no so good code!

	QStringList items;
	CKeyChooserWidget* chooser;
	const int count = m_chooserWidgets.count();

	for (int i = 0; i < count; ++i) {
		chooser = m_chooserWidgets.at(i);
		const QString currentText =
			(chooser && chooser->comboBox())
			? chooser->comboBox()->currentText()
			: QString::null;

		if (currentText.isEmpty() || i > activeID) {
			break;
		}

		items << currentText;
	}

	QString newKey("/");
	newKey.append(items.join("/"));
	if (newKey.length() > 1) {
		newKey.remove(newKey.length(),1); //remove the traling slash
	}

	//  qWarning("key changed: setting to %s", newKey.latin1());
	m_key->key(newKey);
	setKey(m_key);
}

/** Updates the keychoosers for the given key but emit no signal. */
void CBookKeyChooser::updateKey(CSwordKey* key) {
	setKey(key, false);
}

