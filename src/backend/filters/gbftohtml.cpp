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

#include "gbftohtml.h"

#include <cstdlib>
#include <QRegExp>
#include <QString>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../managers/cswordbackend.h"

// Sword includes:
#include <utilxml.h>


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

    CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName( module->getName() );

    if (m && !(m->has(CSwordModuleInfo::lemmas) || m->has(CSwordModuleInfo::morphTags) || m->has(CSwordModuleInfo::strongNumbers))) { //only parse if the module has strongs or lemmas
        return 1; //WARNING: Return alread here
    }

    //Am Anfang<WH07225> schuf<WH01254><WTH8804> Gott<WH0430> Himmel<WH08064> und<WT> Erde<WH0776>.
    //A simple word<WT> means: No entry for this word "word"
    QString result;

    QString t = QString::fromUtf8(buf.c_str());

    QRegExp tag("([.,;:]?<W[HGT][^>]*>\\s*)+");

    QStringList list;

    int lastMatchEnd = 0;

    int pos = tag.indexIn(t, 0);

    if (pos == -1) { //no strong or morph code found in this text
        return 1; //WARNING: Return already here
    }

    //split the text into parts which end with the GBF tag marker for strongs/lemmas
    while (pos != -1) {
        list.append(t.mid(lastMatchEnd, pos + tag.matchedLength() - lastMatchEnd));

        lastMatchEnd = pos + tag.matchedLength();
        pos = tag.indexIn(t, pos + tag.matchedLength());
    }

    //append the trailing text to the list.
    if (!t.right(t.length() - lastMatchEnd).isEmpty()) {
        list.append(t.right(t.length() - lastMatchEnd));
    }

    //list is now a list of words with 1-n Strongs at the end, which belong to this word.

    //now create the necessary HTML in list entries and concat them to the result
    tag = QRegExp("<W([HGT])([^>]*)>");
    tag.setMinimal(true);

    for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
        QString e = (*it); //current entry to process
        //qWarning(e.latin1());

        //check if there is a word to which the strongs info belongs to.
        //If yes, wrap that word with the strongs info
        //If not, leave out the strongs info, because it can't be tight to a text
        //Comparing the first char with < is not enough, because the tokenReplace is done already
        //so there might be html tags already.
        const bool textPresent = (e.trimmed().remove(QRegExp("[.,;:]")).left(2) != "<W");

        if (!textPresent) {
            result += (*it);
            continue;
        }

        int pos = tag.indexIn(e, 0); //try to find a strong number marker
        bool insertedTag = false;
        bool hasLemmaAttr = false;
        bool hasMorphAttr = false;

        QString value = QString();
        int tagAttributeStart = -1;

        while (pos != -1) { //work on all strong/lemma tags in this section, should be between 1-3 loops
            const bool isMorph = (tag.cap(1) == "T");
            value = isMorph ? tag.cap(2) : tag.cap(2).prepend( tag.cap(1) );

            if (value.isEmpty()) {
                break;
            }

            //insert the span
            if (!insertedTag) { //we have to insert a new tag end and beginning, i.e. our first loop
                e.replace(pos, tag.matchedLength(), "</span>");
                pos += 7;

                //skip blanks, commas, dots and stuff at the beginning, it doesn't belong to the morph code
                QString rep("<span ");
                rep.append(isMorph ? "morph" : "lemma").append("=\"").append(value).append("\">");

                hasMorphAttr = isMorph;
                hasLemmaAttr = !isMorph;

                int startPos = 0;
                QChar c = e[startPos];

                while ((startPos < pos) && (c.isSpace() || c.isPunct())) {
                    ++startPos;

                    c = e[startPos];
                }

                e.insert( startPos, rep );
                tagAttributeStart = startPos + 6; //to point to the start of the attributes
                pos += rep.length();
            }
            else { //add the attribute to the existing tag
                e.remove(pos, tag.matchedLength());

                if (tagAttributeStart == -1) {
                    continue; //nothing valid found
                }

                if ((!isMorph && hasLemmaAttr) || (isMorph && hasMorphAttr)) { //we append another attribute value, e.g. 3000 gets 3000|5000
                    //search the existing attribute start
                    QRegExp attrRegExp( isMorph ? "morph=\".+(?=\")" : "lemma=\".+(?=\")" );
                    attrRegExp.setMinimal(true);
                    const int foundPos = e.indexOf(attrRegExp, tagAttributeStart);

                    if (foundPos != -1) {
                        e.insert(foundPos + attrRegExp.matchedLength(), QString("|").append(value));
                        pos += value.length() + 1;

                        hasLemmaAttr = !isMorph;
                        hasMorphAttr = isMorph;
                    }
                }
                else { //attribute was not yet inserted
                    QString attr = QString(isMorph ? "morph" : "lemma").append("=\"").append(value).append("\" ");

                    e.insert(tagAttributeStart, attr);
                    pos += attr.length();

                    hasMorphAttr = isMorph;
                    hasLemmaAttr = !isMorph;
                }

                //tagAttributeStart remains the same
            }

            insertedTag = true;
            pos = tag.indexIn(e, pos);
        }

        result += e;
    }

    if (list.count())
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
        size_t const tokenLength = strlen(token);

        BT_ASSERT(dynamic_cast<UserData *>(userData));
        UserData * const myUserData = static_cast<UserData *>(userData);
        // Hack to be able to call stuff like Lang():
        sword::SWModule const * const myModule =
                const_cast<sword::SWModule *>(myUserData->module);

        /* We use several append calls because appendFormatted slows down
           filtering, which should be fast. */

        if (!strncmp(token, "WG", 2u)
            || !strncmp(token, "WH", 2u)
            || !strncmp(token, "WT", 2u))
        {
            buf.append('<').append(token).append('>');
        } else if (!strncmp(token, "RB", 2u)) {
            myUserData->hasFootnotePreTag = true;
            buf.append("<span class=\"footnotepre\">");
        } else if (!strncmp(token, "RF", 2u)) {
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
        } else if (!strncmp(token, "Rf", 2u)) { // End of footnote
            userData->suspendTextPassThru = false;
        } else if (!strncmp(token, "FN", 2u)) {
            // The end </font> tag is inserted in addTokenSubsitute
            buf.append("<font face=\"");
            for (size_t i = 2u; i < tokenLength; i++)
                if (token[i] != '\"')
                    buf.append(token[i]);
            buf.append("\">");
        } else if (!strncmp(token, "CA", 2u)) { // ASCII value <CA##> in hex
            BT_ASSERT(tokenLength == 4u);
            buf.append(static_cast<char>(hexToChar(token + 2u)));
        } else {
            return GBFHTML::handleToken(buf, token, userData);
        }
    }

    return true;
}
