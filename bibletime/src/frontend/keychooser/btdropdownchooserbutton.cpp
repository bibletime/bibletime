/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btdropdownchooserbutton.h"

#include <QMenu>
#include <QTime> //temp

#include <QDebug>

const unsigned int ARROW_HEIGHT = 12;

BtDropdownChooserButton::BtDropdownChooserButton()
 : QToolButton()
{
	setAutoRaise(true);
	setArrowType(Qt::DownArrow);
	setFixedHeight(ARROW_HEIGHT);
	setFocusPolicy(Qt::NoFocus);
	setPopupMode(QToolButton::InstantPopup);
	
	//does this need new class? can the menu be updated with standard signals/slots?
	//add menu later
	//populate menu, connect each menu Action to a slot which reads the text and changes the book/chapter/verse

	//test
	QMenu* menu = new QMenu(this);
	setMenu(menu);

}


BtDropdownChooserButton::~BtDropdownChooserButton() {}

void BtDropdownChooserButton::mousePressEvent(QMouseEvent*)
{
	qDebug("BtDropdownChooserButton::mousePressEvent");
	//recreate the menu
	menu()->clear();
	QString ctime = QTime::currentTime().toString();
	menu()->addAction(ctime);
	qDebug() << "Added time:" << ctime;
	
}
