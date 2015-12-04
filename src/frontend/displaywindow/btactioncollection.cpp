/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/btactioncollection.h"

#include <QAction>
#include <QDebug>
#include <QKeySequence>
#include <QString>
#include <QStringList>
#include "util/btconnect.h"
#include "util/directory.h"


class BtActionItem: public QObject {

    public: /* Methods: */

        BtActionItem(QAction *action, QObject *parent = nullptr)
                : QObject(parent), defaultKeys(action->shortcut()), action(action)
        {
            // Intentionally empty
        }

    public: /* Fields: */

        QKeySequence defaultKeys;
        QAction* action;

};

QList<QAction*> BtActionCollection::actions() {
    QList<QAction*> actionList;
    for (ActionMap::const_iterator iter = m_actions.constBegin();
         iter != m_actions.constEnd();
         ++iter)
    {
        actionList.append(iter.value()->action);
    }
    return actionList;
}

QAction & BtActionCollection::action(QString const & name) const {
    BtActionItem const * const foundItem = findActionItem(name);
    BT_ASSERT(foundItem);
    return *(foundItem->action);
}

void BtActionCollection::addAction(QString const & name,
                                   QAction * const action)
{
    BT_ASSERT(action);
    ActionMap::iterator it = m_actions.find(name);
    if (it != m_actions.constEnd())
        delete *it;

    m_actions.insert(name, new BtActionItem(action, this));
}

void BtActionCollection::addAction(QString const & name,
                                   QObject const * const receiver,
                                   char const * const member)
{
    QAction* action = new QAction(name, this);
    if (receiver && member)
        BT_CONNECT(action,   SIGNAL(triggered()),
                   receiver, SLOT(triggered()));
    return addAction(name, action);
}

QKeySequence BtActionCollection::getDefaultShortcut(QAction* action) {
    for (ActionMap::const_iterator iter = m_actions.constBegin();
         iter != m_actions.constEnd();
         ++iter)
    {
        if (iter.value()->action == action) {
            return iter.value()->defaultKeys;
        }
    }
    return QKeySequence();
}

void BtActionCollection::readShortcuts(const QString &group) {
    QHash<QString, QList <QKeySequence > > shortcuts = btConfig().getShortcuts(group);
    for(QHash<QString, QList <QKeySequence> >::const_iterator iter = shortcuts.begin();
                                                             iter != shortcuts.end();
                                                             ++iter)
        if (BtActionItem const * const foundItem = findActionItem(iter.key()))
            foundItem->action->setShortcuts(iter.value());
}

void BtActionCollection::writeShortcuts(const QString &group) {
    QHash< QString, QList<QKeySequence> > shortcuts;
    for (ActionMap::const_iterator iter = m_actions.constBegin();
        iter != m_actions.constEnd();
        ++iter)
    {
        shortcuts.insert(iter.key(), iter.value()->action->shortcuts());
    }
    btConfig().setShortcuts(group, shortcuts);
}

BtActionItem * BtActionCollection::findActionItem(QString const & name) const {
    ActionMap::const_iterator const it = m_actions.find(name);
    return (it != m_actions.constEnd()) ? *it : nullptr;
}
