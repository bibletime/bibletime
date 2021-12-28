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

#pragma once

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swfilter.h>
#pragma GCC diagnostic pop


namespace sword {
class SWBuf;
class SWKey;
class SWModule;
} // namespace sword

namespace Filters {

/**
  \brief Plain text to HTML conversion filter.
*/
class PlainToHtml: public sword::SWFilter {

    protected: // methods:

        /** Reimplemented from sword::SWFilter. */
        char processText(sword::SWBuf &buf,
                         const sword::SWKey *key,
                         const sword::SWModule *module = nullptr) override;

};

} // namespace Filters
