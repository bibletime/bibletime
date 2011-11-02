/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/profile/cprofilewindow.h"


namespace Profile {

CProfileWindow::CProfileWindow(CSwordModuleInfo::ModuleType t)
    : type(t)
    , windowGeometry()
    , modules()
    , key(QString::null)
    , scrollbarPosH(0)
    , scrollbarPosV(0)
    , maximized(false)
    , hasFocus(false)
    , windowSettings(0)
    , writeWindowType(0)
{
    // Intentionally empty
}

} //end of namespace Profile
