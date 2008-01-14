/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#include "btinstallpage.h"
#include "btinstallpage.moc"

#include "frontend/bookshelfmanager/btinstallmgr.h"
#include "frontend/bookshelfmanager/new/btconfigpage.h"
#include "frontend/cmodulechooserdialog.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"
#include "util/cpointers.h"

#include <boost/scoped_ptr.hpp>


#include <QAction>
#include <QApplication>
#include <QWidget>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTabBar>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

using namespace BookshelfManager;

// *********************************************************
// *********** Config dialog page: Install/Update **********
// *********************************************************

BtInstallPage::BtInstallPage()
	: BtConfigPage()
{
	initView();
	initConnections();
}

void BtInstallPage::setInstallEnabled(bool b)
{
	m_installButton->setEnabled(b);
}

void BtInstallPage::initView()
{
	QVBoxLayout *mainLayout;
	
	// installation path chooser
	QHBoxLayout *pathLayout;
	QSpacerItem *pathSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_pathLabel = new QLabel(tr("Install Path:"), this);
	m_pathCombo = new QComboBox(this);
	initPathCombo(); // set the paths and the current path
	m_configurePathButton = new QPushButton(tr("Configure..."), this);
	m_configurePathButton->setEnabled(false); //TODO after writing the widget
	
	pathLayout->addItem(pathSpacer);
	pathLayout->addWidget(m_pathCombo);
	pathLayout->addWidget(m_configurePathButton);
	mainLayout->addLayout(pathLayout);
	
	// Source widget
	//QTabWidget* m_sourcesTabWidget;
	BtSourceWidget* m_sourceWidget = new BtSourceWidget(this);
	mainLayout->addWidget(m_sourceWidget);

	// Install button
	QHBoxLayout *installButtonLayout = new QHBoxLayout();
	QSpacerItem *installButtonSpacer = new QSpacerItem(371, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	installButtonLayout->addItem(installButtonSpacer);
	m_installButton = new QPushButton(tr("Install and Update"), this);
	m_installButton->setEnabled(false);
	installButtonLayout->addWidget(m_installButton);
	
	mainLayout->addLayout(installButtonLayout);
	
	// reset the install button text, it was set to the longer
	// one to make it wide enough
	m_installButton->setText(tr("Install"));
}

void BtInstallPage::initConnections()
{
	QObject::connect(m_pathCombo, SIGNAL(activated(const QString&)), this , SLOT(slotPathChanged()));
	QObject::connect(m_configurePathButton, SIGNAL(clicked()), this, SLOT(slotEditPaths()));
	QObject::connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstall()));
	
	//source widget has its own connections, not here
}

void BtInstallPage::initPathCombo()
{
	//populate the combo list
	m_pathCombo->clear();
	
	QStringList targets = BTInstallMgr::Tool::LocalConfig::targetList();
	for (QStringList::iterator it = targets.begin(); it != targets.end(); ++it)  {
		if ((*it).isEmpty()) continue;
		m_pathCombo->addItem(*it);
	}
	
	// TODO:choose the current value from config
}

void BtInstallPage::slotPathChanged(const QString& pathText)
{
	// TODO: save to config
}

void BtInstallPage::slotEditPaths()
{
	// Now: do nothing, editing is done in another page
	// (we have to catch the signal sent from there to refresh the combo)
	return;
 
	// Later: open the dialog
	
	// if the dialog was accepted, set the paths and save them
	// and repopulate the combo
}

void BtInstallPage::slotInstall()
{
	//TODO: what to do if there are identical module names?
	
	// create the confirmation dialog
	// (module tree dialog, modules taken from all sources)
	QString dlgTitle(tr("Install/Update modules?"));
	QString dlgLabel(tr("Do you really want to install these modules?"));
	
	ListCSwordModuleInfo moduleList;
	
//	CInstallModuleChooserDialog* dlg = new CInstallModuleChooserDialog(m_sourceWidget->m_tabBar, m_sourceWidget->m_viewStack, dlgTitle, dlgLabel, &moduleList);
	//dlg->setGrouping(BTModuleTreeItem::Mod);
	
}


