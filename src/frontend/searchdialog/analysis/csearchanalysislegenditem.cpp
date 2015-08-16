/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/analysis/csearchanalysislegenditem.h"

#include <QFont>
#include <QPainter>
#include <QPen>
#include "backend/drivers/cswordmoduleinfo.h"
#include "frontend/searchdialog/analysis/csearchanalysisscene.h"


namespace Search {

const int ITEM_TEXT_SIZE = 8;

const int LEGEND_INNER_BORDER = 5;
const int LEGEND_DELTAY = 4;


void CSearchAnalysisLegendItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->save();

    //the outer rectangle
    QPoint p1( (int)(rect().x()), (int)(rect().y()) );
    QPoint p2( (int)(rect().x() + rect().width() ), (int)(rect().y() + rect().height()));
    QRect r(p1, p2);
    r = r.normalized();
    painter->drawRect(r);

    QFont f = painter->font();
    f.setPointSize(ITEM_TEXT_SIZE);
    painter->setFont(f);

    //   for (unsigned int index=0; index < m_moduleList->count(); index++){
    int moduleIndex = 0;
    Q_FOREACH(CSwordModuleInfo const * const m, m_moduleList) {
        // the module color indicators
        QPoint p1( (int)(rect().x()) + LEGEND_INNER_BORDER, (int)(rect().y()) + LEGEND_INNER_BORDER + moduleIndex*(LEGEND_DELTAY + ITEM_TEXT_SIZE) );
        QPoint p2(p1.x() + ITEM_TEXT_SIZE, p1.y() + ITEM_TEXT_SIZE);
        QRect r(p1, p2);
        painter->fillRect(r, QBrush(CSearchAnalysisScene::getColor(moduleIndex)) );
        r = r.normalized();
        painter->drawRect(r);

        QPoint p3( p2.x() + LEGEND_INNER_BORDER, p2.y() );
        painter->drawText(p3, m->name() );

        ++moduleIndex;
    }
    painter->restore();
}


}
