//
// C++ Interface: btindexmodule
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BTINDEXMODULE_H
#define BTINDEXMODULE_H

#include "btindexitem.h"

#include "util/ctoolclass.h"

class BTModuleTreeItem;
class CSwordModuleInfo;



class BTIndexModule : public BTIndexItem
{
public:
    BTIndexModule(BTModuleTreeItem* treeItem, QTreeWidgetItem* previous);

    ~BTIndexModule();

    virtual const bool enableAction(KAction* action);
    CSwordModuleInfo* moduleInfo();

private:
	CSwordModuleInfo* m_moduleInfo;
};

#endif
