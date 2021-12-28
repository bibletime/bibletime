/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QObject>

#include <QAction>
#include <QKeySequence>
#include <QMap>
#include <QString>
#include "../../util/btassert.h"


class BtActionCollection: public QObject {

    Q_OBJECT

private: // types:

    class Item: public QObject {

    public: // methods:

        Item(QAction * const action, QObject * const parent)
            : QObject{parent}
            , m_defaultKeys{action->shortcut()}
            , m_action{action}
        {}

    public: // fields:

        QKeySequence const m_defaultKeys;
        QAction * const m_action;

    };
    using ActionMap = QMap<QString, Item *>;

public: // methods:

    BtActionCollection(QObject * const parent = nullptr)
            : QObject{parent}
    {}

    void addAction(QString const & name, QAction * const action);

    void removeAction(QString const & name);

    QAction & action(QString const & name) const;

    template <typename T>
    T & actionAs(QString const & name) const {
        QAction & a = action(name);
        BT_ASSERT(dynamic_cast<T *>(&a));
        return static_cast<T &>(a);
    }

    template <typename F>
    void foreachQAction(F && f) const {
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

private: // methods:

    Item * findActionItem(QString const & name) const;

private: // fields:

    ActionMap m_actions;

};
