/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/cscrollerwidgetset.h"

#include <QString>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "frontend/keychooser/cscrollbutton.h"


const unsigned int WIDTH = 16;
const unsigned int ARROW_HEIGHT = 12;
const unsigned int MOVER_HEIGHT = 6;

CScrollerWidgetSet::CScrollerWidgetSet(QWidget *parent) : QWidget(parent) {
    m_layout = new QVBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setAlignment(this, Qt::AlignHCenter | Qt::AlignCenter);

    btn_up = new QToolButton(this);
    btn_up->setArrowType(Qt::UpArrow);

    btn_up->setFixedSize(WIDTH, ARROW_HEIGHT);
    btn_up->setFocusPolicy(Qt::NoFocus);
    btn_up->setAutoRaise(true);

    btn_fx = new CScrollButton(this);
    btn_fx->setFixedSize(WIDTH, MOVER_HEIGHT);
    btn_fx->setFocusPolicy(Qt::NoFocus);

    btn_down = new QToolButton(this);
    btn_down->setArrowType(Qt::DownArrow);
    btn_down->setFixedSize(WIDTH, ARROW_HEIGHT);
    btn_down->setFocusPolicy(Qt::NoFocus);
    btn_down->setAutoRaise(true);

    m_layout->addWidget( btn_up, 0 );
    m_layout->addWidget( btn_fx, 0 );
    m_layout->addWidget( btn_down, 0 );
    setMinimumWidth(WIDTH); // Kludge to add some spacing but seems to work.

    connect(btn_fx, SIGNAL(lock()), SLOT(slotLock()));
    connect(btn_fx, SIGNAL(unlock()), SLOT(slotUnlock()));
    connect(btn_fx, SIGNAL(change_requested(int)), SLOT(slotScroller(int)));
    connect(btn_up, SIGNAL(clicked()), SLOT(slotUpClick()));
    connect(btn_down, SIGNAL(clicked()), SLOT(slotDownClick()));
}

/** Sets the tooltips for the given entries using the parameters as text. */
void CScrollerWidgetSet::setToolTips( const QString nextEntryTip, const QString scrollButtonTip, const QString previousEntryTip) {
    btn_fx->setToolTip(scrollButtonTip);
    btn_down->setToolTip(nextEntryTip);
    btn_up->setToolTip(previousEntryTip);
}


void CScrollerWidgetSet::wheelEvent( QWheelEvent* e ) {
    /**
    * The problem is, that wheel events do everytime have the delta value 120
    */
    const int vchange = ((e->delta() > 0) ? (-1) : (1));

    if (vchange != 0) {//do not emit a change with value 0
        emit change(vchange);
        e->accept();
    }
    else {
        e->ignore();
    }
}

void CScrollerWidgetSet::slotLock() {
    emit scroller_pressed();
}
void CScrollerWidgetSet::slotUnlock() {
    emit scroller_released();
}
void CScrollerWidgetSet::slotScroller(int n) {
    emit change(n);
}
void CScrollerWidgetSet::slotUpClick() {
    slotScroller(-1);
}
void CScrollerWidgetSet::slotDownClick() {
    slotScroller(1);
}
