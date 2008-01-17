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
#include "frontend/bookshelfmanager/new/btconfigdialog.h"
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
#include <QFileInfo>
#include <QFrame>
#include <QRect>
#include <QStyle>
#include <QStyleOptionTabWidgetFrame>

using namespace BookshelfManager;

// *********************************************************
// *********** Config dialog page: Install/Update **********
// *********************************************************

BtInstallPage::BtInstallPage()
	: BtConfigPage()
{
	qDebug("BtInstallPage::BtInstallPage() start");
	initView();
	initConnections();
}

void BtInstallPage::setInstallEnabled(bool b)
{
	qDebug("void BtInstallPage::setInstallEnabled(bool b) start");
	m_installButton->setEnabled(b);
}

void BtInstallPage::initView()
{
	setContentsMargins(0,5,0,5);
	qDebug("void BtInstallPage::initView() start");
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0,5,0,5);
	// installation path chooser
	QHBoxLayout* pathLayout = new QHBoxLayout();
	pathLayout->setContentsMargins(0,5,0,5);
	QSpacerItem *pathSpacer= new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QLabel* pathLabel = new QLabel(tr("Install Path:"));
	m_pathCombo = new QComboBox();
	initPathCombo(); // set the paths and the current path
	m_configurePathButton = new QPushButton(tr("Configure..."));
	m_configurePathButton->setEnabled(false); //TODO after writing the widget
	
	pathLayout->addItem(pathSpacer);
	pathLayout->addWidget(pathLabel);
	pathLayout->addWidget(m_pathCombo);
	pathLayout->addWidget(m_configurePathButton);
	mainLayout->addLayout(pathLayout);
	
	// Source widget
	//QTabWidget* m_sourcesTabWidget;
	BtSourceWidget* m_sourceWidget = new BtSourceWidget(this);
	mainLayout->addWidget(m_sourceWidget);

	// Install button
	QHBoxLayout *installButtonLayout = new QHBoxLayout();
	installButtonLayout->setContentsMargins(0,5,0,5);
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
	qDebug("void BtInstallPage::initConnections() start");
	QObject::connect(m_pathCombo, SIGNAL(activated(const QString&)), this , SLOT(slotPathChanged()));
	QObject::connect(m_configurePathButton, SIGNAL(clicked()), this, SLOT(slotEditPaths()));
	QObject::connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstall()));
	
	//source widget has its own connections, not here
}

void BtInstallPage::initPathCombo()
{
	qDebug("void BtInstallPage::initPathCombo() start");
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
	qDebug("void BtInstallPage::slotEditPaths() start");
	// Now: do nothing, editing is done in another page
	// (we have to catch the signal sent from there to refresh the combo)
	return;
 
	// Later: open the dialog
	
	// if the dialog was accepted, set the paths and save them
	// and repopulate the combo
}

void BtInstallPage::slotInstall()
{
	qDebug("void BtInstallPage::slotInstall() start");
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
	return tr("Install and update works. Add remote or local sources, refresh them, select the works to be installed/updated and click Install. <b>WARNING:</b> If you live in a persecuted country and don't want to risk detection don't use remote sources.");
}
QString BtInstallPage::header()
{
	return tr("Install/Update");
}



// ****************************************************************
// ******** Installation source and module list widget ************
// ****************************************************************

BtSourceArea::BtSourceArea(const QString& sourceName)
	: QWidget(),
	m_sourceName(sourceName)
{
	qDebug() << "BtSourceArea::BtSourceArea, " << m_sourceName;
	initView();
}

BtSourceArea::~BtSourceArea()
{}

void BtSourceArea::initView()
{
	qDebug("BtSourceArea::initView");
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	// There are no views for the stack yet, see initSources
	m_view = new QTreeWidget(this);
	mainLayout->addWidget(m_view);

	qDebug("void BtSourceWidget::createTabWidget() refresh label");
	QHBoxLayout *refreshLabelLayout = new QHBoxLayout();
	QLabel *refreshLabel = new QLabel(tr("Last refreshed:"));
	m_refreshTimeLabel = new QLabel();
	QSpacerItem *refreshLabelSpacer = new QSpacerItem(201, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	refreshLabelLayout->addWidget(refreshLabel);
	refreshLabelLayout->addWidget(m_refreshTimeLabel);
	refreshLabelLayout->addItem(refreshLabelSpacer);
	mainLayout->addLayout(refreshLabelLayout);

	// source related button row
	qDebug("void BtSourceWidget::createTabWidget() source buttons");
	QHBoxLayout *sourceLayout = new QHBoxLayout();
	m_refreshButton = new QPushButton(tr("Refresh"));
	m_refreshButton->setEnabled(false);
	QSpacerItem *sourceSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_editButton = new QPushButton(tr("Edit..."));
	m_editButton->setEnabled(false); // TODO after writing the edit widget
	m_deleteButton = new QPushButton(tr("Delete"));
	m_deleteButton->setEnabled(false);
	m_addButton = new QPushButton(tr("Add..."));

	sourceLayout->addWidget(m_refreshButton);
	sourceLayout->addItem(sourceSpacer);
	sourceLayout->addWidget(m_editButton);
	sourceLayout->addWidget(m_deleteButton);
	sourceLayout->addWidget(m_addButton);

	mainLayout->addLayout(sourceLayout);

	//populate the treewidget with the module list
	createModuleTree();
}

// doesn't handle widgets/ui but only the tree items and backend
bool BtSourceArea::createModuleTree()
{
	qDebug("BtSourceArea::createModuleTree start");
	//if the tree already exists for this source,
	// the selections should be preserved
	
	BTInstallMgr iMgr;
	sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, m_sourceName);

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
	
	// create the BTModuleTreeItem from the moduleList

	// use the treeItem and view root item to populate the tree
}

