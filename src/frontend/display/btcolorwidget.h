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

#include <QFrame>

#include <QColor>
#include <QObject>
#include <QSize>
#include <QString>


class QMouseEvent;
class QWidget;

class BtColorWidget: public QFrame {

    Q_OBJECT

public:

    BtColorWidget(QWidget * parent = nullptr);

    QSize sizeHint() const override;

public Q_SLOTS:

    void setColor(QColor const & color);

protected: // methods:

    void mouseReleaseEvent(QMouseEvent * event) override;

Q_SIGNALS:

    void changed(QColor const & color);

};
