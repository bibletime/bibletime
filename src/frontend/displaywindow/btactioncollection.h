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

public: // methods:

    BtActionCollection(QObject * const parent = nullptr)
            : QObject{parent}
    {}

    QMap<QString, QAction *> const & actions() const noexcept
    { return m_actions; }

    void addAction(QString const & name, QAction * const action);

    void removeAction(QString const & name);

    QAction & action(QString const & name) const;

    template <typename T>
    T & actionAs(QString const & name) const {
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

    static QKeySequence getDefaultShortcut(QAction & action);

private: // fields:

    QMap<QString, QAction *> m_actions;

};
