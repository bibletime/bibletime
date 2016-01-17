/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTASSERT_H
#define BTASSERT_H

#include <cassert>
#include <QtGlobal>


#ifndef NDEBUG
#ifndef QT_NO_DEBUG
#define BT_ASSERT(...) ([&]{ Q_ASSERT(__VA_ARGS__); }())
#else
#define BT_ASSERT(...) assert(__VA_ARGS__)
#endif
#else
#define BT_ASSERT(...) assert(true)
#endif

#endif /* BTASSERT_H */
