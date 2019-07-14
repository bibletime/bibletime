/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btquickwidget.h"

#include <QMimeData>
#include <QQuickItem>
#include "../../../util/btassert.h"
#include "../../../util/directory.h"
#include "../../BtMimeData.h"
#include "btqmlinterface.h"
#include "../../display/btmodelviewreaddisplay.h"
#include "../../../backend/drivers/cswordmoduleinfo.h"
#include "../../../backend/managers/cswordbackend.h"
#include "../../displaywindow/cdisplaywindow.h"


BtQuickWidget::BtQuickWidget(BtQmlScrollView* parent)
    : QQuickWidget(parent),
      m_scrollView(parent) {

    setAcceptDrops(true);

    namespace DU = util::directory;
    QString qmlFile = QCoreApplication::applicationDirPath() + "/" + "DisplayView.qml";
    setSource(QUrl::fromLocalFile(qmlFile));
}

BtQmlInterface* BtQuickWidget::getQmlInterface() const {
    return m_scrollView->getQmlInterface();
}

// Reimplementation from QQuickWidget
void BtQuickWidget::dragEnterEvent( QDragEnterEvent* e ) {
    if ( ! e->mimeData()->hasFormat("BibleTime/Bookmark"))
        return;

    const QMimeData* mimedata = e->mimeData();
    if (mimedata == nullptr)
        return;

    const BTMimeData* btmimedata = qobject_cast<const BTMimeData*>(mimedata);
    if (btmimedata == nullptr)
        return;

    BookmarkItem item = (qobject_cast<const BTMimeData*>(e->mimeData()))->bookmark();
    QString moduleName = item.module();
    CSwordModuleInfo *m = CSwordBackend::instance()->findModuleByName(moduleName);
    BT_ASSERT(m);

    // Is bible reference bookmark compatible with the module type?
    CSwordModuleInfo::ModuleType bookmarkType = m->type();
    if ((bookmarkType == CSwordModuleInfo::Bible ||
        bookmarkType == CSwordModuleInfo::Commentary)) {
        if (getQmlInterface()->isBibleOrCommentary()) {
            e->acceptProposedAction();
            return;
        }
    }

    QQuickWidget::dragEnterEvent(e);
    return;
}

// Reimplementation from QQuickWidget
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

// Reimplementation from QQuickWidget
void BtQuickWidget::dragMoveEvent( QDragMoveEvent* e ) {
    if (e->mimeData()->hasFormat("BibleTime/Bookmark")) {
        e->acceptProposedAction();
        return;
    }
    //don't accept the action!
    e->ignore();
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

void BtQuickWidget::scroll(int pixels) {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant vPixels(pixels);
    QMetaObject::invokeMethod(root,"scroll",
                              Q_ARG(QVariant, vPixels));
}
