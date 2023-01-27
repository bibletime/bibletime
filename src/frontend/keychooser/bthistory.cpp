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

#include "bthistory.h"

#include <QAction>
#include <QVariant>
#include "../../backend/keys/cswordkey.h"
#include "../../util/btassert.h"


namespace {
char const ActionText[] = "BtHistory key";
}

BTHistory::BTHistory(QObject * const parent)
    : QObject(parent)
{ BT_ASSERT(class_invariant()); }

void BTHistory::add(CSwordKey* newKey) {
    BT_ASSERT(newKey);
    // Add new key Action after current index if we were not using the history functions,
    // if it's not a duplicate and if it's not empty.
    if (!m_inHistoryFunction &&    ((m_index < 0) || (newKey->key() != m_historyList.at(m_index)->property(ActionText).toString()) )) {
        if (!newKey->key().isEmpty()) {
            auto * const a = new QAction(newKey->key(), this);
            a->setProperty(ActionText, newKey->key());
            m_historyList.insert(++m_index, a);
        }
        // \todo history limit?
        sendChangedSignal();
    }
    BT_ASSERT(class_invariant());
}

void BTHistory::move(QAction* historyItem) {
    //BT_ASSERT(historyItem);
    BT_ASSERT(m_historyList.count());

    m_inHistoryFunction = true;
    //find the action in the list
    m_index = m_historyList.indexOf(historyItem);
    //move to the selected item in the list, it will be the current item
    Q_EMIT historyMoved(m_historyList.at(m_index)->property(ActionText).toString()); // signal to "outsiders"; key has been changed
    sendChangedSignal();

    m_inHistoryFunction = false;
    BT_ASSERT(class_invariant());
}

void BTHistory::back() {
    if ( m_index >= 1) {
        move(m_historyList.at(m_index - 1));
    }
    BT_ASSERT(class_invariant());
}

void BTHistory::fw() {
    if (m_index < (m_historyList.size() - 1)) {
        move(m_historyList.at(m_index + 1));
    }
    BT_ASSERT(class_invariant());
}

QList<QAction*> BTHistory::getBackList() {

    QList<QAction*> list;
    for (int i = m_index - 1; i >= 0; --i) {
        list.append(m_historyList.at(i));
    }

    BT_ASSERT(class_invariant());
    return list;
}

QList<QAction*> BTHistory::getFwList() {
    QList<QAction*> list;
    for (int i = m_index + 1; i < m_historyList.size(); ++i) {
        list.append(m_historyList.at(i));
    }

    BT_ASSERT(class_invariant());
    return list;
}

void BTHistory::sendChangedSignal() {
    bool backEnabled = m_index > 0; //there are items in the back list
    bool fwEnabled = m_historyList.size() > m_index + 1; //there are items in the fw list
    Q_EMIT historyChanged(backEnabled, fwEnabled);
    BT_ASSERT(class_invariant());
}

bool BTHistory::class_invariant() {
    for (int i = 0; i < m_historyList.size(); ++i) {
        if (!m_historyList.at(i) || m_historyList.at(i)->property(ActionText).toString().isEmpty()) return false;
    }
    if (!(m_index >= -1 && m_index < m_historyList.size())) return false;
    return true;
}
