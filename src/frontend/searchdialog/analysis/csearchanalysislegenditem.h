/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QGraphicsRectItem>
#include "../../../backend/cswordmodulesearch.h"


class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Search {

class CSearchAnalysisLegendItem : public QGraphicsRectItem {
    public: // methods:
        CSearchAnalysisLegendItem(CSwordModuleSearch::Results const * results)
            : m_results(results)
        {}

    private: // methods:
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    private: // fields:

        CSwordModuleSearch::Results const * const m_results;

};

} // namespace Search
