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

#ifndef SEARCHCSEARCHANALYSISVIEW_H
#define SEARCHCSEARCHANALYSISVIEW_H

#include <QGraphicsView>

#include <QSize>


class QResizeEvent;

namespace Search {

class CSearchAnalysisScene;

/**
    @author The BibleTime team <info@bibletime.info>
*/
class CSearchAnalysisView : public QGraphicsView {
    public:
        CSearchAnalysisView(CSearchAnalysisScene* scene, QWidget* parent);

    protected:

        void resizeEvent(QResizeEvent* e) override;

};

}

#endif
