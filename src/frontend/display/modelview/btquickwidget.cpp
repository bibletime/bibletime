/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btquickwidget.h"
#include "../../bibletime.h"

#include <QCursor>
#include <QGuiApplication>
#include <QMimeData>
#include <QMouseEvent>
#include <QQmlEngine>
#include <QQuickItem>
#include <Qt>
#include <QWheelEvent>
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
    engine()->addImportPath("qrc:/qml");
    QString qmlFile = "qrc:/qml/DisplayView.qml";
    QUrl url(qmlFile);
    setSource(url);
    setupScrollTimer();
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

int BtQuickWidget::getSelectedColumn() const {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant var;
    QMetaObject::invokeMethod(root,"getSelectedColumn",
                                        Qt::DirectConnection, Q_RETURN_ARG(QVariant, var));
    return var.toInt();
}

int BtQuickWidget::getFirstSelectedIndex() const {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant var;
    QMetaObject::invokeMethod(root,"getFirstSelectedIndex",
                                        Qt::DirectConnection, Q_RETURN_ARG(QVariant, var));
    return var.toInt();
}

int BtQuickWidget::getLastSelectedIndex() const {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant var;
    QMetaObject::invokeMethod(root,"getLastSelectedIndex",
                                        Qt::DirectConnection, Q_RETURN_ARG(QVariant, var));
    return var.toInt();
}

CSwordKey* BtQuickWidget::getMouseClickedKey() {
    return getQmlInterface()->getMouseClickedKey();
}

void BtQuickWidget::scroll(int pixels) {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant vPixels(pixels);
    QMetaObject::invokeMethod(root,"scroll",
                              Q_ARG(QVariant, vPixels));
}

// Catch Leave event here insteaded of leaveEvent(e), because
// QMdiSubwindow does not pass leaveEvent on down.
bool BtQuickWidget::event(QEvent* e) {
    if (e->type() == QEvent::Leave)
        getQmlInterface()->cancelMagTimer();
    return QQuickWidget::event(e);
}

void BtQuickWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    event->accept();
    return;
}

void BtQuickWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mousePressed(event->x(), event->y());
        event->accept();
        return;
    }
    return QQuickWidget::mousePressEvent(event);
}

void BtQuickWidget::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton) {
        int y = event->y();
        if ( y < 0) {
            startScrollTimer();
        } else if (y > height()) {
            startScrollTimer();
        } else {
            stopScrollTimer();
        }

        mouseMove(event->x(), event->y());
        event->accept();
        return;
    }
    return QQuickWidget::mouseMoveEvent(event);
}

void BtQuickWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mouseReleased(event->x(), event->y());
        event->accept();
        return;
    }
    return QQuickWidget::mouseReleaseEvent(event);
}

void BtQuickWidget::mousePressed(int x, int y) {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant vX(x);
    QVariant vY(y);
    QMetaObject::invokeMethod(root,"leftMousePress",
                              Q_ARG(QVariant, vX), Q_ARG(QVariant, vY));

}

void BtQuickWidget::mouseMove(int x, int y) {
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant vX(x);
    QVariant vY(y);
    QMetaObject::invokeMethod(root,"leftMouseMove",
                              Q_ARG(QVariant, vX), Q_ARG(QVariant, vY));
}

void BtQuickWidget::mouseReleased(int x, int y) {
    stopScrollTimer();
    QQuickItem* root = rootObject();
    BT_ASSERT(root);
    QVariant vX(x);
    QVariant vY(y);
    QMetaObject::invokeMethod(root,"leftMouseRelease",
                              Q_ARG(QVariant, vX), Q_ARG(QVariant, vY));
}

void BtQuickWidget::setupScrollTimer() {
    m_timer.setInterval(100);
    connect(&m_timer, &QTimer::timeout, this, &BtQuickWidget::scrollTimerSlot);
    m_timer.setSingleShot(false);
}

void BtQuickWidget::startScrollTimer() {
    if (m_timer.isActive())
        return;
    m_timer.start();
}

void BtQuickWidget::stopScrollTimer() {
    if (m_timer.isActive())
        m_timer.stop();
}

void BtQuickWidget::scrollTimerSlot() {
    QPoint globalPoint = QCursor::pos();
    QPoint point = mapFromGlobal(globalPoint);
    int y = point.y();
    if ((y >= 0) & (y-height() < 0))
        return;
    if (y < 0) {
        scroll(-8);
    } else {
        scroll(8);
    }
    int y2 = y * y;
    if (y2 > 100)
        y2 = 100;
    int interval = 500 / y2;
    m_timer.setInterval(interval);
}

void BtQuickWidget::wheelEvent(QWheelEvent * event) {
    BibleTime::instance()->autoScrollStop();
    QQuickWidget::wheelEvent(event);
}
