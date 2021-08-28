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

#include "plaintohtml.h"

#include <QDebug>

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swbuf.h>
#pragma GCC diagnostic pop


char Filters::PlainToHtml::processText(sword::SWBuf &text,
                                       const sword::SWKey * /*key*/,
                                       const sword::SWModule * /*module*/)
{
    sword::SWBuf orig = text;
    const char * from = orig.c_str();
    bool inFootNote = false;

    for (text = "<p>"; *from; from++) {
        switch (*from) {

        case '\n':
            if (text.size() > 3) { // ignore leading newlines
                if (from[1] == '\n') { // two or more newlines denote a new paragraph
                    text += "</p><p>";
                    do {
                        from++;
                    } while (from[1] == '\n');
                } else { // only one new line
                    text += "<br/>";
                }
            }
            break;

        case '<':
            // This is a special case: Newlines in the plaintext editor are stored as <br />, not as \n
            // we need to let them through
            /// \todo is this quirk necessary?
            if ((from[1] == 'b')
                && (from[2] == 'r')
                && (from[3] == ' ')
                && (from[4] == '/')
                && (from[5] == '>'))
            {
                text += "<br/>";
                from += 5;
            } else {
                text += "&lt;";
            }
            break;

        case '>':
            text += "&gt;";
            break;

        case '&':
            text += "&amp;";
            break;

        case '{': // footnote start
            if (inFootNote) {
                text += *from;
            } else {
                text += "<span class=\"footnote\">";
                inFootNote = true;
            }
            break;

        case '}': // footnote end
            if (inFootNote) {
                text += "</span>";
                inFootNote = false;
            }
            [[fallthrough]];

        default:
            text += *from;
            break;

        }
    }

    // Handle missing footnode end:
    if (inFootNote) {
        qWarning() << "PlainToHtml filter detected missing footnote end.";
        text += "</span>";
    }

    text += "</p>";
    return 0;
}
