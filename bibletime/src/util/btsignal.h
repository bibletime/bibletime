/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTSIGNAL_H
#define BTSIGNAL_H

#include <QObject>

class BtSignal : public QObject {
        Q_OBJECT
    public:
        /**
        * the constructor
        */
        BtSignal(QObject *parent = 0);
        ~BtSignal();
        void emitChanged();
    signals:
        void changed();
};
#endif
