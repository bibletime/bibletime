/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CDISPLAYWINDOWFACTORY_H
#define CDISPLAYWINDOWFACTORY_H

#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cplainwritewindow.h"


class CReadWindow;
class CSwordModuleInfo;

/// \todo Make CDisplayWindowFactory a namespace instead?
class CDisplayWindowFactory {
    public:
        static CReadWindow* createReadInstance(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent);
        static CPlainWriteWindow* createWriteInstance(const QList<CSwordModuleInfo*> & modules, CMDIArea * parent, CPlainWriteWindow::WriteWindowType type = CPlainWriteWindow::HTMLWindow);
        static CSwordModuleInfo::ModuleType getModuleType(QObject* widget);

    private:
        CDisplayWindowFactory();
};

#endif
