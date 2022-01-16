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

#include "cswordcommentarymoduleinfo.h"

#include <string_view>

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swmodule.h>
#pragma GCC diagnostic pop


bool CSwordCommentaryModuleInfo::isWritable() const {
    /*
      A module is only writable if it's a RawFiles module with writable
      returning true.
    */
    using namespace std::literals;
    return swordModule().getConfigEntry("ModDrv") == "RawFiles"sv
           && swordModule().isWritable();
}
