/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/filters/btplaintohtmlfilter.h"


Filters::BtPlainToHtmlFilter::BtPlainToHtmlFilter() : sword::SWFilter() {
}

/** No descriptions */
char Filters::BtPlainToHtmlFilter::processText(sword::SWBuf& text, const sword::SWKey* /*key*/, const sword::SWModule* /*module*/) {
    int count = 0;

    sword::SWBuf orig = text;
    const char *from = orig.c_str();
    for (text = ""; *from; from++) {
        if ((*from == '\n') && (from[1] == '\n')) { // two newlinea are a paragraph
            text += "<P>";
            from++;
            continue;
        }
        //This is a special case: Newlines in the plaintext editor are stored as <br />, not as \n
        //we need to let them through
        else if ((*from == '<') && (from[1] == 'b') && (from[2] == 'r') && (from[3] == ' ') && (from[4] == '/') && (from[5] == '>')) {
            text += "<br />";
            from += 5;
            continue;
        }
        else if ((*from == '\n')) { // only one new line
            text += "<br/>";
            continue;
        }
        else if (*from == '<') {
            text += "&lt;";
            continue;
        }
        else if (*from == '>') {
            text += "&gt;";
            continue;
        }
        else if (*from == '&') {
            text += "&amp;";
            continue;
        }
        else if (*from == '{') { //footnote start
            text += "<font color=\"#800000\"><small> ("; /// \bug Possible color conflict
            continue;
        }
        else if (*from == '}') { //footnote end
            text += ") </small></font>";
            continue;
        }
        else if ((*from == ' ') && (count > 5000)) {
            text += "<wbr/>";
            count = 0;
            continue;
        }

        text += *from;
        count++;
    }
    return 0;
}
