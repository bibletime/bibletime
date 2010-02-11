/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef OSISMORPHSEGMENTATION_H
#define OSISMORPHSEGMENTATION_H

// Sword includes:
#include <swbuf.h>
#include <swoptfilter.h>


namespace Filters {

/** This Filter shows/hides headings in a OSIS text.
 * @author Martin Gruner
 */
class OSISMorphSegmentation : public sword::SWOptionFilter {
        static const char oName[];
        static const char oTip[];
        static const sword::SWBuf choices[3];
        static const sword::StringList oValues;

    public:
        OSISMorphSegmentation();
        virtual ~OSISMorphSegmentation();

        virtual char processText(sword::SWBuf &text, const sword::SWKey *key = 0, const sword::SWModule *module = 0);
};

}

#endif
