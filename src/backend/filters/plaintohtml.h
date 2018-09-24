/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef FILTERS_PLAINTOHTML_H
#define FILTERS_PLAINTOHTML_H

#include <swordxx/swfilter.h>


namespace swordxx {
    class SWKey;
    class SWModule;
}

namespace Filters {

/**
  \brief Plain text to HTML conversion filter.
*/
class PlainToHtml: public swordxx::SWFilter {

    protected: /* Methods: */

        /** Reimplemented from swordxx::SWFilter. */
        char processText(std::string &buf,
                         const swordxx::SWKey *key,
                         const swordxx::SWModule *module = nullptr) override;

};

} // namespace Filters

#endif
