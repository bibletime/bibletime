/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <utility>


namespace util {

#if defined(__cpp_lib_to_underlying) && (__cpp_lib_to_underlying >= 202102L)
template <typename ... Args>
constexpr auto to_underlying(Args && ... args)
{ return std::to_underlying(std::forward<Args>(args)...); }
#else
template <typename T>
[[nodiscard]] constexpr std::underlying_type_t<T>
to_underlying(T value) noexcept
{ return static_cast<std::underlying_type_t<T>>(value); }
#endif

} // namespace util
