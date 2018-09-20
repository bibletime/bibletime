/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

/**
    The BtQuickWidget is a subclass of QQuickWidget. The subclass was
    needed to be able to catch the drop event when a bookmark is dropped
    on a read window.
  */

#ifndef BTQUICKWIDGET_H
#define BTQUICKWIDGET_H

#include <QWidget>
#include <QQuickWidget>

class BtQuickWidget : public QQuickWidget {
    Q_OBJECT

public:
    BtQuickWidget(QWidget* widget = nullptr);

    void moveContentLocation(double value);

private:
    void dropEvent( QDropEvent* e ) override;

signals:
    void referenceDropped(const QString& reference);
};


#endif
