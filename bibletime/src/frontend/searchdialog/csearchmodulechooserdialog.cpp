/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "csearchmodulechooserdialog.h"
#include "csearchmodulechooserdialog.moc"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "backend/btmoduletreeitem.h"

#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/directoryutil.h"

#include <QDialog>
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QStringList>
#include <QDebug>
#include <QHeaderView>

#include <klocale.h>

namespace Search {

CSearchModuleChooserDialog::CSearchModuleChooserDialog( QWidget* parent, QString title, QString label,
														ListCSwordModuleInfo selectedModules)
	: CModuleChooserDialog(parent, title, label),
	m_selectedModules(selectedModules)
{
	m_hiddenFilter = new BTModuleTreeItem::HiddenOff();
	QList<BTModuleTreeItem::Filter*> filters;
	filters.append(m_hiddenFilter);
	setFilters(filters);
	init();
};

CSearchModuleChooserDialog::~CSearchModuleChooserDialog()
{
	//see the ctor
	delete m_hiddenFilter;
}

void CSearchModuleChooserDialog::initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem)
{
	widgetItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	if (m_selectedModules.contains(btItem->moduleInfo()))
		widgetItem->setCheckState(0, Qt::Checked);
	else
		widgetItem->setCheckState(0, Qt::Unchecked);
}

} //end of namespace Search
