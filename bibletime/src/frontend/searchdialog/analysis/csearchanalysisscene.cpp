//
// C++ Implementation: csearchanalysisscene
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "csearchanalysisscene.h"
#include "csearchanalysisitem.h"
#include "csearchanalysislegenditem.h"

#include "frontend/searchdialog/csearchdialog.h"

#include "backend/keys/cswordversekey.h"
#include "util/ctoolclass.h"

#include <QHashIterator>
#include <QFileDialog>
#include <QTextCodec>

#include <kapplication.h>
#include <klocale.h>

namespace Search {

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


CSearchAnalysisScene::CSearchAnalysisScene(QObject *parent )
	: QGraphicsScene(parent),
	m_scaleFactor(0.0),
	m_legend(0)
{
	setBackgroundBrush(QBrush(Qt::white));
	//m_itemList.resize(67);
	//m_canvasItemList.setAutoDelete(true);
	//resize(1,1);
	setSceneRect(0,0,1,1);
	connect(this, SIGNAL(resized()), SLOT(slotResized()));
}


QHash<QString, CSearchAnalysisItem*>* CSearchAnalysisScene::getSearchAnalysisItemList() {
	// Returns pointer to the search analysis items
	return &m_itemList;
}

/** Starts the analysis of the search result. This should be called only once because QCanvas handles the updates automatically. */
void CSearchAnalysisScene::analyse(ListCSwordModuleInfo modules) {
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
	m_legend = new CSearchAnalysisLegendItem(&m_moduleList);
	addItem(m_legend);
	//m_legend->setX(LEFT_BORDER);
	//m_legend->setY(UPPER_BORDER);
	//m_legend->setPos(LEFT_BORDER, UPPER_BORDER);
	//m_legend->setSize(LEGEND_WIDTH,
	//				  LEGEND_INNER_BORDER*2 + ITEM_TEXT_SIZE*numberOfModules + LEGEND_DELTAY*(numberOfModules-1));
	m_legend->setRect(LEFT_BORDER, UPPER_BORDER,
		LEGEND_WIDTH, LEGEND_INNER_BORDER*2 + ITEM_TEXT_SIZE*numberOfModules + LEGEND_DELTAY*(numberOfModules-1) );
	m_legend->show();

	int xPos = LEFT_BORDER + m_legend->rect().width() + SPACE_BETWEEN_PARTS;
	int moduleIndex = 0;
	m_maxCount = 0;
	int count = 0;
	CSwordVerseKey key(0);
	key.key("Genesis 1:1");

	CSearchAnalysisItem* analysisItem = m_itemList[key.book()];
	bool ok = true;
	while (ok && analysisItem) {
		//   for (moduleIndex = 0,m_moduleList.first(); m_moduleList.current(); m_moduleList.next(),++moduleIndex) {
		moduleIndex = 0;
		ListCSwordModuleInfo::iterator end_it = m_moduleList.end();
		for (ListCSwordModuleInfo::iterator it(m_moduleList.begin()); it != end_it; ++it) {
			KApplication::kApplication()->processEvents( QEventLoop::AllEvents, 10 ); //10 ms only
			if (!m_lastPosList.contains(*it)) {
				m_lastPosList.insert(*it,0);
			}

			analysisItem->setCountForModule(moduleIndex, (count = getCount(key.book(), *it)));
			m_maxCount = (count > m_maxCount) ? count : m_maxCount;

			++moduleIndex;
		}
		//analysisItem->setX(xPos);
		//analysisItem->setY(UPPER_BORDER);
		analysisItem->setRect(xPos, UPPER_BORDER, analysisItem->rect().width(), analysisItem->rect().height());
		analysisItem->show();

		xPos += (int)analysisItem->width() + SPACE_BETWEEN_PARTS;
		ok = key.next(CSwordVerseKey::UseBook);
		analysisItem = m_itemList[key.book()];
	}
	//resize(xPos+BAR_WIDTH+(m_moduleList.count()-1)*BAR_DELTAX+RIGHT_BORDER, height() );
	setSceneRect(0,0, xPos+BAR_WIDTH+(m_moduleList.count()-1)*BAR_DELTAX+RIGHT_BORDER, height() );
	slotResized();
}

/** Sets te module list used for the analysis. */
void CSearchAnalysisScene::setModules(ListCSwordModuleInfo modules) {
	m_moduleList.clear();
	//  for (modules.first(); modules.current(); modules.next()) {
	ListCSwordModuleInfo::iterator end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
		if ( ((*it)->type() == CSwordModuleInfo::Bible) || ((*it)->type() == CSwordModuleInfo::Commentary) ) { //a Bible or an commentary
			m_moduleList.append(*it);
		}
	}

