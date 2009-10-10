/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/cbooktreechooser.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/keys/cswordtreekey.h"
#include "frontend/keychooser/bthistory.h"


CBookTreeChooser::CBookTreeChooser(QList<CSwordModuleInfo*> modules, CSwordKey *key, QWidget *parent)
        : CKeyChooser(modules, key, parent),
        m_key( dynamic_cast<CSwordTreeKey*>(key) ) {

    setModules(modules, false);

    //if there is no module there is no key either
    if (!modules.count()) {
        m_modules.clear();
        m_key = 0;
    }

    //now setup the keychooser widgets
    m_treeView = new QTreeWidget(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_treeView);
    m_treeView->header()->hide();

    //when user selects the item whe must react
    connect(m_treeView, SIGNAL(currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(itemActivated(QTreeWidgetItem*)));

    setKey(key);
    adjustFont();
    connect(this, SIGNAL(keyChanged(CSwordKey*)), history(), SLOT(add(CSwordKey*)) );
}

CBookTreeChooser::~CBookTreeChooser() {}

/** Sets a new key to this keychooser. Inherited from ckeychooser. */
void CBookTreeChooser::setKey(CSwordKey* key) {
    setKey(key, false);
}

/** Sets a new key to this keychooser. Inherited from ckeychooser. */
void CBookTreeChooser::setKey(CSwordKey* newKey, const bool emitSignal) {
    qDebug("CBookTreeChooser::setKey");

    if (m_key != newKey ) {
        m_key = dynamic_cast<CSwordTreeKey*>(newKey);
    }

    const QString key = m_key->key(); //key as text, path

    QTreeWidgetItem* matching_item = m_treeView->topLevelItem(0);

    QTreeWidgetItemIterator it(m_treeView);
    while (*it) {
        if ((*it)->text(1) == key) {
            matching_item = (*it);
            break;
        }
        ++it;
    }

    m_treeView->setCurrentItem( matching_item );
    m_treeView->scrollToItem(matching_item);

    if (emitSignal) {
        emit keyChanged(m_key);
    }
}

/** Returns the key of this keychooser. Inherited from ckeychooser.*/
CSwordKey*  CBookTreeChooser::key() {
    return m_key;
}

/** Sets another module to this keychooser. Inherited from ckeychooser (therefore
the list of modules instead of one). */
void CBookTreeChooser::setModules(const QList<CSwordModuleInfo*>& modules, const bool refresh) {

    //Add given modules into private list
    m_modules.clear();
    QList<CSwordModuleInfo*>::const_iterator end_it = modules.end();
    for (QList<CSwordModuleInfo*>::const_iterator it(modules.begin()); it != end_it; ++it) {
        if (CSwordBookModuleInfo* book = dynamic_cast<CSwordBookModuleInfo*>(*it)) {
            m_modules.append(book);
        }
    }

    //if there exists a module and a key, setup the visible tree
    if (refresh && m_modules.count() && m_key) {
        const uint offset = m_key->getOffset(); //actually unnecessary, taken care of in setupTree
        setupTree();
        m_key->setOffset( offset );

        adjustFont(); //only when refresh is set.
    }
}

/** From ckeychooser. */
void CBookTreeChooser::adjustFont() {
    //Make sure the entries are displayed correctly.
    m_treeView->setFont( CBTConfig::get(m_modules.first()->language()).second );

}


/** Refreshes the content. Inherited from ckeychooser. */
void CBookTreeChooser::refreshContent() {
    if (m_key) {
        updateKey(m_key); //refresh with current key
    }
}


//TODO: itemActivated is called too many times. As tested in GDB, the function
//is called twice with the pointer to the correct book and twice with a null
//pointer.

/** Slot for signal when item is selected by user. */
void CBookTreeChooser::itemActivated( QTreeWidgetItem* item ) {
    qDebug("CBookTreeChooser::itemActivated");
    //Sometimes Qt calls this function with a null pointer.
    if (item) {
        m_key->key(item->text(1));
        //tell possible listeners about the change
        emit keyChanged(m_key);
    }
}

/** Inherited from ckeychooser */
void CBookTreeChooser::updateKey( CSwordKey* key ) {
    setKey(key, false);
}

/** Reimplementation to handle tree creation on show. */
void CBookTreeChooser::show() {
    CKeyChooser::show();

    if (!m_treeView->topLevelItemCount()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        setupTree(); //create the tree structure
        m_treeView->resize(m_treeView->sizeHint());
        QApplication::restoreOverrideCursor();
    }
}

/** Creates the tree structure in the view. */
void CBookTreeChooser::setupTree() {
    m_treeView->clear();

    const unsigned long offset = m_key->getOffset();

    m_key->root();
    addKeyChildren(m_key, m_treeView->invisibleRootItem());

    m_key->setOffset( offset );
    setKey(m_key, false); //the module may have changed
}

/** Populates tree widget with items. */
void CBookTreeChooser::addKeyChildren(CSwordTreeKey* key, QTreeWidgetItem* item) {
    if (key->hasChildren()) {
        key->firstChild();
        do {
            QStringList columns;
            columns << key->getLocalNameUnicode() << key->key();
            QTreeWidgetItem *i = new QTreeWidgetItem(item, columns, QTreeWidgetItem::Type);
            i->setData(0, Qt::ToolTipRole, key->getLocalNameUnicode());
            int offset = key->getOffset();
            addKeyChildren(key, i);
            key->setOffset(offset);
        }
        while (key->nextSibling());
    }
}

void CBookTreeChooser::setKey(QString& newKey) {
    m_key->key(newKey);
    setKey(m_key);
}
