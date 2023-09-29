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

#pragma once

/**
    The BtQuickWidget is a subclass of QQuickWidget. The subclass was
    needed to be able to catch the drop event when a bookmark is dropped
    on a read window.
  */

#include <QTimer>
#include <QWidget>
#include <QQuickWidget>


class CSwordKey;
class BtQmlInterface;

class BtQuickWidget : public QQuickWidget {
    Q_OBJECT

public:
    BtQuickWidget(QWidget * const parent = nullptr);

    void scroll(int pixels);
    void updateReferenceText();
    void pageDown();
    void pageUp();

    CSwordKey* getMouseClickedKey();

    BtQmlInterface * qmlInterface() const noexcept { return m_qmlInterface; }

protected:
       void dragMoveEvent(QDragMoveEvent * event) override;
       void dragEnterEvent( QDragEnterEvent* e ) override;
       void dropEvent( QDropEvent* e ) override;

       virtual bool event(QEvent* e) override;
       virtual void mousePressEvent(QMouseEvent *event) override;
       virtual void mouseMoveEvent(QMouseEvent *event) override;
       virtual void mouseReleaseEvent(QMouseEvent *event) override;
       virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
       virtual void wheelEvent(QWheelEvent * event) override;

private: // methods:

   template <typename ... Args>
   void callQml(char const * const method, Args && ... args) {
       QMetaObject::invokeMethod(rootObject(),
                                 method,
                                 Q_ARG(QVariant, args)...);
   }

private:

    BtQmlInterface * const m_qmlInterface;

    QTimer m_scrollTimer;

Q_SIGNALS:
    void referenceDropped(const QString& reference);
};
