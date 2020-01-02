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

#include "btactioncollection.h"

#include "../../util/btconnect.h"
#include "../../util/directory.h"


QAction & BtActionCollection::action(QString const & name) const {
    Item const * const foundItem = findActionItem(name);
    BT_ASSERT(foundItem);
    return *(foundItem->m_action);
}

void BtActionCollection::addAction(QString const & name,
                                   QAction * const action)
{
    BT_ASSERT(action);
    BT_ASSERT(m_actions.find(name) == m_actions.end());
    Item * const item = new Item{action, this};
    try {
        m_actions.insert(name, item);
    } catch (...) {
        delete item;
        throw;
    }
}

void BtActionCollection::addAction(QString const & name,
                                   QObject const * const receiver,
                                   char const * const member)
{
    QAction * const action = new QAction{name, this};
    try {
        if (receiver && member)
            BT_CONNECT(action,   SIGNAL(triggered()),
                       receiver, SLOT(triggered()));
        return addAction(name, action);
    } catch (...) {
        delete action;
        throw;
    }
}

void BtActionCollection::removeAction(QString const & name) {
    #ifndef NDEBUG
    int const r =
    #endif
            m_actions.remove(name);
    BT_ASSERT(r > 0);
}

QKeySequence BtActionCollection::getDefaultShortcut(QAction * action) const {
    for (Item * const item : m_actions)
        if (item->m_action == action)
            return item->m_defaultKeys;
    return QKeySequence{};
}

void BtActionCollection::readShortcuts(QString const & group) {
    BtConfig::ShortcutsMap shortcuts = btConfig().getShortcuts(group);
    for (auto it = shortcuts.begin(); it != shortcuts.end(); ++it)
        if (Item const * const foundItem = findActionItem(it.key()))
            foundItem->m_action->setShortcuts(it.value());
}

void BtActionCollection::writeShortcuts(QString const & group) const {
    BtConfig::ShortcutsMap shortcuts;
    for (auto it = m_actions.begin(); it != m_actions.end(); ++it)
        shortcuts.insert(it.key(), it.value()->m_action->shortcuts());
    btConfig().setShortcuts(group, shortcuts);
}

BtActionCollection::Item * BtActionCollection::findActionItem(
        QString const & name) const
{
    ActionMap::const_iterator const it = m_actions.find(name);
    return (it != m_actions.constEnd()) ? *it : nullptr;
}
