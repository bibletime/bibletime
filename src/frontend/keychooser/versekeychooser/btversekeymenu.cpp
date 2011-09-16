/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/versekeychooser/btversekeymenu.h"

#include <QMenu>
#include <QMouseEvent>
#include <QTimerEvent>


BtVerseKeyMenu::BtVerseKeyMenu(QWidget* parent)
        : QMenu(parent)
        , m_timerId(0)
        , m_firstClickLock(true)
{
    connect(this, SIGNAL(aboutToShow()),
            this, SLOT(startFirstClickDelayTimer()));
}

void BtVerseKeyMenu::startFirstClickDelayTimer() {
    m_firstClickLock = true;
    killTimer(m_timerId);
    m_timerId = startTimer(300);
}

void BtVerseKeyMenu::timerEvent(QTimerEvent* e) {
    if (e->timerId() == m_timerId) {
        killTimer(m_timerId);
        m_firstClickLock = false;
    }
    else {
        QMenu::timerEvent(e);
    }
}

void BtVerseKeyMenu::mouseReleaseEvent(QMouseEvent* e) {
    if (m_firstClickLock) return;
    QMenu::mouseReleaseEvent(e);
}
