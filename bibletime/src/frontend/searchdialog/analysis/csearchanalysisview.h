//
// C++ Interface: csearchanalysisview
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SEARCHCSEARCHANALYSISVIEW_H
#define SEARCHCSEARCHANALYSISVIEW_H

#include <QGraphicsView>

namespace Search {

/**
	@author The BibleTime team <info@bibletime.info>
*/
class CSearchAnalysisView : public QGraphicsView
{
public:
    CSearchAnalysisView(CSearchAnalysisScene* scene, QWidget* parent);

    ~CSearchAnalysisView();

/**
	* Returns the sizeHint for this view
	* We give back the size of the parent widgetas default.
	* This is a reimplementation from QCanvasView::sizeHint().
	*/
	virtual QSize sizeHint();
	/**
	* Returns the item at position p or 0 if there is no item.
	*/
	CSearchAnalysisItem* itemAt( const QPoint& p );

protected:
	/**
	* Reimplementation.
	*/
	virtual void resizeEvent(QResizeEvent* e);

private:
	class ToolTip{
	public:
		ToolTip(QWidget* parent);
		virtual ~ToolTip() {}
		;
		/**
		* Displays a tooltip for position p
		*/
		virtual void maybeTip(const QPoint &pos);
	};
	
	ToolTip* m_toolTip;

};

}

#endif
