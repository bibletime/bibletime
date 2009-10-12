/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/mainindex/bookshelf/chidemodulechooserdialog.h"

#include <QButtonGroup>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSpacerItem>
#include <QStringList>
#include <QTreeWidget>
#include <QVBoxLayout>
#include "backend/btmoduletreeitem.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/directory.h"


CHideModuleChooserDialog::CHideModuleChooserDialog( QWidget* parent, QString title, QString label, QString currentModule)
        : CModuleChooserDialog(parent, title, label),
        m_currentModule(currentModule),
        m_focusItem(0) {
    QObject::connect(this, SIGNAL(modulesChanged(QList<CSwordModuleInfo*>, QTreeWidget*)), this, SLOT(applyHiddenModules(QList<CSwordModuleInfo*>)));
    init();
    if (m_focusItem) {
        treeWidget()->scrollToItem(m_focusItem);
    }
}


void CHideModuleChooserDialog::initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem) {
    widgetItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (btItem->moduleInfo()->isHidden())
        widgetItem->setCheckState(0, Qt::Checked);
    else
        widgetItem->setCheckState(0, Qt::Unchecked);
    if (m_currentModule == widgetItem->text(0)) {
        m_focusItem = widgetItem;
    }
}

void CHideModuleChooserDialog::applyHiddenModules(QList<CSwordModuleInfo*> hiddenModules) {
    qDebug("CHideModuleChooserDialog::applyHiddenModules");
    QList<CSwordModuleInfo*> allModules = CPointers::backend()->moduleList();
    foreach(CSwordModuleInfo* i, allModules) {
        if (hiddenModules.contains(i)) {
            i->setHidden(true);
        }
        else {
            i->setHidden(false);
        }

    }
}
