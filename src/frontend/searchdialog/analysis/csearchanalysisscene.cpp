/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "csearchanalysisscene.h"

#include <algorithm>
#include <QApplication>
#include <QFileDialog>
#include <QMap>
#include <QTextCodec>
#include <QTextDocument>
#include <QVector>
#include <type_traits>
#include <utility>
#include "../../../backend/keys/cswordversekey.h"
#include "../../../util/btassert.h"
#include "../../../util/tool.h"
#include "../csearchdialog.h"

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

CSearchAnalysisScene::CSearchAnalysisScene(
        CSwordModuleSearch::Results const & results,
        QObject * parent)
    : QGraphicsScene(parent)
{
    setBackgroundBrush(QBrush(Qt::white));
    setSceneRect(0, 0, 1, 1);

    /**
    * Steps of analysing our search result;
    * -Create the items for all available books ("Genesis" - "Revelation")
    * -Iterate through all modules we analyse
    *  -Go through all books of this module
    *   -Find out how many times we found the book
    *   -Set the count to the items which belongs to the book
    */
    for (auto it = results.begin(); it != results.end(); ++it) {
        const CSwordModuleInfo *m = it.key();
        if ( (m->type() == CSwordModuleInfo::Bible) || (m->type() == CSwordModuleInfo::Commentary) ) { //a Bible or an commentary
            m_results.insert(m, it.value());
        }
    }

    const int numberOfModules = m_results.count();
    if (!numberOfModules)
        return;
    m_legend = std::make_unique<CSearchAnalysisLegendItem>(m_results.keys());
    addItem(m_legend.get());
    m_legend->setRect(LEFT_BORDER, UPPER_BORDER,
                      LEGEND_WIDTH, LEGEND_INNER_BORDER*2 + ITEM_TEXT_SIZE*numberOfModules + LEGEND_DELTAY*(numberOfModules - 1) );
    m_legend->show();

    int moduleIndex = 0;
    for (auto * const keyPtr : m_results.keys()) {
        qApp->processEvents(QEventLoop::AllEvents);
        auto const & moduleResults = m_results[keyPtr];
        auto const numModuleResults = moduleResults.getCount();
        for (int i = 0u; i < numModuleResults; ++i) {
            /* m_results only contains results from Bibles and
               Commentaries, as filtered above. */
            BT_ASSERT(dynamic_cast<sword::VerseKey const *>(
                          moduleResults.getElement(i)));
            auto const * const vk =
                    static_cast<sword::VerseKey const *>(
                        moduleResults.getElement(i));
            auto key = std::tuple(vk->getTestament(), vk->getBook());
            CSearchAnalysisItem * analysisItem;
            static_assert(std::is_same_v<decltype(key),
                                         decltype(m_itemList)::key_type>, "");
            if (auto const it = m_itemList.find(key); it != m_itemList.end()) {
                analysisItem = it->second;
            } else {
                analysisItem =
                        new CSearchAnalysisItem(
                            QString::fromUtf8(vk->getBookName()),
                            m_results.size());
                m_itemList.emplace(std::move(key), analysisItem);
            }
            auto const count = ++analysisItem->counts()[moduleIndex];
            m_maxCount = std::max(m_maxCount, count);
        }

        ++moduleIndex;
    }

    int xPos = static_cast<int>(LEFT_BORDER
                                + m_legend->rect().width()
                                + SPACE_BETWEEN_PARTS);
    for (auto const & vp : m_itemList) {
        auto & analysisItem = *vp.second;
        addItem(&analysisItem);
        analysisItem.setRect(xPos,
                             UPPER_BORDER,
                             analysisItem.rect().width(),
                             analysisItem.rect().height());
        xPos += static_cast<int>(analysisItem.width() + SPACE_BETWEEN_PARTS);

        QString toolTip("<center><b>");
        toolTip.append(analysisItem.bookName().toHtmlEscaped())
               .append("</b></center><hr/><table cellspacing=\"0\" "
                       "cellpadding=\"3\" width=\"100%\" height=\"100%\" "
                       "align=\"center\">");

        int i = 0;
        for (auto it = m_results.begin(); it != m_results.end(); ++it) {
            const CSwordModuleInfo * const info = it.key();

            auto const count = it.value().getCount();
            double const percent =
                    (info && count)
                    ? ((static_cast<double>(analysisItem.counts()[i])
                        * static_cast<double>(100.0))
                       / static_cast<double>(count))
                    : 0.0;
            toolTip.append("<tr bgcolor=\"white\"><td><b><font color=\"")
                   .append(getColor(i).name()).append("\">")
                   .append(info ? info->name() : QString())
                   .append("</font></b></td><td>")
                   .append(QString::number(analysisItem.counts()[i]))
                   .append(" (")
                   .append(QString::number(percent, 'g', 2))
                   .append("%)</td></tr>");
            ++i;
        }
        toolTip.append("</table>");
        analysisItem.setToolTip(toolTip);
    }
    setSceneRect(0, 0, xPos + BAR_WIDTH + (m_results.count() - 1)*BAR_DELTAX + RIGHT_BORDER, height() );
    slotResized();
}

/** No descriptions */
void CSearchAnalysisScene::slotResized() {
    double scaleFactor;
    if (m_maxCount <= 0) {
        scaleFactor = 0.0;
    } else {
        scaleFactor = static_cast<double>(height() - UPPER_BORDER - LOWER_BORDER - BAR_LOWER_BORDER - 100 - (m_results.count() - 1) * BAR_DELTAY)
                      / static_cast<double>(m_maxCount);
    }
    for (auto const & vp : m_itemList) {
        auto & analysisItem = *vp.second;
        analysisItem.setScaleFactor(scaleFactor);
        analysisItem.setRect(analysisItem.rect().x(),
                             UPPER_BORDER,
                             BAR_WIDTH + (m_results.count() - 1) * BAR_DELTAX,
                             height() - LOWER_BORDER - BAR_LOWER_BORDER);
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

void CSearchAnalysisScene::saveAsHTML() {
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

    for (auto * const keyPtr : m_results.keys()) {
        text += "<th>";
        text += keyPtr->name().toHtmlEscaped();
        text += "</th>";
    }
    text += "</tr>";

    for (auto const & vp : m_itemList) {
        auto const & analysisItem = *vp.second;
        text += "<tr><td>";
        text += analysisItem.bookName().toHtmlEscaped();
        text += "</td>";
        for (auto const count : analysisItem.counts())
            text += "<td class=\"r\">" + QString::number(count) + "</td>";
        text += "</tr>";
    }
    text += "<tr><th class=\"r\">";
    text += tr("Total hits");
    text += "</th>";

    for (auto const & resultValue : m_results) {
        text += "<td class=\"r\">";
        text += QString::number(resultValue.getCount());
        text += "</td>";
    }

    text += "</tr></table><p style=\"text-align:center;font-size:x-small\">";
    text += tr("Created by <a href=\"https://bibletime.info/\">BibleTime</a>");
    text += "</p></body></html>";

    util::tool::savePlainFile(fileName, text, QTextCodec::codecForName("UTF-8"));
}

void CSearchAnalysisScene::resizeHeight(int height) {
    setSceneRect(0, 0, sceneRect().width(), height);
    slotResized();
}

} // namespace Search {
