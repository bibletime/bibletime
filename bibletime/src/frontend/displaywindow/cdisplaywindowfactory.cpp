/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/cdisplaywindowfactory.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "frontend/displaywindow/cbiblereadwindow.h"
#include "frontend/displaywindow/cbookreadwindow.h"
#include "frontend/displaywindow/ccommentaryreadwindow.h"
#include "frontend/displaywindow/chtmlwritewindow.h"
#include "frontend/displaywindow/clexiconreadwindow.h"
#include "frontend/displaywindow/cplainwritewindow.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/displaywindow/cwritewindow.h"
#include "frontend/cmdiarea.h"


CReadWindow* CDisplayWindowFactory::createReadInstance(QList<CSwordModuleInfo*> modules, CMDIArea* parent) {
    qDebug("CDisplayWindowFactory::createReadInstance");
    switch (modules.first()->type()) {
        case CSwordModuleInfo::Bible:
            return new CBibleReadWindow(modules, parent);
        case CSwordModuleInfo::Commentary:
            return new CCommentaryReadWindow(modules, parent);
        case CSwordModuleInfo::Lexicon:
            return new CLexiconReadWindow(modules, parent);
        case CSwordModuleInfo::GenericBook:
            return new CBookReadWindow(modules, parent);
        default:
            qWarning("unknown module type");
            break;
    }
    return 0;
}

CWriteWindow* CDisplayWindowFactory::createWriteInstance(QList<CSwordModuleInfo*> modules, CMDIArea* parent, const CDisplayWindow::WriteWindowType type) {
    if (type == CDisplayWindow::HTMLWindow) {
        return new CHTMLWriteWindow(modules, parent);
    }
    else {
        return new CPlainWriteWindow(modules, parent);
    }
    return 0;
}
