/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BT_ACTION_COLLECTION_H
#define BT_ACTION_COLLECTION_H

#include <QObject>

#include <QList>
#include <QMap>

#include "backend/config/btconfig.h"

class BtActionItem;
class QAction;
class QKeySequence;
class QString;

class BtActionCollection : public QObject {
        Q_OBJECT
    public:
        BtActionCollection(QObject* parent);
        ~BtActionCollection();
        QAction* addAction(const QString& name, QAction* action);
        QAction* addAction(const QString &name, const QObject *receiver, const char* member = 0);
        QList<QAction*> actions();
        QAction* action(const QString& name);
        void readShortcuts(const QString &group);
        void writeShortcuts(const QString& group);
        QKeySequence getDefaultShortcut(QAction* action);

    private:
        QMap<QString, BtActionItem*> m_actions;
};

#endif