void BtSourceArea::createTreeItem()
{

}



// ****************************************************************
// ******** Tab Widget that holds source widgets ******************
// ****************************************************************

BtSourceWidget::BtSourceWidget(BtInstallPage* parent)
	: QTabWidget(parent),
	m_page(parent)
{
	qDebug("BtSourceWidget::BtSourceWidget start");
	initSources();
	
	//choose the page from config
	
}

BtSourceArea* BtSourceWidget::area()
{
	return dynamic_cast<BtSourceArea*>(currentWidget());
}

void BtSourceWidget::initSourceConnections()
{
	qDebug("void BtSourceWidget::initSourceConnections() start");
	if (area()) {
		connect(area()->m_refreshButton, SIGNAL(clicked()), SLOT(slotRefresh()));
		connect(area()->m_editButton, SIGNAL(clicked()), SLOT(slotEdit()));
		connect(area()->m_deleteButton, SIGNAL(clicked()), SLOT(slotDelete()));
		connect(area()->m_addButton, SIGNAL(clicked()), SLOT(slotAdd()));
		connect(area()->m_view, SIGNAL(itemSelectionChanged()), SLOT(slotModuleSelectionChanged()));
	}
	qDebug("void BtSourceWidget::initSourceConnections() end");
}

void BtSourceWidget::slotEdit()
{
	// open the source editor dialog
	
	// if the source was changed, init the sources
	
}

void BtSourceWidget::slotDelete()
{
	qDebug("void BtSourceWidget::slotDelete() start");
	// ask for confirmation
	
	// remove from backend
	
	// remove tab and view
	qDebug("doesn't really remove the source from the backend yet!");
	QWidget* w = currentWidget();
	removeTab(currentIndex());
	delete w;
}

void BtSourceWidget::slotAdd()
{
	qDebug("void BtSourceWidget::slotAdd() start");
	// open dialog
	// don't destroy when closing
	
	// if accepted, add source to backend...
	
	// add source to this widget
	// addSource(sourceName);
	//destroy the dialog
	
	// when the item selection is changed
	// the install button has to be updated
}


void BtSourceWidget::slotRefresh()
{
	qDebug("void BtSourceWidget::slotRefresh() start");
	// (re)build the module cache for the source
	// BACKEND
	
	// rebuild the view tree and refresh the view
	area()->createModuleTree();
}


// init the tabbar, setup the module tree for the current source
void BtSourceWidget::initSources()
{
	qDebug("void BtSourceWidget::initSources() start");
	// create the list of source names
	//QStringList sourceList;

	// ***** Use the backend to get the list of sources *****
	BTInstallMgr::Tool::RemoteConfig::initConfig();
	BTInstallMgr mgr;
	QStringList sourceList = BTInstallMgr::Tool::RemoteConfig::sourceList(&mgr);

	// Add a default entry, the Crosswire main repository
	// TODO: this is easy for the user, but should the edit dialog
	// open automatically?
	if (!sourceList.count()) {
		sword::InstallSource is("FTP");   //default return value
		is.caption = "Crosswire";
		is.source = "ftp.crosswire.org";
		is.directory = "/pub/sword/raw";
		//TODO: passive ftp
		BTInstallMgr::Tool::RemoteConfig::addSource(&is);

		BTInstallMgr mgr; //make sure we're uptodate
		sourceList = BTInstallMgr::Tool::RemoteConfig::sourceList(&mgr);

		Q_ASSERT( sourceList.count() > 0 );
	}

	foreach (QString sourceName, sourceList) {	
		addSource(sourceName);
	}
	
	// select the current source from the config
}

void BtSourceWidget::addSource(const QString& sourceName)
{
	qDebug("void BtSourceWidget::addSource(const QString& sourceName) start");
	BTInstallMgr mgr;
	sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&mgr, sourceName);

	// Here the tab UI is created and added to the tab widget
	BtSourceArea* area = new BtSourceArea(sourceName);
	int tabNumber = this->addTab(area, sourceName);

	if (BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)) {
		
	}
	else { // local source
		QFileInfo fi( is.directory.c_str() );
		if (fi.isDir() && fi.isReadable()) {
			
		}
	}

	// TODO: add "remote/local", server, path etc.
	QString toolTip(sourceName + QString("<br>"));
	tabBar()->setTabToolTip(tabNumber, toolTip);
	initSourceConnections();
}


void BtSourceWidget::updateList(const QString& sourceName)
{
	qDebug("BtSourceWidget::updateList start");
	//createModuleTree();
}

void BtSourceWidget::slotModuleSelectionChanged()
{
	//TODO: removing a source should update this also
	qDebug("BtSourceWidget::slotModuleSelectionChanged start");
	//bool install = false;
	//bool update = false;
	
	QList<QTreeWidgetItem*> selectedItems;
	
	for (int i = 0; i < count(); i++) {
		QTreeWidget* view = dynamic_cast<QTreeWidget*>(widget(i));
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
{	qDebug("CInstallModuleChooserDialog::CInstallModuleChooserDialog start");}

void CInstallModuleChooserDialog::initModuleItem(BTModuleTreeItem* btItem, QTreeWidgetItem* widgetItem)
{
	qDebug("CInstallModuleChooserDialog::initModuleItem start");
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
