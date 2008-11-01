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

BtDropdownChooserButton::BtDropdownChooserButton(CKeyReferenceWidget* ref, Type type)
	: QToolButton(),
	m_ref(ref),
	m_type(type)
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


BtDropdownChooserButton::~BtDropdownChooserButton() {}

void BtDropdownChooserButton::mousePressEvent(QMouseEvent* e)
{
	qDebug("BtDropdownChooserButton::mousePressEvent");
	//recreate the menu
	QMenu* m = menu();
	m->clear();

	//w_chapter->reset(m_modules.first()->chapterCount(m_key->book()), chapter-1, false);
	//w_verse->reset(m_modules.first()->verseCount(m_key->book(), chapter), verse-1, false);
	if (m_type == Verse) {
		int count = m_ref->m_module->verseCount(m_ref->m_key->book(), m_ref->m_key->Chapter());
		for (int i = 1; i <= count; i++) {
			m->addAction(QString::number(i));
		}
	}
	if (m_type == Chapter) {
		int count = m_ref->m_module->chapterCount(m_ref->m_key->book());
		for (int i = 1; i <= count; i++) {
			m->addAction(QString::number(i));
		}
	}
	if (m_type == Book) {
		QStringList* booklist = m_ref->m_module->books();
		foreach (QString bookname, *booklist) {
			m->addAction(bookname);
		}
	}
	qDebug() << "Added items";
	QToolButton::mousePressEvent(e);
}
