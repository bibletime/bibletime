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

#ifndef CONFIG_INTERFACE_H
#define CONFIG_INTERFACE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include "backend/config/btconfig.h"


namespace btm {

class ConfigInterface : public QObject {

    Q_OBJECT

public:
    ConfigInterface();

    Q_INVOKABLE QString getUserBaseDir();
    Q_INVOKABLE QString getUserHomeSwordDir();
    Q_INVOKABLE QString getWritableTmpDir();

    Q_INVOKABLE bool boolValue(const QString& configKey, bool defaultValue);
    Q_INVOKABLE void setBoolValue(const QString& configKey, bool value);

private:

};

}

#endif
