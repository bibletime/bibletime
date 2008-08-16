/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINDEXMODULE_H
#define BTINDEXMODULE_H

#include "btindexitem.h"

#include "util/ctoolclass.h"

class BTModuleTreeItem;
class CSwordModuleInfo;

class QAction;
class QMimeData;


class BTIndexModule : public BTIndexItem
{
public:
	BTIndexModule(BTModuleTreeItem* treeItem, QTreeWidgetItem* previous);

	~BTIndexModule();

	virtual const bool enableAction(QAction* action);
	virtual bool acceptDrop(const QMimeData* data);
	CSwordModuleInfo* moduleInfo();

private:
	CSwordModuleInfo* m_moduleInfo;
};

#endif
