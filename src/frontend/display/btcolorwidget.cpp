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

#include "btcolorwidget.h"

#include <QColorDialog>
#include <QMouseEvent>
#include <QPalette>
#include <QSizePolicy>
#include <Qt>


BtColorWidget::BtColorWidget(QWidget * parent)
        : QFrame(parent)
{
    setFrameShadow(QFrame::Sunken);
    setFrameShape(QFrame::StyledPanel);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Window);
}

QSize BtColorWidget::sizeHint() const { return QSize(35, 18); }

void BtColorWidget::setColor(QColor const & color) {
    QPalette p(palette());
    p.setColor(QPalette::Normal, QPalette::Window, color);
    setPalette(p);
    update();
}

void BtColorWidget::mouseReleaseEvent(QMouseEvent * event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        QColor const color(QColorDialog::getColor(
            palette().color(QPalette::Normal, QPalette::Window),
            this));
        if (color.isValid()) {
            setColor(color);
            Q_EMIT changed(color);
        }
    }
}
