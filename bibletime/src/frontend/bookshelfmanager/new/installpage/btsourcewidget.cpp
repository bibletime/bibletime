/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btsourcewidget.h"
#include "btsourcewidget.moc"

#include "btinstallpage.h"
#include "btsourcearea.h"
#include "btinstallprogressdialog.h"
#include "btinstallmodulechooserdialog.h"

#include "frontend/bookshelfmanager/new/btmodulemanagerdialog.h"
#include "frontend/bookshelfmanager/cswordsetupinstallsourcesdialog.h"
#include "frontend/bookshelfmanager/new/bt_installmgr.h"
#include "frontend/bookshelfmanager/new/backend.h"


#include <QString>
#include <QWidget>
#include <QTabWidget>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QApplication>
#include <QFileInfo>
#include <QTabBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>

// ****************************************************************
// ******** Tab Widget that holds source widgets ******************
// ****************************************************************

BtSourceWidget::BtSourceWidget(BtInstallPage* parent)
	: QTabWidget(parent),
	m_page(parent)
{
	qDebug("BtSourceWidget::BtSourceWidget start");
	initSources();
	
	// TODO: choose the page from config
	
}

BtSourceArea* BtSourceWidget::area()
{
	return dynamic_cast<BtSourceArea*>(currentWidget());
}

QString BtSourceWidget::currentSourceName()
{
	qDebug() << "BtSourceWidget::currentSourceName: " << m_sourceNameList.at(currentIndex());
	return m_sourceNameList.at(currentIndex());
}

void BtSourceWidget::initSourceConnections()
{
	qDebug("void BtSourceWidget::initSourceConnections() start");
	if (area()) {
		connect(area()->m_refreshButton, SIGNAL(clicked()), SLOT(slotRefresh()));
		connect(area()->m_editButton, SIGNAL(clicked()), SLOT(slotEdit()));
		connect(area()->m_deleteButton, SIGNAL(clicked()), SLOT(slotDelete()));
		connect(area()->m_addButton, SIGNAL(clicked()), SLOT(slotAdd()));
		connect(area(), SIGNAL(signalSelectionChanged(QString, int)), SLOT(slotModuleSelectionChanged(QString, int)) );
	}
	qDebug("void BtSourceWidget::initSourceConnections() end");
}

void BtSourceWidget::slotEdit()
{
	qDebug("BtSourceWidget::slotEdit");
	// open the source editor dialog
	
	// if the source was changed, init the sources
	
}

void BtSourceWidget::slotDelete()
{
	qDebug("void BtSourceWidget::slotDelete() start");
	// ask for confirmation
	int ret = QMessageBox::warning(this, tr("Delete Source?"),
			tr("Do you really want to delete this source?"),
			QMessageBox::Yes | QMessageBox::No);

	if (ret == QMessageBox::Yes) {
		backend::deleteSource(currentSourceName());
		
		// remove the UI elements
		m_sourceNameList.removeAt(currentIndex());
		QWidget* w = currentWidget();
		removeTab(currentIndex());
		delete w;
	}
}

void BtSourceWidget::slotAdd()
{
	qDebug("void BtSourceWidget::slotAdd() start");
	qDebug("open the old dialog, TODO: write new one");
	sword::InstallSource newSource = BookshelfManager::CSwordSetupInstallSourcesDialog::getSource();
	if ( !((QString)newSource.type.c_str()).isEmpty() ) { // we have a valid source to add
		backend::addSource(newSource);
		addSource(QString(newSource.caption.c_str()));
	}
}


