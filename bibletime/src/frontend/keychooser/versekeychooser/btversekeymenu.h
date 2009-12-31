/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTVERSEKEYMENU_H
#define BTVERSEKEYMENU_H

#include <QMenu>


class QMouseEvent;
class QTimerEvent;

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
        ~BtVerseKeyMenu() {}
    protected:
        virtual void mouseReleaseEvent(QMouseEvent* event);
        /** Frees the mouse button release after the delay has elapsed.*/
        virtual void timerEvent(QTimerEvent* event);
    private slots:
        /** Starts the delay timer for the first mouse button release.*/
        void startFirstClickDelayTimer();

    private:
        int m_timerId;
        bool m_firstClickLock;
};

#endif
