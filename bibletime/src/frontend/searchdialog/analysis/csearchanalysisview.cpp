/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "csearchanalysisview.h"
#include "csearchanalysisscene.h"

#include <QWidget>
#include <QGraphicsView>
#include <QResizeEvent>


namespace Search {

CSearchAnalysisView::CSearchAnalysisView(CSearchAnalysisScene* scene, QWidget* parent)
 : QGraphicsView(scene, parent)
{
	setFocusPolicy(Qt::WheelFocus);
	resize(sizeHint());
}


/** Returns the sizeHint for this view */
QSize CSearchAnalysisView::sizeHint() const {
	if ( parentWidget() )
		return parentWidget()->sizeHint();
	return QGraphicsView::sizeHint();
}

/** No descriptions */
void CSearchAnalysisView::resizeEvent( QResizeEvent* e) {
	QGraphicsView::resizeEvent(e);
	scene()->setSceneRect(0,0, scene()->sceneRect().width(), viewport()->height() );
}


/** Returns the item at position p. If there no item at that point return 0.
Is needed?
*/
//CSearchAnalysisItem* CSearchAnalysisView::itemAt( const QPoint& p )
//{
//}



}
