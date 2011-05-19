/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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
         iter++)
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
         iter++)
    {
        if (iter.value()->action == action) {
            return iter.value()->defaultKeys;
        }
    }
    return QKeySequence();
}

void BtActionCollection::readSettings() {
    QSettings* settings = CBTConfig::getConfig();
    settings->beginGroup(m_groupName);

    Q_FOREACH (const QString &key, settings->childKeys()) {
        QAction *a = action(key);
        if (a == 0)
            continue;

        QVariant variant = settings->value(key);
        qDebug() << variant << " | " << variant.typeName();
        if (variant.type() != QVariant::List
            && variant.type() != QVariant::StringList)
        {
            continue;
        }

        QList<QKeySequence> shortcuts;
        if (variant.type() == QVariant::List) { // For BibleTime before 2.9
            Q_FOREACH (const QVariant &shortcut, variant.toList()) {
                shortcuts.append(shortcut.toString());
            }
        } else {
            Q_ASSERT(variant.type() == QVariant::StringList);
            Q_FOREACH (const QString &shortcut, variant.toStringList()) {
                shortcuts.append(shortcut);
            }
        }
        a->setShortcuts(shortcuts);
    }

    settings->endGroup();
}

void BtActionCollection::writeSettings() {
    QSettings* settings = CBTConfig::getConfig();
    settings->beginGroup(m_groupName);

    for (ActionMap::const_iterator iter = m_actions.constBegin();
         iter != m_actions.constEnd();
         iter++)
    {
        // Write beautiful string lists (since 2.9):
        QStringList varList;
        Q_FOREACH (const QKeySequence &shortcut, iter.value()->action->shortcuts()) {
            /// \note saving QKeySequences directly doesn't appear to work!
            varList.append(shortcut.toString());
        }
        settings->setValue(iter.key(), varList);
        qDebug() << ">>" << settings->value(iter.key()).typeName();
    }

    settings->endGroup();
}
