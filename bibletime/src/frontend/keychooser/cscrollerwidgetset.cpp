/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


//BibleTime includes
#include "cscrollbutton.h"
#include "cscrollerwidgetset.h"
#include "cscrollerwidgetset.moc"

//Qt includes
#include <QString>
#include <QToolButton>
#include <QVBoxLayout>

const unsigned int WIDTH = 16;
const unsigned int ARROW_HEIGHT = 12;
const unsigned int MOVER_HEIGHT = 6;

CScrollerWidgetSet::CScrollerWidgetSet(QWidget *parent) : QWidget(parent) {
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0,0,0,0);
	m_layout->setAlignment(this, Qt::AlignHCenter | Qt::AlignCenter);

	//setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

	//TODO: old constructor gave "name" argument, is that needed? "btn_up" etc.
	btn_up = new QToolButton(this);
	btn_up->setArrowType(Qt::UpArrow);
	
	btn_up->setFixedSize(WIDTH, ARROW_HEIGHT);
	btn_up->setFocusPolicy(Qt::NoFocus);

	btn_fx = new CScrollButton(this);
	btn_fx->setFixedSize(WIDTH, MOVER_HEIGHT);
	btn_fx->setFocusPolicy(Qt::NoFocus);

	btn_down = new QToolButton(this);
	btn_down->setArrowType(Qt::DownArrow);
	btn_down->setFixedSize(WIDTH, ARROW_HEIGHT);
	btn_down->setFocusPolicy(Qt::NoFocus);
	m_layout->addWidget( btn_up,0 );
	m_layout->addWidget( btn_fx,0 );
	m_layout->addWidget( btn_down,0 );
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

void CScrollerWidgetSet::slotLock() { emit scroller_pressed(); }
void CScrollerWidgetSet::slotUnlock() { emit scroller_released(); }
void CScrollerWidgetSet::slotScroller(int n) { emit change(n); }
void CScrollerWidgetSet::slotUpClick() { slotScroller(-1); }
void CScrollerWidgetSet::slotDownClick() { slotScroller(1); }
