/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btdropdownchooserbutton.h"
#include "ckeyreferencewidget.h"

#include <QMenu>
#include <QTime> //temp

#include <QDebug>

const unsigned int ARROW_HEIGHT = 12;

BtDropdownChooserButton::BtDropdownChooserButton(CKeyReferenceWidget* ref)
	: QToolButton(),
	m_ref(ref)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	
	setAutoRaise(true);
	setArrowType(Qt::NoArrow);
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


void BtDropdownChooserButton::mousePressEvent(QMouseEvent* e)
{
	qDebug("BtDropdownChooserButton::mousePressEvent");
	//recreate the menu
	menu()->clear();
	this->newList();

	QToolButton::mousePressEvent(e);
}


BtBookDropdownChooserButton::BtBookDropdownChooserButton(CKeyReferenceWidget* ref)
	: BtDropdownChooserButton(ref)
{
	// set tooltip
}

void BtBookDropdownChooserButton::newList()
{
	QMenu* m = menu();
	QStringList* booklist = ref()->m_module->books();
	foreach (QString bookname, *booklist) {
		m->addAction(bookname);
	}
}



BtChapterDropdownChooserButton::BtChapterDropdownChooserButton(CKeyReferenceWidget* ref)
	: BtDropdownChooserButton(ref)
{
	// set tooltip
}

void BtChapterDropdownChooserButton::newList()
{
	QMenu* m = menu();
	int count = ref()->m_module->chapterCount(ref()->m_key->book());
	for (int i = 1; i <= count; i++) {
		m->addAction(QString::number(i));
	}
}



BtVerseDropdownChooserButton::BtVerseDropdownChooserButton(CKeyReferenceWidget* ref)
	: BtDropdownChooserButton(ref)
{
	// set tooltip
}

void BtVerseDropdownChooserButton::newList()
{
	QMenu* m = menu();
	int count = ref()->m_module->verseCount(ref()->m_key->book(), ref()->m_key->Chapter());
	for (int i = 1; i <= count; i++) {
		m->addAction(QString::number(i));
	}
}
