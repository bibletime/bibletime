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

#pragma once

#include <QGraphicsRectItem>
#include "../../../backend/cswordmodulesearch.h"


namespace Search {

class CSearchAnalysisScene;

class CSearchAnalysisLegendItem : public QGraphicsRectItem {
    public: /* Methods: */
        CSearchAnalysisLegendItem(CSwordModuleSearch::Results const * results)
            : m_results(results)
        {}

    private: /* Methods: */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    private: /* Fields: */

        CSwordModuleSearch::Results const * const m_results;

};

} // namespace Search
