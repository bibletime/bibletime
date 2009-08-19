/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btindexpage.h"

#include "frontend/cmoduleindexdialog.h"
#include "backend/config/cbtconfig.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"
#include "util/cpointers.h"
#include "util/directoryutil.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"

//Qt includes
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCheckBox>
#include <QDir>
#include <QPushButton>


BtIndexPage::BtIndexPage()
	: BtConfigPage()
{

	QVBoxLayout *vboxLayout;
	QHBoxLayout *hboxLayout;
    vboxLayout = new QVBoxLayout(this);
 
	m_autoDeleteOrphanedIndicesBox = new QCheckBox(this);
	m_autoDeleteOrphanedIndicesBox->setToolTip(tr("If selected, those indexes which have no corresponding work will be deleted when BibleTime starts"));
	m_autoDeleteOrphanedIndicesBox->setText(tr("Automatically delete orphaned indexes when BibleTime starts"));
	vboxLayout->addWidget(m_autoDeleteOrphanedIndicesBox);

	m_moduleList = new QTreeWidget(this);
	vboxLayout->addWidget(m_moduleList);

	hboxLayout = new QHBoxLayout();

	QSpacerItem *spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	hboxLayout->addItem(spacerItem);

	m_deleteButton = new QPushButton(this);
	m_deleteButton->setToolTip(tr("Delete the selected indexes"));
	m_deleteButton->setText(tr("Delete"));
	hboxLayout->addWidget(m_deleteButton);

	m_createButton = new QPushButton(this);
	m_createButton->setToolTip(tr("Create new indexes for the selected works"));
	m_createButton->setText(tr("Create..."));
	hboxLayout->addWidget(m_createButton);

	vboxLayout->addLayout(hboxLayout);

	// configure the list view
	m_moduleList->setHeaderLabels( (QStringList(tr("Work")) << tr("Index size")) );
	m_moduleList->setRootIsDecorated(true);
	m_moduleList->setColumnWidth(0, CToolClass::mWidth(m_moduleList, 20) );
	//m_moduleList->setTextAlignment(1, Qt::AlignRight); see doc...
	m_moduleList->setSortingEnabled(false);

	m_autoDeleteOrphanedIndicesBox->setChecked( CBTConfig::get( CBTConfig::autoDeleteOrphanedIndices ) );

	// icons for our buttons
	m_createButton->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::bookshelfmgr::indexpage::create_icon));
	m_deleteButton->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::bookshelfmgr::indexpage::delete_icon));

	// connect our signals/slots
	connect(m_createButton, SIGNAL(clicked()), this, SLOT(createIndices()));
	connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteIndices()));
	connect(CPointers::backend(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)), SLOT(slotSwordSetupChanged()));

	populateModuleList();
}

BtIndexPage::~BtIndexPage()
{
	CBTConfig::set( CBTConfig::autoDeleteOrphanedIndices, m_autoDeleteOrphanedIndicesBox->isChecked() );

}
QString BtIndexPage::label()
{
	return tr("Create new search indexes and delete created indexes for the installed works.");
}
QString BtIndexPage::iconName()
{
	return CResMgr::bookshelfmgr::indexpage::icon;
}
QString BtIndexPage::header()
{
	return tr("Search Indexes");
}


/** Populates the module list with installed modules and orphaned indices */
void BtIndexPage::populateModuleList() {
	m_moduleList->clear();
		
	// populate installed modules
	m_modsWithIndices = new QTreeWidgetItem(m_moduleList);
	m_modsWithIndices->setText(0, tr("Works with indexes"));
	m_modsWithIndices->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_modsWithIndices->setExpanded(true);

	m_modsWithoutIndices = new QTreeWidgetItem(m_moduleList);
	m_modsWithoutIndices->setText(0, tr("Works without indexes"));
	m_modsWithoutIndices->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsTristate);
	m_modsWithoutIndices->setExpanded(true);



	QList<CSwordModuleInfo*>& modules = CPointers::backend()->moduleList();
	QList<CSwordModuleInfo*>::iterator end_it = modules.end();
	for (QList<CSwordModuleInfo*>::iterator it = modules.begin(); it != end_it; ++it) {
		QTreeWidgetItem* item = 0;
		
		if ((*it)->hasIndex()) {
			item = new QTreeWidgetItem(m_modsWithIndices);
			item->setText(0, (*it)->name());
			item->setText(1, QString("%1 ").arg((*it)->indexSize() / 1024) + tr("KiB"));
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			item->setCheckState(0, Qt::Unchecked);
		}
		else {
			item = new QTreeWidgetItem(m_modsWithoutIndices);
			item->setText(0, (*it)->name());
			item->setText(1, QString("0 ") + tr("KiB"));
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			item->setCheckState(0, Qt::Checked);
		}
	}
}

/** Creates indices for selected modules if no index currently exists */
void BtIndexPage::createIndices()
{
	bool indicesCreated = false;
	QList<CSwordModuleInfo*> moduleList;

	for (int i = 0; i < m_modsWithoutIndices->childCount(); i++) {
		if (m_modsWithoutIndices->child(i)->checkState(0) == Qt::Checked) {
			CSwordModuleInfo* module = CPointers::backend()->findModuleByName(m_modsWithoutIndices->child(i)->text(0).toUtf8());
			if (module) {
				moduleList.append( module );
				indicesCreated = true;
			}
		}
	}

	//Shows the progress dialog
	if (indicesCreated) {
		CModuleIndexDialog::getInstance()->indexAllModules( moduleList );
		populateModuleList();
	}
}

/** Deletes indices for selected modules */
void BtIndexPage::deleteIndices()
{
	bool indicesDeleted = false;
	
	for (int i = 0; i < m_modsWithIndices->childCount(); i++) {
		if (m_modsWithIndices->child(i)->checkState(0) == Qt::Checked) {
			CSwordModuleInfo* module = CPointers::backend()->findModuleByName(m_modsWithIndices->child(i)->text(0).toUtf8());
			if (module) {
				CSwordModuleInfo::deleteIndexForModule( module->name() );
				indicesDeleted = true;
			}
		}
	}

	// repopulate the list if an action was taken
	if (indicesDeleted) {
		populateModuleList();
	}
}

void BtIndexPage::deleteOrphanedIndices()
{
	QDir dir(CSwordModuleInfo::getGlobalBaseIndexLocation());
	dir.setFilter(QDir::Dirs);
	CSwordModuleInfo* module;
	
	for (unsigned int i = 0; i < dir.count(); i++) {
		if (dir[i] != "." && dir[i] != "..") {
			if ( (module = CPointers::backend()->findModuleByName(dir[i])) ) { //mod exists
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

void BtIndexPage::slotSwordSetupChanged()
{
	populateModuleList();
}

