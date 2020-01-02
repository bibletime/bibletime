/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef SEARCHCSEARCHANALYSISLEGENDITEM_H
#define SEARCHCSEARCHANALYSISLEGENDITEM_H

#include <QGraphicsRectItem>

#include "../../../backend/drivers/btmodulelist.h"


namespace Search {

class CSearchAnalysisLegendItem : public QGraphicsRectItem {
    public: /* Methods: */
        inline CSearchAnalysisLegendItem(const BtConstModuleList &modules)
            : m_moduleList(modules) {}

    private: /* Methods: */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    private: /* Fields: */
        BtConstModuleList m_moduleList;

};

} // namespace Search

#endif
