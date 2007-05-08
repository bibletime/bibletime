/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cscrollbutton.h"

#include "frontend/cbtconfig.h"

#include <stdlib.h>
#include <math.h>

//Qt includes
#include <qevent.h>
#include <qapplication.h>
#include <qcursor.h>

CScrollButton::CScrollButton(QWidget *parent, const char *name ) : QToolButton(parent,name) {
	setFocusPolicy(QWidget::WheelFocus);
	setCursor( splitVCursor );

	m_isLocked = false;
	connect(this, SIGNAL(pressed() ), SLOT(was_pressed() ));
	connect(this, SIGNAL(released()), SLOT(was_released()));
}

const bool CScrollButton::isLocked( ) const {
	return m_isLocked;
}

void CScrollButton::was_pressed( ) {
	QApplication::setOverrideCursor( BlankCursor );
	m_isLocked = true;
	lock_Point = get_lock_Point();

	emit lock()
		;
}

void CScrollButton::was_released( ) {
	QApplication::restoreOverrideCursor();
	m_isLocked = false;

	emit unlock();
}

const QPoint CScrollButton::get_lock_Point() const {
	return mapToGlobal( QPoint( width()/2, height()/2 ) );
}

void CScrollButton::mouseMoveEvent( QMouseEvent* e ) {
	if (m_isLocked) {
		int vchange = (QCursor::pos().y() - lock_Point.y());

		if (abs(vchange) < 10) {
			vchange = (int)((vchange>0 ? 1 : -1) * pow(abs(vchange), 0.3));
		}
		else if (abs(vchange) < 30) {
			vchange = (int)((vchange>0 ? 1 : -1) * pow(abs(vchange), 0.6));
		}
		else if (abs(vchange) < 40) {
			vchange = (int)((vchange>0 ? 1 : -1) * pow(abs(vchange), 1.2));
		}
		else {
			vchange = (int)((vchange>0 ? 1 : -1) * pow(abs(vchange), 2.0));
		}

		if (vchange) { //not emit 0
			emit change_requested( vchange );
		}

		QCursor::setPos( lock_Point );
	}
	else {
		QToolButton::mouseMoveEvent(e);
	}
}



/** If the wheel of the mouse is used while the mouse stays over our scrollbutton the content is  scrolled like the mouse was pressed and moved. */
void CScrollButton::wheelEvent( QWheelEvent* e ) {
	/**
	* The problem is, that wheel events do everytime have the delta value 120
	*/
	const int vchange = ((e->delta() > 0) ? (1) : (-1));

	if (vchange!=0) {//do not emit a change with value 0
		emit change_requested( vchange );
		e->accept();
	}
	else {
		e->ignore();
	}
}
