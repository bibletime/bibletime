/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BT_THMLPLAIN_H
#define BT_THMLPLAIN_H

#include <swbuf.h>
#include <swfilter.h>

namespace Filters {

/** This filter converts ThML text to plain text
*/
class BT_ThMLPlain : public sword::SWFilter {
    protected:
        virtual char processText(sword::SWBuf &text, const sword::SWKey *key = 0, const sword::SWModule *module = 0);
    public:
        BT_ThMLPlain();
};

}
#endif
