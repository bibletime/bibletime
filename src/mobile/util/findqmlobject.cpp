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

#include "findqmlobject.h"

#include <QQuickItem>
#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include "util/btassert.h"

namespace btm {

QQuickItem* findQmlObject(const QString& objectName) {

    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QQuickItem * rootObject = nullptr;
    if (viewer != nullptr)
        rootObject = viewer->rootObject();
    QQuickItem* object = nullptr;
    if (rootObject != nullptr)
        object = rootObject->findChild<QQuickItem*>(objectName);
    BT_ASSERT(object);
    return object;
}

}
