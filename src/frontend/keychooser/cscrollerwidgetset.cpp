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

#include "cscrollerwidgetset.h"

#include <QPoint>
#include <QString>
#include <Qt>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "../../util/btconnect.h"
#include "cscrollbutton.h"


#define WIDTH (static_cast<unsigned int>(16))
#define ARROW_HEIGHT (static_cast<unsigned int>(12))
#define MOVER_HEIGHT (static_cast<unsigned int>(6))


CScrollerWidgetSet::CScrollerWidgetSet(QWidget * parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setAlignment(this, Qt::AlignHCenter | Qt::AlignCenter);

    m_buttonUp = new QToolButton(this);
    m_buttonUp->setArrowType(Qt::UpArrow);

    m_buttonUp->setFixedSize(WIDTH, ARROW_HEIGHT);
    m_buttonUp->setFocusPolicy(Qt::NoFocus);
    m_buttonUp->setAutoRaise(true);

    m_scrollButton = new CScrollButton(this);
    m_scrollButton->setFixedSize(WIDTH, MOVER_HEIGHT);
    m_scrollButton->setFocusPolicy(Qt::NoFocus);

    m_buttonDown = new QToolButton(this);
    m_buttonDown->setArrowType(Qt::DownArrow);
    m_buttonDown->setFixedSize(WIDTH, ARROW_HEIGHT);
    m_buttonDown->setFocusPolicy(Qt::NoFocus);
    m_buttonDown->setAutoRaise(true);

    m_layout->addWidget(m_buttonUp, 0);
    m_layout->addWidget(m_scrollButton, 0);
    m_layout->addWidget(m_buttonDown, 0);
    setMinimumWidth(WIDTH); // Kludge to add some spacing but seems to work.

    BT_CONNECT(m_scrollButton, &CScrollButton::lock,
               this, &CScrollerWidgetSet::scroller_pressed);
    BT_CONNECT(m_scrollButton, &CScrollButton::unlock,
               this, &CScrollerWidgetSet::scroller_released);
    BT_CONNECT(m_scrollButton, &CScrollButton::change_requested,
               this, &CScrollerWidgetSet::change);
    BT_CONNECT(m_buttonUp, &QToolButton::clicked, [this]{ Q_EMIT change(-1); });
    BT_CONNECT(m_buttonDown, &QToolButton::clicked,
               [this]{ Q_EMIT change(1); });
}

/** Sets the tooltips for the given entries using the parameters as text. */
void CScrollerWidgetSet::setToolTips(const QString & nextEntryTip,
                                     const QString & scrollButtonTip,
                                     const QString & previousEntryTip)
{
    m_scrollButton->setToolTip(scrollButtonTip);
    m_buttonDown->setToolTip(nextEntryTip);
    m_buttonUp->setToolTip(previousEntryTip);
}


void CScrollerWidgetSet::wheelEvent(QWheelEvent * e) {
    int const delta = e->angleDelta().y();
    if (delta == 0) {
        e->ignore();
    } else {
        Q_EMIT change((delta > 0) ? -1 : 1);
        e->accept();
    }
}
