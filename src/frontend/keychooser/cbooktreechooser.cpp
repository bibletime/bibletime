/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cbooktreechooser.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordbookmoduleinfo.h"
#include "../../backend/keys/cswordtreekey.h"
#include "../../util/btconnect.h"
#include "bthistory.h"


CBookTreeChooser::CBookTreeChooser(const BtConstModuleList & modules,
                                   BTHistory * historyPtr,
                                   CSwordKey * key,
                                   QWidget * parent)
    : CKeyChooser(modules, historyPtr, parent)
    , m_key(dynamic_cast<CSwordTreeKey *>(key))
{

    setModules(modules, false);

    //if there is no module there is no key either
    if (!modules.count()) {
        m_modules.clear();
        m_key = nullptr;
    }

    //now setup the keychooser widgets
    m_treeView = new QTreeWidget(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_treeView);
    m_treeView->setHeaderHidden(true);

    //when user selects the item whe must react
    BT_CONNECT(m_treeView,
               SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
               SLOT(itemActivated(QTreeWidgetItem *)));

    setKey(key);
    adjustFont();
    BT_CONNECT(this, SIGNAL(keyChanged(CSwordKey *)),
               history(), SLOT(add(CSwordKey *)));
}

/** Sets a new key to this keychooser. Inherited from ckeychooser. */
void CBookTreeChooser::setKey(CSwordKey* key) {
    setKey(key, false);
}

/** Sets a new key to this keychooser. Inherited from ckeychooser. */
void CBookTreeChooser::setKey(CSwordKey* newKey, const bool emitSignal) {

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

void CBookTreeChooser::setModules(const BtConstModuleList &modules,
                                  bool refresh)
{
    using CSBMI = CSwordBookModuleInfo;

    //Add given modules into private list
    m_modules.clear();
    Q_FOREACH(CSwordModuleInfo const * const m, modules) {
        const CSBMI *book = dynamic_cast<const CSBMI*>(m);
        if (book != nullptr) {
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
    m_treeView->setFont(btConfig().getFontForLanguage(*m_modules.first()->language()).second);
}


/** Refreshes the content. Inherited from ckeychooser. */
void CBookTreeChooser::refreshContent() {
    if (m_key) {
        updateKey(m_key); //refresh with current key
    }
}


/// \todo itemActivated is called too many times. As tested in GDB, the function
//is called twice with the pointer to the correct book and twice with a null
//pointer.

/** Slot for signal when item is selected by user. */
void CBookTreeChooser::itemActivated( QTreeWidgetItem* item ) {
    //Sometimes Qt calls this function with a null pointer.
    if (item) {
        m_key->setKey(item->text(1));
        //tell possible listeners about the change
        emit keyChanged(m_key);
    }
}

/** Inherited from ckeychooser */
void CBookTreeChooser::updateKey( CSwordKey* key ) {
    setKey(key, false);
}

/** Reimplementation to handle tree creation on show. */
void CBookTreeChooser::doShow() {
    show();
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

void CBookTreeChooser::setKey(const QString & newKey) {
    m_key->setKey(newKey);
    setKey(m_key);
}
