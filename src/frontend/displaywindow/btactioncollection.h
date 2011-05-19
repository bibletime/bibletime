/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BT_ACTION_COLLECTION_H
#define BT_ACTION_COLLECTION_H

#include <QObject>

#include <QList>
#include <QMap>


class BtActionItem;
class QAction;
class QKeySequence;

class BtActionCollection: public QObject {

        Q_OBJECT

    private: /* Types: */

        typedef QMap<QString, BtActionItem*> ActionMap;

    public: /* Methods: */

        inline BtActionCollection(QObject *parent = 0) : QObject(parent) {}

        QAction* addAction(const QString& name, QAction* action);

        QAction* addAction(const QString &name, const QObject *receiver, const char* member = 0);

        QList<QAction*> actions();

        QAction *action(const QString &name) const;

        inline void setConfigGroup(const QString &group) {
            m_groupName = group;
        }

        void readSettings();

        void writeSettings();

        QKeySequence getDefaultShortcut(QAction* action);

    private: /* Fields: */

        ActionMap m_actions;
        QString m_groupName;

};

#endif
