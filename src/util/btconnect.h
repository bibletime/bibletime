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
#include "btassert.h"


#ifndef NDEBUG
#define BT_CONNECT(...) \
    [&,this]{ \
        auto r(this->connect(__VA_ARGS__)); \
        BT_ASSERT(r); \
        return r; \
    }()
#else
#define BT_CONNECT(...) connect(__VA_ARGS__)
#endif
