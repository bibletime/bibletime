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

#include <QFont>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QRect>
#include "../../../backend/drivers/cswordmoduleinfo.h"
#include "csearchanalysisscene.h"


namespace Search {

const int ITEM_TEXT_SIZE = 8;

//used for the shift between the bars
const int BAR_DELTAX = 4;
const int BAR_DELTAY = 2;
const int BAR_WIDTH  = 2 + (2*BAR_DELTAX);  //should be equal or bigger than the label font size
// Used for the text below the bars
const int BAR_LOWER_BORDER = 100;

CSearchAnalysisItem::CSearchAnalysisItem(
        QString const & bookname,
        CSwordModuleSearch::Results const & results,
        QVector<std::size_t> resultCountArray)
    : m_results(results)
    , m_bookName(bookname)
    , m_resultCountArray(std::move(resultCountArray))
{}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QFont f = painter->font();
    f.setPointSize(ITEM_TEXT_SIZE);
    painter->setFont(f);

    auto const moduleCount = m_resultCountArray.size();

    /**
    * We have to paint so many bars as we have modules available (we use moduleCount)
    * We paint inside the area which is given by height and width of this rectangle item
    */
    int index = 0;
    int drawn = 0;
    std::size_t Value = 0;

    //find out the biggest value
    for (index = 0;index < moduleCount; index++) {
        if (m_resultCountArray[index] > Value) {
            Value = m_resultCountArray[index];
        }
    }

    while (drawn < moduleCount) {
        for (index = 0; index < moduleCount; index++) {
            if (m_resultCountArray[index] == Value) {
                #define S(...) static_cast<int>(__VA_ARGS__)
                QPoint p1(S(rect().x()) + (moduleCount - drawn - 1)*BAR_DELTAX,
                          S(rect().height()) + S(y()) - BAR_LOWER_BORDER - (moduleCount - drawn)*BAR_DELTAY);
                QPoint p2(p1.x() + BAR_WIDTH,
                          p1.y() - S(!m_resultCountArray[index] ? 0 : ((m_resultCountArray[index])*m_scaleFactor)));
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
            if (m_resultCountArray[index] < Value && m_resultCountArray[index] >= newValue)
                newValue = m_resultCountArray[index];
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
    auto const moduleCount = m_resultCountArray.size();
    return moduleCount * (moduleCount > 1 ? BAR_DELTAX : 0) + BAR_WIDTH;
}

/** Returns the tooltip for this item. */
QString CSearchAnalysisItem::getToolTip() const {
    QString toolTipString("<center><b>");
    toolTipString.append(m_bookName.toHtmlEscaped())
                 .append("</b></center><hr/><table cellspacing=\"0\" "
                         "cellpadding=\"3\" width=\"100%\" height=\"100%\" "
                         "align=\"center\">");

    /// \todo Fix that loop
    int i = 0;
    for (auto it = m_results.begin(); it != m_results.end(); ++it) {
        const CSwordModuleInfo * const info = it.key();

        const int count = it.value().getCount();
        const double percent = (info && count)
                             ? ((static_cast<double>(m_resultCountArray.at(i))
                                 * static_cast<double>(100.0))
                                / static_cast<double>(count))
                             : 0.0;
        toolTipString.append("<tr bgcolor=\"white\"><td><b><font color=\"")
                     .append(CSearchAnalysisScene::getColor(i).name()).append("\">")
                     .append(info ? info->name() : QString())
                     .append("</font></b></td><td>")
                     .append(QString::number(m_resultCountArray.at(i)))
                     .append(" (")
                     .append(QString::number(percent, 'g', 2))
                     .append("%)</td></tr>");
        ++i;
    }

    toolTipString.append("</table>");
    return toolTipString;
}

}
