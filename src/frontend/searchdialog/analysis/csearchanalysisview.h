/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
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

        /**
            * Returns the sizeHint for this view
            * We give back the size of the parent widgetas default.
            * This is a reimplementation from QCanvasView::sizeHint().
            */
        virtual QSize sizeHint() const override;

    protected:
        /**
        * Reimplementation.
        */
        virtual void resizeEvent(QResizeEvent* e) override;

};

}

#endif
