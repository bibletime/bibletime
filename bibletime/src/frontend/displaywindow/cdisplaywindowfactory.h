//
// C++ Interface: cdisplaywindowfactory
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CDISPLAYWINDOWFACTORY_H
#define CDISPLAYWINDOWFACTORY_H

#include "cdisplaywindow.h"

#include "backend/drivers/cswordmoduleinfo.h"


class CReadWindow;
class CWriteWindow;
class CMDIArea;

class CDisplayWindowFactory
{
public:
	static CReadWindow* createReadInstance(ListCSwordModuleInfo modules, CMDIArea* parent);
	static CWriteWindow* createWriteInstance(ListCSwordModuleInfo modules, CMDIArea* parent, const CDisplayWindow::WriteWindowType type = CDisplayWindow::HTMLWindow);

private:
	CDisplayWindowFactory();
};

#endif