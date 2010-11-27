/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/cscrollbutton.h"

#include <cmath>
#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QWheelEvent>


CScrollButton::CScrollButton(QWidget *parent)
        : QToolButton(parent), m_isLocked(false), m_movement(0) {
    setFocusPolicy(Qt::WheelFocus);
    setCursor(Qt::SplitVCursor);
}

CScrollButton::~CScrollButton() {
    // Intentionally empty
}

void CScrollButton::mousePressEvent(QMouseEvent *e) {
    if (m_isLocked) return;
    if (e->button() != Qt::LeftButton) return;
    m_isLocked = true;
    grabMouse(Qt::BlankCursor);
    emit lock();
}

void CScrollButton::mouseReleaseEvent(QMouseEvent *e) {
    if (!m_isLocked) return;
    if (e->button() != Qt::LeftButton) return;
    m_isLocked = false;
    releaseMouse();
    emit unlock();
}

void CScrollButton::mouseMoveEvent(QMouseEvent *e) {
    if (m_isLocked) {
        // Recalculate the center of the widget (might change during grab):
        QPoint center(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // Calculate movement change:
        int vchange = (e->globalY() - center.y());

        if (vchange != 0) {
            // Adapt the change value, so we get a more natural feeling:
            int avchange(vchange >= 0 ? vchange : -vchange);
            if (avchange < 10) {
                avchange = (int) pow(avchange, 0.3);
            }
            else if (avchange < 30) {
                avchange = (int) pow(avchange, 0.6);
            }
            else if (avchange < 40) {
                avchange = (int) pow(avchange, 1.2);
            }
            else {
                avchange = (int) pow(avchange, 2.0);
            }

            m_movement += vchange >= 0 ? avchange : -avchange;

            // Emit the change request signal only when necessary:
            if (m_movement >= 10 || m_movement <= -10) {
                emit change_requested(m_movement/10);
                m_movement = 0;
            }
        }

        // Move the mouse cursor to the center of this widget:
        QCursor::setPos(center);
    }
    else {
        QToolButton::mouseMoveEvent(e);
    }
}
