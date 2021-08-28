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

#include <QMenu>

#include <QObject>
#include <QString>


class QMouseEvent;
class QTimerEvent;
class QWidget;

/**
* Menu for book/ch/v dropdown lists.
*
* This is implemented mostly because it needs a delay which prevents unwanted actions after
* the menu has been shown and mouse button is released over some item.
*/
class BtVerseKeyMenu : public QMenu {
        Q_OBJECT
    public:
        BtVerseKeyMenu(QWidget* parent);
    protected:
        void mouseReleaseEvent(QMouseEvent* event) override;
        /** Frees the mouse button release after the delay has elapsed.*/
        void timerEvent(QTimerEvent* event) override;

    private:
        int m_timerId;
        bool m_firstClickLock;
};
