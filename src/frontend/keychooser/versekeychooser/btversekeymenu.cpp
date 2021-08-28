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

#include "btversekeymenu.h"

#include <QMenu>
#include <QTimerEvent>
#include "../../../util/btconnect.h"


BtVerseKeyMenu::BtVerseKeyMenu(QWidget* parent)
        : QMenu(parent)
        , m_timerId(0)
        , m_firstClickLock(true)
{
    BT_CONNECT(this, &QMenu::aboutToShow,
               [this] {
                   m_firstClickLock = true;
                   if (m_timerId) {
                       killTimer(m_timerId);
                       m_timerId = 0;
                   }
                   m_timerId = startTimer(300);
               });
}

void BtVerseKeyMenu::timerEvent(QTimerEvent* e) {
    if (e->timerId() == m_timerId) {
        if (m_timerId) {
            killTimer(m_timerId);
            m_timerId = 0;
        }
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
