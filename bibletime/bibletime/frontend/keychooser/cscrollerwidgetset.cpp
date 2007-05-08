/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


//BibleTime includes
#include "cscrollbutton.h"
#include "cscrollerwidgetset.h"

//BibleTime frontend includes
#include "frontend/cbtconfig.h"

//Qt includes
#include <qlineedit.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qevent.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qrect.h>
#include <qlayout.h>

const unsigned int WIDTH = 16;
const unsigned int ARROW_HEIGHT = 12;
const unsigned int MOVER_HEIGHT = 6;

CScrollerWidgetSet::CScrollerWidgetSet(QWidget *parent, const char *name) : QWidget(parent,name) {
	m_layout = new QVBoxLayout(this);
	m_layout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

	//setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

	btn_up = new QToolButton( UpArrow, this, "btn_up" );
	btn_up->setFixedSize(WIDTH, ARROW_HEIGHT);
	btn_up->setFocusPolicy(QWidget::NoFocus);

	btn_fx = new CScrollButton( this, "btn_fx" );
	btn_fx->setFixedSize(WIDTH, MOVER_HEIGHT);
	btn_fx->setFocusPolicy(QWidget::NoFocus);

	btn_down = new QToolButton( DownArrow, this, "btn_down" );
	btn_down->setFixedSize(WIDTH, ARROW_HEIGHT);
	btn_down->setFocusPolicy(QWidget::NoFocus);
	m_layout->addWidget( btn_up,0 );
	m_layout->addWidget( btn_fx,0 );
	m_layout->addWidget( btn_down,0 );
	setMinimumWidth(WIDTH+4); // Kludge to add some spacing but seems to work.

	connect(btn_fx, SIGNAL(lock()), SLOT(slotLock()));
	connect(btn_fx, SIGNAL(unlock()), SLOT(slotUnlock()));
	connect(btn_fx, SIGNAL(change_requested(int)), SLOT(slotScroller(int)));
	connect(btn_up, SIGNAL(clicked()), SLOT(slotUpClick()));
	connect(btn_down, SIGNAL(clicked()), SLOT(slotDownClick()));
}

/** Sets the tooltips for the given entries using the parameters as text. */
void CScrollerWidgetSet::setToolTips( const QString nextEntryTip, const QString scrollButtonTip, const QString previousEntryTip) {
	QToolTip::add (btn_fx,  scrollButtonTip);
	QToolTip::add (btn_down, nextEntryTip);
	QToolTip::add (btn_up, previousEntryTip);
}

void CScrollerWidgetSet::slotLock() { emit scroller_pressed(); }
void CScrollerWidgetSet::slotUnlock() { emit scroller_released(); }
void CScrollerWidgetSet::slotScroller(int n) { emit change(n); }
void CScrollerWidgetSet::slotUpClick() { slotScroller(-1); }
void CScrollerWidgetSet::slotDownClick() { slotScroller(1); }
