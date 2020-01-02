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

#ifndef SEARCHCSEARCHANALYSISSCENE_H
#define SEARCHCSEARCHANALYSISSCENE_H

#include <QGraphicsScene>

#include <QColor>
#include <QHash>
#include <QMap>
#include "../../../backend/cswordmodulesearch.h"
#include "csearchanalysisitem.h"


class CSwordModuleInfo;

namespace Search {

class CSearchAnalysisLegendItem;

/**
    @author The BibleTime team <info@bibletime.info>
*/
class CSearchAnalysisScene : public QGraphicsScene {
        Q_OBJECT
    public:
        CSearchAnalysisScene(QObject* parent);

        /**
        * Starts the analysis of the search result.
        * This should be called only once because
        * QCanvas handles the updates automatically.
        */
        void analyse(const CSwordModuleSearch::Results &results);

        /**
        * This function returns a color for each module
        * @return The color at position index in the list
        */
        static QColor getColor(int index);

        void reset();
        /**
         * resize the height of the scene
         */
        void resizeHeight(int height);

    public slots:
        void saveAsHTML();

    protected slots: // Protected slots
        /**
        * No descriptions
        */
        void slotResized();

    protected:
        void setResults(const CSwordModuleSearch::Results &results);

    private:
        /**
        * Returns the count of the book in the module
        */
        unsigned int getCount(const QString &book, const CSwordModuleInfo *module);

        CSwordModuleSearch::Results m_results;
        QHash<QString, CSearchAnalysisItem*> m_itemList;
        QMap<const CSwordModuleInfo*, unsigned int> m_lastPosList;
        int m_maxCount;
        double m_scaleFactor;
        CSearchAnalysisLegendItem* m_legend;
};

}

#endif
