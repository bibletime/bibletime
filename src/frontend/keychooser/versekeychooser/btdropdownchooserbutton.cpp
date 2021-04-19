/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
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

template <typename TriggeredFunctor>
BtDropdownChooserButton::BtDropdownChooserButton(
        BtBibleKeyWidget * const ref,
        TriggeredFunctor && triggeredFunctor)
    : QToolButton()
    , m_ref(ref)
{
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
    BT_CONNECT(m, &BtVerseKeyMenu::triggered,
               std::forward<TriggeredFunctor>(triggeredFunctor));
}


void BtDropdownChooserButton::mousePressEvent(QMouseEvent* e) {
    //recreate the menu
    menu()->clear();
    newList(*menu(), *m_ref);

    QToolButton::mousePressEvent(e);
}

void BtDropdownChooserButton::wheelEvent(QWheelEvent * e) {
    int const delta = e->angleDelta().y();
    if (delta == 0) {
        e->ignore();
    } else {
        Q_EMIT stepItem((delta > 0) ? -1 : 1);
        e->accept();
    }
}


//******************Book dropdown button*************************************/

BtBookDropdownChooserButton::BtBookDropdownChooserButton(
        BtBibleKeyWidget * const ref)
    : BtDropdownChooserButton(
        ref,
        [ref](QAction * const action)
        { ref->slotChangeBook(action->property("bookname").toString()); })
{
    setToolTip(tr("Select book"));
    BT_CONNECT(this, &BtDropdownChooserButton::stepItem,
               ref, &BtBibleKeyWidget::slotStepBook);
}

void BtBookDropdownChooserButton::newList(QMenu & menu,
                                          BtBibleKeyWidget const & keyWidget)
{
    for (auto const & bookname : keyWidget.m_module->books())
        menu.addAction(bookname)->setProperty("bookname", bookname);
}


//****************** Chapter dropdown button *************************************/

BtChapterDropdownChooserButton::BtChapterDropdownChooserButton(
        BtBibleKeyWidget * const ref)
    : BtDropdownChooserButton(
        ref,
        [ref](QAction * const action)
        { ref->slotChangeChapter(action->property("chapter").toInt()); })
{
    setToolTip(tr("Select chapter"));
    BT_CONNECT(this, &BtDropdownChooserButton::stepItem,
               ref, &BtBibleKeyWidget::slotStepChapter);
}

void BtChapterDropdownChooserButton::newList(QMenu & menu,
                                             BtBibleKeyWidget const & kw)
{
    int count = kw.m_module->chapterCount(kw.m_key->book());
    for (int i = 1; i <= count; i++)
        menu.addAction(QString::number(i))->setProperty("chapter", i);
}


//****************** Verse dropdown button *************************************/

BtVerseDropdownChooserButton::BtVerseDropdownChooserButton(
        BtBibleKeyWidget * const ref)
    : BtDropdownChooserButton(
        ref,
        [ref](QAction * const action)
        { ref->slotChangeVerse(action->property("verse").toInt()); })
{
    setToolTip(tr("Select verse"));
    BT_CONNECT(this, &BtDropdownChooserButton::stepItem,
               ref, &BtBibleKeyWidget::slotStepVerse);
}

void BtVerseDropdownChooserButton::newList(QMenu & menu,
                                           BtBibleKeyWidget const & kw)
{
    int count = kw.m_module->verseCount(kw.m_key->bookName(), kw.m_key->chapter());
    for (int i = 1; i <= count; i++)
        menu.addAction(QString::number(i))->setProperty("verse", i);
}
