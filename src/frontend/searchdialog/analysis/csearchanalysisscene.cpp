/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "csearchanalysisscene.h"

#include <QApplication>
#include <QFileDialog>
#include <QHashIterator>
#include <QTextCodec>
#include <QTextDocument>
#include "../../../backend/keys/cswordversekey.h"
#include "../../../util/tool.h"
#include "../csearchdialog.h"
#include "csearchanalysisitem.h"
#include "csearchanalysislegenditem.h"

// Sword includes
#include <listkey.h>


namespace Search {

const int SPACE_BETWEEN_PARTS = 5;
const int RIGHT_BORDER = 15;
const int LEFT_BORDER = 15;
const int LOWER_BORDER = 10;
const int UPPER_BORDER = 10;

const int ITEM_TEXT_SIZE = 8;

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
        m_legend(nullptr) {
    setBackgroundBrush(QBrush(Qt::white));
    setSceneRect(0, 0, 1, 1);
}

/** Starts the analysis of the search result. This should be called only once because QCanvas handles the updates automatically. */
void CSearchAnalysisScene::analyse(
        const CSwordModuleSearch::Results &results)
{
    using RCI = CSwordModuleSearch::Results::const_iterator;

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

    int xPos = static_cast<int>(LEFT_BORDER + m_legend->rect().width() + SPACE_BETWEEN_PARTS);
    int moduleIndex = 0;
    m_maxCount = 0;
    int count = 0;
    CSwordVerseKey key(nullptr);
    key.setKey("Genesis 1:1");

    CSearchAnalysisItem* analysisItem = m_itemList[key.book()];
    bool ok = true;
    while (ok && analysisItem) {
        moduleIndex = 0;
        for (RCI it = m_results.begin(); it != m_results.end(); ++it) {
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
            xPos += static_cast<int>(analysisItem->width() + SPACE_BETWEEN_PARTS);
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
    using RCI = CSwordModuleSearch::Results::const_iterator;

    m_results.clear();
    for (RCI it = results.begin(); it != results.end(); ++it) {
        const CSwordModuleInfo *m = it.key();
        if ( (m->type() == CSwordModuleInfo::Bible) || (m->type() == CSwordModuleInfo::Commentary) ) { //a Bible or an commentary
            m_results.insert(m, it.value());
        }
    }

    m_itemList.clear();
    CSearchAnalysisItem* analysisItem = nullptr;
    CSwordVerseKey key(nullptr);
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
    m_legend = nullptr;

    update();
}

/** No descriptions */
void CSearchAnalysisScene::slotResized() {
    if (m_maxCount <= 0) {
        m_scaleFactor = 0.0;
    } else {
        m_scaleFactor = static_cast<double>(height() - UPPER_BORDER - LOWER_BORDER - BAR_LOWER_BORDER - 100 - (m_results.count() - 1) * BAR_DELTAY)
                        / static_cast<double>(m_maxCount);
    }
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
    const sword::ListKey & result = m_results[module];

    const int length = book.length();
    unsigned int i = m_lastPosList[module];
    unsigned int count = 0;
    const unsigned int resultCount = result.getCount();
    while (i < resultCount) {
        if (strncmp(book.toUtf8(), result.getElement(i)->getText(), length))
            break;
        i++;
        ++count;
    }
    m_lastPosList.insert(module, i);
    return count;
}

void CSearchAnalysisScene::saveAsHTML() {
    using RCI = CSwordModuleSearch::Results::const_iterator;

    auto const fileName =
            QFileDialog::getSaveFileName(
                nullptr,
                tr("Save Search Analysis"),
                QString(),
                QObject::tr("HTML files") +" (*.html *.htm);;"
                + QObject::tr("All files") + " (*)");
    if (fileName.isEmpty())
        return;

    QString text("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                 "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" "
                 "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
                 "<html xmlns=\"http://www.w3.org/1999/xhtml\"><head><title>");
    {
        const QString title(tr("BibleTime Search Analysis"));
        text += title;
        text += "</title>"
                "<style type=\"text/css\">"
                    "body{background-color:#fff;color:#000}"
                    "table{border-collapse:collapse}"
                    "td{border:1px solid #333}"
                    "th{font-size:130%;text-align:left;vertical-align:top}"
                    "td,th{text-align:left;padding:0.2em 0.5em}"
                    ".r{text-align:right}"
                "</style>"
                "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>"
            "</head><body><h1>";
        text += title;
    }
    text += "</h1><p><span style=\"font-weight:bold\">";
    text += tr("Search text:");
    text += "</span>&nbsp;";
    text += CSearchDialog::getSearchDialog()->searchText().toHtmlEscaped();
    text += "</p><table><caption>";
    text += tr("Results by work and book");
    text += "</caption><tr><th>";
    text += tr("Book");
    text += "</th>";

    for (RCI it = m_results.begin(); it != m_results.end(); ++it) {
        text += "<th>";
        text += it.key()->name().toHtmlEscaped();
        text += "</th>";
    }
    text += "</tr>";

    CSwordVerseKey key(nullptr);
    key.setKey("Genesis 1:1");

    do {
        text += "<tr><td>";
        const QString keyBook(key.book());
        text += keyBook.toHtmlEscaped();
        text += "</td>";

        int mi = 0; // Module index
        for (RCI it = m_results.begin(); it != m_results.end(); ++it, ++mi) {
            text += "<td class=\"r\">";
            text += QString::number(m_itemList.value(keyBook)->getCountForModule(mi));
            text += "</td>";
        }
        text += "</tr>";
    } while (key.next(CSwordVerseKey::UseBook));
    text += "<tr><th class=\"r\">";
    text += tr("Total hits");
    text += "</th>";

    for (RCI it = m_results.begin(); it != m_results.end(); ++it) {
        text += "<td class=\"r\">";
        text += QString::number(it.value().getCount());
        text += "</td>";
    }

    text += "</tr></table><p style=\"text-align:center;font-size:x-small\">";
    text += tr("Created by <a href=\"http://www.bibletime.info/\">BibleTime</a>");
    text += "</p></body></html>";

    util::tool::savePlainFile(fileName, text, QTextCodec::codecForName("UTF-8"));
}

void CSearchAnalysisScene::resizeHeight(int height) {
    setSceneRect(0, 0, sceneRect().width(), height);
    slotResized();
}

} // namespace Search {
