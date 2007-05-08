/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSEARCHANALYSIS_H
#define CSEARCHANALYSIS_H

//BibleTime includes

#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"
#include "backend/cswordmodulesearch.h"

#include "util/cpointers.h"

//Sword includes


//Qt includes
#include <qwidget.h>
#include <qstring.h>
#include <qcanvas.h>
#include <qdict.h>
#include <qtooltip.h>

//KDE includes
#include <kdialog.h>
#include <kdialogbase.h>
#include <klistview.h>


class QTextEdit;

namespace Search {
	namespace Analysis {

//forward declarations
class CSearchAnalysisItem;
class CSearchAnalysisLegendItem;
class CSearchAnalysisView;

/**
 * CSearchDialogAnaylsis shows the graphical analysis of the search result.
 * @author The BibleTime Team
 */
class CSearchAnalysis : public QCanvas {
	Q_OBJECT
public:
	CSearchAnalysis(QObject *parent=0, const char *name=0);
	virtual ~CSearchAnalysis();
	/**
	* Starts the analysis of the search result.
	* This should be called only once because
	* QCanvas handles the updates automatically.
	*/
	void analyse(ListCSwordModuleInfo modules);
	/**
	* This function returns a color for each module
	* @return The color at position index in the list
	*/
	static QColor getColor(int index);
	/*
	* This function returns a pointer to the list of AnalysisItems
	*/
	QDict<CSearchAnalysisItem>* getSearchAnalysisItemList();
	void reset();

protected slots: // Protected slots
	/**
	* No descriptions
	*/
	void slotResized();

protected:
	void setModules(ListCSwordModuleInfo modules);

private:
	/**
	* Returns the count of the book in the module
	*/
	const unsigned int getCount( const QString book, CSwordModuleInfo* module );

	ListCSwordModuleInfo m_moduleList;
	QDict<CSearchAnalysisItem> m_canvasItemList;
	QMap<CSwordModuleInfo*,unsigned int> m_lastPosList;
	int m_maxCount;
	double m_scaleFactor;
	CSearchAnalysisLegendItem* m_legend;

public slots: // Public slots
	void saveAsHTML();
};


class CSearchAnalysisDialog : public KDialogBase  {
	Q_OBJECT
public:
	CSearchAnalysisDialog(ListCSwordModuleInfo modules, QWidget* parentDialog);
	~CSearchAnalysisDialog();

protected: // Protected methods
	/**
	 * Initializes the widgets SIGNAL and SLOT connections,.
	 */
	void initConnections();
	
	/**
	 * Initializes this dialog.
	 */
	void initView();

private:
	CSearchAnalysis* m_analysis;
	CSearchAnalysisView* m_analysisView;
};


//----------------------------------------------------------
//----------------------------------------------------------

/** Paints the bars for one book for one or more modules
 *
 */
class CSearchAnalysisItem : public QCanvasRectangle  {
public:
	/**
	* Standard constructor.
	*/
	CSearchAnalysisItem(QCanvas* parent, const int moduleCount, const QString& bookname, double *scaleFactor, ListCSwordModuleInfo* modules);
	virtual ~CSearchAnalysisItem();
	/**
	* Sets the resultcount of this item
	*/
	void setCountForModule( const int moduleIndex, const int count);

	/**
	* Returns the resultcount of this item
	*/
	int getCountForModule( const int moduleIndex);
	/**
	* Returns the width of this item.
	*/
	virtual int width();
	/**
	* Returns the tooltip for this item.
	*/
	const QString getToolTip();

private:
	virtual void draw (QPainter & painter);

	ListCSwordModuleInfo* m_moduleList;
	double *m_scaleFactor;
	QString m_bookName;
	int m_moduleCount;
	QMemArray<int> m_resultCountArray;
	QPixmap* m_bufferPixmap;
};

class CSearchAnalysisLegendItem : public QCanvasRectangle  {

public:
	CSearchAnalysisLegendItem(QCanvas* parent, ListCSwordModuleInfo* list );

private:
	virtual void draw (QPainter & painter);
	ListCSwordModuleInfo* m_moduleList;
};

//----------------------------------------------------------
//----------------------------------------------------------
class CSearchAnalysisView : public QCanvasView  {
	Q_OBJECT
public:
	/**
	* Standard constructor
	*/
	CSearchAnalysisView(QCanvas* canvas, QWidget* parent);
	/**
	* Returns the sizeHint for this view
	* We give back the size of the parent widgetas default.
	* This is a reimplementation from QCanvasView::sizeHint().
	*/
	virtual QSize sizeHint();
	/**
	* Returns the item at position p.
	* If there no item at that point return 0.
	*/
	CSearchAnalysisItem* itemAt( const QPoint& p );

protected:
	/**
	* Reimplementation.
	*/
	virtual void resizeEvent(QResizeEvent* e);

private:
	class ToolTip : public QToolTip {
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

	} //end of namespace Search::Analysis
} //end of namespace Search

#endif
