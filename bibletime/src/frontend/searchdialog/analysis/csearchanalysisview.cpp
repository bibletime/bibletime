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

namespace Search {

CSearchAnalysisView::CSearchAnalysisView(CSearchAnalysisScene* scene, QWidget* parent)
 : QGraphicsView(scene, parent)
{
	setFocusPolicy(Qt::WheelFocus);
	m_toolTip = new ToolTip(this);
	resize(sizeHint());
}


CSearchAnalysisView::~CSearchAnalysisView()
{
}


/** Returns the sizeHint for this view */
QSize CSearchAnalysisView::sizeHint() {
	if ( parentWidget() )
		return parentWidget()->sizeHint();
	return QGraphicsView::sizeHint();
}

/** No descriptions */
void CSearchAnalysisView::resizeEvent( QResizeEvent* e) {
	QGraphicsView::resizeEvent(e);
	scene()->resize( scene()->width(), viewport()->height() );
}


void CSearchAnalysisView::ToolTip::maybeTip(const QPoint& p) {
	CSearchAnalysisView* view = dynamic_cast<CSearchAnalysisView*>(parentWidget());
	if (!view)
		return;
	QPoint point(p);
	point = view->viewport()->mapFrom(view, point);
	CSearchAnalysisItem* i = view->itemAt( view->viewportToContents(point) );
	if (!i)
		return;

	//get type of item and display correct text
	QString text = i->getToolTip();
	if (text.isEmpty())
		return;

	QPoint p1 = view->viewport()->mapTo(view, view->contentsToViewport(i->rect().topLeft()));
	p1.setY(0);
	QPoint p2 = view->viewport()->mapTo(view, view->contentsToViewport(i->rect().bottomRight()));
	p2.setY(view->height());
	QRect r = QRect( p1, p2 );
	if (r.contains(p))
		tip(r, text);
}


/** Returns the item at position p. If there no item at that point return 0.
Is needed?
*/
//CSearchAnalysisItem* CSearchAnalysisView::itemAt( const QPoint& p )
//{
//}



}
