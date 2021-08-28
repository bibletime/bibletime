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

#include "csearchanalysisitem.h"

#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <utility>
#include "csearchanalysisscene.h"


namespace Search {

const int ITEM_TEXT_SIZE = 8;

//used for the shift between the bars
const int BAR_DELTAX = 4;
const int BAR_DELTAY = 2;
const int BAR_WIDTH  = 2 + (2*BAR_DELTAX);  //should be equal or bigger than the label font size
// Used for the text below the bars
const int BAR_LOWER_BORDER = 100;

CSearchAnalysisItem::CSearchAnalysisItem(QString bookname, int numModules)
    : m_bookName(std::move(bookname))
    , m_counts(numModules, 0u)
{}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QFont f = painter->font();
    f.setPointSize(ITEM_TEXT_SIZE);
    painter->setFont(f);

    auto const moduleCount = m_counts.size();

    /**
    * We have to paint so many bars as we have modules available (we use moduleCount)
    * We paint inside the area which is given by height and width of this rectangle item
    */
    int index = 0;
    int drawn = 0;
    std::size_t Value = 0;

    //find out the biggest value
    for (index = 0;index < moduleCount; index++) {
        if (m_counts[index] > Value) {
            Value = m_counts[index];
        }
    }

    while (drawn < moduleCount) {
        for (index = 0; index < moduleCount; index++) {
            if (m_counts[index] == Value) {
                #define S(...) static_cast<int>(__VA_ARGS__)
                QPoint p1(S(rect().x()) + (moduleCount - drawn - 1)*BAR_DELTAX,
                          S(rect().height()) + S(y()) - BAR_LOWER_BORDER - (moduleCount - drawn)*BAR_DELTAY);
                QPoint p2(p1.x() + BAR_WIDTH,
                          p1.y() - S(!m_counts[index] ? 0 : ((m_counts[index])*m_scaleFactor)));
                #undef S
                QRect r(p1, p2);
                painter->fillRect(r, QBrush(CSearchAnalysisScene::getColor(index)) );
                painter->drawRect(r);
                drawn++;
            }
        }
        //finds the next smaller value
        std::size_t newValue = 0u;
        for (index = 0;index < moduleCount; index++)
            if (m_counts[index] < Value && m_counts[index] >= newValue)
                newValue = m_counts[index];
        Value = newValue;
    }
    if (!m_bufferPixmap) {
        m_bufferPixmap = std::make_unique<QPixmap>(width(), BAR_LOWER_BORDER);
        //m_bufferPixmap->resize(width(),BAR_LOWER_BORDER);
        m_bufferPixmap->fill();
        QPainter p(m_bufferPixmap.get());
        f = p.font();
        f.setPointSize(ITEM_TEXT_SIZE);
        p.setFont(f);
        p.rotate(90);
        p.drawText(QPoint(5, 0), m_bookName);
    }
    painter->drawPixmap(QPoint(int(rect().x()), int(rect().height() + y() - BAR_LOWER_BORDER)), *m_bufferPixmap);
}

/** Returns the width of this item. */
int CSearchAnalysisItem::width() const {
    auto const moduleCount = m_counts.size();
    return moduleCount * (moduleCount > 1 ? BAR_DELTAX : 0) + BAR_WIDTH;
}

}
