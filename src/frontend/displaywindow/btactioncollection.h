/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BT_ACTION_COLLECTION_H
#define BT_ACTION_COLLECTION_H

#include <QObject>

#include <QList>
#include <QMap>
#include "backend/config/btconfig.h"
#include "util/btassert.h"


class BtActionItem;
class QAction;
class QKeySequence;
class QString;

class BtActionCollection: public QObject {

        Q_OBJECT

    private: /* Types: */

        using ActionMap = QMap<QString, BtActionItem *>;

    public: /* Methods: */

        inline BtActionCollection(QObject *parent = nullptr) : QObject(parent) {}

        void addAction(QString const & name, QAction * const action);

        void addAction(QString const & name,
                       QObject const * const receiver,
                       const char * const member = nullptr);

        QList<QAction*> actions();

        QAction & action(QString const & name) const;

        template <typename T>
        inline T & actionAs(QString const & name) const {
            QAction & a = action(name);
            BT_ASSERT(dynamic_cast<T *>(&a));
            return static_cast<T &>(a);
        }

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
        QKeySequence getDefaultShortcut(QAction* action) const;

    private: /* Methods: */

        BtActionItem * findActionItem(QString const & name) const;

    private: /* Fields: */

        ActionMap m_actions;

};

#endif
