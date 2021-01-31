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

#ifndef UNREACHABLE_H
#define UNREACHABLE_H

#ifdef __GNUC__
#define UNREACHABLE __builtin_unreachable
#else
#define UNREACHABLE(...)
#endif

#endif /* UNREACHABLE_H */
