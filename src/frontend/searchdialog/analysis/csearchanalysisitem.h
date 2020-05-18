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

#ifndef SEARCHCSEARCHANALYSISITEM_H
#define SEARCHCSEARCHANALYSISITEM_H

#include <QGraphicsRectItem>

#include <QGraphicsScene>
#include "../../../backend/cswordmodulesearch.h"


class CSwordModuleInfo;

namespace Search {

class CSearchAnalysisItem : public QGraphicsRectItem {
    public:
        CSearchAnalysisItem(const int moduleCount, const QString &bookname,
                            double *scaleFactor,
                            const CSwordModuleSearch::Results &results);

        ~CSearchAnalysisItem() override;

        /**
          Sets the resultcount of this item.
        */
        inline void setCountForModule(const int moduleIndex, const int count) {
            m_resultCountArray[moduleIndex] = count;
        }

        /**
          Returns the resultcount of this item.
        */
        inline int getCountForModule(const int moduleIndex) const {
            return m_resultCountArray[moduleIndex];
        }

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
        int m_moduleCount;
        QVector<int> m_resultCountArray;
        QPixmap* m_bufferPixmap;

};

}

#endif
