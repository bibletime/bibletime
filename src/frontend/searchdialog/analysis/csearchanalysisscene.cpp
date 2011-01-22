/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/searchdialog/analysis/csearchanalysisscene.h"

#include <QApplication>
#include <QFileDialog>
#include <QHashIterator>
#include <QTextCodec>
#include "backend/keys/cswordversekey.h"
#include "frontend/searchdialog/analysis/csearchanalysisitem.h"
#include "frontend/searchdialog/analysis/csearchanalysislegenditem.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "util/tool.h"


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
const int BAR_LOWER_BORDER = 90;

const int LEGEND_INNER_BORDER = 5;
const int LEGEND_DELTAY = 4;
const int LEGEND_WIDTH = 85;


CSearchAnalysisScene::CSearchAnalysisScene(QObject *parent )
        : QGraphicsScene(parent),
        m_scaleFactor(0.0),
        m_legend(0) {
    setBackgroundBrush(QBrush(Qt::white));
    setSceneRect(0, 0, 1, 1);
}

/** Starts the analysis of the search result. This should be called only once because QCanvas handles the updates automatically. */
void CSearchAnalysisScene::analyse(
        const CSwordModuleSearch::Results &results)
{
    typedef CSwordModuleSearch::Results::const_iterator RCI;

    /**
    * Steps of analysing our search result;
    * -Create the items for all available books ("Genesis" - "Revelation")
    * -Iterate through all modules we analyse
    *  -Go through all books of this module
    *   -Find out how many times we found the book
    *   -Set the count to the items which belongs to the book
    */
    setResults(results);

    m_lastPosList.clear();
    const int numberOfModules = m_results.count();
    if (!numberOfModules)
        return;
    m_legend = new CSearchAnalysisLegendItem(m_results.keys());
    addItem(m_legend);
    m_legend->setRect(LEFT_BORDER, UPPER_BORDER,
                      LEGEND_WIDTH, LEGEND_INNER_BORDER*2 + ITEM_TEXT_SIZE*numberOfModules + LEGEND_DELTAY*(numberOfModules - 1) );
    m_legend->show();

    int xPos = (int)(LEFT_BORDER + m_legend->rect().width() + SPACE_BETWEEN_PARTS);
    int moduleIndex = 0;
    m_maxCount = 0;
    int count = 0;
    CSwordVerseKey key(0);
    key.setKey("Genesis 1:1");

    CSearchAnalysisItem* analysisItem = m_itemList[key.book()];
    bool ok = true;
    while (ok && analysisItem) {
        moduleIndex = 0;
        for (RCI it = m_results.begin(); it != m_results.end(); it++) {
            qApp->processEvents( QEventLoop::AllEvents );
            if (!m_lastPosList.contains(it.key())) {
                m_lastPosList.insert(it.key(), 0);
            }

            analysisItem->setCountForModule(moduleIndex, (count = getCount(key.book(), it.key())));
            m_maxCount = (count > m_maxCount) ? count : m_maxCount;

            ++moduleIndex;
        }
        if (analysisItem->hasHitsInAnyModule()) {
            analysisItem->setRect(xPos, UPPER_BORDER, analysisItem->rect().width(), analysisItem->rect().height());
            QString tip = analysisItem->getToolTip();
            analysisItem->setToolTip(tip);
            analysisItem->show();
            xPos += (int)analysisItem->width() + SPACE_BETWEEN_PARTS;
        }
        ok = key.next(CSwordVerseKey::UseBook);
        analysisItem = m_itemList[key.book()];
    }
    setSceneRect(0, 0, xPos + BAR_WIDTH + (m_results.count() - 1)*BAR_DELTAX + RIGHT_BORDER, height() );
    slotResized();
}

