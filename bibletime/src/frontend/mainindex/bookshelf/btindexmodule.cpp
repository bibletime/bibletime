//
// C++ Implementation: btindexmodule
//
// Description: 
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "btindexmodule.h"

#include "backend/btmoduletreeitem.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "util/directoryutil.h"

#include <QIcon>
#include <QString>

#include <QDebug>

#include <kaction.h>

BTIndexModule::BTIndexModule(BTModuleTreeItem* treeItem, QTreeWidgetItem* previous)
	: BTIndexItem(previous)
{
	//qDebug("BTIndexModule::BTIndexModule");
	setText(0, treeItem->text()); //set text
	setIcon(0, util::filesystem::DirectoryUtil::getIcon(treeItem->iconName()) );
	m_moduleInfo = treeItem->moduleInfo();
	setToolTip(0, CToolClass::moduleToolTip(moduleInfo()) );

	setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled);
}


BTIndexModule::~BTIndexModule()
{
}


const bool BTIndexModule::enableAction(KAction* action)
{
	if (!action->isEnabled()) {
		IndexAction actionType = (IndexAction)action->property("indexActionType").toInt();
		
		switch (actionType) {
		case EditModule:
			if (moduleInfo()->isWritable()) action->setEnabled(true);
			break;
		case UnlockModule:
			if (moduleInfo()->isEncrypted()) action->setEnabled(true);
			break;
		case HideModules:
			if (!m_moduleInfo->isHidden()) action->setEnabled(true);
		case UpdateModules:
			break;
		}
	}
}


CSwordModuleInfo* BTIndexModule::moduleInfo()
{
	return m_moduleInfo;
}