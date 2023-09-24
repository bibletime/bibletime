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

#include "gbftohtml.h"

#include <cstdlib>
#include <cstring>
#include <QByteArray>
#include <QChar>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QString>
#include <QStringList>
#include <utility>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/cswordbackend.h"

// Sword includes:
#include <gbfhtml.h>
#include <swbasicfilter.h>
#include <swbuf.h>
#include <swkey.h>
#include <swmodule.h>


Filters::GbfToHtml::GbfToHtml() : sword::GBFHTML() {

    setEscapeStringCaseSensitive(true);
    setPassThruUnknownEscapeString(true); //the HTML widget will render the HTML escape codes

    removeTokenSubstitute("Rf");
    //  addTokenSubstitute("RB", "<span>"); //start of a footnote with embedded text

    addTokenSubstitute("FI", "<span class=\"italic\">"); // italics begin
    addTokenSubstitute("Fi", "</span>");

    addTokenSubstitute("FB", "<span class=\"bold\">"); // bold begin
    addTokenSubstitute("Fb", "</span>");

    addTokenSubstitute("FR", "<span class=\"jesuswords\">");
    addTokenSubstitute("Fr", "</span>");

    addTokenSubstitute("FU", "<u>"); // underline begin
    addTokenSubstitute("Fu", "</u>");

    addTokenSubstitute("FO", "<span class=\"quotation\">"); //  Old Testament quote begin
    addTokenSubstitute("Fo", "</span>");


    addTokenSubstitute("FS", "<span class=\"sup\">"); // Superscript begin// Subscript begin
    addTokenSubstitute("Fs", "</span>");

    addTokenSubstitute("FV", "<span class=\"sub\">"); // Subscript begin
    addTokenSubstitute("Fv", "</span>");

    addTokenSubstitute("TT", "<div class=\"booktitle\">");
    addTokenSubstitute("Tt", "</div>");

    addTokenSubstitute("TS", "<div class=\"sectiontitle\">");
    addTokenSubstitute("Ts", "</div>");

    //addTokenSubstitute("PP", "<span class=\"poetry\">"); //  poetry  begin
    //addTokenSubstitute("Pp", "</span>");


    addTokenSubstitute("Fn", "</font>"); //  font  end
    addTokenSubstitute("CL", "<br/>"); //  new line
    addTokenSubstitute("CM", "<br/>"); //  paragraph <!P> is a non showing comment that can be changed in the front end to <P> if desired

    addTokenSubstitute("CG", "&gt;"); // literal greater-than sign
    addTokenSubstitute("CT", "&lt;"); // literal less-than sign

    addTokenSubstitute("JR", "<span class=\"right\">"); // right align begin
    addTokenSubstitute("JC", "<span class=\"center\">"); // center align begin
    addTokenSubstitute("JL", "</span>"); // align end
}

