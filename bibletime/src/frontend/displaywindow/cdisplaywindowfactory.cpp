//
// C++ Implementation: cdisplaywindowfactory
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cdisplaywindowfactory.h"

#include "creadwindow.h"
#include "cbiblereadwindow.h"
#include "ccommentaryreadwindow.h"
#include "clexiconreadwindow.h"
#include "cbookreadwindow.h"
#include "cwritewindow.h"
#include "cplainwritewindow.h"
#include "chtmlwritewindow.h"


#include "backend/drivers/cswordmoduleinfo.h"
#include "frontend/cmdiarea.h"


CReadWindow* CDisplayWindowFactory::createReadInstance(ListCSwordModuleInfo modules, CMDIArea* parent)
{
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


CWriteWindow* CDisplayWindowFactory::createWriteInstance(ListCSwordModuleInfo modules, CMDIArea* parent, const CDisplayWindow::WriteWindowType type)
{
	if (type == CDisplayWindow::HTMLWindow) {
		return new CHTMLWriteWindow(modules, parent);
	}
	else {
		return new CPlainWriteWindow(modules, parent);
	}
	return 0;
}