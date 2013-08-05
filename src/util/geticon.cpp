/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "geticon.h"

#include <QDebug>
#include <QFile>
#include <QMap>
#include "util/directory.h"

namespace util {

static QMap<QString, QIcon> iconCache;
static const QIcon nullIcon;

const QIcon& getIcon(const QString & name) {
    QString plainName(name);
    if (plainName.endsWith(".svg", Qt::CaseInsensitive))
        plainName.chop(4);

    const QMap<QString, QIcon>::const_iterator i = iconCache.find(plainName);
    if (i != iconCache.end())
        return *i;

    const QString iconDir = util::directory::getIconDir().canonicalPath();
    QString iconFileName = iconDir + "/" + plainName + ".svg";
    if (QFile(iconFileName).exists())
        return *iconCache.insert(plainName, QIcon(iconFileName));

    iconFileName = iconDir + "/" + plainName + ".png";
    if (QFile(iconFileName).exists())
        return *iconCache.insert(plainName, QIcon(iconFileName));

    if (plainName != "default") {
        qWarning() << "Cannot find icon file" << iconFileName
                   << ", using default icon.";
        return getIcon("default");
    }

    qWarning() << "Cannot find default icon" << iconFileName
               << ", using null icon.";
    return nullIcon;
}

void clearIconCache() {
    iconCache.clear();
}
}
