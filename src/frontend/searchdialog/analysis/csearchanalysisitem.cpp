/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/analysis/csearchanalysisitem.h"

#include <QFont>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QRect>
#include "backend/drivers/cswordmoduleinfo.h"
#include "frontend/searchdialog/analysis/csearchanalysisscene.h"


namespace Search {

const int SPACE_BETWEEN_PARTS = 5;
const int RIGHT_BORDER = 15;
const int LEFT_BORDER = 15;
const int LOWER_BORDER = 10;
const int UPPER_BORDER = 10;

const int ITEM_TEXT_SIZE = 8;
const int LABEL_TEXT_SIZE = 6;

//used for the shift between the bars
const int BAR_DELTAX = 4;
const int BAR_DELTAY = 2;
const int BAR_WIDTH  = 2 + (2*BAR_DELTAX);  //should be equal or bigger than the label font size
// Used for the text below the bars
const int BAR_LOWER_BORDER = 100;

const int LEGEND_INNER_BORDER = 5;
const int LEGEND_DELTAY = 4;
const int LEGEND_WIDTH = 85;

CSearchAnalysisItem::CSearchAnalysisItem(
        const int moduleCount,
        const QString &bookname,
        double *scaleFactor,
        const CSwordModuleSearch::Results &results)
        : m_results(results),
          m_scaleFactor(scaleFactor),
          m_bookName(bookname),
          m_moduleCount(moduleCount),
          m_bufferPixmap(0)
{
    m_resultCountArray.resize(m_moduleCount);
    int index = 0;
    for (index = 0; index < m_moduleCount; ++index) m_resultCountArray[index] = 0;
}

CSearchAnalysisItem::~CSearchAnalysisItem() {
    delete m_bufferPixmap;
}

/** Sets the resultcount of this item for the given module */
void CSearchAnalysisItem::setCountForModule( const int moduleIndex, const int count) {
    m_resultCountArray[moduleIndex] = count;
}

/** Returns the resultcount of this item for the given module */
int CSearchAnalysisItem::getCountForModule( const int moduleIndex) {
    return m_resultCountArray[moduleIndex];
}


bool CSearchAnalysisItem::hasHitsInAnyModule() {
    foreach (const int hits, m_resultCountArray) {
        if (hits) return true;
    }
    return false;
}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    QFont f = painter->font();
    f.setPointSize(ITEM_TEXT_SIZE);
    painter->setFont(f);

    /**
    * We have to paint so many bars as we have modules available (we use m_moduleCount)
    * We paint inside the area which is given by height and width of this rectangle item
    */
    int index = 0;
    int drawn = 0;
    int Value = 0;

    //find out the biggest value
    for (index = 0;index < m_moduleCount; index++) {
        if (m_resultCountArray[index] > Value) {
            Value = m_resultCountArray[index];
        }
    };

    while (drawn < m_moduleCount) {
        for (index = 0; index < m_moduleCount; index++) {
            if (m_resultCountArray[index] == Value) {
                QPoint p1((int)rect().x() + (m_moduleCount - drawn - 1)*BAR_DELTAX,
                          (int)rect().height() + (int)y() - BAR_LOWER_BORDER - (m_moduleCount - drawn)*BAR_DELTAY);
                QPoint p2(p1.x() + BAR_WIDTH,
                          p1.y() - (int)( !m_resultCountArray[index] ? 0 : ((m_resultCountArray[index])*(*m_scaleFactor))) );
                QRect r(p1, p2);
                painter->fillRect(r, QBrush(CSearchAnalysisScene::getColor(index)) );
                painter->drawRect(r);
                drawn++;
            }
        }
        //finds the next smaller value
        int newValue = 0;
        for (index = 0;index < m_moduleCount; index++)
            if (m_resultCountArray[index] < Value && m_resultCountArray[index] >= newValue)
                newValue = m_resultCountArray[index];
        Value = newValue;
    }
    if (!m_bufferPixmap) {
        m_bufferPixmap = new QPixmap(width(), BAR_LOWER_BORDER);
        //m_bufferPixmap->resize(width(),BAR_LOWER_BORDER);
        m_bufferPixmap->fill();
        QPainter p(m_bufferPixmap);
        f = p.font();
        f.setPointSize(ITEM_TEXT_SIZE);
        p.setFont(f);
        p.rotate(90);
        p.drawText(QPoint(5, 0), m_bookName);
    }
    painter->drawPixmap(QPoint(int(rect().x()), int(rect().height() + y() - BAR_LOWER_BORDER)), *m_bufferPixmap);
}

/** Returns the width of this item. */
int CSearchAnalysisItem::width() {
    return m_moduleCount*(m_moduleCount > 1 ? BAR_DELTAX : 0) + BAR_WIDTH;
}

/** Returns the tooltip for this item. */
const QString CSearchAnalysisItem::getToolTip() {
    typedef CSwordModuleSearch::Results::const_iterator RCI;

    QString toolTipString = QString("<center><b>%1</b></center><hr/>").arg(m_bookName);
    toolTipString += "<table cellspacing=\"0\" cellpadding=\"3\" width=\"100%\" height=\"100%\" align=\"center\">";

    /// \todo Fix that loop
    int i = 0;
    for (RCI it = m_results.begin(); it != m_results.end(); it++) {
        const CSwordModuleInfo *info = it.key();

        /// \warning This is a workaround for sword constness
        sword::ListKey &results = const_cast<sword::ListKey &>(it.value());

        const QColor c = CSearchAnalysisScene::getColor(i);

        toolTipString.append(
            QString("<tr bgcolor=\"white\"><td><b><font color=\"%1\">%2</font></b></td><td>%3 (%4%)</td></tr>")
            .arg(c.name())
            .arg(info ? info->name() : QString::null)
            .arg( m_resultCountArray[i] )
            .arg( (info && m_resultCountArray[i]) ? ((double)m_resultCountArray[i] / (double)results.Count())*(double)100 : 0.0, 0, 'g', 2)
        );
        ++i;
    }

    toolTipString += "</table>";

    return toolTipString;
}

}
