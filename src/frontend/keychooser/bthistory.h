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

#include <QList>
#include <QString>


class CSwordKey;
class QAction;

class BTHistory: public QObject {
        Q_OBJECT
    public:
        BTHistory(QObject * parent);

        /**
        * Return a list of Actions behind the current point, the first of the history list will be the
        * last in the returned list and vice versa.
        */
        QList<QAction*> getBackList();
        /**
        * Return a list of Actions after the current point.
        */
        QList<QAction*> getFwList();

    public Q_SLOTS:
        /**
        * Add a new key to the history.
        */
        void add(CSwordKey* newKey);
        /**
        * Move the current point in history list.
        */
        void move(QAction*);
        /**
        * Go back one step in history.
        */
        void back();
        /**
        * Go forward one step in history.
        */
        void fw();

    Q_SIGNALS:
        /**
        * Signal will be sent when the history has been changed (added, moved)
        */
        void historyChanged(bool backEnabled, bool fwEnabled);
        /**
        * Signal will be sent when the current point in history has moved
        */
        void historyMoved(QString newKey);

    private:

        void sendChangedSignal();
        bool class_invariant();

        QList<QAction*> m_historyList;
        int m_index = -1; //pointer to the current item; -1==empty, 0==first etc.
        bool m_inHistoryFunction = false; //to prevent recursive behaviour
};
