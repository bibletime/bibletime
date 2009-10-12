/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/filters/osismorphsegmentation.h"

// Sword includes:
#include <utilxml.h>


const char Filters::OSISMorphSegmentation::oName[] = "Morph segmentation";
const char Filters::OSISMorphSegmentation::oTip[] = "Toggles morph segmentation On and Off if they exist";

const sword::SWBuf Filters::OSISMorphSegmentation::choices[3] = {"Off", "On", ""};

const sword::StringList Filters::OSISMorphSegmentation::oValues(&choices[0], &choices[2]);

Filters::OSISMorphSegmentation::OSISMorphSegmentation() : sword::SWOptionFilter(oName, oTip, &oValues) {
    setOptionValue("Off");
}

Filters::OSISMorphSegmentation::~OSISMorphSegmentation() {}

char Filters::OSISMorphSegmentation::processText(sword::SWBuf &text, const sword::SWKey */*key*/, const sword::SWModule */*module*/) {
    sword::SWBuf token;
    bool intoken    = false;
    bool hide       = false;

    sword::SWBuf orig( text );
    const char *from = orig.c_str();

    sword::XMLTag tag;

    for (text = ""; *from; ++from) {
        if (*from == '<') {
            intoken = true;
            token = "";
            continue;
        }

        if (*from == '>') { // process tokens
            intoken = false;

            if (!strncmp(token.c_str(), "seg ", 4) || !strncmp(token.c_str(), "/seg", 4)) {
                tag = token;

                if (!tag.isEndTag() && tag.getAttribute("type") && !strcmp("morph", tag.getAttribute("type"))) {  //<seg type="morph"> start tag
                    hide = (option == 0); //only hide if option is Off
                }

                if (hide) { //hides start and end tags as long as hide is set

                    if (tag.isEndTag()) { //</seg>
                        hide = false;
                    }

                    continue; //leave out the current token
                }
            } //end of seg tag handling

            text.append('<');
            text.append(token);
            text.append('>');

            //				hide = false; //not right, because there may be child tags in seg. Only /seg may disable the seg hiding.

            continue;
        } //end of intoken part

        if (intoken) { //copy token
            token.append(*from);
        }
        else { //copy text which is not inside of a tag
            text.append(*from);
        }
    }

    return 0;
}