/** No descriptions */
char Filters::GbfToHtml::processText(sword::SWBuf& buf, const sword::SWKey * key, const sword::SWModule * module) {
    GBFHTML::processText(buf, key, module);

    if (!module->isProcessEntryAttributes()) {
        return 1; //no processing should be done, may happen in a search
    }

    if (auto * const m =
                CSwordBackend::instance().findModuleByName(module->getName()))
    {
        // only parse if the module has strongs or lemmas:
        if (!m->has(CSwordModuleInfo::lemmas)
            && !m->has(CSwordModuleInfo::morphTags)
            && !m->has(CSwordModuleInfo::strongNumbers))
            return 1; //WARNING: Return already here
    }

    //Am Anfang<WH07225> schuf<WH01254><WTH8804> Gott<WH0430> Himmel<WH08064> und<WT> Erde<WH0776>.
    //A simple word<WT> means: No entry for this word "word"


    //split the text into parts which end with the GBF tag marker for strongs/lemmas
    QStringList list;
    {
        auto t = QString::fromUtf8(buf.c_str());
        {
            static QRegularExpression const tag(
                QStringLiteral(R"PCRE(([.,;:]?<W[HGT][^>]*?>\s*)+)PCRE"));

            QRegularExpressionMatch match;
            auto pos = t.indexOf(tag, 0, &match);
            if (pos == -1) //no strong or morph code found in this text
                return 1; //WARNING: Return already here
            do {
                auto const partLength = pos + match.capturedLength();
                list.append(t.left(partLength));
                t.remove(0, partLength);
                pos = t.indexOf(tag, 0, &match);
            } while (pos != -1);
        }

        //append the trailing text to the list.
        if (!t.isEmpty())
            list.append(std::move(t));
    }

    //list is now a list of words with 1-n Strongs at the end, which belong to this word.

    //now create the necessary HTML in list entries and concat them to the result
    static QRegularExpression const tag(
        QStringLiteral(R"PCRE(<W([HGT])([^>]*?)>)PCRE"));

    QString result;
    for (auto & e : list) { // for each entry to process
        //qWarning(e.latin1());

        //check if there is a word to which the strongs info belongs to.
        //If yes, wrap that word with the strongs info
        //If not, leave out the strongs info, because it can't be tight to a text
        //Comparing the first char with < is not enough, because the tokenReplace is done already
        //so there might be html tags already.
        {
            static QRegularExpression const re(
                QStringLiteral(R"PCRE([.,;:])PCRE"));
            if (e.trimmed().remove(re).left(2) == QStringLiteral("<W")) {
                result += e;
                continue;
            }
        }

        bool insertedTag = false;
        bool hasLemmaAttr = false;
        bool hasMorphAttr = false;

        int tagAttributeStart = -1;

        /* Try to find a strong number marker. Work on all strong/lemma tags in
           this section, should be between 1-3 loops: */
        QRegularExpressionMatch match;
        for (auto pos = e.indexOf(tag, 0, &match);
             pos != -1;
             pos = e.indexOf(tag, pos, &match))
        {
            auto const isMorph = match.captured(1) == QStringLiteral("T");
            auto const value =
                isMorph
                ? match.captured(2)
                : match.captured(2).prepend(match.captured(1));

            if (value.isEmpty()) {
                break;
            }

            //insert the span
            if (!insertedTag) { //we have to insert a new tag end and beginning, i.e. our first loop
                e.replace(pos, match.capturedLength(), QStringLiteral("</span>"));
                pos += 7;

                //skip blanks, commas, dots and stuff at the beginning, it doesn't belong to the morph code
                auto rep =
                        QStringLiteral("<span %1=\"%2\">")
                        .arg(isMorph
                             ? QStringLiteral("morph")
                             : QStringLiteral("lemma"),
                             value);

                hasMorphAttr = isMorph;
                hasLemmaAttr = !isMorph;

                int startPos = 0;
                QChar c = e[startPos];

                while ((startPos < pos) && (c.isSpace() || c.isPunct())) {
                    ++startPos;

                    c = e[startPos];
                }

                tagAttributeStart = startPos + 6; //to point to the start of the attributes
                pos += rep.size();
                e.insert(startPos, std::move(rep));
            }
            else { //add the attribute to the existing tag
                e.remove(pos, match.capturedLength());

                if (tagAttributeStart == -1) {
                    continue; //nothing valid found
                }

                if ((!isMorph && hasLemmaAttr) || (isMorph && hasMorphAttr)) { //we append another attribute value, e.g. 3000 gets 3000|5000
                    //search the existing attribute start
                    auto const & attrRegExp =
                        [isMorph]{
                            if (isMorph) {
                                static QRegularExpression const re(
                                    QStringLiteral("morph=\".+?(?=\")"));
                                return re;
                            } else {
                                static QRegularExpression const re(
                                    QStringLiteral("lemma=\".+?(?=\")"));
                                return re;
                            }
                        }();
                    QRegularExpressionMatch match;
                    const int foundPos =
                        e.indexOf(attrRegExp, tagAttributeStart, &match);

                    if (foundPos != -1) {
                        e.insert(foundPos + match.capturedLength(),
                                 QStringLiteral("|") + value);
                        pos += value.length() + 1;

                        hasLemmaAttr = !isMorph;
                        hasMorphAttr = isMorph;
                    }
                }
                else { //attribute was not yet inserted
                    hasMorphAttr = isMorph;
                    hasLemmaAttr = !isMorph;

                    auto attr = QStringLiteral("%1=\"%2\" ")
                                .arg(isMorph
                                     ? QStringLiteral("morph")
                                     : QStringLiteral("lemma"),
                                     value);
                    pos += attr.size();
                    e.insert(tagAttributeStart, std::move(attr));
                }

                //tagAttributeStart remains the same
            }

            insertedTag = true;
        }

        result += e;
    }

    if (!list.isEmpty())
        buf = result.toUtf8().constData();

    return 1;
}

