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

#include "clistwidget.h"

#include "../../util/tool.h"


CListWidget::CListWidget(QWidget* parent)
        : QListWidget(parent) {
    // Intentionally empty
}

QSize CListWidget::sizeHint () const {
    return QSize(100, 120);
}

void CListWidget::setCharWidth(int width) {
    setMaximumWidth(util::tool::mWidth(this, width));
}
