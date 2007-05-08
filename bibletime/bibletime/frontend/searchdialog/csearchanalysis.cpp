/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "csearchanalysis.h"
#include "csearchdialog.h"

#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"

#include "frontend/cbtconfig.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

//Qt includes
#include <qhbox.h>
#include <qvbox.h>
#include <qptrlist.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qmap.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qsizepolicy.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qregexp.h>
#include <qmessagebox.h>

//KDE includes
#include <kapplication.h>
#include <kfiledialog.h>
#include <klocale.h>
#include <kiconloader.h>

namespace Search {
	namespace Analysis {
	
const int SPACE_BETWEEN_PARTS = 5;
const int RIGHT_BORDER = 15;
const int LEFT_BORDER = 15;
const int LOWER_BORDER = 10;
const int UPPER_BORDER = 10;

const int ITEM_TEXT_SIZE = 8;
const int LABEL_TEXT_SIZE = 6;

//used for the shift between the bars
const int BAR_DELTAX = 4;
const int BAR_DELTAY = 2;
const int BAR_WIDTH  = 2 + (2*BAR_DELTAX);  //should be equal or bigger than the label font size
// Used for the text below the bars
const int BAR_LOWER_BORDER = 100;

const int LEGEND_INNER_BORDER = 5;
const int LEGEND_DELTAY = 4;
const int LEGEND_WIDTH = 85;

/****************************/

CSearchAnalysisDialog::CSearchAnalysisDialog( ListCSwordModuleInfo modules, QWidget* parentDialog ) : KDialogBase(Plain, i18n("Search analysis"), Close, Close, parentDialog, 0, true) {
	initView();
	initConnections();
	m_analysis->reset();
	m_analysis->analyse(modules);
	showMaximized();
};

CSearchAnalysisDialog::~CSearchAnalysisDialog() {}
;

/** Initializes this dialog. */
void CSearchAnalysisDialog::initView() {
	QVBoxLayout* layout = new QVBoxLayout(plainPage(),0);

	QPushButton* button = new QPushButton(plainPage(), "button");
	button->setIconSet(SmallIconSet("filesave"));
	button->setText(i18n("Save search analysis as HTML"));
	button->setFixedSize(button->sizeHint());
	layout->addWidget(button);
	layout->addSpacing(10);

	m_analysis = new CSearchAnalysis(plainPage());
	m_analysisView = new CSearchAnalysisView(m_analysis, plainPage());
	m_analysisView->show();
	layout->addWidget(m_analysisView);

	connect(button, SIGNAL(clicked()), m_analysis, SLOT(saveAsHTML()));
}

/** Initializes the widgets SIGNAL and SLOT connections,. */
void CSearchAnalysisDialog::initConnections() {}

/****************************/
/*  CSearchAnalysis         */
/****************************/

CSearchAnalysis::CSearchAnalysis(QObject *parent, const char *name )
: QCanvas(parent,name) {

	m_scaleFactor = 0.0;
	m_legend = 0;
	setBackgroundColor(Qt::white);
	m_canvasItemList.resize(67);
	m_canvasItemList.setAutoDelete(true);
	resize(1,1);
	connect(this, SIGNAL(resized()), SLOT(slotResized()));
}

CSearchAnalysis::~CSearchAnalysis() {}

QDict<CSearchAnalysisItem>* CSearchAnalysis::getSearchAnalysisItemList() {
	// Returns pointer to the search analysis items
	return &m_canvasItemList;
}

/** Starts the analysis of the search result. This should be called only once because QCanvas handles the updates automatically. */
void CSearchAnalysis::analyse(ListCSwordModuleInfo modules) {
	/**
	* Steps of analysing our search result;
	* -Create the items for all available books ("Genesis" - "Revelation")
	* -Iterate through all modules we analyse
	*  -Go through all books of this module
	*   -Find out how many times we found the book
	*   -Set the count to the items which belongs to the book
	*/
	setModules(modules);

	m_lastPosList.clear();
	const int numberOfModules = m_moduleList.count();
	if (!numberOfModules)
		return;
	m_legend = new CSearchAnalysisLegendItem(this, &m_moduleList);
	m_legend->setX(LEFT_BORDER);
	m_legend->setY(UPPER_BORDER);
	m_legend->setSize(LEGEND_WIDTH,
					  LEGEND_INNER_BORDER*2 + ITEM_TEXT_SIZE*numberOfModules + LEGEND_DELTAY*(numberOfModules-1));
	m_legend->show();

	int xPos = LEFT_BORDER + m_legend->width() + SPACE_BETWEEN_PARTS;
	int moduleIndex = 0;
	m_maxCount = 0;
	int count = 0;
	CSwordVerseKey key(0);
	key.key("Genesis 1:1");

	CSearchAnalysisItem* analysisItem = m_canvasItemList[key.book()];
	bool ok = true;
	while (ok && analysisItem) {
		//   for (moduleIndex = 0,m_moduleList.first(); m_moduleList.current(); m_moduleList.next(),++moduleIndex) {
		moduleIndex = 0;
		ListCSwordModuleInfo::iterator end_it = m_moduleList.end();
		for (ListCSwordModuleInfo::iterator it(m_moduleList.begin()); it != end_it; ++it) {
			KApplication::kApplication()->processEvents( 10 ); //10 ms only
			if (!m_lastPosList.contains(*it)) {
				m_lastPosList.insert(*it,0);
			}

			analysisItem->setCountForModule(moduleIndex, (count = getCount(key.book(), *it)));
			m_maxCount = (count > m_maxCount) ? count : m_maxCount;

			++moduleIndex;
		}
		analysisItem->setX(xPos);
		analysisItem->setY(UPPER_BORDER);
		analysisItem->show();

		xPos += (int)analysisItem->width() + SPACE_BETWEEN_PARTS;
		ok = key.next(CSwordVerseKey::UseBook);
		analysisItem = m_canvasItemList[key.book()];
	}
	resize(xPos+BAR_WIDTH+(m_moduleList.count()-1)*BAR_DELTAX+RIGHT_BORDER, height() );
	slotResized();
}

/** Sets te module list used for the analysis. */
void CSearchAnalysis::setModules(ListCSwordModuleInfo modules) {
	m_moduleList.clear();
	//  for (modules.first(); modules.current(); modules.next()) {
	ListCSwordModuleInfo::iterator end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
		if ( ((*it)->type() == CSwordModuleInfo::Bible) || ((*it)->type() == CSwordModuleInfo::Commentary) ) { //a Bible or an commentary
			m_moduleList.append(*it);
		}
	}