namespace {
int hexDigitValue(char const hex) {
    switch (hex) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return hex - '0';
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            return hex - 'a' + 10;
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            return hex - 'A' + 10;
        default:
            BT_ASSERT(false && "Invalid hex code in GBF");
            abort();
    }
}

char hexToChar(char const * const hex) {
    int const first = hexDigitValue(hex[0u]);
    return (first * 16u) + hexDigitValue(hex[1u]);
}
}

bool Filters::GbfToHtml::handleToken(sword::SWBuf &buf, const char *token, sword::BasicFilterUserData *userData) {
    if (!substituteToken(buf, token)) { // More than a simple replace
        size_t const tokenLength = std::strlen(token);

        BT_ASSERT(dynamic_cast<UserData *>(userData));
        UserData * const myUserData = static_cast<UserData *>(userData);
        // Hack to be able to call stuff like Lang():
        sword::SWModule const * const myModule =
                const_cast<sword::SWModule *>(myUserData->module);

        /* We use several append calls because appendFormatted slows down
           filtering, which should be fast. */

        if (!std::strncmp(token, "WG", 2u)
            || !std::strncmp(token, "WH", 2u)
            || !std::strncmp(token, "WT", 2u))
        {
            buf.append('<').append(token).append('>');
        } else if (!std::strncmp(token, "RB", 2u)) {
            myUserData->hasFootnotePreTag = true;
            buf.append("<span class=\"footnotepre\">");
        } else if (!std::strncmp(token, "RF", 2u)) {
            if (myUserData->hasFootnotePreTag) {
                //     qWarning("inserted footnotepre end");
                buf.append("</span>");
                myUserData->hasFootnotePreTag = false;
            }

            buf.append(" <span class=\"footnote\" note=\"")
               .append(myModule->getName())
               .append('/')
               .append(myUserData->key->getShortText())
               .append('/')
               .append(QString::number(myUserData->swordFootnote).toUtf8().constData())
               .append("\">*</span> ");
            myUserData->swordFootnote++;
            userData->suspendTextPassThru = true;
        } else if (!std::strncmp(token, "Rf", 2u)) { // End of footnote
            userData->suspendTextPassThru = false;
        } else if (!std::strncmp(token, "FN", 2u)) {
            // The end </font> tag is inserted in addTokenSubsitute
            buf.append("<font face=\"");
            for (size_t i = 2u; i < tokenLength; i++)
                if (token[i] != '\"')
                    buf.append(token[i]);
            buf.append("\">");
        } else if (!std::strncmp(token, "CA", 2u)) { // ASCII value <CA##> in hex
            BT_ASSERT(tokenLength == 4u);
            buf.append(static_cast<char>(hexToChar(token + 2u)));
        } else {
            return GBFHTML::handleToken(buf, token, userData);
        }
    }

    return true;
}
