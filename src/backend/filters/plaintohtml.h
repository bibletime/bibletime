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

#ifndef FILTERS_PLAINTOHTML_H
#define FILTERS_PLAINTOHTML_H

// Sword includes:
#include <swfilter.h>


namespace sword {
    class SWKey;
    class SWModule;
}

namespace Filters {

/**
  \brief Plain text to HTML conversion filter.
*/
class PlainToHtml: public sword::SWFilter {

    protected: /* Methods: */

        /** Reimplemented from sword::SWFilter. */
        char processText(sword::SWBuf &buf,
                         const sword::SWKey *key,
                         const sword::SWModule *module = nullptr) override;

};

} // namespace Filters

#endif