// implement the BtConfigPage methods

QString BtInstallPage::iconName()
{
	return "install_modules.svg";
}
QString BtInstallPage::label()
{
	// TODO: move the warning to a dialog which is shown when adding a source.
	return tr("Install and update modules. Add remote or local sources, refresh them, select the modules to be installed/updated and click Install. <b>WARNING: If you live in a persecuted country and don't want to risk detection, don't use remote sources.</b>");
}
QString BtInstallPage::header()
{
	return tr("Install/Update");
}


// ****************************************************************
// ******** Installation source and module list widget ************
// ****************************************************************

BtSourceWidget::BtSourceWidget(BtInstallPage* parent)
	: QWidget(parent),
	m_page(parent)
{
	initView();
	initConnections();
	initSources();
	
	//choose the page from config (tabbar then emits "changed" signal)
	
}

// init the view except for the current source/module tree
void BtSourceWidget::initView()
{

	QVBoxLayout *tabLayout;
	
	// TODO: in what layout the tabbar should be, and how?
	m_tabBar = new QTabBar(this);
	tabLayout->addWidget(m_tabBar);

	// There are no views for the stack yet, see initSources
	m_viewStack = new QStackedWidget(this);	
	tabLayout->addWidget(m_viewStack);
	
	QHBoxLayout *refreshLabelLayout;
	QLabel *refreshLabel = new QLabel(tr("Last refreshed:"), this);
	m_refreshTimeLabel = new QLabel(this);
	QSpacerItem *refreshLabelSpacer = new QSpacerItem(201, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	
	refreshLabelLayout->addWidget(refreshLabel);
	refreshLabelLayout->addWidget(m_refreshTimeLabel);
	refreshLabelLayout->addItem(refreshLabelSpacer);
	tabLayout->addLayout(refreshLabelLayout);
	
	// source related button row
	QHBoxLayout *sourceLayout = new QHBoxLayout(this);
	m_refreshButton = new QPushButton(tr("Refresh"), this);
	QSpacerItem *sourceSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_editButton = new QPushButton(tr("Edit..."), this);
	m_editButton->setEnabled(false); // TODO after writing the edit widget
	m_deleteButton = new QPushButton(tr("Delete"), this);
	m_addButton = new QPushButton(tr("Add..."), this);
	
	sourceLayout->addWidget(m_refreshButton);
	sourceLayout->addItem(sourceSpacer);
	sourceLayout->addWidget(m_editButton);
	sourceLayout->addWidget(m_deleteButton);
	sourceLayout->addWidget(m_addButton);
	
	tabLayout->addLayout(sourceLayout);
}

void BtSourceWidget::initConnections()
{
	connect(m_refreshButton, SIGNAL(clicked()), SLOT(slotRefresh()));
	connect(m_editButton, SIGNAL(clicked()), SLOT(slotEdit()));
	connect(m_deleteButton, SIGNAL(clicked()), SLOT(slotDelete()));
	connect(m_addButton, SIGNAL(clicked()), SLOT(slotAdd()));
	connect(m_tabBar, SIGNAL(currentChanged(int)), SLOT(slotSourceSelected(int)) );
}

void BtSourceWidget::slotEdit()
{
	// open the source editor dialog
	
	// if the source was changed, init the sources
	
}

void BtSourceWidget::slotDelete()
{
	// ask for confirmation
	
	// remove from backend
	
	// remove tab and view
	int index = m_tabBar->currentIndex();
	m_tabBar->removeTab(index);
	QTreeWidget* view = dynamic_cast<QTreeWidget*>(m_viewStack->widget(index));
	m_viewStack->removeWidget(view);
	delete view;
}

void BtSourceWidget::slotAdd()
{
	// open dialog
	// don't destroy when closing
	
	// if accepted, add source to backend...
	
	// add source to this widget
	//m_tabBar->addTab(sourceDialog->name());
	QTreeWidget* view = new QTreeWidget;
	m_viewStack->addWidget(view);

	//destroy the dialog
	
	// when the item selection is changed the install button
	// has to be updated
	connect(view, SIGNAL(itemSelectionChanged()), SLOT(slotModuleSelectionChanged()));
}


// when a source (tab) has been chosen
void BtSourceWidget::slotSourceSelected(int index)
{
	//m_sourceName = m_tabBar->tabText(index);
	m_viewStack->setCurrentIndex(index);
	// TODO later: update the refresh time label
	
}


void BtSourceWidget::slotRefresh()
{
	// (re)build the module cache for the source
	// BACKEND
	
	// rebuild the view tree and refresh the view
	int index = m_tabBar->currentIndex();
	QTreeWidget* view = dynamic_cast<QTreeWidget*>(m_viewStack->widget(index));
	createModuleTree(view->invisibleRootItem(), m_tabBar->tabText(index));
}


// init the tabbar, setup the module tree for the current source
void BtSourceWidget::initSources()
{
	// create the list of source names
	QStringList sourceList;
	
	foreach (QString s, sourceList) {
		addSource(s);
	}
	
	// select the current source from the config
}

void BtSourceWidget::addSource(const QString& sourceName)
{
	// add the tab
	m_tabBar->addTab(sourceName);
	// TODO: add "remote/local", path etc.
	QString toolTip(sourceName + QString("<br>"));
	//setTabToolTip(index, );
	
	//create the treewidget and add it to stack
	QTreeWidget* view = new QTreeWidget;
	m_viewStack->addWidget(view);
	
	//populate the treewidget with the module list
	createModuleTree(view->invisibleRootItem(), sourceName);
}

// doesn't handle widgets/ui but only the tree items and backend
bool BtSourceWidget::createModuleTree(QTreeWidgetItem* rootItem, const QString& sourceName)
{
	//if the tree already exists for this source,
	// the selections should be preserved
	
	BTInstallMgr iMgr;
	sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, sourceName);

	if (BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)
			//&& !refreshRemoteModuleCache(sourceName)
	){
		return false;
	}

	boost::scoped_ptr<CSwordBackend> remote_backend( BTInstallMgr::Tool::backend(&is) );
	if (!remote_backend) {
		return false;
	}
	CSwordBackend* local_backend = CPointers::backend();
	
	// create the module list from the (cached) source data
	ListCSwordModuleInfo moduleList = remote_backend->moduleList();
	
	//use itemtree and filters
	
	//return the root item
}

