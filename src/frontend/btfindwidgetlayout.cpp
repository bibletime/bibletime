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

BtFindWidgetLayout::BtFindWidgetLayout(QWidget *parent)
    : QLayout(parent) {
}

BtFindWidgetLayout::~BtFindWidgetLayout() {
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void BtFindWidgetLayout::addItem(QLayoutItem *item) {
    itemList.append(item);
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

int BtFindWidgetLayout::horizontalSpacing() const {
    return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int BtFindWidgetLayout::verticalSpacing() const {
    return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
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

    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    size += QSize(left + right, top + bottom);
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

static void layout_one_row(const QRect& rect, QList<int>& x, QList<int>& widths, int spaceX, int spaceY) {
    x[3] = rect.right() - widths[3];
    x[2] = x[3] - spaceX - widths[2];
    x[1] = x[2] - spaceX - widths[1];
    widths[0] = x[1] -spaceX - x[0];
}

static void layout_two_rows(const QRect& rect, QList<int>& x, QList<int>& y, QList<int>& widths, int spaceX, int lineHeight) {
    x[2] = x[1] + spaceX + widths[1];
    x[3] = x[2] + spaceX + widths[2];
    y[1] = y[0] + lineHeight;
    y[2] = y[1];
    y[3] = y[2];
    widths[0] = rect.right() - x[0];
}

static void layout_three_rows(const QRect& rect, QList<int>& x, QList<int>& y, QList<int>& widths, int spaceX, int lineHeight) {
    x[2] = x[1] + spaceX + widths[1];
    y[1] = y[0] + lineHeight;
    y[2] = y[1];
    y[3] = y[2] + lineHeight;
    widths[0] = rect.right() - x[0];
}

static void layout_four_rows(const QRect& rect, QList<int>& x, QList<int>& y, QList<int>& widths, int spaceY, int lineHight) {
    widths[0] = rect.right() - x[0];
    y[1] = y[0] + spaceY + lineHight;
    y[2] = y[1] + spaceY + lineHight;
    y[3] = y[2] + spaceY + lineHight;
}

static bool fits_in_one_row(const QList<int>& widths, int spaceX, const QRect& rect)
{
    int width = widths[0] + widths[1] + widths[2] + widths[3] + 3 * spaceX;
    return width < rect.width();
}

static bool fits_in_two_rows(const QList<int>& widths, int spaceX, const QRect& rect)
{
    int width = widths[1] + widths[2] + widths[3] + 2 * spaceX;
    return width < rect.width();
}

static bool fits_in_three_rows(const QList<int>& widths, int spaceX, const QRect& rect)
{
    int width = qMax(widths[1] + widths[2] + spaceX, widths[3]);
    return width < rect.width();
}

int BtFindWidgetLayout::doLayout(const QRect &rect, bool testOnly) const {

    Q_ASSERT(itemList.count() == 4);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int spaceX = xSpacing(itemList[0]->widget());
    int spaceY = ySpacing(itemList[0]->widget());
    int lineHeight = itemList[0]->sizeHint().height();

    QList<int> widths, x, y;
    for (int i=0; i<itemList.count(); ++i) {
        widths << itemList[i]->sizeHint().width();
        x << left;
        y << top;
    }

    if (fits_in_one_row(widths, spaceX, effectiveRect))
        layout_one_row(effectiveRect, x, widths, spaceX, spaceY);
    else if (fits_in_two_rows(widths, spaceX, effectiveRect))
        layout_two_rows(effectiveRect, x, y, widths, spaceX, lineHeight);
    else if (fits_in_three_rows(widths, spaceX, effectiveRect))
        layout_three_rows(effectiveRect, x, y, widths, spaceX, lineHeight);
    else
        layout_four_rows(effectiveRect, x, y, widths, spaceY, lineHeight);

    if ( ! testOnly) {
        int yCentering = (effectiveRect.height() - (y[3] - y[0]) - lineHeight)/2;
        int y0c = y[0] + yCentering;
        int y1c = y[1] + yCentering;
        int y2c = y[2] + yCentering;
        int y3c = y[3] + yCentering;
        itemList[0]->setGeometry(QRect(QPoint(x[0], y0c), QSize(widths[0], lineHeight)));
        itemList[1]->setGeometry(QRect(QPoint(x[1], y1c), itemList[1]->sizeHint()));
        itemList[2]->setGeometry(QRect(QPoint(x[2], y2c), itemList[2]->sizeHint()));
        itemList[3]->setGeometry(QRect(QPoint(x[3], y3c), itemList[3]->sizeHint()));
    }
    int height =  y[3] + lineHeight + bottom ;
    return height;
}
