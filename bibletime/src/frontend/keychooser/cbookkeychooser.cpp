/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/cbookkeychooser.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QList>
#include <QWidget>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/keys/cswordtreekey.h"
#include "frontend/keychooser/bthistory.h"


QMap<QObject*, int> boxes;

CBookKeyChooser::CBookKeyChooser(QList<CSwordModuleInfo*> modules, CSwordKey *key, QWidget *parent)
        : CKeyChooser(modules, key, parent), m_layout(0) {

    setModules(modules, false);
    m_key = dynamic_cast<CSwordTreeKey*>(key);
    if (!m_modules.count()) {
        m_key = 0;
    }

    setModules(modules, true);
    setKey(key);

    adjustFont();
    connect(this, SIGNAL(keyChanged(CSwordKey*)), history(), SLOT(add(CSwordKey*)) );
}

CBookKeyChooser::~CBookKeyChooser() {}

void CBookKeyChooser::setKey(CSwordKey* newKey) {
    setKey(newKey, true);
}

/** Sets a new key to this keychooser */
void CBookKeyChooser::setKey(CSwordKey* newKey, const bool emitSignal) {
    if (m_key != newKey) { //set the internal key to the new one
        m_key = dynamic_cast<CSwordTreeKey*>(newKey);
    }

    QString oldKey = m_key->key(); //string backup of key

    if (oldKey.isEmpty()) { //don't set keys equal to "/", always use a key which may have content
        m_key->firstChild();
        oldKey = m_key->key();
    }

    const int oldOffset = m_key->getOffset(); //backup key position

    QStringList siblings; //split up key
    if (m_key && !oldKey.isEmpty()) {
        siblings = oldKey.split('/', QString::SkipEmptyParts);
    }

    int depth = 0;
    int index = 0;

    m_key->root(); //start iteration at root node

    while ( m_key->firstChild() && (depth < siblings.count()) ) {
        QString key = m_key->key();
        index = (depth == 0) ? -1 : 0;

        bool found = false;

        do { //look for matching sibling
            ++index;
            found = (m_key->getLocalNameUnicode() == siblings[depth]);
        }
        while (!found && m_key->nextSibling());

        if (found)
            key = m_key->key(); //found: change key to this level
        else
            m_key->key(key); //not found: restore old key

        setupCombo(key, depth, index);

        //last iteration: check to see if another box can be filled with child entries
        if (depth == siblings.count() - 1 && m_key->hasChildren()) {
            m_key->firstChild();
            setupCombo(m_key->key(), ++depth, 0);
        }

        depth++;
    }

    //clear the combos which were not filled
    for (; depth < m_modules.first()->depth(); ++depth)  {
        CKeyChooserWidget* chooser = m_chooserWidgets.at(depth);
        if (chooser) chooser->reset(0, 0, false);
    }

    if (oldKey.isEmpty()) {
        m_key->root();
    }
    else {
        //m_key->key(oldKey);
        m_key->setOffset(oldOffset);
    }

    if (emitSignal) emit keyChanged(m_key);
}

/** Returns the key of this kechooser. */
CSwordKey* CBookKeyChooser::key() {
    return m_key;
}

/** Sets another module to this keychooser */
void CBookKeyChooser::setModules(const QList<CSwordModuleInfo*>& modules, const bool refresh) {
    m_modules.clear();

    //   for (modules.first(); modules.current(); modules.next()) {
    QList<CSwordModuleInfo*>::const_iterator end_it = modules.end();
    for (QList<CSwordModuleInfo*>::const_iterator it(modules.begin()); it != end_it; ++it) {
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
            m_layout->setSpacing(0);
            m_layout->setContentsMargins(0, 0, 0, 0);
        }

        qDeleteAll(m_chooserWidgets);
        m_chooserWidgets.clear();

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
            w->comboBox()->setCurrentIndex(0);

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
    QListIterator<CKeyChooserWidget*> it(m_chooserWidgets);
    while (it.hasNext()) {
        it.next()->comboBox()->setFont( CBTConfig::get( m_modules.first()->language() ).second );
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

    CSwordTreeKey tmpKey(*m_key);
    tmpKey.key(key);
    tmpKey.parent();
    tmpKey.firstChild();

    QStringList items;
    if (depth > 0) items << QString::null; //insert an empty item at the top

    do {
        items << tmpKey.getLocalNameUnicode();
    }
    while (tmpKey.nextSibling());

    if (chooserWidget) chooserWidget->reset(items, currentItem, false);
}

/** A keychooser changed. Update and emit a signal if necessary. */
void CBookKeyChooser::keyChooserChanged(int /*newIndex*/) {
    const int activeID = boxes[const_cast<QObject*>(sender())]; //no so good code!

    QStringList items;
    CKeyChooserWidget* chooser;

    for (int i = 0; i < m_chooserWidgets.count(); ++i) {
        chooser = m_chooserWidgets.at(i);
        const QString currentText = (chooser && chooser->comboBox()) ? chooser->comboBox()->currentText() : QString::null;

        if (currentText.isEmpty() || i > activeID) {
            break;
        }

        items << currentText;
    }

    QString newKey("/");
    newKey.append(items.join("/"));

    m_key->key(newKey);
    setKey(m_key);
}

/** Updates the keychoosers for the given key but emit no signal. */
void CBookKeyChooser::updateKey(CSwordKey* key) {
    setKey(key, false);
}

void CBookKeyChooser::setKey(QString& newKey) {
    m_key->key(newKey);
    setKey(m_key);
}
