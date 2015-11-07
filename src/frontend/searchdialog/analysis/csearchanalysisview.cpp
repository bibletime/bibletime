/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/analysis/csearchanalysisview.h"

#include <QGraphicsView>
#include <QResizeEvent>
#include <QWidget>
#include "frontend/searchdialog/analysis/csearchanalysisscene.h"


namespace Search {

CSearchAnalysisView::CSearchAnalysisView(CSearchAnalysisScene* scene, QWidget* parent)
        : QGraphicsView(scene, parent) {
    setFocusPolicy(Qt::WheelFocus);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    resize(sizeHint());
}


/** Returns the sizeHint for this view */
QSize CSearchAnalysisView::sizeHint() const {
    return QGraphicsView::sizeHint();
}

/** No descriptions */
void CSearchAnalysisView::resizeEvent( QResizeEvent* e) {
    QGraphicsView::resizeEvent(e);
    scene()->setSceneRect(0, 0, scene()->sceneRect().width(), viewport()->height() );
}

}
