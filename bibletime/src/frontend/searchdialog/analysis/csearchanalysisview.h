/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
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

        ~CSearchAnalysisView() {}

        /**
        	* Returns the sizeHint for this view
        	* We give back the size of the parent widgetas default.
        	* This is a reimplementation from QCanvasView::sizeHint().
        	*/
        virtual QSize sizeHint() const;

        /**
        * Returns the item at position p or 0 if there is no item.
        */
        //CSearchAnalysisItem* itemAt( const QPoint& p );

    protected:
        /**
        * Reimplementation.
        */
        virtual void resizeEvent(QResizeEvent* e);

};

}

#endif
