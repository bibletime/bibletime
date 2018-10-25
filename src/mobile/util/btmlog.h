/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
<<<<<<< HEAD
* Copyright 1999-2018 by the BibleTime developers.
=======
* Copyright 1999-2016 by the BibleTime developers.
>>>>>>> traduction
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTMLOG_H
#define BTMLOG_H

#include <swlog.h>

namespace btm {

class BtmLog : public sword::SWLog
{

public:

    BtmLog();

    void logMessage(const char *message, int level) const override;
};


}
#endif
