/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
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
        BtActionItem(QObject* parent)
                : QObject(parent) {
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
    Q_ASSERT(m_actions[name] == 0);   /// \todo replacing actions is ok???
    int count;
    count = m_actions.count();
    BtActionItem* item = new BtActionItem(this);
    item->action = action;
    item->defaultKeys = action->shortcut();
    m_actions[name] = item;
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

void BtActionCollection::setConfigGroup(const QString &group) {
    m_groupName = group;
}

void BtActionCollection::readSettings() {
    QSettings* settings = CBTConfig::getConfig();
    settings->beginGroup(m_groupName);
    QStringList keyList = settings->childKeys();
    for (int i = 0; i < keyList.size(); i++) {
        QString key = keyList.at(i);
        QVariant variant = settings->value(key);
        QList<QKeySequence> shortcuts;
        if ( variant != QVariant()) {
            QList<QVariant> varShortcuts = variant.toList();
            for (int i = 0; i < varShortcuts.count(); i++) {
                QString keys = varShortcuts.at(i).toString();
                QKeySequence shortcut(keys);
                shortcuts.append(shortcut);
            }
        }
        action(key)->setShortcuts(shortcuts);
    }
    settings->endGroup();
}

static QList<QVariant> keyListToVariantList(const QList<QKeySequence>& keyList) {
    QList<QVariant> varList;
    for (int i = 0; i < keyList.count(); i++) {
        QKeySequence keySeq = keyList.at(i);
        varList.append(keySeq.toString());
    }
    return varList;
}

void BtActionCollection::writeSettings() {
    QSettings* settings = CBTConfig::getConfig();
    settings->beginGroup(m_groupName);
    QMap<QString, BtActionItem*>::const_iterator iter = m_actions.constBegin();
    while (iter != m_actions.constEnd()) {
        QString actionName = iter.key();
        QList<QKeySequence> keyList = iter.value()->action->shortcuts();
        QList<QVariant> varList = keyListToVariantList(keyList);
        settings->setValue(actionName, varList);
        iter++;
    }
    settings->endGroup();
}
