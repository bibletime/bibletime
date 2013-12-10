/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef FILTERS_THMLTOPLAIN_H
#define FILTERS_THMLTOPLAIN_H

// Sword includes:
#include <swbuf.h>
#include <swfilter.h>


namespace Filters {

/**
  \brief ThML text to plain text conversion filter
*/
class ThmlToPlain: public sword::SWFilter {
    protected: /* Methods: */
        /** Reimplemented from sword::SWFilter. */
        virtual char processText(sword::SWBuf &text,
                                 const sword::SWKey *key = 0,
                                 const sword::SWModule *module = 0);
};

} // namespace Filters

#endif
