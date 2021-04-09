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

#include <QGraphicsScene>
#include <memory>
#include "../../../backend/cswordmodulesearch.h"


class CSwordModuleInfo;

namespace Search {

class CSearchAnalysisItem : public QGraphicsRectItem {
    public:
        CSearchAnalysisItem(const QString &bookname,
                            double *scaleFactor,
                            const CSwordModuleSearch::Results &results);

        /**
          Sets the resultcount of this item.
        */
        void setCountForModule(int const moduleIndex, int const count)
        { m_resultCountArray[moduleIndex] = count; }

        /**
          Returns the resultcount of this item.
        */
        int getCountForModule(int const moduleIndex) const
        { return m_resultCountArray[moduleIndex]; }

        /**
        * Does one of the modules contain hits?
        */
        bool hasHitsInAnyModule();
        /**
        * Returns the width of this item.
        */
        int width();
        /**
        * Returns the tooltip for this item.
        */
        const QString getToolTip();

    private:
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    private: /* Fields: */
        CSwordModuleSearch::Results m_results;
        double *m_scaleFactor;
        QString m_bookName;
        QVector<int> m_resultCountArray;
        std::unique_ptr<QPixmap> m_bufferPixmap;

};

}
