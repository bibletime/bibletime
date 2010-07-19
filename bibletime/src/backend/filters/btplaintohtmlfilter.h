/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTPLAINTOHTMLFILTER_H
#define BTPLAINTOHTMLFILTER_H

// Sword includes:
#include <swbuf.h>
#include <swfilter.h>


class SWKey;
class SWModule;

namespace Filters {

/**
  \brief Plain text to HTML conversion filter.
*/
class BtPlainToHtmlFilter: public sword::SWFilter {
    public: /* Methods: */
        BtPlainToHtmlFilter();

    protected: /* Methods: */
        /** Reimplemented from sword::SWFilter. */
        virtual char processText(sword::SWBuf &buf,
                                 const sword::SWKey *key,
                                 const sword::SWModule *module = 0);
};

} // namespace Filters

#endif
