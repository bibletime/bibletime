/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btfindwidgetlayout.h"
#include <QtGui>

/// Custom layout manager for the BtFindWidget. Assumes that 4 items are added to the layout
/// LineEdit, Previous button, Next button, Case Sensitive checkbox

BtFindWidgetLayout::BtFindWidgetLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent),
      m_hSpace(hSpacing),
      m_vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}

BtFindWidgetLayout::BtFindWidgetLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing),
      m_vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}

BtFindWidgetLayout::~BtFindWidgetLayout() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void BtFindWidgetLayout::addItem(QLayoutItem *item) {
    itemList.append(item);
}

int BtFindWidgetLayout::horizontalSpacing() const {
    if (m_hSpace >= 0) {
        return m_hSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int BtFindWidgetLayout::verticalSpacing() const {
    if (m_vSpace >= 0) {
        return m_vSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int BtFindWidgetLayout::count() const {
    return itemList.size();
}

QLayoutItem *BtFindWidgetLayout::itemAt(int index) const {
    return itemList.value(index);
}

QLayoutItem *BtFindWidgetLayout::takeAt(int index) {
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    else
        return 0;
}

Qt::Orientations BtFindWidgetLayout::expandingDirections() const {
    return 0;
}

bool BtFindWidgetLayout::hasHeightForWidth() const {
    return true;
}

int BtFindWidgetLayout::heightForWidth(int width) const {
    int height = doLayout(QRect(0, 0, width, 0), true);
    return height;
}

void BtFindWidgetLayout::setGeometry(const QRect &rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize BtFindWidgetLayout::sizeHint() const {
    return minimumSize();
}

QSize BtFindWidgetLayout::minimumSize() const {
    QSize size;
    QLayoutItem *item;
    foreach (item, itemList)
        size = size.expandedTo(item->minimumSize());

    size += QSize(2*margin(), 2*margin());
    return size;
}

int BtFindWidgetLayout::xSpacing(QWidget* wid) const {
    int spaceX = horizontalSpacing();
    if (spaceX == -1)
        spaceX = wid->style()->layoutSpacing(
            QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
    return spaceX;
}

int BtFindWidgetLayout::ySpacing(QWidget* wid) const {
    int spaceY = verticalSpacing();
    if (spaceY == -1)
        spaceY = wid->style()->layoutSpacing(
            QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
    return spaceY;
}

int BtFindWidgetLayout::doLayout(const QRect &rect, bool testOnly) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    QLayoutItem *item;
    foreach (item, itemList) {
        QWidget *wid = item->widget();
        int spaceX = xSpacing(wid);
        int spaceY = ySpacing(wid);
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!testOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int BtFindWidgetLayout::smartSpacing(QStyle::PixelMetric pm) const {
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, 0, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
