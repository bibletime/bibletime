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

#include <QGraphicsScene>

#include <cstddef>
#include <memory>
#include <map>
#include <QColor>
#include "../../../backend/cswordmodulesearch.h"
#include "csearchanalysisitem.h"
#include "csearchanalysislegenditem.h"


class CSwordModuleInfo;
class QTextStream;

namespace Search {

class CSearchAnalysisScene : public QGraphicsScene {
        Q_OBJECT
    public:
        CSearchAnalysisScene(QString searchedText,
                             CSwordModuleSearch::Results const & results,
                             QObject * parent);

        /**
        * This function returns a color for each module
        * @return The color at position index in the list
        */
        static QColor getColor(int index);

        /**
         * resize the height of the scene
         */
        void resizeHeight(int height);

        void saveAsHTML() const;

    protected Q_SLOTS:
        /**
        * No descriptions
        */
        void slotResized();

    private:

        void saveAsHTML(QTextStream & out) const;

    private:

        QString const m_searchedText;
        CSwordModuleSearch::Results m_results;
        std::map<std::tuple<char, char>, CSearchAnalysisItem *> m_itemList;
        std::size_t m_maxCount = 0;
        std::unique_ptr<CSearchAnalysisLegendItem> m_legend;
};

}
