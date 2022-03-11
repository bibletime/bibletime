/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
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
#include "../../../backend/drivers/cswordmoduleinfo.h"
#include "../../../backend/managers/cswordbackend.h"
#include "../../../util/btassert.h"
#include "../../../util/directory.h"
#include "../../BtMimeData.h"
#include "../../display/btmodelviewreaddisplay.h"
#include "../../displaywindow/cdisplaywindow.h"
#include "btqmlinterface.h"


BtQuickWidget::BtQuickWidget(BtQmlScrollView* parent)
    : QQuickWidget(parent),
      m_scrollView(parent) {

    setAcceptDrops(true);

    engine()->addImportPath("qrc:/qml");
    setSource(QUrl("qrc:/qml/DisplayView.qml"));

    m_scrollTimer.setInterval(100);
    m_scrollTimer.setSingleShot(false);
    connect(&m_scrollTimer, &QTimer::timeout, this,
            [this]{
                int y = mapFromGlobal(QCursor::pos()).y();
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
                m_scrollTimer.setInterval(500 / y2);
            });
}

void BtQuickWidget::dragEnterEvent(QDragEnterEvent * const e) {
    if (auto const * const btmimedata =
                qobject_cast<BTMimeData const *>(e->mimeData()))
    {
        auto const & item = btmimedata->bookmarks().first();
        auto * const m =
                CSwordBackend::instance()->findModuleByName(item.module());
        BT_ASSERT(m);

        // Is bible reference bookmark compatible with the module type?
        CSwordModuleInfo::ModuleType bookmarkType = m->type();
        if ((bookmarkType == CSwordModuleInfo::Bible
             || bookmarkType == CSwordModuleInfo::Commentary)
            && m_scrollView->getQmlInterface()->isBibleOrCommentary())
        {
            e->acceptProposedAction();
        } else {
            QQuickWidget::dragEnterEvent(e);
        }
    }
}

void BtQuickWidget::dropEvent(QDropEvent * const e) {
    if (auto const * const btmimedata =
                qobject_cast<BTMimeData const *>(e->mimeData()))
    {
        BookmarkItem item = btmimedata->bookmarks().first();
        Q_EMIT referenceDropped(item.key());
        e->acceptProposedAction();
        return;
    }
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
    callQml("saveContextMenuIndex",
            x - geometry().x(), // Translate to BtQuickWidget
            y - geometry().y());
}

void BtQuickWidget::updateReferenceText() { callQml("updateReferenceText"); }

int BtQuickWidget::getSelectedColumn() const
{ return getFromQml("getSelectedColumn").toInt(); }

int BtQuickWidget::getFirstSelectedIndex() const
{ return getFromQml("getFirstSelectedIndex").toInt(); }

int BtQuickWidget::getLastSelectedIndex() const
{ return getFromQml("getLastSelectedIndex").toInt(); }

CSwordKey* BtQuickWidget::getMouseClickedKey() {
    return m_scrollView->getQmlInterface()->getMouseClickedKey();
}

void BtQuickWidget::scroll(int const pixels) { callQml("scroll", pixels); }

// Catch Leave event here insteaded of leaveEvent(e), because
// QMdiSubwindow does not pass leaveEvent on down.
bool BtQuickWidget::event(QEvent* e) {
    if (e->type() == QEvent::Leave)
        m_scrollView->getQmlInterface()->cancelMagTimer();
    return QQuickWidget::event(e);
}

void BtQuickWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    event->accept();
    return;
}

void BtQuickWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        callQml("leftMousePress", event->x(), event->y());
        event->accept();
        return;
    }
    QQuickWidget::mousePressEvent(event);
}

void BtQuickWidget::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton) {
        auto const y = event->y();
        if (y < 0 || y > height()) {
            if (!m_scrollTimer.isActive())
                m_scrollTimer.start();
        } else {
            m_scrollTimer.stop();
        }

        callQml("leftMouseMove", event->x(), y);
        event->accept();
        return;
    }
    return QQuickWidget::mouseMoveEvent(event);
}

void BtQuickWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_scrollTimer.stop();
        callQml("leftMouseRelease", event->x(), event->y());
        event->accept();
        return;
    }
    return QQuickWidget::mouseReleaseEvent(event);
}

void BtQuickWidget::wheelEvent(QWheelEvent * event) {
    BibleTime::instance()->autoScrollStop();
    QQuickWidget::wheelEvent(event);
}

QVariant BtQuickWidget::getFromQml(char const * method) const {
    QVariant r;
    QMetaObject::invokeMethod(rootObject(),
                              method,
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QVariant, r));
    return r;
}
