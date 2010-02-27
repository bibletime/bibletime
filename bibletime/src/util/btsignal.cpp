/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "util/btsignal.h"

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
BtSignal::BtSignal(QObject* parent) {
}

BtSignal::~BtSignal() {
}

void BtSignal::emitChanged() {
    emit changed();
}

