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

#ifndef BTM_CONFIG_H
#define BTM_CONFIG_H

#include <QObject>
#include <QColor>

namespace btm {

class BtmConfig : public QObject {
    Q_OBJECT

public:

    Q_INVOKABLE qreal getReal(const QString& settingName, qreal defaultValue);
    Q_INVOKABLE void setReal(const QString& settingName, qreal value);

};

} // end namespace

#endif