void BtSourceWidget::slotRefresh()
{
	qDebug("void BtSourceWidget::slotRefresh() start");
	// (re)build the module cache for the source

	QString sourceName = currentSourceName();

	// quick enough, make it modal so that we don't need to take care of anything else
	m_progressDialog = new QProgressDialog("", tr("Cancel"), 0 ,100, this);
	m_progressDialog->setWindowTitle(tr("Refreshing Source"));
	m_progressDialog->setMinimumDuration(0);

	// TODO: get rid of the backend code, Bt_InstallMgr and progressdialog could handle this
	//write method Bt_InstallMgr::slotRefreshCanceled()
	connect(m_progressDialog, SIGNAL(canceled()), SLOT(slotRefreshCanceled()));

	// BACKEND CODE **********************************************************
	// would this be possible: backend::refreshSource( arguments );
	qDebug("void BtSourceWidget::slotRefresh 1");
	Bt_InstallMgr iMgr;
	m_currentInstallMgr = &iMgr; //for the progress dialog
	sword::InstallSource is = backend::source(sourceName);
	bool success = false;
	qDebug("void BtSourceWidget::slotRefresh 2");
	// connect this directly to the dialog setValue(int) if possible
	connect(&iMgr, SIGNAL(percentCompleted(const int, const int)), SLOT(slotRefreshCompleted(const int, const int)));

	if (backend::isRemote(is)) {
		m_progressDialog->show();
		qApp->processEvents();
		this->slotRefreshCompleted(0,0);
		m_progressDialog->setLabelText(tr("Connecting..."));
		m_progressDialog->setValue(0);
		qApp->processEvents();
		//qApp->flush();
		//qApp->processEvents();
		//m_progressDialog->repaint();
		//qApp->processEvents();
		qDebug("void BtSourceWidget::slotRefresh 3");
		bool successful = iMgr.refreshRemoteSource( &is );
		if (!successful ) { //make sure the sources were updated sucessfully
			success = true;
			m_progressDialog->setValue(100); //make sure the dialog closes
		}
		else {
			qWarning("InstallMgr: refreshRemoteSources returned an error.");
			success = false;
		}
	}
	else {
		// Local source, update the list
		success = true;
	}

	delete m_progressDialog;
	m_progressDialog = 0;
	
	// rebuild the view tree and refresh the view
	if (success) {
		qDebug("void BtSourceWidget::slotRefresh 4");
		area()->createModuleTree();
	}
}

//TODO: try to move this to Bt_InstallMgr
void BtSourceWidget::slotRefreshCanceled()
{
	qDebug("BtSourceWidget::slotRefreshCanceled");
	Q_ASSERT(m_currentInstallMgr);
	if (m_currentInstallMgr) {
		m_currentInstallMgr->terminate();
	}
	qApp->processEvents();
}

//TODO: try to move this to progress dialog
void BtSourceWidget::slotRefreshCompleted(const int, const int current)
{
	qDebug("BtSourceWidget::slotRefreshCompleted");
	if (m_progressDialog) {
		if (m_progressDialog->labelText() != tr("Refreshing...")) {
			m_progressDialog->setLabelText(tr("Refreshing..."));
		}
		m_progressDialog->setValue(current);
	}
	qApp->processEvents();
}

// init the tabbar, setup the module tree for the current source
void BtSourceWidget::initSources()
{
	qDebug("void BtSourceWidget::initSources() start");

	// ***** Use the backend to get the list of sources *****
	backend::initPassiveFtpMode();
	QStringList sourceList = backend::sourceList();

	// Add a default entry, the Crosswire main repository
	// TODO: this is easy for the user, but should the edit dialog
	// open automatically?
	if (!sourceList.count()) {
		sword::InstallSource is("FTP");   //default return value
		is.caption = "Crosswire";
		is.source = "ftp.crosswire.org";
		is.directory = "/pub/sword/raw";
		// passive ftp is not needed here, it's the default

		backend::addSource(is);

		sourceList = backend::sourceList();
		Q_ASSERT( sourceList.count() > 0 );
	}
	qDebug("void BtSourceWidget::initSources 1");
	// Add the sources to the widget
	foreach (QString sourceName, sourceList) {	
		addSource(sourceName);
	}
	// connect this after the tabs have been created,
	// otherwise the signal is caught too early.
	QObject::connect(this, SIGNAL(currentChanged(int)), this, SLOT(slotTabSelected(int)));
	qDebug("void BtSourceWidget::initSources end");
	// TODO: select the current source from the config
	// It's important to choose something because the tree is not initialized until now
	setCurrentIndex(0);
}

void BtSourceWidget::addSource(const QString& sourceName)
{
	qDebug("void BtSourceWidget::addSource(const QString& sourceName) start");
	// The source has already been added to the backend.

	QString type;
	QString server;
	QString path;
	sword::InstallSource is = backend::source(sourceName);
	if (backend::isRemote(is)) {
		type = tr("Remote:");
		server = is.source.c_str();
		path = is.directory.c_str();
	}
	else { // local source
		type = tr("Local:");
		QFileInfo fi( is.directory.c_str() );
		path = is.directory.c_str();
		if (!(fi.isDir() )) {
			path = path + QString(" ") + tr("Not a directory!"); //TODO: change this
		}
		if (!fi.isReadable()) {
			path = path + QString(" ") + tr("Not readable!"); //TODO: change this
		}
	}

	// Here the tab UI is created and added to the tab widget
	BtSourceArea* area = new BtSourceArea(sourceName);
	int tabNumber = this->addTab(area, sourceName);

	// TODO: add "remote/local", server, path etc.
	QString toolTip(QString("<p style='white-space:pre'>") + sourceName + QString("<br/><b>") + type + QString("</b> ") + server + path + QString("</p>"));
	tabBar()->setTabToolTip(tabNumber, toolTip);

	//select the new tab
	setCurrentIndex(tabNumber);
	m_sourceNameList.append(sourceName);
	initSourceConnections();
	qDebug("BtSourceWidget::addSource end");
}

