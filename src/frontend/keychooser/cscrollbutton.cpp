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

#include "cscrollbutton.h"

#include <cmath>
#include <QCursor>
#include <QMouseEvent>
#include <QPoint>
#include <Qt>


CScrollButton::CScrollButton(QWidget *parent)
        : QToolButton(parent), m_isLocked(false), m_movement(0.0) {
    setFocusPolicy(Qt::WheelFocus);
    setCursor(Qt::SplitVCursor);
}

void CScrollButton::mousePressEvent(QMouseEvent *e) {
    if (m_isLocked) return;
    if (e->button() != Qt::LeftButton) return;
    m_isLocked = true;
    grabMouse(Qt::BlankCursor);
    Q_EMIT lock();
}

void CScrollButton::mouseReleaseEvent(QMouseEvent *e) {
    if (!m_isLocked) return;
    if (e->button() != Qt::LeftButton) return;
    m_isLocked = false;
    m_movement = 0.0;
    releaseMouse();
    Q_EMIT unlock();
}

void CScrollButton::mouseMoveEvent(QMouseEvent *e) {
    if (m_isLocked) {
        // Recalculate the center of the widget (might change during grab):
        QPoint center(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // Calculate movement change:
        #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        int vchange = (e->globalY() - center.y());
        #else
        int vchange = (e->globalPosition().toPoint().y() - center.y());
        #endif

        if (vchange != 0) {
            // Adapt the change value, so we get a more natural feeling:
            if(vchange > 0)
                m_movement += pow(vchange/10.0f, 1.2);
            else // (vchange < 0)
                m_movement -= pow(-vchange/10.0f, 1.2);

            // Emit the change request signal only when the mouse was moved far enough
            if (m_movement >= 1.0 || m_movement <= -1.0) {
                Q_EMIT change_requested(static_cast<int>(m_movement));
                m_movement = 0.0;
            }
        }

        // Move the mouse cursor to the center of this widget:
        QCursor::setPos(center);
    }
    else {
        QToolButton::mouseMoveEvent(e);
    }
}
