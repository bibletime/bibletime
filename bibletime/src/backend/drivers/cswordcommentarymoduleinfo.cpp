/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/drivers/cswordcommentarymoduleinfo.h"


bool CSwordCommentaryModuleInfo::isWritable() const {
    /*
      A module is only writable if it's a RawFiles module with writable
      returning true.
    */
    return std::string(module()->getConfigEntry("ModDrv")) == "RawFiles"
           && module()->isWritable();
}
