/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/btactioncollection.h"

#include <QAction>
#include <QDebug>
#include <QKeySequence>
#include <QSettings>
#include <QString>
#include <QStringList>
#include "backend/config/cbtconfig.h"
#include "util/directory.h"


class BtActionItem : public QObject {
    public:
        BtActionItem(QAction *action, QObject *parent = 0)
                : QObject(parent), defaultKeys(action->shortcut()), action(action)
        {
            // Intentionally empty
        }
        QKeySequence defaultKeys;
        QAction* action;
};

BtActionCollection::BtActionCollection(QObject* parent)
        : QObject(parent) {
}

BtActionCollection::~BtActionCollection() {
}

QList<QAction*> BtActionCollection::actions() {
    QList<QAction*> actionList;

    QMap<QString, BtActionItem*>::const_iterator iter = m_actions.constBegin();
    while (iter != m_actions.constEnd()) {
        QAction* action = iter.value()->action;
        actionList.append(action);
        ++iter;
    }
    return actionList;
}

QAction* BtActionCollection::action(const QString& name) {
    if (m_actions.contains(name))
        return m_actions[name]->action;
    qWarning() << "A QAction for a shortcut named" << name << "was requested but it is not defined.";
    return (new QAction(this)); // dummy QAction*
}

QAction* BtActionCollection::addAction(const QString& name, QAction* action) {
    Q_ASSERT(action != 0);
    if (m_actions.contains(name)) {
        delete m_actions[name];
    }
    BtActionItem* item = new BtActionItem(action, this);
    m_actions.insert(name, item);
    return action;
}

QAction* BtActionCollection::addAction(const QString &name, const QObject *receiver, const char* member) {
    QAction* action = new QAction(name, this);
    if (receiver && member) {
        bool ok = connect(action, SIGNAL(triggered()), receiver, SLOT(triggered()));
        Q_ASSERT(ok);
    }
    return addAction(name, action);
}

QKeySequence BtActionCollection::getDefaultShortcut(QAction* action) {
    QMap<QString, BtActionItem*>::const_iterator iter = m_actions.constBegin();
    while (iter != m_actions.constEnd()) {
        if ( iter.value()->action == action) {
            return iter.value()->defaultKeys;
        }
        iter++;
    }
    return QKeySequence();
}

void BtActionCollection::readShortcuts(const QString &group) {
    QHash<QString, QList <QKeySequence > > shortcuts = btconfiguration::BtConfig.getInstance().getShortcuts(group);
    for(QHash::const_iterator<QString, QList <QKeySequence > iter = shortcuts.begin();
                                                             iter != shortcuts.end();
                                                             iter++)
        action(iter.key())->setShortcuts(iter.value());
}

void BtActionCollection::writeShortcuts(const QString &group) {
    QHash< QString, QList<QKeySequence> > shortcuts;
    for(QMap<QString, BtActionItem*>::const_iterator iter = m_actions.begin();
                                                             iter != m_actions.end();
                                                             iter++)
        shortcuts.insert(iter.key(), iter.value()->action->shortcuts());
    btconfiguration::BtConfig.getInstance().setShortcuts(group, shortcuts);
}
