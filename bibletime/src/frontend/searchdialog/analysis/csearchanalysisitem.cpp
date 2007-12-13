//
// C++ Implementation: csearchanalysisitem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "csearchanalysisitem.h"

#include <QGraphicsRectItem>
#include <QPainter>
#include <QFont>
#include <QPen>
#include <QPoint>
#include <QRect>


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




CSearchAnalysisItem::CSearchAnalysisItem(QGraphicsScene *parent, const int moduleCount, const QString &bookname, double *scaleFactor, ListCSwordModuleInfo* modules)
	: QGraphicsRectItem(parent),
	m_moduleList( modules ),
	m_scaleFactor(scaleFactor),
	m_bookName(bookname),
	m_moduleCount(moduleCount),
	m_bufferPixmap(0)
{
	m_resultCountArray.resize(m_moduleCount);
	int index = 0;
	for (index = 0; index < m_moduleCount; ++index)
		m_resultCountArray[index] = 0;
	setToolTip(getToolTip());
}

CSearchAnalysisItem::~CSearchAnalysisItem()
{
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

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisItem::paint(QPainter& painter) {
	QFont f = painter.font();
	f.setPointSize(ITEM_TEXT_SIZE);
	painter.setFont(f);

	setPen(QPen(black,1));
	setBrush(Qt::red);
	/**
	* We have to paint so many bars as we have modules available (we use m_moduleCount)
	* We paint inside the area which is given by height and width of this rectangle item
	*/
	int index = 0;
	int drawn = 0;
	int Value = 0;

	//find out the biggest value
	for (index=0;index < m_moduleCount; index++) {
		if (m_resultCountArray[index] > Value) {
			Value = m_resultCountArray[index];
		}
	};

	while (drawn < m_moduleCount) {
		for (index = 0; index < m_moduleCount; index++) {
			if (m_resultCountArray[index] == Value) {
				QPoint p1((int)x() + (m_moduleCount-drawn-1)*BAR_DELTAX,
						  (int)height() + (int)y() - BAR_LOWER_BORDER - (m_moduleCount-drawn)*BAR_DELTAY);
				QPoint p2(p1.x() + BAR_WIDTH,
						  p1.y() - (int)( !m_resultCountArray[index] ? 0 : ((m_resultCountArray[index])*(*m_scaleFactor))) );
				QRect r(p1, p2);
				painter.fillRect(r, QBrush(CSearchAnalysis::getColor(index)) );
				painter.drawRect(r);
				drawn++;
			}
		}
		//finds the next smaller value
		int newValue = 0;
		for (index=0;index < m_moduleCount; index++)
			if (m_resultCountArray[index] < Value && m_resultCountArray[index] >= newValue)
				newValue = m_resultCountArray[index];
		Value = newValue;
	}
	if (!m_bufferPixmap) {
		m_bufferPixmap = new QPixmap();
		m_bufferPixmap->resize(width(),BAR_LOWER_BORDER);
		m_bufferPixmap->fill();
		QPainter p(m_bufferPixmap);
		f = p.font();
		f.setPointSize(ITEM_TEXT_SIZE);
		p.setFont(f);
		p.rotate(90);
		p.drawText(QPoint(5,0), m_bookName);
	}
	painter.drawPixmap(QPoint(int(x()),int(height()+y()-BAR_LOWER_BORDER)), *m_bufferPixmap);
}

/** Returns the width of this item. */
int CSearchAnalysisItem::width() {
	return m_moduleCount*(m_moduleCount>1 ? BAR_DELTAX : 0) + BAR_WIDTH;
}

/** Returns the tooltip for this item. */
const QString CSearchAnalysisItem::getToolTip() {
	QString toolTipString = QString("<center><b>%1</b></center><hr/>").arg(m_bookName);
	toolTipString += "<table cellspacing=\"0\" cellpadding=\"3\" width=\"100%\" height=\"100%\" align=\"center\">";

	//ToDo: Fix that loop
	int i = 0;
	ListCSwordModuleInfo::iterator end_it = m_moduleList->end();

	for (ListCSwordModuleInfo::iterator it(m_moduleList->begin()); it != end_it; ++it) {
		//  for (int i = 0; i < m_moduleCount; ++i) {
		CSwordModuleInfo* info = (*it);
		const QColor c = CSearchAnalysis::getColor(i);

		ret.append(
			QString("<tr bgcolor=\"white\"><td><b><font color=\"#%1\">%2</font></b></td><td>%3 (%4%)</td></tr>")
			.arg(QString().sprintf("%02X%02X%02X",c.red(),c.green(),c.blue()))
			.arg(info ? info->name() : QString::null)
			.arg( m_resultCountArray[i] )
			.arg( (info && m_resultCountArray[i])? ((double)m_resultCountArray[i] / (double)info->searchResult().Count())*(double)100 : 0.0, 0, 'g', 2)
		);
		++i;
	}

	toolTipString += "</table>";

	return toolTipString;
}

}