//
void BtSourceWidget::slotModuleSelectionChanged(QString sourceName, int selectedCount)
{
	//TODO: editing sources should update the map also
	qDebug("BtSourceWidget::slotModuleSelectionChanged start");

	int overallCount = 0;
	m_selectedModulesCountMap.insert(sourceName, selectedCount);
	foreach (int count, m_selectedModulesCountMap) {
		qDebug() << "add" << count << "to overall count of selected modules";
		overallCount += count;
	}
	
	if (overallCount > 0) {
		m_page->setInstallEnabled(true);
	} else {
		m_page->setInstallEnabled(false);
	}
}

void BtSourceWidget::slotTabSelected(int index)
{
	BtSourceArea* area = dynamic_cast<BtSourceArea*>(currentWidget());
	if (area) area->initTreeFirstTime();
}

void BtSourceWidget::slotInstall()
{
	qDebug("void BtInstallPage::slotInstall start");
	
	// create the confirmation dialog
	// (module tree dialog, modules taken from all sources)
	QString dlgTitle(tr("Install/Update works?"));
	QString dlgLabel(tr("Do you really want to install these works?") +
		QString("<br/><br/><small>") +
				tr("Only one version of a work can be installed at the same time. Select only one if there are items marked with red.") +
				QString("</small>"));

	// with empty list we avoid creating the module tree inside the dialog code
	ListCSwordModuleInfo emptyList;
	BtInstallModuleChooserDialog* dlg = new BtInstallModuleChooserDialog(this, dlgTitle, dlgLabel, &emptyList);
	//dlg->setGrouping(BTModuleTreeItem::Mod);
	QTreeWidget* treeWidget = dlg->treeWidget();
	QTreeWidgetItem* rootItem = treeWidget->invisibleRootItem();

	QStringList nameList;

	// loop through each tab
	for (int tab = 0; tab < count(); ++tab) {
		BtSourceArea* sArea = dynamic_cast<BtSourceArea*>(widget(tab));
		if (sArea && sArea->selectedModules()->count() > 0) {
			// there are selected modules in the source, create items for these
			QTreeWidgetItem* sourceItem = new QTreeWidgetItem(rootItem);
			sourceItem->setText(0, m_sourceNameList.at(tab));
			sourceItem->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsTristate|Qt::ItemIsEnabled);
			foreach (QString mName, sArea->selectedModules()->keys()) {
				dlg->initModuleItem(mName, sourceItem);
			}
			sourceItem->setExpanded(true);
		}
	}

	//user accepts the dialog
	connect(dlg, SIGNAL(modulesChanged(ListCSwordModuleInfo, QTreeWidget*)), SLOT(slotInstallAccepted(ListCSwordModuleInfo, QTreeWidget*)) );
	// user checks/unchecks an item, needed for preventing double items
	QObject::connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), dlg, SLOT(slotItemChecked(QTreeWidgetItem*, int)));
	dlg->exec();
	// The OK signal sent by the dialog is catched with slotInstallAccepted.
}

void BtSourceWidget::slotStopInstall(QTreeWidget* treeWidget)
{
	qDebug("BtSourceWidget::slotStopInstall");
	// not needed?
}

void BtSourceWidget::slotInstallAccepted(ListCSwordModuleInfo modules, QTreeWidget* treeWidget)
{
	qDebug("BtSourceWidget::slotInstallAccepted");

	BtModuleManagerDialog* parentDialog = dynamic_cast<BtModuleManagerDialog*>(dynamic_cast<BtInstallPage*>(parent())->parentDialog());

 	BtInstallProgressDialog* dlg = new BtInstallProgressDialog(parentDialog, treeWidget, dynamic_cast<BtInstallPage*>(parent())->selectedInstallPath());

	if (!parentDialog) qDebug("error, wrong parent!");
	QObject::connect(dlg, SIGNAL(swordSetupChanged()), parentDialog, SLOT(slotSwordSetupChanged()));
	// the progress dialog is now modal, it can be made modeless later.
	dlg->exec();
	//TODO: disable the Install button
	
	qDebug("BtSourceWidget::slotInstallAccepted end");
}
