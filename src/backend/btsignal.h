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

#include <QObject>

#include <QString>


/**
* BtSignal
* The purpose of this class is to emit Qt signals for other classes
* that are not derived from QObject. It can be used as a member
* variable of those classes.
*
* There are some classes it is not possible to derive from QObject and
* have the signals work. Certain multiple inheritance classes which cannot
* have QObject as the first derived class, cannot use Qt signals.
*/
class BtSignal: public QObject {

    Q_OBJECT

public:

    BtSignal(QObject * parent = nullptr);

    /**
      Immediately emits the beforeChanged() signal.
    */
    void emitSignal() { Q_EMIT signal(); }

Q_SIGNALS:

    void signal();

};
