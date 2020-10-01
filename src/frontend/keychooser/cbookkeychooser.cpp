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

#include "cbookkeychooser.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QList>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/cswordbookmoduleinfo.h"
#include "../../backend/keys/cswordtreekey.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "bthistory.h"


#define ID_PROPERTY_NAME "CBookKeyChooser_ID"

CBookKeyChooser::CBookKeyChooser(const BtConstModuleList & modules,
                                 BTHistory * historyPtr,
                                 CSwordKey * key,
                                 QWidget * parent)
    : CKeyChooser(modules, historyPtr, parent)
    , m_layout(nullptr)
{
    setModules(modules, false);
    m_key = dynamic_cast<CSwordTreeKey * >(key);
    if (!m_modules.count())
        m_key = nullptr;

    setModules(modules, true);
    setKey(key);

    adjustFont();
    BT_CONNECT(this,      SIGNAL(keyChanged(CSwordKey *)),
               history(), SLOT(add(CSwordKey *)));
}

void CBookKeyChooser::setKey(CSwordKey * newKey) {
    setKey(newKey, true);
}

/** Sets a new key to this keychooser */
void CBookKeyChooser::setKey(CSwordKey * newKey, const bool emitSignal) {
    if (m_key != newKey) // Set the internal key to the new one
        m_key = dynamic_cast<CSwordTreeKey*>(newKey);

    QString oldKey(m_key->key());

    if (oldKey.isEmpty()) { // Don't set keys equal to "/", always use a key which may have content
        m_key->firstChild();
        oldKey = m_key->key();
    }

    const int oldOffset = m_key->getOffset();

    QStringList siblings; // Split up key
    if (m_key && !oldKey.isEmpty())
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        siblings = oldKey.split('/', QString::SkipEmptyParts);
#else
        siblings = oldKey.split('/', Qt::SkipEmptyParts);
#endif

    int depth = 0;

    m_key->root(); //start iteration at root node

    while (m_key->firstChild() && (depth < siblings.count())) {
        QString key = m_key->key();
        int index = (depth == 0) ? -1 : 0;

        bool found = false;

        do { //look for matching sibling
            ++index;
            found = (m_key->getLocalNameUnicode() == siblings[depth]);
        } while (!found && m_key->nextSibling());

        if (found) {
            key = m_key->key(); //found: change key to this level
        } else {
            m_key->setKey(key); //not found: restore old key
        }

        setupCombo(key, depth, index);

        //last iteration: check to see if another box can be filled with child entries
        if (depth == siblings.count() - 1 && m_key->hasChildren()) {
            m_key->firstChild();
            setupCombo(m_key->key(), ++depth, 0);
        }

        depth++;
    }

    //clear the combos which were not filled
    for (; depth < m_modules.first()->depth(); ++depth) {
        CKeyChooserWidget * const chooser = m_chooserWidgets.at(depth);
        if (chooser)
            chooser->reset(0, 0, false);
    }

    if (oldKey.isEmpty()) {
        m_key->root();
    } else {
        //m_key->key(oldKey);
        m_key->setOffset(oldOffset);
    }

    if (emitSignal)
        emit keyChanged(m_key);
}

/** Returns the key of this kechooser. */
CSwordKey * CBookKeyChooser::key() {
    return m_key;
}

/** Sets another module to this keychooser */
void CBookKeyChooser::setModules(const BtConstModuleList & modules,
                                 bool refresh)
{
    using CSBMI = CSwordBookModuleInfo;
    m_modules.clear();

    //   for (modules.first(); modules.current(); modules.next()) {
    Q_FOREACH(const CSwordModuleInfo * const m, modules) {
        if (m->type() == CSwordModuleInfo::GenericBook ) {
            const CSBMI * const book = dynamic_cast<const CSBMI *>(m);
            if (book != nullptr)
                m_modules.append(book);
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
            CKeyChooserWidget * const w = new CKeyChooserWidget(0, this);
            m_chooserWidgets.append(w);

            //don't allow a too high width, try to keep as narrow as possible
            //to aid users with smaller screen resolutions
            int totalWidth = 200; //only 1 level
            if (m_modules.first()->depth() > 1) {
                if (m_modules.first()->depth() > 3) {
                    totalWidth = 400; //4+ levels
                } else {
                    totalWidth = 300; //2-3 levels
                }
            }

            int maxWidth = static_cast<int>(static_cast<float>(totalWidth)
                                            / m_modules.first()->depth());

            w->comboBox().setMaximumWidth(maxWidth);
            w->comboBox().setCurrentIndex(0);

            BT_CONNECT(w, SIGNAL(changed(int)),  SLOT(keyChooserChanged(int)));
            BT_CONNECT(w, SIGNAL(focusOut(int)), SLOT(keyChooserChanged(int)));

            m_layout->addWidget(w);
            w->setProperty(ID_PROPERTY_NAME, i+1);
            w->show();
        }

        //set the tab order of the key chooser widgets

        CKeyChooserWidget * chooser = nullptr;
        CKeyChooserWidget * chooser_prev = nullptr;
        const int count = m_chooserWidgets.count();
        for (int i = 0; i < count; i++) {
            chooser = m_chooserWidgets.at(i);
            BT_ASSERT(chooser);
            if (chooser_prev)
                QWidget::setTabOrder(chooser_prev, chooser);

            chooser_prev = chooser;
        }
        QWidget::setTabOrder(chooser, nullptr);

        updateKey(m_key);
        adjustFont(); // only when refresh is set.
    }
}

/** No descriptions */
void CBookKeyChooser::adjustFont() {
    //Make sure the entries are displayed correctly.
    QListIterator<CKeyChooserWidget *> it(m_chooserWidgets);
    while (it.hasNext())
        it.next()->comboBox().setFont(btConfig().getFontForLanguage(*m_modules.first()->language()).second);
}

/** Refreshes the content. */
void CBookKeyChooser::refreshContent() {
    if (m_key)
        updateKey(m_key); // Refresh with current key
}

void CBookKeyChooser::setupCombo(const QString & key,
                                 const int depth,
                                 const int currentItem)
{
    CKeyChooserWidget * const chooserWidget = m_chooserWidgets.at(depth);

    CSwordTreeKey tmpKey(*m_key);
    tmpKey.setKey(key);
    tmpKey.sword::TreeKeyIdx::parent();
    tmpKey.firstChild();

    QStringList items;
    if (depth > 0)
        items.append(QString()); // Insert an empty item at the top

    do {
        items.append(tmpKey.getLocalNameUnicode());
    } while (tmpKey.nextSibling());

    if (chooserWidget)
        chooserWidget->reset(items, currentItem, false);
}

/** A keychooser changed. Update and emit a signal if necessary. */
void CBookKeyChooser::keyChooserChanged(int newIndex) {
    Q_UNUSED(newIndex);
    QStringList items;

    const int max = std::min(m_chooserWidgets.count(),
                             sender()->property(ID_PROPERTY_NAME).toInt());
    for (int i = 0; i < max; i++) {
        CKeyChooserWidget * const chooser = m_chooserWidgets.at(i);
        BT_ASSERT(chooser);
        const QString currentText = chooser->comboBox().currentText();
        if (currentText.isEmpty())
            break;

        items.append(currentText);
    }

    QString newKey("/");
    newKey.append(items.join("/"));

    m_key->setKey(newKey);
    setKey(m_key);
}

/** Updates the keychoosers for the given key but emit no signal. */
void CBookKeyChooser::updateKey(CSwordKey * key) {
    setKey(key, false);
}

void CBookKeyChooser::setKey(const QString & newKey) {
    m_key->setKey(newKey);
    setKey(m_key);
}
