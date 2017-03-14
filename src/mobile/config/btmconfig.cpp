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

#include "btmconfig.h"

#include "backend/config/btconfig.h"

namespace btm {

qreal BtmConfig::getReal(const QString& settingName, qreal defaultValue) {
    return btConfig().value<qreal>(settingName, defaultValue);
}

void BtmConfig::setReal(const QString& settingName, qreal value) {
    btConfig().setValue<qreal>(settingName, value);
}

} // end namespace

