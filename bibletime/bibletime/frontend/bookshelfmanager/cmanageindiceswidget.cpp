/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmanageindiceswidget.h"

#include "frontend/cmoduleindexdialog.h"
#include "frontend/cbtconfig.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"
#include "util/cpointers.h"

#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"

//Qt includes
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QDir>
#include <QPushButton>
#include <QLabel>

//KDE includes
#include <klocale.h>
#include <kiconloader.h>

namespace BookshelfManager {


//TODO: The qt4 tree view probably doesn't work out of the box. Needs attention.

/** Constructor */
CManageIndicesWidget::CManageIndicesWidget(QWidget* parent) 
	: QWidget(parent)
{
	setupUi(this);
	initView();
	populateModuleList();
};

CManageIndicesWidget::~CManageIndicesWidget()
{
	CBTConfig::set( CBTConfig::autoDeleteOrphanedIndices, m_autoDeleteOrphanedIndicesBox->isChecked() );

}

/** Initializes the look and feel of this page */
void CManageIndicesWidget::initView()
{
	// Set description label
	QVBoxLayout* box = new QVBoxLayout(m_labelFrame);
	QLabel* mainLabel = CToolClass::explanationLabel(m_labelFrame,
		i18n("Manage module search indices"),
		i18n("You can use the list below to create and/or delete search indices for your installed works."));
	box->addWidget(mainLabel);

	// configure the list view
	m_moduleList->setHeaderLabels( (QStringList(i18n("Module")) << i18n("Index size")) );
	//m_moduleList->addColumn(i18n("Module"));
	//m_moduleList->addColumn(i18n("Index size"));
	m_moduleList->setRootIsDecorated(true);
	m_moduleList->setColumnWidth(0, 150);
	//m_moduleList->setColumnAlignment(1, Qt::AlignRight); //didn't find this from qt4
	m_moduleList->setSortingEnabled(false);

	m_autoDeleteOrphanedIndicesBox->setChecked( CBTConfig::get( CBTConfig::autoDeleteOrphanedIndices ) );

	// icons for our buttons
	m_createIndicesButton->setIcon(SmallIcon("folder_new", 16));
	m_deleteIndicesButton->setIcon(SmallIcon("remove", 16));

	// connect our signals/slots
	connect(m_createIndicesButton, SIGNAL(clicked()), this, SLOT(createIndices()));
	connect(m_deleteIndicesButton, SIGNAL(clicked()), this, SLOT(deleteIndices()));
}

/** Populates the module list with installed modules and orphaned indices */
void CManageIndicesWidget::populateModuleList() {
	m_moduleList->clear();
		
	// populate installed modules
	m_modsWithIndices = new QTreeWidgetItem(m_moduleList);
	m_modsWithIndices->setText(0, i18n("Modules with indices"));
	m_modsWithIndices->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_modsWithIndices->setExpanded(true);

	//m_modsWithoutIndices = new Q3CheckListItem(m_moduleList, i18n("Modules without indices"),
	//	Q3CheckListItem::CheckBoxController);
	//m_modsWithoutIndices->setOpen(true);
	m_modsWithoutIndices = new QTreeWidgetItem(m_moduleList);
	m_modsWithoutIndices->setText(0, i18n("Modules without indices"));
	m_modsWithoutIndices->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_modsWithoutIndices->setExpanded(true);



	ListCSwordModuleInfo& modules = CPointers::backend()->moduleList();
	ListCSwordModuleInfo::iterator end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it = modules.begin(); it != end_it; ++it) {
		QTreeWidgetItem* item = 0;
		
		if ((*it)->hasIndex()) {
			item = new QTreeWidgetItem(m_modsWithIndices);
			item->setText(0, (*it)->name());
			item->setText(1, QString("%1 ").arg((*it)->indexSize() / 1024) + i18n("KiB"));
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			item->setCheckState(0, Qt::Unchecked);
		}
		else {
			item = new QTreeWidgetItem(m_modsWithoutIndices);
			item->setText(0, (*it)->name());
			item->setText(1, QString("0 ") + i18n("KiB"));
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			item->setCheckState(0, Qt::Checked);
		}
	}
}

/** Creates indices for selected modules if no index currently exists */
void CManageIndicesWidget::createIndices()
{
	QTreeWidgetItem* top = m_modsWithoutIndices;
	bool indicesCreated = false;
	ListCSwordModuleInfo moduleList;

	int childCount = top->childCount();
	if (childCount > 0) {
		QTreeWidgetItem* item = top->child(0);
		for (int i = 1; top && i < childCount; i++) {
		//while (item) {
			if (item->checkState(0) == Qt::Checked) {
				CSwordModuleInfo* module =
					CPointers::backend()->findModuleByName(item->text(0).toUtf8());

			
				if (module) {
					moduleList.append( module );
					indicesCreated = true;
				}
			}
			item = top->child(i);
		}
	}

	//Shows the progress dialog
	if (indicesCreated) {
		CModuleIndexDialog::getInstance()->indexAllModules( moduleList );
		populateModuleList();
	}
}

/** Deletes indices for selected modules and selected orphans */
void CManageIndicesWidget::deleteIndices()
{
	// delete installed module indices
	QTreeWidgetItem* top = m_modsWithIndices;
	bool indicesDeleted = false;
	//QTreeWidgetItem* item = (QTreeWidgetItem*)top->firstChild();
	//while (item) {
	int childCount = top->childCount();
	if (childCount > 0) {
		QTreeWidgetItem* item = top->child(0);
		for (int i = 1; top && i < childCount; i++) {
			if (item->checkState(0) == Qt::Checked) {
				CSwordModuleInfo* module =
					CPointers::backend()->findModuleByName(item->text(0).toUtf8());
				if (module) {
					CSwordModuleInfo::deleteIndexForModule( module->name() );
					indicesDeleted = true;
				}
			}
			item = top->child(i);
		}
	}

	// repopulate the list if an action was taken
	if (indicesDeleted) {
		populateModuleList();
	}
}

void CManageIndicesWidget::deleteOrphanedIndices()
{
	QDir dir(CSwordModuleInfo::getGlobalBaseIndexLocation());
	dir.setFilter(QDir::Dirs);
	CSwordModuleInfo* module;
	
	for (unsigned int i = 0; i < dir.count(); i++) {
		if (dir[i] != "." && dir[i] != "..") {
			if (module = CPointers::backend()->findModuleByName( dir[i] ) ) { //mod exists
				if (!module->hasIndex()){ //index files found, but wrong version etc.
					CSwordModuleInfo::deleteIndexForModule( dir[i] );
				}
			}
			else{ //no module exists
				if (CBTConfig::get( CBTConfig::autoDeleteOrphanedIndices ) ){
					CSwordModuleInfo::deleteIndexForModule( dir[i] );
				}
			}
		}
	}
}


}
