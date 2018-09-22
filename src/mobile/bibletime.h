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

#pragma once

#include <QObject>

namespace btm {

class BibleTime : public QObject {
    Q_OBJECT

public:
    BibleTime(QObject* parent = nullptr);

private:
    void initBackends();
    void initSwordConfigFile();

};

}
