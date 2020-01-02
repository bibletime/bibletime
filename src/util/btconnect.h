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

#ifndef BTCONNECT_H
#define BTCONNECT_H

#include <QObject>
#include "btassert.h"
#include "btwrap.h"


#ifndef NDEBUG
#define BT_CONNECT_WITH(with, ...) \
    do { \
        bool const ok = with connect(__VA_ARGS__); \
        BT_ASSERT(ok); \
    } while (false)
#define BT_CONNECT(...)         BT_CONNECT_WITH(,         BT_WRAP(__VA_ARGS__))
#define BT_CONNECT_QOBJECT(...) BT_CONNECT_WITH(QObject::,BT_WRAP(__VA_ARGS__))
#else
#define BT_CONNECT_WITH(with, ...)  with connect(__VA_ARGS__)
#define BT_CONNECT(...)                  connect(__VA_ARGS__)
#define BT_CONNECT_QOBJECT(...) QObject::connect(__VA_ARGS__)
#endif

#endif /* BTCONNECT_H */
