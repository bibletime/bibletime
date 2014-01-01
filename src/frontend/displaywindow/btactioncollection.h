/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
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

        /*!
         * \brief Read shortcuts from config.
         *
         * Read the shortcuts for the given group
         * from the configuration and add them to
         * this action collection.
         *
         * \param[in] group Shortcut group to read actions from.
         */
        void readShortcuts(const QString &group);

        /*!
         * \brief Write shortcuts to config.
         *
         * Write the shortcuts of this action collection
         * to the given group in the configuration.
         *
         * \param[in] group Shortcut group to write actions to.
         */
        void writeShortcuts(const QString& group);
        QKeySequence getDefaultShortcut(QAction* action);

    private: /* Fields: */

        ActionMap m_actions;

};

#endif
