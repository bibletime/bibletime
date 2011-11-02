/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CPROFILEWINDOW_H
#define CPROFILEWINDOW_H

#include <QRect>
#include <QString>
#include <QStringList>
#include "backend/managers/cswordbackend.h"


class CSwordModuleInfo;

namespace Profile {

/** Contains the settings for one window saved in the profile.
  * @author The BibleTime team
  */
struct CProfileWindow {

    CProfileWindow(CSwordModuleInfo::ModuleType type = CSwordModuleInfo::Unknown);

    CSwordModuleInfo::ModuleType type;
    QRect windowGeometry;
    QStringList modules;
    QString key;
    int scrollbarPosH;
    int scrollbarPosV;
    bool maximized;
    bool hasFocus;
    int windowSettings;
    int writeWindowType;

};

} //end of namespace Profile

#endif
