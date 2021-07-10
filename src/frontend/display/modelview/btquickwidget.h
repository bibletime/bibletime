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

/**
    The BtQuickWidget is a subclass of QQuickWidget. The subclass was
    needed to be able to catch the drop event when a bookmark is dropped
    on a read window.
  */

#ifndef BTQUICKWIDGET_H
#define BTQUICKWIDGET_H

#include <QTimer>
#include <QWidget>
#include <QQuickWidget>
#include "btqmlscrollview.h"

class BtQmlInterface;

class BtQuickWidget : public QQuickWidget {
    Q_OBJECT

public:
    BtQuickWidget(BtQmlScrollView* widget);

    void mousePressed(int x, int y);
    void mouseMove(int x, int y);
    void mouseReleased(int x, int y);

    void saveContextMenuIndex(int x, int y);
    void scroll(int pixels);
    void updateReferenceText();

    int getSelectedColumn() const;
    int getFirstSelectedIndex() const;
    int getLastSelectedIndex() const;
    CSwordKey* getMouseClickedKey();

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

private slots:
    void scrollTimerSlot();

private:
    BtQmlInterface* getQmlInterface() const;
    void setupScrollTimer();
    void startScrollTimer();
    void stopScrollTimer();


    BtQmlScrollView* m_scrollView;

    QTimer m_timer;

signals:
    void referenceDropped(const QString& reference);
};


#endif
