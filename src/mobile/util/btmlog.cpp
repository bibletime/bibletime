/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "mobile/util/btmlog.h"
#include <QDebug>

namespace btm {

BtmLog::BtmLog()
    : SWLog() {
}

void BtmLog::logMessage(const char* message, int level) const {
    qDebug() << message;
}

}
