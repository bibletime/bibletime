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

#ifndef TOUNDERLYING_H
#define TOUNDERLYING_H

#include <type_traits>


template <typename T>
constexpr std::underlying_type_t<T> toUnderlying(T const value)
{ return static_cast<std::underlying_type_t<T>>(value); }

#endif /* TOUNDERLYING_H */