/** Sets the module list used for the analysis. */
void CSearchAnalysisScene::setResults(
        const CSwordModuleSearch::Results &results)
{
    typedef CSwordModuleSearch::Results::const_iterator RCI;

    m_results.clear();
    for (RCI it = results.begin(); it != results.end(); it++) {
        const CSwordModuleInfo *m = it.key();
        if ( (m->type() == CSwordModuleInfo::Bible) || (m->type() == CSwordModuleInfo::Commentary) ) { //a Bible or an commentary
            m_results.insert(m, it.value());
        }
    }

    m_itemList.clear();
    CSearchAnalysisItem* analysisItem = 0;
    CSwordVerseKey key(0);
    key.setKey("Genesis 1:1");
    do {
        analysisItem = new CSearchAnalysisItem(m_results.count(), key.book(), &m_scaleFactor, m_results);
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

    if (m_legend) m_legend->hide();

    delete m_legend;
    m_legend = 0;

    update();
}

/** No descriptions */
void CSearchAnalysisScene::slotResized() {
    m_scaleFactor = (double)( (double)(height() - UPPER_BORDER - LOWER_BORDER - BAR_LOWER_BORDER - 100 - (m_results.count() - 1) * BAR_DELTAY)
                              / (double)m_maxCount);
    QHashIterator<QString, CSearchAnalysisItem*> it( m_itemList );
    while ( it.hasNext() ) {
        it.next();
        if (it.value()) {
            it.value()->setRect(it.value()->rect().x(), UPPER_BORDER, BAR_WIDTH + (m_results.count() - 1)*BAR_DELTAX, height() - LOWER_BORDER - BAR_LOWER_BORDER);
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

unsigned int CSearchAnalysisScene::getCount(const QString &book,
                                            const CSwordModuleInfo* module)
{
    /// \warning This is a workaround for sword constness
    sword::ListKey result = m_results[module];

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
    m_lastPosList.insert(module, i);
    return count;
}

void CSearchAnalysisScene::saveAsHTML() {
    typedef CSwordModuleSearch::Results::const_iterator RCI;

    const QString fileName = QFileDialog::getSaveFileName(0, tr("Save Search Analysis"), QString::null, tr("HTML files (*.html;*.HTML;*.HTM;*.htm)") );
    if (fileName.isEmpty()) return;

    int count = 0;
    QString countStr = "";
    QString m_searchAnalysisHTML = "";
    QString tableTitle = "";
    QString tableTotals = "";
    QString VerseRange = "";
    const QString txtCSS = QString("<style type=\"text/css\">\ntd {border:1px solid black;}\nth {font-size: 130%; text-align:left; vertical-align:top;}\n</style>\n");
    const QString metaEncoding = QString("<META http-equiv=Content-Type content=\"text/html; charset=utf-8\">");
    CSwordVerseKey key(0);

    key.setKey("Genesis 1:1");

    CSearchAnalysisItem* analysisItem = m_itemList.value( key.book() );

    QString text = "<html>\n<head>\n<title>" + tr("BibleTime Search Analysis") + "</title>\n" + txtCSS + metaEncoding + "</head>\n<body>\n";
    text += "<table>\n<tr><th>" + tr("Search text :") + "</th><th>" + CSearchDialog::getSearchDialog()->searchText() + "</th></tr>\n";

    tableTitle = "<tr><th align=\"left\">" + tr("Book") + "</th>";
    tableTotals = "<tr><td align=\"left\">" + tr("Total hits") + "</td>";

    for (RCI it = m_results.begin(); it != m_results.end(); it++) {
        const CSwordModuleInfo *mod = it.key();
        tableTitle += QString("<th align=\"left\">") + mod->name() + QString("</th>");

        /// \warning This is a workaround for sword constness
        sword::ListKey searchResult = it.value();
        countStr.setNum(searchResult.Count());

        tableTotals += QString("<td align=\"right\">") + countStr + QString("</td>");
    }
    tableTitle += QString("</tr>\n");
    tableTotals += QString("</tr>\n");

    m_searchAnalysisHTML = "";
    bool ok = true;
    while (ok) {
        m_searchAnalysisHTML += QString("<tr><td>") + key.book() + QString("</td>");
        analysisItem = m_itemList.value( key.book() );

        int moduleIndex = 0;
        for (RCI it = m_results.begin(); it != m_results.end(); it++) {
            count = analysisItem->getCountForModule(moduleIndex);
            countStr.setNum(count);
            m_searchAnalysisHTML += QString("<td align=\"right\">") + countStr + QString("</td>");

            ++moduleIndex;
        }
        m_searchAnalysisHTML += QString("</tr>\n");
        ok = key.next(CSwordVerseKey::UseBook);
    }

    text += QString("<table>\n") + tableTitle + tableTotals + m_searchAnalysisHTML + QString("</table>\n");
    text += QString("<center>") + tr("Created by <a href=\"http://www.bibletime.info/\">BibleTime</a>") + QString("</center>");
    text += QString("</body></html>");

    util::tool::savePlainFile(fileName, text, false, QTextCodec::codecForName("UTF8"));
}

void CSearchAnalysisScene::resizeHeight(int height) {
    setSceneRect(0, 0, sceneRect().width(), height);
    slotResized();
}

}
