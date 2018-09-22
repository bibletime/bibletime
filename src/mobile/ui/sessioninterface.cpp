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

#include "sessioninterface.h"

#include "backend/config/btconfig.h"
#include "mobile/ui/btstyle.h"

namespace btm {

SessionInterface::SessionInterface() :
    QObject() {

}

int SessionInterface::getColorTheme() {
    BtConfig & conf = btConfig();
    return conf.sessionValue<int>("ColorTheme", BtStyle::darkTheme);
}

int SessionInterface::getWindowArrangementMode() {
    BtConfig & conf = btConfig();
    return conf.sessionValue<int>("MainWindow/MDIArrangementMode");
}

QStringList SessionInterface::getWindowList() {
    BtConfig & conf = btConfig();
    QStringList w = conf.sessionValue<QStringList>("windowsList");
    return w;
}

QStringList SessionInterface::getWindowModuleList(const QString& win) {
    BtConfig & conf = btConfig();
    const QString windowGroup = "window/" + win + '/';
    QStringList moduleNames = conf.sessionValue<QStringList>(windowGroup + "modules");
    return moduleNames;
}

QString SessionInterface::getWindowKey(const QString& win) {
    BtConfig & conf = btConfig();
    const QString windowGroup = "window/" + win + '/';
    const QString key = conf.sessionValue<QString>(windowGroup + "key");
    return key;
}

void SessionInterface::setColorTheme(const QString& color) {
    BtConfig & conf = btConfig();
    conf.setSessionValue("ColorTheme",color);
}

void SessionInterface::setWindowArrangementMode(int mode) {
    BtConfig & conf = btConfig();
    conf.setSessionValue("MainWindow/MDIArrangementMode", mode);
}

void SessionInterface::setWindowList(const QStringList& list) {
    BtConfig & conf = btConfig();
    conf.setSessionValue("windowsList", list);
}

void SessionInterface::setWindowModuleList(int index, const QStringList& modules) {
    BtConfig & conf = btConfig();
    const QString windowKey = QString::number(index);
    const QString windowGroup = "window/" + windowKey + '/';
    conf.beginGroup(windowGroup);
    conf.setSessionValue("modules", modules);
    conf.endGroup();
}

void SessionInterface::setWindowKey(int index, const QString& key) {
    BtConfig & conf = btConfig();
    const QString windowKey = QString::number(index);
    const QString windowGroup = "window/" + windowKey + '/';
    conf.beginGroup(windowGroup);
    conf.setSessionValue("key", key);
    conf.endGroup();
}

}
