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

#include "csearchanalysislegenditem.h"

#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <vector>
#include "../../../backend/cswordmodulesearch.h"
#include "../../../backend/drivers/cswordmoduleinfo.h"
#include "csearchanalysisscene.h"


namespace Search {

const int ITEM_TEXT_SIZE = 8;

const int LEGEND_INNER_BORDER = 5;
const int LEGEND_DELTAY = 4;


void CSearchAnalysisLegendItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
    painter->save();

    //the outer rectangle
    #define S(...) static_cast<int>(__VA_ARGS__)
    QPoint p1(S(rect().x()), S(rect().y()));
    QPoint p2(S(rect().x() + rect().width()), S(rect().y() + rect().height()));
    QRect r(p1, p2);
    r = r.normalized();
    painter->drawRect(r);

    QFont f = painter->font();
    f.setPointSize(ITEM_TEXT_SIZE);
    painter->setFont(f);

    //   for (unsigned int index=0; index < m_moduleList->count(); index++){
    int moduleIndex = 0;
    for (auto const & result : *m_results) {
        auto const * const m = result.module;
        // the module color indicators
        QPoint p1(S(rect().x()) + LEGEND_INNER_BORDER, S(rect().y()) + LEGEND_INNER_BORDER + moduleIndex*(LEGEND_DELTAY + ITEM_TEXT_SIZE));
    #undef S
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
