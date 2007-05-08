/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cbooktreechooser.h"
#include "backend/cswordtreekey.h"
#include "backend/cswordbookmoduleinfo.h"
#include "frontend/cbtconfig.h"

//Qt includes
#include <qlayout.h>
#include <qheader.h>
#include <qlistview.h>
#include <qcursor.h>

//KDE includes
#include <kapplication.h>

////////////

CBookTreeChooser::TreeItem::TreeItem(QListViewItem* parent, QListViewItem* after, CSwordTreeKey* key, const QString keyName)
: KListViewItem(parent, after),
m_key(key),
m_keyName(keyName) {
	const unsigned long offset = m_key->getOffset();

	m_key->key(m_keyName);
	setText(0, QString(m_key->getLocalName()) );

	m_key->setOffset( offset );
};

CBookTreeChooser::TreeItem::TreeItem(QListViewItem* parent,CSwordTreeKey* key, const QString keyName)
: KListViewItem(parent),
m_key(key),
m_keyName(keyName) {
	const unsigned int offset = m_key->getOffset();

	m_key->key(m_keyName);
	setText(0, QString(m_key->getLocalName()) );

	m_key->setOffset( offset );
};

CBookTreeChooser::TreeItem::TreeItem(QListView* view, QListViewItem* after,CSwordTreeKey* key, const QString keyName)
: KListViewItem(view,after),
m_key(key),
m_keyName(keyName) {
	const unsigned int offset = m_key->getOffset();

	m_key->key(m_keyName);
	setText(0, QString(m_key->getLocalName()) );

	m_key->setOffset( offset );
};

const QString& CBookTreeChooser::TreeItem::key() const {
	return m_keyName;
};

/** Initializes this item with the correct caption. */
void CBookTreeChooser::TreeItem::setup() {
	const unsigned int offset = m_key->getOffset();

	m_key->key(m_keyName);
	setExpandable(m_key->hasChildren());

	m_key->setOffset( offset );

	KListViewItem::setup();
}

void CBookTreeChooser::TreeItem::createChilds() {
	//make sure that we don't change the status of the key!

	const unsigned long offset = m_key->getOffset();

	m_key->key(m_keyName);

	if (m_key->hasChildren()) {
		m_key->firstChild(); //go to the first child

		QListViewItem* oldItem = 0;

		do {
			if (oldItem) {
				oldItem = new TreeItem(this, oldItem, m_key, m_key->key());
			}
			else {
				oldItem = new TreeItem(this, m_key, m_key->key());
			}

			//    oldItem->setExpandable( m_key->hasChildren() );
		}
		while (m_key->nextSibling());
	}

	m_key->setOffset( offset ); //restore the old state
}

void CBookTreeChooser::TreeItem::setOpen(bool o) {
	//setup the tree under this item
	if ((!isOpen() && o) && childCount() == 0) {
		listView()->viewport()->setCursor(WaitCursor);
		createChilds();
		listView()->viewport()->unsetCursor();
	}

	KListViewItem::setOpen(o);
}

//////////////////////////////////

CBookTreeChooser::CBookTreeChooser(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent, const char *name)
: CKeyChooser(modules, key, parent,name),
m_key( dynamic_cast<CSwordTreeKey*>(key) ) {
	setModules(modules, false);

	if (!modules.count()) {
		m_modules.clear();
		m_key = 0;
	}

	//now setup the keychooser widgets
	QHBoxLayout* layout = new QHBoxLayout(this);

	m_treeView = new KListView(this);
	layout->addWidget(m_treeView);
	m_treeView->addColumn("Tree");
	m_treeView->header()->hide();
	m_treeView->setSorting(-1);
	m_treeView->setRootIsDecorated(true);
	m_treeView->setFullWidth(true);
	connect(m_treeView, SIGNAL(executed(QListViewItem*)), SLOT(itemActivated(QListViewItem*)));

	setKey(key);
	adjustFont();
}

CBookTreeChooser::~CBookTreeChooser() {}

/** Sets a new key to this keychooser */
void CBookTreeChooser::setKey(CSwordKey* key/*newKey*/) {
	setKey(key, false);
}

