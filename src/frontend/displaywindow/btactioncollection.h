/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BT_ACTION_COLLECTION_H
#define BT_ACTION_COLLECTION_H

#include <QObject>

#include <QAction>
#include <QKeySequence>
#include <QMap>
#include <QString>
#include "../../backend/config/btconfig.h"
#include "../../util/btassert.h"


class BtActionCollection: public QObject {

    Q_OBJECT

private: /* Types: */

    class Item: public QObject {

    public: /* Methods: */

        Item(QAction * const action, QObject * const parent)
            : QObject{parent}
            , m_defaultKeys{action->shortcut()}
            , m_action{action}
        {}

    public: /* Fields: */

        QKeySequence const m_defaultKeys;
        QAction * const m_action;

    };
    using ActionMap = QMap<QString, Item *>;

public: /* Methods: */

    inline BtActionCollection(QObject * const parent = nullptr)
            : QObject{parent}
    {}

    void addAction(QString const & name, QAction * const action);

    void addAction(QString const & name,
                   QObject const * const receiver,
                   const char * const member = nullptr);

    void removeAction(QString const & name);

    QAction & action(QString const & name) const;

    template <typename T>
    inline T & actionAs(QString const & name) const {
        QAction & a = action(name);
        BT_ASSERT(dynamic_cast<T *>(&a));
        return static_cast<T &>(a);
    }

    template <typename F>
    inline void foreachQAction(F && f) const {
        for (Item const * const item : m_actions)
            f(*(item->m_action), item->m_defaultKeys);
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
    void readShortcuts(QString const & group);

    /*!
     * \brief Write shortcuts to config.
     *
     * Write the shortcuts of this action collection
     * to the given group in the configuration.
     *
     * \param[in] group Shortcut group to write actions to.
     */
    void writeShortcuts(QString const & group) const;

    QKeySequence getDefaultShortcut(QAction * const action) const;

private: /* Methods: */

    Item * findActionItem(QString const & name) const;

private: /* Fields: */

    ActionMap m_actions;

};

#endif
