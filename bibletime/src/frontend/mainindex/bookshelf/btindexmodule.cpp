/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btindexmodule.h"

#include "backend/btmoduletreeitem.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "util/directoryutil.h"
#include "frontend/cdragdrop.h"

#include <QIcon>
#include <QString>
#include <QDebug>
#include <QAction>



BTIndexModule::BTIndexModule(BTModuleTreeItem* treeItem, QTreeWidgetItem* previous)
        : BTIndexItem(previous)
{
    namespace DU = util::filesystem::directoryutil;

    //qDebug("BTIndexModule::BTIndexModule");
    setText(0, treeItem->text()); //set text
    setIcon(0, DU::getIcon(treeItem->iconName()));
    m_moduleInfo = treeItem->moduleInfo();
    setToolTip(0, CToolClass::moduleToolTip(moduleInfo()) );

    setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable);
    if (!m_moduleInfo->isHidden()) {
        setFlags(flags() | Qt::ItemIsEnabled);
    }
}


BTIndexModule::~BTIndexModule() {
}


bool BTIndexModule::enableAction(QAction* action) const {
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
            case AboutModule:
            case Grouping:
            case SearchModules:
            case ShowAllModules:
                break;
        }
    }
    return true; //TODO: check, is this correct?
}


bool BTIndexModule::acceptDrop(const QMimeData* data) {
    //TODO: check the module type of the reference, accept only proper type.
    // Bible accepts biblical references and plain text (for search),
    // but it could accept a lexicon entry too for search, especially a Strong's entry.
    // Lexicon could accept lexicon references but also biblical references to find all places where
    // the verse is referred to. Same with genbooks.

    const BTMimeData* btData = dynamic_cast<const BTMimeData*>(data);
    if (btData) {
        return true;
    }
    if (data->hasText()) {
        return true;
    }
    return false;
}


CSwordModuleInfo* BTIndexModule::moduleInfo() const {
    return m_moduleInfo;
}
