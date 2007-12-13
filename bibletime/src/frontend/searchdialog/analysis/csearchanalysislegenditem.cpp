//
// C++ Implementation: csearchanalysislegenditem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "csearchanalysislegenditem.h"
#include "csearchanalysisscene.h"

#include "backend/drivers/cswordmoduleinfo.h"

#include <QPainter>
#include <QPen>
#include <QFont>

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


CSearchAnalysisLegendItem::CSearchAnalysisLegendItem(QGraphicsScene *parent, ListCSwordModuleInfo *list )
	: QGraphicsRectItem(parent)
{
	m_moduleList = list;
}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisLegendItem::paint(QPainter& painter) {
	painter.save();

	setPen( QPen(Qt::black,2) );
	setBrush( QBrush(Qt::white) );
	//the outer rectangle
	QPoint p1( (int)x(), (int)y() );
	QPoint p2( (int)x()+rect().width(), (int)y() + rect().height() );
	QRect r(p1, p2);
	r.normalize();
	painter.drawRect(r);

	QFont f = painter.font();
	f.setPointSize(ITEM_TEXT_SIZE);
	painter.setFont(f);

	//   for (unsigned int index=0; index < m_moduleList->count(); index++){
	int moduleIndex = 0;
	ListCSwordModuleInfo::iterator end_it = m_moduleList->end();
	for (ListCSwordModuleInfo::iterator it(m_moduleList->begin()); it != end_it; ++it) {
		// the module color indicators
		QPoint p1( (int)x() + LEGEND_INNER_BORDER, (int)y() + LEGEND_INNER_BORDER + moduleIndex*(LEGEND_DELTAY + ITEM_TEXT_SIZE) );
		QPoint p2(p1.x() + ITEM_TEXT_SIZE, p1.y() + ITEM_TEXT_SIZE);
		QRect r(p1,p2);
		painter.fillRect(r, QBrush(CSearchAnalysisScene::getColor(moduleIndex)) );
		r.normalize();
		painter.drawRect(r);

		QPoint p3( p2.x() + LEGEND_INNER_BORDER, p2.y() );
		painter.drawText(p3, (*it)->name() );

		++moduleIndex;
	}
	painter.restore();
}


}
