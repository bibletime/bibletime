/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btmmoduletextfilter.h"

/*
 * This filter provides a method for modifying text generated
 * by BtModuleTextModel. Specifically is can modify footnotes
 * to hyperlinks. This is needed because QML rich text usage
 * does not allow DOM access for hovered items as implemented
 * by BibleTime Desktop. By using hyperlinks the footnotes and
 * cross references can be clicked and corresponding actions
 * will show the necessary information.
 */

namespace btm {

BtmModuleTextFilter::BtmModuleTextFilter() :
    m_showReferences(false) {
}

QString BtmModuleTextFilter::processText(const QString &text) {
    QString localText = fixNonRichText(text);
    splitText(localText);
    fixDoubleBR();
    if (m_showReferences) {
        rewriteFootnoteAsLink();
        rewriteLemmaMorphAsLink();
    }
    return m_parts.join("");
}

void BtmModuleTextFilter::setShowReferences(bool on) {
    m_showReferences = on;
}

QString BtmModuleTextFilter::fixNonRichText(const QString& text) {
    // Fix !P tag which is not rich text
    QString localText = text;
    int index = 0;
    while ((index = localText.indexOf("<!P>")) >= 0)
        localText.remove(index,4);
    return localText;
}

void BtmModuleTextFilter::rewriteFootnoteAsLink() {
    int partCount = m_parts.count();
    for (int i=0; i<partCount; ++i) {
        QString part = m_parts.at(i);
        if (part.startsWith("<") && part.contains("class=\"footnote\"")) {

            QRegExp rxlen("note=\"([^\"]*)");
            int pos = rxlen.indexIn(part);
            if (pos > -1) {
                QString noteValue = rxlen.cap(1);
                QString footnoteText = m_parts.at(i+1);
                QString url = "sword://footnote/" + noteValue + "/" + footnoteText;
                QString newEntry = "<a href=\"" + url + "\">";
                m_parts[i] = newEntry;
                m_parts[i+1] = "(" + footnoteText + ")";
                m_parts[i+2] = "</a>";
            }
        }
    }
}

void BtmModuleTextFilter::rewriteLemmaMorphAsLink() {
    int partCount = m_parts.count();
    for (int i=0; i<partCount; ++i) {
        QString part = m_parts.at(i);
        QString value;
        if (part.startsWith("<") && (
                    part.contains("lemma=\"") ||
                    part.contains("morph=\"") ) ) {

            QRegExp rx1("lemma=\"([^\"]*)");
            int pos1 = rx1.indexIn(part);
            if (pos1 > -1)
                value = "lemma=" + rx1.cap(1) +"||";

            QRegExp rx2("morph=\"([^\"]*)");
            int pos2 = rx2.indexIn(part);
            if (pos2 > -1)
                value += "morph" + rx2.cap(1);

            QString refText = m_parts.at(i+1);
            QString url = "sword://lemmamorph/" + value + "/" + refText;
            QString newEntry = "<a href=\"" + url + "\">";
            m_parts[i] = newEntry;
            m_parts[i+2] = "</a>";

        }
    }
}

void BtmModuleTextFilter::splitText(const QString& text) {
    m_parts.clear();
    int from = 0;

    while (from < text.length()) {

        // Get text before tag
        int end = text.indexOf("<", from);
        if (end == -1)
            end = text.length();
        m_parts.append(text.mid(from, end-from));
        from = end;

        //Get tag text
        end = text.indexOf(">", from);
        if (end == -1)
            end = text.length();
        m_parts.append(text.mid(from, end-from+1));
        from = end+1;
    }
}

void BtmModuleTextFilter::fixDoubleBR() {
    for (int index = 2; index < m_parts.count(); ++index) {
        QRegExp rx("<br\\s+/>");
        if (m_parts.at(index).contains(rx) && m_parts.at(index-2).contains(rx))
            m_parts[index] = "";
    }
}

int BtmModuleTextFilter::partsContains(const QString& text, int start) {
    if (start >= m_parts.count())
        return -1;
    for (int index=start; index < m_parts.count(); ++index) {
        if (m_parts.at(index).startsWith(text))
            return index;
    }
    return -1;
}

} // end namespace
