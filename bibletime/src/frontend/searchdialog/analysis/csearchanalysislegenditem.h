//
// C++ Interface: csearchanalysislegenditem
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SEARCHCSEARCHANALYSISLEGENDITEM_H
#define SEARCHCSEARCHANALYSISLEGENDITEM_H

#include "backend/drivers/cswordmoduleinfo.h"

#include <QGraphicsRectItem>

namespace Search {

/**
	@author The BibleTime team <info@bibletime.info>
*/
class CSearchAnalysisLegendItem : public QGraphicsRectItem
{
public:
	CSearchAnalysisLegendItem(ListCSwordModuleInfo* list );

private:
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
	ListCSwordModuleInfo* m_moduleList;

};

}

#endif
