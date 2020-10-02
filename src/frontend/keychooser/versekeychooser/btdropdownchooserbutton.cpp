/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btdropdownchooserbutton.h"

#include <QWheelEvent>
#include "../../../util/btconnect.h"
#include "btversekeymenu.h"
#include "btbiblekeywidget.h"


const unsigned int ARROW_HEIGHT = 15;

BtDropdownChooserButton::BtDropdownChooserButton(BtBibleKeyWidget* ref)
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
//    KAcceleratorManager::setNoAccel(m);
    setMenu(m);
    BT_CONNECT(m,    SIGNAL(triggered(QAction *)),
               this, SLOT(slotMenuTriggered(QAction *)));
}


void BtDropdownChooserButton::mousePressEvent(QMouseEvent* e) {
    //recreate the menu
    menu()->clear();
    this->newList();

    QToolButton::mousePressEvent(e);
}

void BtDropdownChooserButton::wheelEvent(QWheelEvent * e) {
    int const delta = e->angleDelta().y();
    if (delta == 0) {
        e->ignore();
    } else {
        emit stepItem((delta > 0) ? -1 : 1);
        e->accept();
    }
}


//******************Book dropdown button*************************************/

BtBookDropdownChooserButton::BtBookDropdownChooserButton(BtBibleKeyWidget* ref)
        : BtDropdownChooserButton(ref) {
    setToolTip(tr("Select book"));
    BT_CONNECT(this, SIGNAL(stepItem(int)), m_ref, SLOT(slotStepBook(int)));
}

void BtBookDropdownChooserButton::newList() {
    QMenu* m = menu();
    QStringList* booklist = ref()->m_module->books();
    Q_FOREACH(QString const & bookname, *booklist)
        m->addAction(bookname)->setProperty("bookname", bookname);
}

void BtBookDropdownChooserButton::slotMenuTriggered(QAction* action) {
    m_ref->slotChangeBook(action->property("bookname").toString());
}


//****************** Chapter dropdown button *************************************/

BtChapterDropdownChooserButton::BtChapterDropdownChooserButton(BtBibleKeyWidget* ref)
        : BtDropdownChooserButton(ref) {
    setToolTip(tr("Select chapter"));
    BT_CONNECT(this, SIGNAL(stepItem(int)), m_ref, SLOT(slotStepChapter(int)));
}

void BtChapterDropdownChooserButton::newList() {
    QMenu* m = menu();
    int count = ref()->m_module->chapterCount(ref()->m_key->book());
    for (int i = 1; i <= count; i++)
        m->addAction(QString::number(i))->setProperty("chapter", i);
}

void BtChapterDropdownChooserButton::slotMenuTriggered(QAction* action) {
    m_ref->slotChangeChapter(action->property("chapter").toInt());
}


//****************** Verse dropdown button *************************************/

BtVerseDropdownChooserButton::BtVerseDropdownChooserButton(BtBibleKeyWidget* ref)
        : BtDropdownChooserButton(ref) {
    setToolTip(tr("Select verse"));
    BT_CONNECT(this, SIGNAL(stepItem(int)), m_ref, SLOT(slotStepVerse(int)));
}

void BtVerseDropdownChooserButton::newList() {
    QMenu* m = menu();
    int count = ref()->m_module->verseCount(ref()->m_key->book(), ref()->m_key->getChapter());
    for (int i = 1; i <= count; i++)
        m->addAction(QString::number(i))->setProperty("verse", i);
}

void BtVerseDropdownChooserButton::slotMenuTriggered(QAction* action) {
    m_ref->slotChangeVerse(action->property("verse").toInt());
}
