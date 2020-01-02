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

#include "cswordcommentarymoduleinfo.h"


bool CSwordCommentaryModuleInfo::isWritable() const {
    /*
      A module is only writable if it's a RawFiles module with writable
      returning true.
    */
    return !qstrcmp(module().getConfigEntry("ModDrv"), "RawFiles")
           && module().isWritable();
}
