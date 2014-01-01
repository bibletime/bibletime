/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTOSISMORPHSEGMENTATION_H
#define BTOSISMORPHSEGMENTATION_H

// Sword includes:
#include <swbuf.h>
#include <swoptfilter.h>


namespace Filters {

/**
  \brief This Filter shows/hides headings in a OSIS text.
  \author Martin Gruner
*/
class BtOSISMorphSegmentation: public sword::SWOptionFilter {

public: /* Methods: */

    BtOSISMorphSegmentation();

    virtual char processText(sword::SWBuf & text,
                             const sword::SWKey * key = 0,
                             const sword::SWModule * module = 0);

private: /* Fields: */

    static const char oName[];
    static const char oTip[];
    static const sword::SWBuf choices[3];
    static const sword::StringList oValues;

};

} /* namespace Filters { */

#endif /* BTOSISMORPHSEGMENTATION_H */