	m_canvasItemList.clear();
	CSearchAnalysisItem* analysisItem = 0;
	CSwordVerseKey key(0);
	key.key("Genesis 1:1");
	do {
		analysisItem = new CSearchAnalysisItem(this, m_moduleList.count(), key.book(), &m_scaleFactor, &m_moduleList);
		analysisItem->hide();
		m_canvasItemList.insert(key.book(), analysisItem);
	}
	while (key.next(CSwordVerseKey::UseBook));
	update();
}

/** Sets back the items and deletes things to cleanup */
void CSearchAnalysis::reset() {
	m_scaleFactor = 0.0;

	QDictIterator<CSearchAnalysisItem> it( m_canvasItemList ); // iterator for items
	while ( it.current() ) {
		it.current()->hide();
		++it;
	}
	m_lastPosList.clear();

	if (m_legend) {
		m_legend->hide();
	}

	delete m_legend;
	m_legend = 0;

	update();
}

/** No descriptions */
void CSearchAnalysis::slotResized() {
	m_scaleFactor = (double)( (double)(height()-UPPER_BORDER-LOWER_BORDER-BAR_LOWER_BORDER-(m_moduleList.count()-1)*BAR_DELTAY)
							  /(double)m_maxCount);
	QDictIterator<CSearchAnalysisItem> it( m_canvasItemList );
	while ( it.current() ) {
		it.current()->setSize(BAR_WIDTH + (m_moduleList.count()-1)*BAR_DELTAX, height()-UPPER_BORDER-LOWER_BORDER);
		it.current()->setY(UPPER_BORDER);
		++it;
	}
	update();
}

/** This function returns a color for each module */
QColor CSearchAnalysis::getColor(int index) {
	switch (index) {
		case  0:
		return Qt::red;
		case  1:
		return Qt::darkGreen;
		case  2:
		return Qt::blue;
		case  3:
		return Qt::cyan;
		case  4:
		return Qt::magenta;
		case  5:
		return Qt::darkRed;
		case  6:
		return Qt::darkGray;
		case  7:
		return Qt::black;
		case  8:
		return Qt::darkCyan;
		case  9:
		return Qt::darkMagenta;
		default:
		return Qt::red;
	}
}

/** Returns the count of the book in the module */
const unsigned int CSearchAnalysis::getCount( const QString book, CSwordModuleInfo* module ) {
	sword::ListKey& result = module->searchResult();
	const int length = book.length();
	unsigned int i = m_lastPosList[module];
	unsigned int count = 0;
	const unsigned int resultCount = result.Count();
	while (i < resultCount) {
		if ( strncmp(book.utf8(), (const char*)*result.GetElement(i), length) )
			break;
		i++;
		++count;
	}
	m_lastPosList.contains(module) ? m_lastPosList.replace(module,i) : m_lastPosList.insert(module,i);

	return count;
}


//------------------------------------------------------------------
//------------------------------------------------------------------

CSearchAnalysisItem::CSearchAnalysisItem(QCanvas *parent, const int moduleCount, const QString &bookname, double *scaleFactor, ListCSwordModuleInfo* modules)
: QCanvasRectangle(parent),
m_moduleList( modules ),
m_scaleFactor(scaleFactor),
m_bookName(bookname),
m_moduleCount(moduleCount),
m_bufferPixmap(0) {
	m_resultCountArray.resize(m_moduleCount);
	int index = 0;
	for (index = 0; index < m_moduleCount; ++index)
		m_resultCountArray[index] = 0;
}

CSearchAnalysisItem::~CSearchAnalysisItem() {
	delete m_bufferPixmap;
}

/** Sets the resultcount of this item for the given module */
void CSearchAnalysisItem::setCountForModule( const int moduleIndex, const int count) {
	m_resultCountArray[moduleIndex] = count;
}

/** Returns the resultcount of this item for the given module */
int CSearchAnalysisItem::getCountForModule( const int moduleIndex) {
	return m_resultCountArray[moduleIndex];
}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisItem::draw(QPainter& painter) {
	QFont f = painter.font();
	f.setPointSize(ITEM_TEXT_SIZE);
	painter.setFont(f);

	setPen(QPen(black,1));
	setBrush(Qt::red);
	/**
	* We have to paint so many bars as we have modules available (we use m_moduleCount)
	* We paint inside the area which is given by height and width of this rectangle item
	*/
	int index = 0;
	int drawn = 0;
	int Value = 0;

	//find out the biggest value
	for (index=0;index < m_moduleCount; index++) {
		if (m_resultCountArray[index] > Value) {
			Value = m_resultCountArray[index];
		}
	};

	while (drawn < m_moduleCount) {
		for (index = 0; index < m_moduleCount; index++) {
			if (m_resultCountArray[index] == Value) {
				QPoint p1((int)x() + (m_moduleCount-drawn-1)*BAR_DELTAX,
						  (int)height() + (int)y() - BAR_LOWER_BORDER - (m_moduleCount-drawn)*BAR_DELTAY);
				QPoint p2(p1.x() + BAR_WIDTH,
						  p1.y() - (int)( !m_resultCountArray[index] ? 0 : ((m_resultCountArray[index])*(*m_scaleFactor))) );
				QRect r(p1, p2);
				painter.fillRect(r, QBrush(CSearchAnalysis::getColor(index)) );
				painter.drawRect(r);
				drawn++;
			}
		}
		//finds the next smaller value
		int newValue = 0;
		for (index=0;index < m_moduleCount; index++)
			if (m_resultCountArray[index] < Value && m_resultCountArray[index] >= newValue)
				newValue = m_resultCountArray[index];
		Value = newValue;
	}
	if (!m_bufferPixmap) {
		m_bufferPixmap = new QPixmap();
		m_bufferPixmap->resize(width(),BAR_LOWER_BORDER);
		m_bufferPixmap->fill();
		QPainter p(m_bufferPixmap);
		f = p.font();
		f.setPointSize(ITEM_TEXT_SIZE);
		p.setFont(f);
		p.rotate(90);
		p.drawText(QPoint(5,0), m_bookName);
	}
	painter.drawPixmap(QPoint(int(x()),int(height()+y()-BAR_LOWER_BORDER)), *m_bufferPixmap);
}

/** Returns the width of this item. */
int CSearchAnalysisItem::width() {
	return m_moduleCount*(m_moduleCount>1 ? BAR_DELTAX : 0) + BAR_WIDTH;
}

/** Returns the tooltip for this item. */
const QString CSearchAnalysisItem::getToolTip() {
	QString ret = QString("<center><b>%1</b></center><hr/>").arg(m_bookName);
	ret += "<table cellspacing=\"0\" cellpadding=\"3\" width=\"100%\" height=\"100%\" align=\"center\">";

	//ToDo: Fix that loop
	int i = 0;
	ListCSwordModuleInfo::iterator end_it = m_moduleList->end();

	for (ListCSwordModuleInfo::iterator it(m_moduleList->begin()); it != end_it; ++it) {
		//  for (int i = 0; i < m_moduleCount; ++i) {
		CSwordModuleInfo* info = (*it);
		const QColor c = CSearchAnalysis::getColor(i);

		ret.append(
			QString("<tr bgcolor=\"white\"><td><b><font color=\"#%1\">%2</font></b></td><td>%3 (%4%)</td></tr>")
			.arg(QString().sprintf("%02X%02X%02X",c.red(),c.green(),c.blue()))
			.arg(info ? info->name() : QString::null)
			.arg( m_resultCountArray[i] )
			.arg( (info && m_resultCountArray[i])? ((double)m_resultCountArray[i] / (double)info->searchResult().Count())*(double)100 : 0.0, 0, 'g', 2)
		);
		++i;
	}

	ret += "</table>";

	return ret;
}

//------------------------------------------------------------------
//------------------------------------------------------------------

CSearchAnalysisView::CSearchAnalysisView(QCanvas* canvas, QWidget* parent)
: QCanvasView(canvas, parent) {
	setFocusPolicy(QWidget::WheelFocus);
	m_toolTip = new ToolTip(this);
	resize(sizeHint());
}

/** Returns the sizeHint for this view */
QSize CSearchAnalysisView::sizeHint() {
	if ( parentWidget() )
		return parentWidget()->sizeHint();
	return QCanvasView::sizeHint();
}

/** No descriptions */
void CSearchAnalysisView::resizeEvent( QResizeEvent* e) {
	QCanvasView::resizeEvent(e);
	canvas()->resize( canvas()->width(), viewport()->height() );
}

CSearchAnalysisView::ToolTip::ToolTip(QWidget* parent) : QToolTip(parent) {}

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


/** Returns the item at position p. If there no item at that point return 0. */
CSearchAnalysisItem* CSearchAnalysisView::itemAt( const QPoint& p ) {
	QCanvasItemList l = canvas()->collisions(p);
	if (!l.count())
		return 0;
	return dynamic_cast<CSearchAnalysisItem*>(l.first());
}

//------------------------------------------------------------------
//------------------------------------------------------------------

CSearchAnalysisLegendItem::CSearchAnalysisLegendItem(QCanvas *parent, ListCSwordModuleInfo *list )
: QCanvasRectangle(parent) {
	m_moduleList = list;
}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisLegendItem::draw (QPainter& painter) {
	painter.save();

	setPen( QPen(black,2) );
	setBrush( Qt::white );
	//the outer rectangle
	QPoint p1( (int)x(), (int)y() );
	QPoint p2( (int)x()+width(), (int)y() + height() );
	QRect r(p1, p2);
	r.normalize();
	painter.drawRect(r);

	QFont f = painter.font();
	f.setPointSize(ITEM_TEXT_SIZE);
	painter.setFont(f);

	//   for (unsigned int index=0; index < m_moduleList->count(); index++){
	int moduleIndex = 0;
	ListCSwordModuleInfo::iterator end_it = m_moduleList->end();
	for (ListCSwordModuleInfo::iterator it(m_moduleList->begin()); it != end_it; ++it) {
		// the module color indicators
		QPoint p1( (int)x() + LEGEND_INNER_BORDER, (int)y() + LEGEND_INNER_BORDER + moduleIndex*(LEGEND_DELTAY + ITEM_TEXT_SIZE) );
		QPoint p2(p1.x() + ITEM_TEXT_SIZE, p1.y() + ITEM_TEXT_SIZE);
		QRect r(p1,p2);
		painter.fillRect(r, QBrush(CSearchAnalysis::getColor(moduleIndex)) );
		r.normalize();
		painter.drawRect(r);

		QPoint p3( p2.x() + LEGEND_INNER_BORDER, p2.y() );
		painter.drawText(p3, (*it)->name() );

		++moduleIndex;
	}
	painter.restore();
}

/** No descriptions */
void CSearchAnalysis::saveAsHTML() {
	const QString file = KFileDialog::getSaveFileName(QString::null,
						 QString("*.html | %1").arg(i18n("HTML files")),
						 0,
						 i18n("Save Search Analysis"));
	if (file.isNull()) {
		return;
	}

	int moduleIndex = 0;
	int count = 0;
	QString countStr = "";
	QString m_searchAnalysisHTML = "";
	QString tableTitle = "";
	QString tableTotals = "";
	QString VerseRange = "";
	const QString txtCSS = QString("<style type=\"text/css\">\ntd {border:1px solid black;}\nth {font-size: 130%; text-align:left; vertical-align:top;}\n</style>\n");
	const QString metaEncoding = QString("<META http-equiv=Content-Type content=\"text/html; charset=utf-8\">");
	CSwordVerseKey key(0);
	sword::ListKey searchResult;

	key.key("Genesis 1:1");

	CSearchAnalysisItem* analysisItem = m_canvasItemList.find( key.book() );

	QString text = "<html>\n<head>\n<title>" + i18n("BibleTime Search Analysis") + "</title>\n" + txtCSS + metaEncoding + "</head>\n<body>\n";
	text += "<table>\n<tr><th>" + i18n("Search text :") + "</th><th>" + CSearchDialog::getSearchDialog()->searchText() + "</th></tr>\n";

	tableTitle = "<tr><th align=\"left\">" + i18n("Book") + "</th>";
	tableTotals = "<tr><td align=\"left\">" + i18n("Total hits") + "</td>";
	//   for (moduleIndex = 0,m_moduleList.first(); m_moduleList.current(); m_moduleList.next(),++moduleIndex) {
	moduleIndex = 0;
	ListCSwordModuleInfo::iterator end_it = m_moduleList.end();
	for (ListCSwordModuleInfo::iterator it(m_moduleList.begin()); it != end_it; ++it) {
		tableTitle += QString("<th align=\"left\">") + (*it)->name() + QString("</th>");
		searchResult = (*it)->searchResult();
		countStr.setNum(searchResult.Count());

		tableTotals += QString("<td align=\"right\">") + countStr + QString("</td>");
		++moduleIndex;
	}
	tableTitle += QString("</tr>\n");
	tableTotals += QString("</tr>\n");

	m_searchAnalysisHTML = "";
	bool ok = true;
	while (ok) {
		m_searchAnalysisHTML += QString("<tr><td>") + key.book() + QString("</td>");
		analysisItem = m_canvasItemList.find( key.book() );

		//    for (moduleIndex = 0, m_moduleList.first(); m_moduleList.current(); m_moduleList.next(), ++moduleIndex) {
		moduleIndex = 0;
		ListCSwordModuleInfo::iterator end_it = m_moduleList.end();
		for (ListCSwordModuleInfo::iterator it(m_moduleList.begin()); it != end_it; ++it) {
			count = analysisItem->getCountForModule(moduleIndex);
			countStr.setNum(count);
			m_searchAnalysisHTML += QString("<td align=\"right\">") + countStr + QString("</td>");

			++moduleIndex;
		}
		m_searchAnalysisHTML += QString("</tr>\n");
		ok = key.next(CSwordVerseKey::UseBook);
	}

	text += QString("<table>\n") + tableTitle + tableTotals + m_searchAnalysisHTML + QString("</table>\n");
	text += QString("<center>") + i18n("Created by") + QString(" <a href=\"http://www.bibletime.info/\">BibleTime</a></center>");
	text += QString("</body></html>");

	CToolClass::savePlainFile(file, text, false, QTextStream::UnicodeUTF8);
}
 
	} //end of namespace Search::Analysis
} //end of namespace Search
