/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

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