/** Sets a new key to this keychooser */
void CBookTreeChooser::setKey(CSwordKey* newKey, const bool emitSignal) {
	if (m_key != newKey ) {
		m_key = dynamic_cast<CSwordTreeKey*>(newKey);
	}

	const QString key = m_key->key();

	QStringList siblings;
	if (m_key && !key.isEmpty()) {
		siblings = QStringList::split("/",key,false);
	}

	//find the right listview item
	const int count = siblings.count();
	int index = 0;
	QString currentSibling = siblings[index];

	QListViewItem* child = m_treeView->firstChild();
	while( child && index < count ) {
		if (child->text(0) == currentSibling) { //found a parent of our item
			//found right entry?
			TreeItem* i = dynamic_cast<TreeItem*>(child);
			if (!i || i->key() == key) {
				break;
			}
			i->setOpen(true); //automatically creates childs
			child = i->firstChild();
			currentSibling = siblings[++index];
		}
		else {
			child = child->nextSibling();
		}
	}

	m_treeView->setCurrentItem( child );
	m_treeView->setSelected( child, true );
	m_treeView->ensureItemVisible(child);

	if (emitSignal) {
		emit keyChanged(m_key);
	}
}

/** Returns the key of this kechooser. */
CSwordKey* const CBookTreeChooser::key() {
	return m_key;
}

/** Sets another module to this keychooser */
void CBookTreeChooser::setModules(const ListCSwordModuleInfo& modules, const bool refresh) {
	m_modules.clear();

	//   for (modules.first(); modules.current(); modules.next()) {
	ListCSwordModuleInfo::const_iterator end_it = modules.end();
	for (ListCSwordModuleInfo::const_iterator it(modules.begin()); it != end_it; ++it) {
		if (CSwordBookModuleInfo* book = dynamic_cast<CSwordBookModuleInfo*>(*it)) {
			m_modules.append(book);
		}
	}

	if (refresh && m_modules.count() && m_key) {
		const uint offset = m_key->getOffset();
		setupTree();
		m_key->setOffset( offset );

		/*  m_key->root();
		  m_key->firstChild();
		  setKey(m_key, true);*/

		//  m_treeView->clear();
		//
		//    const QString oldKey = m_key->key();
		//  m_key->root();
		//  m_key->firstChild();
		//  setupTree(0,0,m_key);
		//
		//    m_key->key(oldKey);
		//
		//  updateKey(m_key);
		adjustFont(); //only when refresh is set.
	}
}

/** No descriptions */
void CBookTreeChooser::adjustFont() {
	//Make sure the entries are displayed correctly.
	// if ( m_modules.first()->isUnicode() ){
	m_treeView->setFont( CBTConfig::get
							 (m_modules.first()->language()).second );
	//  qWarning("Unicode Book detected");
	// }
	// else{
	//  m_treeView->setFont( CBTConfig::get(CBTConfig::standard) );
	//  qWarning("Standard Book detected");
	// }
}


/** Refreshes the content. */
void CBookTreeChooser::refreshContent() {
	if (m_key) {
		updateKey( m_key ); //refresh with current key
	}
}

/** No descriptions */
void CBookTreeChooser::itemActivated( QListViewItem* item ) {
	TreeItem* i = dynamic_cast<TreeItem*>(item);
	if (!i) {
		return;
	}

	m_key->key( i->key() );

	if (i->childCount() > 0 || i->isExpandable()) {
		i->setOpen(true);
	}

	emit keyChanged(m_key);
}

/** No descriptions */
void CBookTreeChooser::updateKey( CSwordKey* key ) {
	setKey(key, false);
}

/** Reimplementationm to handle tree creation on show. */
void CBookTreeChooser::show() {
	CKeyChooser::show();

	if (!m_treeView->childCount()) {
		KApplication::setOverrideCursor(WaitCursor);
		setupTree(); //create the first level of the tree structure
		m_treeView->resize(m_treeView->sizeHint());
		KApplication::restoreOverrideCursor();
	}
}

/** Creates the first level of the tree structure. */
void CBookTreeChooser::setupTree() {
	m_treeView->clear();

	const unsigned long offset = m_key->getOffset();
	m_key->root();

	if (m_key->hasChildren()) {
		QListViewItem* item = 0;

		m_key->firstChild();
		do {
			//the TreeItem constructor doesn't change the state of the key
			item = new TreeItem(m_treeView, item, m_key, m_key->key());
		}
		while (m_key->nextSibling());
	}

	/*   m_key->root();
	   m_key->firstChild();*/
	m_key->setOffset( offset );
	setKey(m_key, false); //the module may have changed
}
