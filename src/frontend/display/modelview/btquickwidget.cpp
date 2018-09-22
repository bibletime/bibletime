/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "util/btassert.h"
#include "frontend/display/modelview/btquickwidget.h"
#include "frontend/BtMimeData.h"
#include "util/directory.h"
#include <QMimeData>
#include <QQuickItem>

BtQuickWidget::BtQuickWidget(QWidget* parent)
    : QQuickWidget(parent) {

    setAcceptDrops(true);

    namespace DU = util::directory;
    QString qmlFile = DU::getJavascriptDir().canonicalPath() + "/" + "DisplayView.qml";
    setSource(QUrl::fromLocalFile(qmlFile));
}

// Reimplementation from QWidget
void BtQuickWidget::dropEvent( QDropEvent* e ) {
    if (e->mimeData()->hasFormat("BibleTime/Bookmark")) {
        //see docs for BTMimeData and QMimeData
        const QMimeData* mimedata = e->mimeData();
        if (mimedata != nullptr) {
            const BTMimeData* btmimedata = qobject_cast<const BTMimeData*>(mimedata);
            if (btmimedata != nullptr) {
                BookmarkItem item = (qobject_cast<const BTMimeData*>(e->mimeData()))->bookmark();
                emit referenceDropped(item.key());
                e->acceptProposedAction();
                return;
            }
        }
    };
}

void BtQuickWidget::moveContentLocation(double value) {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant vValue(value);
    QMetaObject::invokeMethod(root,"moveContentLocation", Q_ARG(QVariant, vValue));
}

void BtQuickWidget::saveContextMenuIndex(int x, int y) {
    int x1 = x - geometry().x();  // Translate to BtQuickWidget
    int y1 = y - geometry().y();
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant vX(x1);
    QVariant vY(y1);
    QMetaObject::invokeMethod(root,"saveContextMenuIndex",
                              Q_ARG(QVariant, vX), Q_ARG(QVariant, vY));
}

void BtQuickWidget::updateReferenceText() {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QMetaObject::invokeMethod(root,"updateReferenceText");
}
