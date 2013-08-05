/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef FIND_QML_OBJECT
#define FIND_QML_OBJECT

#include <QString>

class QQuickItem;

namespace btm {

// Finds the named QML object that is located at the
// top level of the QML objects.
QQuickItem* findQmlObject(const QString& objectName);

}

#endif
