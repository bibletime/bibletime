//
// C++ Implementation: btindexfolder
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "btindexfolder.h"
#include "backend/btmoduletreeitem.h"
#include "util/directoryutil.h"

BTIndexFolder::BTIndexFolder(BTModuleTreeItem* treeItem, QTreeWidgetItem* parent)
	: BTIndexItem(parent)
{
	setText(0, treeItem->text());
	setIcon(0, util::filesystem::DirectoryUtil::getIcon(treeItem->iconName()));
}
