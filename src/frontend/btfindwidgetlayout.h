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

#ifndef BTFINDWIDGETLAYOUT_H
#define BTFINDWIDGETLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>
#include <QWidgetItem>

class BtFindWidgetLayout : public QLayout {

public:
    BtFindWidgetLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    BtFindWidgetLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~BtFindWidgetLayout();

    void addItem(QLayoutItem *item);
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;
    int xSpacing(QWidget* wid) const;
    int ySpacing(QWidget* wid) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};

#endif
