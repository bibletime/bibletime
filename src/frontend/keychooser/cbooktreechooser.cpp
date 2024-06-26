/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cbooktreechooser.h"

#include <memory>
#include <QApplication>
#include <QCursor>
#include <QHBoxLayout>
#include <QStringList>
#include <Qt>
#include <QTreeWidgetItemIterator>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordbookmoduleinfo.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/keys/cswordkey.h"
#include "../../backend/keys/cswordtreekey.h"
#include "../../util/btconnect.h"


CBookTreeChooser::CBookTreeChooser(const BtConstModuleList & modules,
                                   CSwordKey * key,
                                   QWidget * parent)
    : CKeyChooser(parent)
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
    BT_CONNECT(m_treeView, &QTreeWidget::currentItemChanged,
               this, &CBookTreeChooser::itemActivated);

    setKey(key);
    adjustFont();
}

/** Sets a new key to this keychooser. Inherited from ckeychooser. */
void CBookTreeChooser::setKey(CSwordKey * newKey) {
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
}

void CBookTreeChooser::setModules(const BtConstModuleList &modules,
                                  bool refresh)
{
    using CSBMI = CSwordBookModuleInfo;

    //Add given modules into private list
    m_modules.clear();
    for (auto const * const m : modules) {
        const CSBMI *book = dynamic_cast<const CSBMI*>(m);
        if (book != nullptr) {
            m_modules.append(book);
        }
    }

    //if there exists a module and a key, setup the visible tree
    if (refresh && m_modules.count() && m_key) {
        auto const offset = m_key->offset(); //actually unnecessary, taken care of in setupTree
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
        Q_EMIT keyChanged(m_key);
    }
}

/** Inherited from ckeychooser */
void CBookTreeChooser::updateKey( CSwordKey* key ) {
    setKey(key);
}

/** Reimplementation to handle tree creation on show. */
void CBookTreeChooser::showEvent(QShowEvent * const showEvent) {
    if (!m_treeView->topLevelItemCount()) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        setupTree(); //create the tree structure
        m_treeView->resize(m_treeView->sizeHint());
        QApplication::restoreOverrideCursor();
    }
    CKeyChooser::showEvent(showEvent);
}

/** Creates the tree structure in the view. */
void CBookTreeChooser::setupTree() {
    m_treeView->clear();

    auto const offset = m_key->offset();

    m_key->positionToRoot();
    addKeyChildren(m_key, m_treeView->invisibleRootItem());

    m_key->setOffset( offset );
    setKey(m_key); // the module may have changed
}

/** Populates tree widget with items. */
void CBookTreeChooser::addKeyChildren(CSwordTreeKey* key, QTreeWidgetItem* item) {
    if (key->hasChildren()) {
        key->positionToFirstChild();
        do {
            QStringList columns;
            columns << key->getLocalNameUnicode() << key->key();
            QTreeWidgetItem *i = new QTreeWidgetItem(item, columns, QTreeWidgetItem::Type);
            i->setData(0, Qt::ToolTipRole, key->getLocalNameUnicode());
            auto const offset = key->offset();
            addKeyChildren(key, i);
            key->setOffset(offset);
        }
        while (key->positionToNextSibling());
    }
}
