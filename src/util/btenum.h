/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2022 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#define BT_ENUM(name,...) \
    enum name { __VA_ARGS__ }; \
    static constexpr std::initializer_list<name> name ## _E = { __VA_ARGS__ }
