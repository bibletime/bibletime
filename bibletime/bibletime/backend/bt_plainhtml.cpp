/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cswordmoduleinfo.h"
#include "cswordbackend.h"
#include "util/cpointers.h"

#include "bt_plainhtml.h"

//system includes
#include <stdlib.h>
#include <stdio.h>

//Sword includes
#include <utilxml.h>

//Qt includes
#include <qregexp.h>
#include <qstring.h>

using namespace Filters;

BT_PLAINHTML::BT_PLAINHTML() : sword::SWFilter() {
}

/** No descriptions */
char BT_PLAINHTML::processText(sword::SWBuf& text, const sword::SWKey * key, const sword::SWModule * module) {
	int count = 0;

	SWBuf orig = text;
	const char *from = orig.c_str();
	for (text = ""; *from; from++) 
	{
		if ((*from == '\n') && (from[1] == '\n')) // two newlinea are a paragraph
		{
			text += "<P>";
			from++;
			continue;
		}
		//This is a special case: Newlines in the plaintext editor are stored as <br />, not as \n
		//we need to let them through
		else if ((*from == '<') && (from[1] == 'b') && (from[2] == 'r') && (from[3] == ' ') && (from[4] == '/') && (from[5] == '>')){
			text += "<br />";
			from += 5;
			continue;
		}
		else if ((*from == '\n')){ // only one new line
			text += "<BR>";
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
		else if (*from == '&'){
			text += "&amp;";
			continue;
		}
		else if (*from == '{') { //footnote start
			text += "<FONT COLOR=\"#80000\"><SMALL> (";
			continue;
		}
		else if (*from == '}') //footnote end
		{
			text += ") </SMALL></FONT>";
			continue;
		}
		else if ((*from == ' ') && (count > 5000))
		{
			text += "<WBR>";
			count = 0;
			continue;
		}

		text += *from;
		count++;
	}
	return 0;
}
