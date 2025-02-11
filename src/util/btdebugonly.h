/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#ifdef NDEBUG
#define BT_DEBUG_ONLY(...)
#define BT_NDEBUG_ONLY(...) __VA_ARGS__
#else
#define BT_DEBUG_ONLY(...) __VA_ARGS__
#define BT_NDEBUG_ONLY(...)
#endif
