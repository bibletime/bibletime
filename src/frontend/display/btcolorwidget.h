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

#ifndef BTCOLORWIDGET_H
#define BTCOLORWIDGET_H

#include <QFrame>


class BtColorWidget: public QFrame {

    Q_OBJECT

public:

    BtColorWidget(QWidget * parent = nullptr);

    QSize sizeHint() const override;

public slots:

    void setColor(QColor const & color);

protected: /* Methods: */

    void mouseReleaseEvent(QMouseEvent * event) override;

signals:

    void changed(QColor const & color);

};

#endif


