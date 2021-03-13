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

#if !defined(QT_NO_DEBUG) && !defined(NDEBUG)
#include <QtGlobal>
#define BT_ASSERT(...) ([&]{ Q_ASSERT(__VA_ARGS__); }())
#else
#include <cassert>
#define BT_ASSERT(...) assert(__VA_ARGS__)
#endif
