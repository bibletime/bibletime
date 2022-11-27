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
#include "btversekeymenu.h"
#include "btbiblekeywidget.h"


const unsigned int ARROW_HEIGHT = 15;

BtDropdownChooserButton::BtDropdownChooserButton(
        void (BtBibleKeyWidget::*populateMenu)(QMenu &),
        BtBibleKeyWidget & parent)
    : QToolButton(&parent)
    , m_populateMenu(populateMenu)
    , m_parent(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setAutoRaise(false);
    setArrowType(Qt::NoArrow);
    setFixedHeight(ARROW_HEIGHT);
    setFocusPolicy(Qt::NoFocus);
    setPopupMode(QToolButton::InstantPopup);
    setStyleSheet(
                QStringLiteral(
                    "QToolButton{margin:0px;}"
                    "QToolButton::menu-indicator{"
                        "subcontrol-position:center center"
                    "}"));

    setMenu(new BtVerseKeyMenu(this));
}


void BtDropdownChooserButton::mousePressEvent(QMouseEvent* e) {
    //recreate the menu
    menu()->clear();
    (m_parent.*m_populateMenu)(*menu());

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
