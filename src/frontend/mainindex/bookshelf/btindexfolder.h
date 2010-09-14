/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINDEXFOLDER_H
#define BTINDEXFOLDER_H

#include "btindexitem.h"

#include <QString>

class BTModuleTreeItem;

/**
	@author The BibleTime team <info@bibletime.info>
*/
class BTIndexFolder : public BTIndexItem {
    public:
        BTIndexFolder(BTModuleTreeItem* treeItem, QTreeWidgetItem* p);

        ~BTIndexFolder() {}
};

#endif
