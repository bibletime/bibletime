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
#include <QRegularExpression>
#include <QRegularExpressionMatch>


namespace {

QStringList splitText(QString const & text) {
    QStringList parts;
    int from = 0;
    while (from < text.length()) {

        // Get text before tag
        int end = text.indexOf('<', from);
        if (end == -1)
            end = text.length();
        parts.append(text.mid(from, end-from));
        from = end;

        //Get tag text
        end = text.indexOf('>', from);
        if (end == -1)
            end = text.length();
        parts.append(text.mid(from, end-from+1));
        from = end+1;
    }
    return parts;
}

void fixDoubleBR(QStringList & parts) {
    static QRegularExpression const rx(R"regex(<br\s*/>)regex");
    for (int index = 2; index < parts.count(); ++index) {
        if (parts.at(index).contains(rx) && parts.at(index-2).contains(rx))
            parts[index] = "";
    }
}

// Typical input:  <span class="footnote" note="ESV2011/Luke 11:37/1">
// Output:         <span class="footnote" note="ESV2011/Luke 11:37/1">1</span>

int rewriteFootnoteAsLink(QStringList & parts, int i, QString const & part) {
    if (i + 2 >= parts.count())
        return 1;

    static QRegularExpression const rx(R"regex(note="([^"]*))regex");
    if (auto const match = rx.match(part); match.hasMatch()) {
        auto const & footnoteText = parts.at(i + 1);
        parts[i] =
            QStringLiteral(
                R"HTML(<a class="footnote" href="sword://footnote/%1=%2">)HTML")
            .arg(match.captured(1)).arg(footnoteText);
        parts[i+1] = QStringLiteral("(%1)").arg(footnoteText);
        parts[i+2] = QStringLiteral("</a>");
        return 3;
    }
    return 1;
}

// Packs attribute part of href into the link
// Typical input: <a name="Luke11_29" href="sword://Bible/ESV2011/Luke 11:29">
// Output:        <a href="sword://Bible/ESV2011/Luke 11:29||name=Luke11_29">

void rewriteHref(QStringList & parts, int i, QString const & part) {
    static QRegularExpression const rx(
                R"regex(<a\s+(\w+)="([^"]*)"\s+(\w+)="([^"]*)")regex");
    if (auto const match = rx.match(part); match.hasMatch())
        parts[i] =
            ((match.captured(1) == QStringLiteral("href"))
             ? QStringLiteral(R"HTML(<a %1="%2||%3=%4" name="crossref">)HTML")
             : QStringLiteral(R"HTML(<a %3="%4||%1=%2" name="crossref">)HTML"))
            .arg(match.captured(1),
                 match.captured(2),
                 match.captured(3),
                 match.captured(4));
}

// Typical input: <span lemma="H07225">God</span>
// Output: "<a href="sword://lemmamorph/lemma=H0430||/God" style="color: black">"
int rewriteLemmaOrMorphAsLink(QStringList & parts, int i, QString const & part)
{
    if (i + 2 >= parts.count())
        return 1;

    QString value;
    {
        static QRegularExpression const rx(R"regex(lemma="([^"]*)")regex");
        if (auto const match = rx.match(part); match.hasMatch())
            value = QStringLiteral("lemma=") + match.captured(1);
    }{
        static QRegularExpression const rx(R"regex(morph="([^"]*)")regex");
        if (auto const match = rx.match(part); match.hasMatch()) {
            if (value.isEmpty()) {
                value = QStringLiteral("morph=") + match.captured(1);
            } else {
                value = QStringLiteral("%1||morph=%2")
                        .arg(value, match.captured(1));
            }
        }
    }

    auto const & refText = parts.at(i + 1);
    parts[i] =
            QStringLiteral(
                R"HTM(<a id="lemmamorph" href="sword://lemmamorph/%1/%2">)HTM")
            .arg(value, refText);
    parts[i + 2] = QStringLiteral("</a>");
    return 3;
}

int rewriteTag(QStringList & parts, int i, QString const & tag) {
    if (i + 2 >= parts.count())
        return 1;
    parts[i] = "<" + tag + ">";
    parts[i+2] ="</" + tag + ">";
    return 3;
}

int rewriteTitle(QStringList & parts, int i, QString const & tag) {
    if (i + 2 >= parts.count())
        return 1;
    parts[i] = "<div><big><" + tag + ">";
    parts[i+2] ="</" + tag + "></big></div>";
    return 3;
}

int rewriteClass(QStringList & parts, int i, QString const & part) {

    if (part.contains(QStringLiteral(R"HTML(class="footnote")HTML"))) {
        return rewriteFootnoteAsLink(parts, i, part);
    } else if (part.contains(QStringLiteral(R"HTML(class="bold")HTML"))) {
        return rewriteTag(parts, i, "b");
    } else if (part.contains(QStringLiteral(R"HTML(class="italic")HTML"))) {
        return rewriteTag(parts, i, "i");
    } else if (part.contains(QStringLiteral(R"HTML(class="chaptertitle")HTML"))) {
        return rewriteTitle(parts, i, "b");
    } else if (part.contains(QStringLiteral(R"HTML(class="sectiontitle")HTML"))) {
        return rewriteTitle(parts, i, "b");
    } else if (part.contains(QStringLiteral(R"HTML(class="booktitle")HTML"))) {
        return rewriteTitle(parts, i, "b");
    }
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
        while ((index = localText.indexOf(QStringLiteral("<!P>"))) >= 0)
            localText.remove(index,4);
    }
    auto parts = splitText(localText);
    fixDoubleBR(parts);

    for (int i = 0; i < parts.count();) {
        if (auto const & part = parts.at(i); part.startsWith('<')) { // is tag
            if (part.contains(QStringLiteral(R"HTML(class=)HTML"))) {
                i += rewriteClass(parts, i, part);
            } else if (part.contains(QStringLiteral(R"HTML(class="footnote")HTML"))) {
                i += rewriteFootnoteAsLink(parts, i, part);
            } else if (part.contains(QStringLiteral(R"HTML(href=")HTML"))) {
                rewriteHref(parts, i, part);
                ++i;
            } else if (part.contains(QStringLiteral(R"HTML(lemma=")HTML"))
                       || part.contains(QStringLiteral(R"HTML(morph=")HTML")))
            {
                i += rewriteLemmaOrMorphAsLink(parts, i, part);
            } else {
                ++i;
            }
        } else {
            ++i;
        }
    }
    return parts.join(QString());
}
