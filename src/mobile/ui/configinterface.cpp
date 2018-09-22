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

#include "configinterface.h"
#include <QStandardPaths>
#include "util/directory.h"

namespace btm {

ConfigInterface::ConfigInterface() :
    QObject() {
}


bool ConfigInterface::boolValue(const QString& configKey, bool defaultValue) {
    return btConfig().value<bool>(configKey, defaultValue);
}

void ConfigInterface::setBoolValue(const QString& configKey, bool value) {
    btConfig().setValue(configKey, value);
}

QString ConfigInterface::getUserBaseDir() {
    return util::directory::getUserBaseDir().absolutePath();
}

QString ConfigInterface::getUserHomeSwordDir() {
    return util::directory::getUserHomeSwordDir().absolutePath();
}

QString ConfigInterface::getWritableTmpDir() {
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation);
}

}
