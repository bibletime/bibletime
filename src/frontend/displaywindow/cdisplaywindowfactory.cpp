/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
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
#include "frontend/cmdiarea.h"


CReadWindow* CDisplayWindowFactory::createReadInstance(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent) {
    CReadWindow* win = 0;
    switch (modules.first()->type()) {
        case CSwordModuleInfo::Bible:
            win = new CBibleReadWindow(modules, parent);
        break;
        case CSwordModuleInfo::Commentary:
            win = new CCommentaryReadWindow(modules, parent);
        break;
        case CSwordModuleInfo::Lexicon:
            win = new CLexiconReadWindow(modules, parent);
        break;
        case CSwordModuleInfo::GenericBook:
            win = new CBookReadWindow(modules, parent);
        break;
        default:
            qWarning("unknown module type");
            break;
    }
    return win;
}

CPlainWriteWindow * CDisplayWindowFactory::createWriteInstance(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent, CPlainWriteWindow::WriteWindowType type) {
    if (type == CPlainWriteWindow::HTMLWindow) {
        return new CHTMLWriteWindow(modules, parent);
    }
    else {
        return new CPlainWriteWindow(modules, parent);
    }
    return 0;
}

CSwordModuleInfo::ModuleType CDisplayWindowFactory::getModuleType(QObject* widget) {
    if (qobject_cast<CBibleReadWindow*>(widget) != 0 )
        return CSwordModuleInfo::Bible;
    if (qobject_cast<CCommentaryReadWindow*>(widget) != 0 )
        return CSwordModuleInfo::Commentary;
    if (qobject_cast<CBookReadWindow*>(widget) != 0 )
        return CSwordModuleInfo::GenericBook;
    if (qobject_cast<CLexiconReadWindow*>(widget) != 0 )
        return CSwordModuleInfo::Lexicon;
    return CSwordModuleInfo::Unknown;
}
