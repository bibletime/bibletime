/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btindexfolder.h"
#include "backend/btmoduletreeitem.h"
#include "util/directoryutil.h"

BTIndexFolder::BTIndexFolder(BTModuleTreeItem* treeItem, QTreeWidgetItem* parent)
        : BTIndexItem(parent) {
    setText(0, treeItem->text());
    setIcon(0, util::filesystem::DirectoryUtil::getIcon(treeItem->iconName()));
}
