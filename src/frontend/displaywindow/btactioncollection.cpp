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

#include "btactioncollection.h"

#include <QHash>
#include "../../backend/config/btconfig.h"
#include "../../util/btassert.h"

namespace {

char const defaultShortcutPropertyName[] = "BtDefaultShortcut";

} // anonymous namespace

QAction & BtActionCollection::action(QString const & name) const {
    auto const it = m_actions.find(name);
    BT_ASSERT(it != m_actions.end());
    return **it;
}

void BtActionCollection::addAction(QString const & name,
                                   QAction * const action)
{
    BT_ASSERT(action);
    BT_ASSERT(m_actions.find(name) == m_actions.end());
    auto const it = m_actions.insert(name, action);
    try {
        action->setProperty(defaultShortcutPropertyName, action->shortcut());
    } catch (...) {
        m_actions.erase(it);
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

QKeySequence BtActionCollection::getDefaultShortcut(QAction & action)
{ return action.property(defaultShortcutPropertyName).value<QKeySequence>(); }

void BtActionCollection::readShortcuts(QString const & group) {
    BtConfig::ShortcutsMap shortcuts = btConfig().getShortcuts(group);
    for (auto it = shortcuts.begin(); it != shortcuts.end(); ++it)
        if (auto const actionIt = m_actions.find(it.key());
            actionIt != m_actions.end())
            (*actionIt)->setShortcuts(it.value());
}

void BtActionCollection::writeShortcuts(QString const & group) const {
    BtConfig::ShortcutsMap shortcuts;
    for (auto it = m_actions.begin(); it != m_actions.end(); ++it)
        shortcuts.insert(it.key(), it.value()->shortcuts());
    btConfig().setShortcuts(group, shortcuts);
}
