/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/versekeychooser/btdropdownchooserbutton.h"

#include <QDebug>
#include <QWheelEvent>
#include "frontend/keychooser/versekeychooser/btversekeymenu.h"
#include "frontend/keychooser/versekeychooser/ckeyreferencewidget.h"


const unsigned int ARROW_HEIGHT = 15;

BtDropdownChooserButton::BtDropdownChooserButton(CKeyReferenceWidget* ref)
        : QToolButton(),
        m_ref(ref) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setAutoRaise(false);
    setArrowType(Qt::NoArrow);
    setFixedHeight(ARROW_HEIGHT);
    setFocusPolicy(Qt::NoFocus);
    setPopupMode(QToolButton::InstantPopup);
    setStyleSheet("QToolButton{margin:0px;}QToolButton::menu-indicator{subcontrol-position: center center;}");

    BtVerseKeyMenu* m = new BtVerseKeyMenu(this);
//	KAcceleratorManager::setNoAccel(m);
    setMenu(m);
    QObject::connect(m, SIGNAL(triggered(QAction*)), this, SLOT(slotMenuTriggered(QAction*)));
}


void BtDropdownChooserButton::mousePressEvent(QMouseEvent* e) {
    //qDebug("BtDropdownChooserButton::mousePressEvent");
    //recreate the menu
    menu()->clear();
    this->newList();

    QToolButton::mousePressEvent(e);
}

void BtDropdownChooserButton::wheelEvent(QWheelEvent* e) {
    // The problem is, that wheel events do everytime have the delta value 120
    const int vchange = ((e->delta() > 0) ? (-1) : (1));

    if (vchange != 0) {//do not emit a change with value 0
        emit stepItem(vchange);
        e->accept();
    }
    else {
        e->ignore();
    }
}


//******************Book dropdown button*************************************/

BtBookDropdownChooserButton::BtBookDropdownChooserButton(CKeyReferenceWidget* ref)
        : BtDropdownChooserButton(ref) {
    setToolTip(tr("Select book"));
    QObject::connect(this, SIGNAL(stepItem(int)), m_ref, SLOT(slotStepBook(int)));
}

void BtBookDropdownChooserButton::newList() {
    QMenu* m = menu();
    QStringList* booklist = ref()->m_module->books();
    foreach (QString bookname, *booklist) {
        m->addAction(bookname);
    }
}

void BtBookDropdownChooserButton::slotMenuTriggered(QAction* action) {
    qDebug() << "BtBookDropdownChooserButton::slotMenuTriggered" << action->text();
    m_ref->slotChangeBook(action->text());
}


//****************** Chapter dropdown button *************************************/

BtChapterDropdownChooserButton::BtChapterDropdownChooserButton(CKeyReferenceWidget* ref)
        : BtDropdownChooserButton(ref) {
    setToolTip(tr("Select chapter"));
    QObject::connect(this, SIGNAL(stepItem(int)), m_ref, SLOT(slotStepChapter(int)));
}

void BtChapterDropdownChooserButton::newList() {
    QMenu* m = menu();
    int count = ref()->m_module->chapterCount(ref()->m_key->book());
    for (int i = 1; i <= count; i++) {
        m->addAction(QString::number(i));
    }
}

void BtChapterDropdownChooserButton::slotMenuTriggered(QAction* action) {
    m_ref->slotChangeChapter(action->text().toInt());
}


//****************** Verse dropdown button *************************************/

BtVerseDropdownChooserButton::BtVerseDropdownChooserButton(CKeyReferenceWidget* ref)
        : BtDropdownChooserButton(ref) {
    setToolTip(tr("Select verse"));
    QObject::connect(this, SIGNAL(stepItem(int)), m_ref, SLOT(slotStepVerse(int)));
}

void BtVerseDropdownChooserButton::newList() {
    QMenu* m = menu();
    int count = ref()->m_module->verseCount(ref()->m_key->book(), ref()->m_key->Chapter());
    for (int i = 1; i <= count; i++) {
        m->addAction(QString::number(i));
    }
}

void BtVerseDropdownChooserButton::slotMenuTriggered(QAction* action) {
    m_ref->slotChangeVerse(action->text().toInt());
}
