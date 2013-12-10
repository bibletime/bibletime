/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/btactioncollection.h"

#include <QAction>
#include <QDebug>
#include <QKeySequence>
#include <QString>
#include <QStringList>
#include "util/directory.h"


class BtActionItem: public QObject {

    public: /* Methods: */

        BtActionItem(QAction *action, QObject *parent = 0)
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

QAction *BtActionCollection::action(const QString &name) const {
    ActionMap::const_iterator it = m_actions.find(name);
    if (it != m_actions.constEnd())
        return (*it)->action;

    qWarning() << "A QAction for a shortcut named" << name << "was requested but it is not defined.";
    return 0;
}

QAction* BtActionCollection::addAction(const QString& name, QAction* action) {
    Q_ASSERT(action != 0);
    ActionMap::iterator it = m_actions.find(name);
    if (it != m_actions.constEnd())
        delete *it;

    m_actions.insert(name, new BtActionItem(action, this));
    return action;
}

QAction* BtActionCollection::addAction(const QString &name, const QObject *receiver, const char* member) {
    QAction* action = new QAction(name, this);
    if (receiver && member) {
        bool ok = connect(action,   SIGNAL(triggered()),
                          receiver, SLOT(triggered()));
        Q_ASSERT(ok);
    }
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
    {
        QAction *a = action(iter.key());
        if (a == 0)
            continue;
        action(iter.key())->setShortcuts(iter.value());
    }
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
