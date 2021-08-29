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

#include "bttextfilter.h"

#include <QDebug>


BtTextFilter::BtTextFilter() = default;

BtTextFilter::~BtTextFilter() = default;

QString BtTextFilter::processText(const QString &text) {
    if (text.isEmpty())
        return text;
    QString localText = text;
    { // Fix !P tag which is not rich text:
        int index = 0;
        while ((index = localText.indexOf("<!P>")) >= 0)
            localText.remove(index,4);
    }
    auto parts = splitText(localText);
    fixDoubleBR(parts);
    if (m_showReferences) {

        int i = 0;
        int count = parts.count();
        do {
            QString part = parts.at(i);

            if (part.startsWith("<") && part.contains("class=\"footnote\"")) {
                i= i + rewriteFootnoteAsLink(parts, i, part);

            } else if (part.startsWith("<") && (part.contains("href=\"") ) ) {
                i = i + rewriteHref(parts, i, part);

            } else if (part.startsWith("<") && (
                           part.contains("lemma=\"") ||part.contains("morph=\"") ) ) {
                i= i+ rewriteLemmaOrMorphAsLink(parts, i, part);

            } else {
                i++;
            }
        } while (i < count);

    }
    return parts.join("");
}

QStringList BtTextFilter::splitText(const QString& text) {
    QStringList parts;
    int from = 0;
    while (from < text.length()) {

        // Get text before tag
        int end = text.indexOf("<", from);
        if (end == -1)
            end = text.length();
        parts.append(text.mid(from, end-from));
        from = end;

        //Get tag text
        end = text.indexOf(">", from);
        if (end == -1)
            end = text.length();
        parts.append(text.mid(from, end-from+1));
        from = end+1;
    }
    return parts;
}

void BtTextFilter::fixDoubleBR(QStringList & parts) {
    for (int index = 2; index < parts.count(); ++index) {
        QRegExp rx("<br\\s+/>");
        if (parts.at(index).contains(rx) && parts.at(index-2).contains(rx))
            parts[index] = "";
    }
}

// Typical input:  <span class="footnote" note="ESV2011/Luke 11:37/1">
// Output:         <span class="footnote" note="ESV2011/Luke 11:37/1">1</span>

int BtTextFilter::rewriteFootnoteAsLink(QStringList & parts, int i, const QString& part) {
    if (i+2 > parts.count())
        return 1;

    QRegExp rxlen("note=\"([^\"]*)");
    int pos = rxlen.indexIn(part);
    if (pos > -1) {
        QString noteValue = rxlen.cap(1);
        QString footnoteText = parts.at(i+1);
        QString url = "sword://footnote/" + noteValue + "=" + footnoteText;
        QString newEntry = "<a class=\"footnote\" href=\"" + url + "\">";
        parts[i] = newEntry;
        parts[i+1] = "(" + footnoteText + ")";
        parts[i+2] = "</a>";
        return 3;
    }
    return 1;
}

// Packs attribute part of href into the link
// Typical input: <a name="Luke11_29" href="sword://Bible/ESV2011/Luke 11:29">
// Output:        <a href="sword://Bible/ESV2011/Luke 11:29||name=Luke11_29">

int BtTextFilter::rewriteHref(QStringList & parts, int i, const QString& part) {
    QRegExp rx1("<a\\s(\\w+)=\"([\\s\\S]*)\"\\s(\\w+)=\"([\\s\\S]*)\"");
    rx1.setMinimal(false);
    int pos1 = rx1.indexIn(part);
    if (pos1 >= 0 && rx1.captureCount() == 4) {
        QString newEntry;
        if (rx1.cap(1) == "href")
            newEntry = "<a " + rx1.cap(1) + "=\"" + rx1.cap(2) + "||" + rx1.cap(3) + "=" + rx1.cap(4) + "\" name=\"crossref\">";
        else
            newEntry = "<a " + rx1.cap(3) + "=\"" + rx1.cap(4) + "||" + rx1.cap(1) + "=" + rx1.cap(2) + "\" name=\"crossref\">";

        parts[i] = newEntry;
    }
    return 1;
}

// Typical input: <span lemma="H07225">God</span>
// Output: "<a href="sword://lemmamorph/lemma=H0430||/God" style="color: black">"
int BtTextFilter::rewriteLemmaOrMorphAsLink(QStringList & parts, int i, const QString& part) {

    QString value;
    QRegExp rx1("lemma=\"([^\"]*)*");
    int pos1 = rx1.indexIn(part);
    if (pos1 > -1)
        value = "lemma=" + rx1.cap(1);

    QRegExp rx2("morph=\"([^\"]*)(\"){0,1}");
    int pos2 = rx2.indexIn(part);
    if (pos2 > -1) {
        if (!value.isEmpty())
            value += "||";
        value += "morph=" + rx2.cap(1);
    }

    QString refText = parts.at(i+1);
    QString url = "sword://lemmamorph/" + value + "/" + refText;
    QString newEntry;
    newEntry = "<a id=lemmamorph href=\"" + url + "\">";
    parts[i] = newEntry;
    parts[i+2] = "</a>";
    return 3;
}

void BtTextFilter::setShowReferences(bool on) {
    m_showReferences = on;
}
