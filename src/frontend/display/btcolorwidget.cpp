/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/display/btcolorwidget.h"

#include <QColor>
#include <QColorDialog>
#include <QMouseEvent>
#include <QPalette>


BtColorWidget::BtColorWidget(QWidget* parent)
        : QFrame(parent) {
    setFrameShadow(QFrame::Sunken);
    setFrameShape(QFrame::StyledPanel);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAutoFillBackground(true);
}

BtColorWidget::~BtColorWidget() {
}

QSize BtColorWidget::sizeHint() const {
    return QSize(35, 18);
}

void BtColorWidget::setColor(const QColor& color) {
    QPalette p = palette();
    p.setColor(QPalette::Normal, QPalette::Window, color);
    setPalette(p);

    if (color.isValid())
        m_color = color;
    else
        m_color = QColor(0, 0, 0);
    update();
}

void BtColorWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        showColorDialog();
        return;
    }
}

void BtColorWidget::showColorDialog() {
    QColor color = QColorDialog::getColor(m_color, this);
    if (color.isValid()) {
        m_color = color;
        emit changed(m_color);
    }
}