	m_itemList.clear();
	CSearchAnalysisItem* analysisItem = 0;
	CSwordVerseKey key(0);
	key.key("Genesis 1:1");
	do {
		analysisItem = new CSearchAnalysisItem(m_moduleList.count(), key.book(), &m_scaleFactor, &m_moduleList);
		addItem(analysisItem);
		analysisItem->hide();
		m_itemList.insert(key.book(), analysisItem);
	}
	while (key.next(CSwordVerseKey::UseBook));
	update();
}

/** Sets back the items and deletes things to cleanup */
void CSearchAnalysisScene::reset() {
	m_scaleFactor = 0.0;

	QHashIterator<QString, CSearchAnalysisItem*> it( m_itemList ); // iterator for items
	while ( it.hasNext() ) {
		it.next();
		if (it.value()) it.value()->hide();
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
void CSearchAnalysisScene::slotResized() {
	m_scaleFactor = (double)( (double)(height()-UPPER_BORDER-LOWER_BORDER-BAR_LOWER_BORDER-(m_moduleList.count()-1)*BAR_DELTAY)
							  /(double)m_maxCount);
	QHashIterator<QString, CSearchAnalysisItem*> it( m_itemList );
	while ( it.hasNext() ) {
		if (it.value()) {
			//it.value()->setSize(BAR_WIDTH + (m_moduleList.count()-1)*BAR_DELTAX, height()-UPPER_BORDER-LOWER_BORDER);
			//it.value()->setY(UPPER_BORDER);
			it.value()->setRect(it.value()->rect().x(), UPPER_BORDER, BAR_WIDTH + (m_moduleList.count()-1)*BAR_DELTAX, height()-UPPER_BORDER-LOWER_BORDER);
		}
	}
	update();
}

/** This function returns a color for each module */
QColor CSearchAnalysisScene::getColor(int index) {
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
const unsigned int CSearchAnalysisScene::getCount( const QString book, CSwordModuleInfo* module ) {
	sword::ListKey& result = module->searchResult();
	const int length = book.length();
	unsigned int i = m_lastPosList[module];
	unsigned int count = 0;
	const unsigned int resultCount = result.Count();
	while (i < resultCount) {
		if ( strncmp(book.toUtf8(), (const char*)*result.GetElement(i), length) )
			break;
		i++;
		++count;
	}
	m_lastPosList.insert(module,i);
	return count;
}

void CSearchAnalysisScene::saveAsHTML() {
	//const QString file = KFileDialog::getSaveFileName(QString::null,
	//					 QString("*.html | %1").arg(i18n("HTML files")),
	//					 0,
	//					 i18n("Save Search Analysis"));
	const QString fileName = QFileDialog::getSaveFileName(0,
															i18n("Save Search Analysis"),
															QString::null,
															QString(i18n("HTML files (*.html;*.HTML;*.HTM;*.htm)")) );
	if (fileName.isEmpty()) return;


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

	CSearchAnalysisItem* analysisItem = m_itemList.value( key.book() );

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
		analysisItem = m_itemList.value( key.book() );

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

	CToolClass::savePlainFile(fileName, text, false, QTextCodec::codecForName("UTF8"));
}


}
