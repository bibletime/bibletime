/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BT_TEIHTML_H
#define BT_TEIHTML_H

// Sword includes:
#include <teihtmlhref.h>
#include <swbuf.h>

namespace Filters {

/** BibleTime's TEI to HTMl filter.
* This filter works on TEI tags and outputs HTML in the structure supported by BibleTime.
*/

class BT_TEIHTML : public sword::TEIHTMLHREF {

    public:
        BT_TEIHTML();
        virtual bool handleToken(sword::SWBuf &buf, const char *token, sword::BasicFilterUserData *userData);
    private:
        void renderReference(const char *osisRef, sword::SWBuf &buf, sword::BasicFilterUserData *myUserData);
};

} //end of Filters namespace

#endif
