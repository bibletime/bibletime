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
#include "btversekeymenu.h"

#include <kacceleratormanager.h> // needed to remove the automatic shortcuts

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
	setStyleSheet("QToolButton{margin:0px;}QToolButton::menu-indicator{subcontrol-position: center center;}");

	BtVerseKeyMenu* m = new BtVerseKeyMenu(this);
	KAcceleratorManager::setNoAccel(m);
	setMenu(m);
	QObject::connect(m, SIGNAL(triggered(QAction*)), this, SLOT(slotMenuTriggered(QAction*)));

}


void BtDropdownChooserButton::mousePressEvent(QMouseEvent* e)
{
	//qDebug("BtDropdownChooserButton::mousePressEvent");
	//recreate the menu
	menu()->clear();
	this->newList();

	QToolButton::mousePressEvent(e);
}


BtBookDropdownChooserButton::BtBookDropdownChooserButton(CKeyReferenceWidget* ref)
	: BtDropdownChooserButton(ref)
{
	setToolTip(tr("Select book"));	
}

void BtBookDropdownChooserButton::newList()
{
	QMenu* m = menu();
	QStringList* booklist = ref()->m_module->books();
	foreach (QString bookname, *booklist) {
		m->addAction(bookname);
	}
}

void BtBookDropdownChooserButton::slotMenuTriggered(QAction* action)
{
	qDebug() << "BtBookDropdownChooserButton::slotMenuTriggered" << action->text();
	m_ref->slotChangeBook(action->text());
}


BtChapterDropdownChooserButton::BtChapterDropdownChooserButton(CKeyReferenceWidget* ref)
	: BtDropdownChooserButton(ref)
{
	setToolTip(tr("Select chapter"));
}

void BtChapterDropdownChooserButton::newList()
{
	QMenu* m = menu();
	int count = ref()->m_module->chapterCount(ref()->m_key->book());
	for (int i = 1; i <= count; i++) {
		m->addAction(QString::number(i));
	}
}

void BtChapterDropdownChooserButton::slotMenuTriggered(QAction* action)
{
	m_ref->slotChangeChapter(action->text().toInt());
}


BtVerseDropdownChooserButton::BtVerseDropdownChooserButton(CKeyReferenceWidget* ref)
	: BtDropdownChooserButton(ref)
{
	setToolTip(tr("Select verse"));
}

void BtVerseDropdownChooserButton::newList()
{
	QMenu* m = menu();
	int count = ref()->m_module->verseCount(ref()->m_key->book(), ref()->m_key->Chapter());
	for (int i = 1; i <= count; i++) {
		m->addAction(QString::number(i));
	}
}

void BtVerseDropdownChooserButton::slotMenuTriggered(QAction* action)
{
	m_ref->slotChangeVerse(action->text().toInt());
}
