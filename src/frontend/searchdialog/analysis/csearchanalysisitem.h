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

#include <cstddef>
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
        void setCountForModule(int const moduleIndex, std::size_t const count)
        { m_resultCountArray[moduleIndex] = count; }

        /**
          Returns the resultcount of this item.
        */
        std::size_t getCountForModule(int const moduleIndex) const
        { return m_resultCountArray[moduleIndex]; }

        /**
        * Does one of the modules contain hits?
        */
        bool hasHitsInAnyModule() const;
        /**
        * Returns the width of this item.
        */
        int width() const;
        /**
        * Returns the tooltip for this item.
        */
        QString getToolTip() const;

    private:
        void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

    private: /* Fields: */
        CSwordModuleSearch::Results m_results;
        double *m_scaleFactor;
        QString m_bookName;
        QVector<std::size_t> m_resultCountArray;
        std::unique_ptr<QPixmap> m_bufferPixmap;

};

}
