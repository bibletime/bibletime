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

#include "cbookkeychooser.h"

#include <algorithm>
#include <memory>
#include <QComboBox>
#include <QHBoxLayout>
#include <QStringList>
#include <Qt>
#include <QtAlgorithms>
#include <QtGlobal>
#include <QVariant>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordbookmoduleinfo.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/keys/cswordkey.h"
#include "../../backend/keys/cswordtreekey.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "ckeychooserwidget.h"


#define ID_PROPERTY_NAME "CBookKeyChooser_ID"

CBookKeyChooser::CBookKeyChooser(const BtConstModuleList & modules,
                                 CSwordKey * key,
                                 QWidget * parent)
    : CKeyChooser(parent)
    , m_layout(nullptr)
{
    m_key = dynamic_cast<CSwordTreeKey * >(key);
    setModules(modules, true);
    setKey(key);

    adjustFont();
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
        m_key->positionToFirstChild();
        oldKey = m_key->key();
    }

    auto const oldOffset = m_key->offset();

    QStringList siblings; // Split up key
    if (m_key && !oldKey.isEmpty())
        siblings = oldKey.split('/', Qt::SkipEmptyParts);

    int depth = 0;

    m_key->positionToRoot(); //start iteration at root node

    while (m_key->positionToFirstChild() && (depth < siblings.count())) {
        QString key = m_key->key();
        int index = (depth == 0) ? -1 : 0;

        bool found = false;

        do { //look for matching sibling
            ++index;
            found = (m_key->getLocalNameUnicode() == siblings[depth]);
        } while (!found && m_key->positionToNextSibling());

        if (found) {
            key = m_key->key(); //found: change key to this level
        } else {
            m_key->setKey(key); //not found: restore old key
        }

        setupCombo(key, depth, index);

        //last iteration: check to see if another box can be filled with child entries
        if (depth == siblings.count() - 1 && m_key->hasChildren()) {
            m_key->positionToFirstChild();
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
        m_key->positionToRoot();
    } else {
        //m_key->key(oldKey);
        m_key->setOffset(oldOffset);
    }

    if (emitSignal)
        Q_EMIT keyChanged(m_key);
}

/** Returns the key of this kechooser. */
CSwordKey * CBookKeyChooser::key() {
    return m_key;
}

/** Sets another module to this keychooser */
void CBookKeyChooser::setModules(const BtConstModuleList & modules,
                                 bool refresh)
{
    m_modules.clear();

    //   for (modules.first(); modules.current(); modules.next()) {
    for (auto const * const m : modules)
        if (m->type() == CSwordModuleInfo::GenericBook)
            m_modules.append(static_cast<CSwordBookModuleInfo const *>(m));

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

            BT_CONNECT(w, &CKeyChooserWidget::changed,
                       this, &CBookKeyChooser::keyChooserChanged);
            BT_CONNECT(w, &CKeyChooserWidget::focusOut,
                       this, &CBookKeyChooser::keyChooserChanged);

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
    auto const & font =
        btConfig().getFontForLanguage(*m_modules.first()->language()).second;
    for (CKeyChooserWidget * const w : m_chooserWidgets)
        w->comboBox().setFont(font);
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
    tmpKey.positionToParent();
    tmpKey.positionToFirstChild();

    QStringList items;
    if (depth > 0)
        items.append(QString()); // Insert an empty item at the top

    do {
        items.append(tmpKey.getLocalNameUnicode());
    } while (tmpKey.positionToNextSibling());

    if (chooserWidget)
        chooserWidget->reset(items, currentItem, false);
}

/** A keychooser changed. Update and emit a signal if necessary. */
void CBookKeyChooser::keyChooserChanged(int newIndex) {
    Q_UNUSED(newIndex)
    QStringList items;

    const int max = std::min(static_cast<int>(m_chooserWidgets.size()),
                             sender()->property(ID_PROPERTY_NAME).toInt());
    for (int i = 0; i < max; i++) {
        CKeyChooserWidget * const chooser = m_chooserWidgets.at(i);
        BT_ASSERT(chooser);
        const QString currentText = chooser->comboBox().currentText();
        if (currentText.isEmpty())
            break;

        items.append(currentText);
    }

    m_key->setKey('/' + items.join('/'));
    setKey(m_key);
}

/** Updates the keychoosers for the given key but emit no signal. */
void CBookKeyChooser::updateKey(CSwordKey * key) {
    setKey(key, false);
}
