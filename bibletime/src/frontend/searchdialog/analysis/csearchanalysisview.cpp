//
// C++ Implementation: csearchanalysisview
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
