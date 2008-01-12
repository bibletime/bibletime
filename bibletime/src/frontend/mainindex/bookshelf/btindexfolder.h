//
// C++ Interface: btindexfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BTINDEXFOLDER_H
#define BTINDEXFOLDER_H

#include "btindexitem.h"

#include <QString>

class BTModuleTreeItem;

/**
	@author The BibleTime team <info@bibletime.info>
*/
class BTIndexFolder : public BTIndexItem
{
public:
    BTIndexFolder(BTModuleTreeItem* treeItem, QTreeWidgetItem* p);

    ~BTIndexFolder() {}
};

#endif
