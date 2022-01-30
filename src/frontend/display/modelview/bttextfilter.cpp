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


namespace {

QStringList splitText(QString const & text) {
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

void fixDoubleBR(QStringList & parts) {
    for (int index = 2; index < parts.count(); ++index) {
        QRegExp rx("<br\\s*/>");
        if (parts.at(index).contains(rx) && parts.at(index-2).contains(rx))
            parts[index] = "";
    }
}

// Typical input:  <span class="footnote" note="ESV2011/Luke 11:37/1">
// Output:         <span class="footnote" note="ESV2011/Luke 11:37/1">1</span>

int rewriteFootnoteAsLink(QStringList & parts, int i, QString const & part) {
    if (i + 2 >= parts.count())
        return 1;

    QRegExp rxlen("note=\"([^\"]*)");
    if (rxlen.indexIn(part) > -1) {
        auto const & footnoteText = parts.at(i + 1);
        parts[i] = "<a class=\"footnote\" href=\"sword://footnote/"
                   + rxlen.cap(1) + "=" + footnoteText + "\">";
        parts[i+1] = "(" + footnoteText + ")";
        parts[i+2] = "</a>";
        return 3;
    }
    return 1;
}

// Packs attribute part of href into the link
// Typical input: <a name="Luke11_29" href="sword://Bible/ESV2011/Luke 11:29">
// Output:        <a href="sword://Bible/ESV2011/Luke 11:29||name=Luke11_29">

void rewriteHref(QStringList & parts, int i, QString const & part) {
    QRegExp rx1("<a\\s+(\\w+)=\"([^\"]*)\"\\s+(\\w+)=\"([^\"]*)\"");
    rx1.setMinimal(false);
    if (rx1.indexIn(part) >= 0) {
        if (rx1.cap(1) == "href")
            parts[i] = "<a " + rx1.cap(1) + "=\"" + rx1.cap(2) + "||" + rx1.cap(3) + "=" + rx1.cap(4) + "\" name=\"crossref\">";
        else
            parts[i] = "<a " + rx1.cap(3) + "=\"" + rx1.cap(4) + "||" + rx1.cap(1) + "=" + rx1.cap(2) + "\" name=\"crossref\">";
    }
}

// Typical input: <span lemma="H07225">God</span>
// Output: "<a href="sword://lemmamorph/lemma=H0430||/God" style="color: black">"
int rewriteLemmaOrMorphAsLink(QStringList & parts, int i, QString const & part)
{
    if (i + 2 >= parts.count())
        return 1;

    QString value;
    {
        QRegExp rx1("lemma=\"([^\"]*)*");
        if (rx1.indexIn(part) > -1)
            value = "lemma=" + rx1.cap(1);
    }{
        QRegExp rx2("morph=\"([^\"]*)(\"){0,1}");
        if (rx2.indexIn(part) > -1) {
            if (value.isEmpty()) {
                value = "morph=" + rx2.cap(1);
            } else {
                value += "||morph=" + rx2.cap(1);
            }
        }
    }

    auto const & refText = parts.at(i + 1);
    parts[i] = "<a id=\"lemmamorph\" href=\"sword://lemmamorph/"
               + value + "/" + refText + "\">";
    parts[i+2] = "</a>";
    return 3;
}

} // anonymous namespace

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

    for (int i = 0; i < parts.count();) {
        auto const & part = parts.at(i);
        auto const partIsTag = part.startsWith('<');

        if (partIsTag && part.contains("class=\"footnote\"")) {
            i += rewriteFootnoteAsLink(parts, i, part);
        } else if (partIsTag && (part.contains("href=\""))) {
            rewriteHref(parts, i, part);
            ++i;
        } else if (partIsTag
                   && (part.contains("lemma=\"") || part.contains("morph=\"")))
        {
            i += rewriteLemmaOrMorphAsLink(parts, i, part);
        } else {
            i++;
        }
    }
    return parts.join("");
}
