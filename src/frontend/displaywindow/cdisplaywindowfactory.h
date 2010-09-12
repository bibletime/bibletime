/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CDISPLAYWINDOWFACTORY_H
#define CDISPLAYWINDOWFACTORY_H

#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cwritewindow.h"


class CMDIArea;
class CReadWindow;
class CSwordModuleInfo;
class CWriteWindow;

/// \todo Make CDisplayWindowFactory a namespace instead?
class CDisplayWindowFactory {
    public:
        static CReadWindow* createReadInstance(QList<CSwordModuleInfo*> modules, CMDIArea* parent);
        static CWriteWindow* createWriteInstance(QList<CSwordModuleInfo*> modules, CMDIArea* parent, const CWriteWindow::WriteWindowType type = CWriteWindow::HTMLWindow);
        static const CSwordModuleInfo::ModuleType getModuleType(QObject* widget);

    private:
        CDisplayWindowFactory();
};

#endif