void BtSourceWidget::updateList(const QString& sourceName)
{

	//createModuleTree();
}

void BtSourceWidget::slotModuleSelectionChanged()
{
	//bool install = false;
	//bool update = false;
	
	QList<QTreeWidgetItem*> selectedItems;
	
	for (int i = 0; i < m_tabBar->count(); i++) {
		QTreeWidget* view = dynamic_cast<QTreeWidget*>(m_viewStack->widget(i));
		selectedItems += view->selectedItems();
	}
	
	if (selectedItems.isEmpty()) {
		m_page->setInstallEnabled(false);
	} else {
		m_page->setInstallEnabled(true);
	}
}

// ***************************************************************
// ************* Dialog for confirming the install ***************
// ***************************************************************

CInstallModuleChooserDialog::CInstallModuleChooserDialog(QTabBar* tabBar, QStackedWidget* viewStack, QWidget* parent, QString title, QString label, ListCSwordModuleInfo* moduleInfo)
	: CModuleChooserDialog(parent, title, label, moduleInfo),
	m_tabBar(tabBar),
	m_viewStack(viewStack)
{}

void CInstallModuleChooserDialog::initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem)
{
	// TODO: double entries?
	
	QString name = btItem->text();
	QString source;
	for(int i = 0; i < m_tabBar->count(); i++) {
		// try to find the item in the source view
		QTreeWidget* view = dynamic_cast<QTreeWidget*>(m_viewStack->widget(i));
		QList<QTreeWidgetItem*> matching = view->findItems(m_tabBar->tabText(i), Qt::MatchExactly);
		if (!matching.isEmpty()) {
			source = m_tabBar->tabText(i);
			break;
		}
	}
	
	QString text = name + " (" + source + ")";
	widgetItem->setText(0, source);
	widgetItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	widgetItem->setCheckState(0, Qt::Checked);
}
