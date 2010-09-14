/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btversekeymenu.h"

#include <QMenu>
#include <QTimerEvent>
#include <QMouseEvent>

#include <QDebug>

BtVerseKeyMenu::BtVerseKeyMenu(QWidget* parent)
        : QMenu(parent),
        m_timerId(0),
        m_firstClickLock(true) {
    qDebug("BtVerseKeyMenu::BtVerseKeyMenu");
    QObject::connect(this, SIGNAL(aboutToShow()), this, SLOT(startFirstClickDelayTimer()));
}

void BtVerseKeyMenu::startFirstClickDelayTimer() {
    //qDebug() << "BtVerseKeyMenu::startFirstClickDelayTimer";
    m_firstClickLock = true;
    killTimer(m_timerId);
    m_timerId = startTimer(300);
}

void BtVerseKeyMenu::timerEvent(QTimerEvent* e) {
    if (e->timerId() == m_timerId) {
        //qDebug() << "BtVerseKeyMenu::timerEvent";
        killTimer(m_timerId);
        m_firstClickLock = false;
    }
    else {
        QMenu::timerEvent(e);
    }
}

void BtVerseKeyMenu::mouseReleaseEvent(QMouseEvent* e) {
    //qDebug() << "BtVerseKeyMenu::mouseReleaseEvent";
    if (m_firstClickLock) return;
    //qDebug() << "BtVerseKeyMenu::mouseReleaseEvent 2";
    QMenu::mouseReleaseEvent(e);
}
