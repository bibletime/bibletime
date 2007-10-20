/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cswordsetupdialog.h"
#include "cswordsetupmodulelistview.h"
#include "cswordsetupinstallsourcesdialog.h"
#include "cmanageindiceswidget.h"
#include "btinstallmgr.h"

#include "backend/managers/cswordbackend.h"
#include "backend/drivers/cswordmoduleinfo.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/scoped_resource.h"
#include "util/directoryutil.h"

//QT includes
#include <QFileDialog>
#include <QDir>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QFileInfo>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QProgressDialog>
#include <QMessageBox>
#include <QDebug>

//KDE includes
#include <kapplication.h>
#include <kconfig.h>
#include <keditlistbox.h>
#include <klocale.h>
#include <kstandarddirs.h>

//Sword includes
#include <installmgr.h>
#include <swmodule.h>
#include <swversion.h>

using namespace sword;

namespace BookshelfManager {

CSwordSetupDialog::CSwordSetupDialog(QWidget *parent) : KPageDialog(parent),
	m_removeModuleListView(0),
	m_installModuleListPage(0),
	m_installModuleListView(0),
	m_progressDialog(0),
	m_refreshedRemoteSources(false)
{
	setFaceType(KPageDialog::List);
//	setIconListAllVisible(true);
	m_swordSetupChanged = false;

	initSwordConfig();
	initInstall();
	initRemove();
	initManageIndices();
	connect(this, SIGNAL(okClicked()), SLOT(slotOk()));
}

void CSwordSetupDialog::initSwordConfig() {
	QWidget* page = new QWidget;
	m_swordConfigPage = addPage(page, i18n("Bookshelf path(s)"));
	page->setMinimumSize(500,400);

	QGridLayout* layout = new QGridLayout(page);
	layout->setMargin(5);

	layout->setSpacing(10);
	layout->setColumnStretch(0,1);
	layout->setRowStretch(5,1);

	QLabel* mainLabel = CToolClass::explanationLabel(page,
						i18n("Configure bookshelf path(s)"),
						i18n("You can store your bookshelfs in one or more directories, which you can specify here.")
													);
	layout->addWidget(mainLabel, 0, 0, 1, 2);

	QString swordConfPath = BTInstallMgr::Tool::LocalConfig::swordConfigFilename();
	QLabel* confPathLabel = new QLabel(i18n("Your bookshelf configuration file is: ").append("<b>%1</b>").arg(swordConfPath), page);
	confPathLabel->setWordWrap(true);
	layout->addWidget(confPathLabel, 1, 0, 1, 2);

	m_swordPathListBox = new QTreeWidget(page);
	m_swordPathListBox->setHeaderLabels(QStringList() << i18n("Path to bookshelf"));
	
	//TODO: No such signal QTreeWidget::selectionChanged()
	connect(m_swordPathListBox, SIGNAL(selectionChanged()), this, SLOT(slot_swordPathSelected()));
	layout->addWidget(m_swordPathListBox, 2, 0, 4, 1);

	m_swordEditPathButton = new QPushButton(i18n("Edit Entry"), page);
	m_swordEditPathButton->setIcon(util::filesystem::DirectoryUtil::getIcon("edit"));
	connect(m_swordEditPathButton, SIGNAL(clicked()), this, SLOT(slot_swordEditClicked()));
	layout->addWidget(m_swordEditPathButton, 2, 1);

	m_swordAddPathButton = new QPushButton(i18n("Add Entry"), page);
	m_swordAddPathButton->setIcon(util::filesystem::DirectoryUtil::getIcon("edit_add"));
	connect(m_swordAddPathButton, SIGNAL(clicked()), this, SLOT(slot_swordAddClicked()));
	layout->addWidget(m_swordAddPathButton, 3, 1);

	m_swordRemovePathButton = new QPushButton(i18n("Remove Entry"), page);
	m_swordRemovePathButton->setIcon(util::filesystem::DirectoryUtil::getIcon("editdelete"));
	connect(m_swordRemovePathButton, SIGNAL(clicked()), this, SLOT(slot_swordRemoveClicked()));
	layout->addWidget(m_swordRemovePathButton, 4, 1);

	setupSwordPathListBox();
}

void CSwordSetupDialog::initInstall() {
	QWidget* page = new QWidget;
	m_installPage = addPage(page, i18n("Install/Update works"));

	QVBoxLayout* vboxlayout = new QVBoxLayout(page);
	QHBoxLayout* hboxlayout = new QHBoxLayout();

	vboxlayout->addLayout(hboxlayout);

	m_installStackedWidget = new QStackedWidget(page);
	hboxlayout->addWidget(m_installStackedWidget);

	m_installSourcePage = new QWidget(0);
	m_installStackedWidget->addWidget(m_installSourcePage);

	//  m_installSourcePage->setMinimumSize(500,400);

	QGridLayout* layout = new QGridLayout(m_installSourcePage/*, 7, 3*/);
	layout->setMargin(5);
	layout->setSpacing(10);
	layout->setRowStretch(6,5);
	layout->setColumnStretch(0,5);

	QLabel* installLabel = CToolClass::explanationLabel(m_installSourcePage,
							i18n("Install/update works - Step 1"),
							i18n("Please choose a (local or remote) library and a bookshelf path to install the work(s) to. \
After that step click on the connect button.<br/>\
<b>WARNING: If you live in a persecuted country and do not wish to risk detection you should NOT use \
the module remote installation feature!</b>")
														);
	layout->addWidget(installLabel, 0, 0, 1, 3);

	QLabel* sourceHeadingLabel = new QLabel(QString("<b>%1</b>").arg(i18n("Select library")), m_installSourcePage);
	layout->addWidget(sourceHeadingLabel, 1, 0);

	m_sourceCombo = new QComboBox(m_installSourcePage);
	layout->addWidget(m_sourceCombo, 2, 0);

	QPushButton* deleteSourceButton = new QPushButton(i18n("Delete library"), m_installSourcePage);
	deleteSourceButton->setIcon(util::filesystem::DirectoryUtil::getIcon("remove"));
	connect(deleteSourceButton, SIGNAL(clicked()), SLOT(slot_installDeleteSource()));
	layout->addWidget(deleteSourceButton, 2, 1, Qt::AlignLeft);

	QPushButton* addSourceButton = new QPushButton(i18n("Add library"), m_installSourcePage);
	addSourceButton->setIcon(util::filesystem::DirectoryUtil::getIcon("folder_new"));
	connect(addSourceButton, SIGNAL(clicked()), SLOT(slot_installAddSource()));
	layout->addWidget(addSourceButton, 2, 2, Qt::AlignLeft);

	m_sourceLabel = new QLabel(m_installSourcePage);
	layout->addWidget(m_sourceLabel, 3, 0);

	QLabel* targetHeadingLabel = new QLabel(QString("<b>%1</b>").arg(i18n("Select bookshelf path")), m_installSourcePage);
	layout->addWidget(targetHeadingLabel, 4, 0);

	m_targetCombo = new QComboBox(m_installSourcePage);
	layout->addWidget(m_targetCombo, 5, 0);

	m_targetLabel = new QLabel(m_installSourcePage);
	layout->addWidget(m_targetLabel, 6, 0, Qt::AlignTop);

	//part beloew main layout with the back/next buttons
	QHBoxLayout* myHBox = new QHBoxLayout();
	vboxlayout->addLayout(myHBox);

	m_installBackButton = new QPushButton(i18n("Back"), page);
	m_installBackButton->setIcon(util::filesystem::DirectoryUtil::getIcon("back"));
	myHBox->addWidget(m_installBackButton);

	myHBox->addSpacing(10);
	myHBox->addStretch(5);

	m_installContinueButton = new QPushButton(i18n("Connect to library"), page);
	m_installContinueButton->setIcon(util::filesystem::DirectoryUtil::getIcon("forward"));
	connect(m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_connectToSource()));
	myHBox->addWidget(m_installContinueButton);

	m_installBackButton->setEnabled(false);

	connect(m_sourceCombo, SIGNAL( highlighted(const QString&) ), SLOT( slot_sourceSelected( const QString&) ));
	connect(m_targetCombo, SIGNAL( highlighted(const QString&) ), SLOT( slot_targetSelected( const QString&) ));
	populateInstallCombos();

	slot_sourceSelected( m_sourceCombo->currentText() );
}

void CSwordSetupDialog::initRemove() {
	QWidget* page = new QWidget;
	m_removePage = addPage(page, i18n("Remove works"));

	page->setMinimumSize(500,400);

	QGridLayout* layout = new QGridLayout(page);
	layout->setMargin(5);

	layout->setSpacing(10);
	layout->setColumnStretch(1,1);
	layout->setRowStretch(2,1);

	QLabel* mainLabel= CToolClass::explanationLabel(page,
							i18n("Remove installed work(s)"),
							i18n("This dialog lets you remove installed works from your system. Choose the modules and then click on the remove button.")
														);
	layout->addWidget(mainLabel, 0, 0, 1, 2);

	QLabel* headingLabel = new QLabel(QString("<b>%1</b>").arg(i18n("Select works to be uninstalled")), page);
	layout->addWidget(headingLabel, 1, 0, 1, 2);

	m_removeModuleListView = new CSwordSetupModuleListView(page, false);
	layout->addWidget( m_removeModuleListView, 2, 0, 1, 2);

	m_removeRemoveButton = new QPushButton(i18n("Remove selected work(s)"), page);
	m_removeRemoveButton->setIcon( util::filesystem::DirectoryUtil::getIcon("edittrash") );
	layout->addWidget(m_removeRemoveButton, 3, 1, Qt::AlignRight);

	connect(m_removeRemoveButton, SIGNAL(clicked()),
			this, SLOT(slot_doRemoveModules()));

	populateRemoveModuleListView();
}

void CSwordSetupDialog::initManageIndices()
{
	QWidget* page = new QWidget;
	m_manageIndiciesPage = addPage(page, i18n("Manage search indicies"));
	
	page->setMinimumSize(500,400);
	QGridLayout* layout = new QGridLayout(page);
	layout->setMargin(5);
	layout->setSpacing(10);

	CManageIndicesWidget* mi = new CManageIndicesWidget(page);
	layout->addWidget(mi, 0, 0);
}

void CSwordSetupDialog::slotOk() {
	qDebug("CSwordSetupDialog::slotOk");
	writeSwordConfig();
	KDialog::accepted();
	emit signalSwordSetupChanged( );
}

void CSwordSetupDialog::writeSwordConfig() {
	if (m_swordSetupChanged && m_swordPathListBox->topLevelItemCount() > 1) {
		QStringList targets;
		QTreeWidgetItemIterator it(m_swordPathListBox);
		while (*it) {
			if (!(*it)->text(0).isEmpty()) {
				targets << (*it)->text(0);
			}
			++it;
		}
		BTInstallMgr::Tool::LocalConfig::setTargetList(targets); //creates new Sword config
	}
}

void CSwordSetupDialog::showPart( CSwordSetupDialog::Parts ID ) {
	switch (ID) {
		case CSwordSetupDialog::Sword:
			setCurrentPage(m_swordConfigPage);
			break;
		case CSwordSetupDialog::Install:
			setCurrentPage(m_installPage);
			break;
		case CSwordSetupDialog::Remove:
			setCurrentPage(m_removePage);
			break;
		default:
			break;
	}
}


void CSwordSetupDialog::populateInstallCombos() {
	m_sourceCombo->clear();

	BTInstallMgr::Tool::RemoteConfig::initConfig();

	QStringList list;
	{
		BTInstallMgr mgr;
		list = BTInstallMgr::Tool::RemoteConfig::sourceList(&mgr);
	}
	if (!list.count()) { //add Crosswire entry
		InstallSource is("FTP");   //default return value
		is.caption = "Crosswire";
		is.source = "ftp.crosswire.org";
		is.directory = "/pub/sword/raw";
		BTInstallMgr::Tool::RemoteConfig::addSource(&is);

		BTInstallMgr mgr; //make sure we're uptodate
		list = BTInstallMgr::Tool::RemoteConfig::sourceList(&mgr);

		Q_ASSERT( list.count() > 0 );
	}

	BTInstallMgr mgr;
	for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
		sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&mgr, *it);

		if (BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)) {
			m_sourceCombo->addItem( i18n("[Remote]") + " " + *it ); //remote source
		}
		else { // local source
			QFileInfo fi( is.directory.c_str() );
			if (fi.isDir() && fi.isReadable()) {
				m_sourceCombo->addItem( i18n("[Local]") + " " + *it );
			}
		}
	}

	//Fill in the targets in the targets combobox
	//list = (m_targetCombo->count()) ? m_swordPathListBox : BTInstallMgr::Tool::LocalConfig::targetList();
	if (m_targetCombo->count()) { //we already read in the list once, we have to use the Sword paths list items now because this list is newer
		list.clear();
		QTreeWidgetItemIterator it( m_swordPathListBox );
		while (*it) {
			list << (*it)->text(0);
			it++;
		}
	}
	else {
		list = BTInstallMgr::Tool::LocalConfig::targetList();
	}

	m_targetCombo->clear();
	for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
		QFileInfo fi(*it);
		if (fi.isDir() && fi.isWritable()) {
			m_targetCombo->addItem( *it );
		}
	}

	//init widget states
	m_targetCombo->setEnabled( (m_targetCombo->count() > 0) );
	m_installContinueButton->setEnabled(
		(m_sourceCombo->count() > 0) && (m_targetCombo->count() > 0)
	);

	slot_sourceSelected( m_sourceCombo->currentText() );
}


void CSwordSetupDialog::slot_sourceSelected(const QString &sourceName) {
	//remove status parta
	QString source = sourceName;

	QString rep = i18n("[Local]") + " ";
	int i = source.indexOf(rep);
	if (i>=0) {
		source.remove(i, rep.length());
	}
	rep = i18n("[Remote]") + " ";
	i = source.indexOf(rep);
	if (i>=0) {
		source.remove(i, rep.length());
	}

	BTInstallMgr mgr;
	QString url;
	sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&mgr, source) ;

	if (BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)) {
		url = QString("ftp://%1%2").arg(is.source.c_str()).arg(is.directory.c_str());
	}
	else {
		url = QString("%1").arg(is.directory.c_str());
	}
	m_sourceLabel->setText( url );

	m_refreshedRemoteSources = false;
}


void CSwordSetupDialog::slot_targetSelected(const QString &targetName) {
	m_targetLabel->setText( m_targetMap[targetName] );
	target = m_targetMap[targetName];
}


void CSwordSetupDialog::slot_doRemoveModules() {

	QStringList moduleList =  m_removeModuleListView->selectedModules();

	if ( moduleList.empty() ) {
		return; //no message, just do nothing
	}

	const QString message = i18n("You selected the following work(s): ")
		.append(moduleList.join(", "))
		.append("\n\n")
		.append(i18n("Do you really want to remove them from your system?"));

	if ((QMessageBox::question(this, i18n("Confirmation"), message, QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)) {  //Yes was pressed.
		sword::InstallMgr installMgr;
		QMap<QString, sword::SWMgr*> mgrDict; //maps config paths to SWMgr objects

		for ( QStringList::Iterator it = moduleList.begin(); it != moduleList.end(); ++it ) {
			if (CSwordModuleInfo* m = backend()->findModuleByName(*it)) { //module found?
				QString prefixPath = m->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
				QString dataPath = m->config(CSwordModuleInfo::DataPath);

				if (dataPath.left(2) == "./") {
					dataPath = dataPath.mid(2);
				}

				if (prefixPath.contains(dataPath)) { //remove module part to get the prefix path
					prefixPath = prefixPath.remove( prefixPath.indexOf(dataPath), dataPath.length() );
				}
				else { //fall back to default Sword config path
					prefixPath = QString::fromLatin1(backend()->prefixPath);
				}

				sword::SWMgr* mgr = mgrDict[ prefixPath ];
				if (!mgr) { //create new mgr if it's not yet available
					mgrDict.insert(prefixPath, new sword::SWMgr(prefixPath.toLocal8Bit()));
					mgr = mgrDict[ prefixPath ];
				}

				installMgr.removeModule(mgr, m->name().toLatin1());
			}
		}

		CPointers::backend()->reloadModules();
		populateRemoveModuleListView(); //rebuild the tree
		populateInstallModuleListView( currentInstallSource() ); //rebuild the tree

		//delete all mgrs
		//mgrDict.setAutoDelete(true);
		qDeleteAll(mgrDict);
		mgrDict.clear();
	}
}

void CSwordSetupDialog::populateRemoveModuleListView() {
	CSwordBackend myBackend;
	KApplication::kApplication()->processEvents();
	myBackend.initModules();

	m_removeModuleListView->clear();

	ListCSwordModuleInfo list = myBackend.moduleList();
	int mod = 0;
	sword::SWConfig moduleConfig("");

	mod = 1;
	ListCSwordModuleInfo::iterator end_it = list.end();

	for (ListCSwordModuleInfo::iterator it(list.begin()); it != end_it; ++it, ++mod) {
		m_removeModuleListView->addModule((*it), (*it)->config(CSwordModuleInfo::ModuleVersion));
	}

	m_removeModuleListView->finish();
}

const bool CSwordSetupDialog::refreshRemoteModuleCache( const QString& sourceName ) {
	if (m_refreshedRemoteSources) { //the module info is up-to-date
		return true;
	}

	BTInstallMgr iMgr;
	m_currentInstallMgr = &iMgr; //for the progress dialog
	sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, sourceName);
	bool success = false;

	m_progressDialog = new QProgressDialog("", i18n("Cancel"), 0 ,100, this);
	m_progressDialog->setWindowTitle(i18n("Download"));
	m_progressDialog->setMinimumDuration(0);

	connect(m_progressDialog, SIGNAL(canceled()), SLOT(slot_moduleRefreshProgressCancelClicked()));
	connect(&iMgr, SIGNAL(completed(const int, const int)), SLOT(slot_moduleRefreshCompleted(const int, const int)));

	if (BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)) {
		//   int errorCode = 0;
		if (!m_refreshedRemoteSources) {
			if (!iMgr.refreshRemoteSource( &is ) ) { //make sure the sources were updates sucessfully
				m_refreshedRemoteSources = true;
				success = true;
				m_progressDialog->setValue(100); //make sure the dialog closes
			}
			else { //an error occurres, the KIO library should display an error message
				qWarning("InstallMgr: refreshRemoteSources returned an error.");
				m_refreshedRemoteSources = false;
				success = false;
			}
		}
	}

	delete m_progressDialog;
	m_progressDialog = 0;

	return success;
}

bool CSwordSetupDialog::populateInstallModuleListView( const QString& sourceName ) {
	KApplication::kApplication()->processEvents();
	if (!m_installModuleListView) { // this may be an update after removing modules
		return false;
	}

	m_installModuleListView->clear();

	BTInstallMgr iMgr;
	sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, sourceName);

	if (BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)
			&& !refreshRemoteModuleCache(sourceName)) {
		//   qWarning("finish");
		m_installModuleListView->finish();
		return false;
	}

	//kind of a hack to provide a pointer to mgr next line
	//   qWarning("createing remote_backend");
	util::scoped_ptr<CSwordBackend> remote_backend( BTInstallMgr::Tool::backend(&is) );
	//  qWarning("config path1 is %s", remote_backend->configPath);
	//  qWarning("config path2 is %s", BTInstallMgr::Tool::backend(&is)->configPath ); //mem leak
	//   qWarning("after creating remote_backend");
	Q_ASSERT(remote_backend);
	Q_ASSERT( BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is) );
	if (!remote_backend) {
		m_installModuleListView->finish();
		return false;
	}

	CSwordBackend* local_backend = CPointers::backend();
	Q_ASSERT(local_backend);
	//  qWarning("local backend has path %s", local_backend->);
	KApplication::kApplication()->processEvents();
	//local_backend.initModules();

	//  qWarning("config path3 is %s", remote_backend->configPath);
	ListCSwordModuleInfo mods = remote_backend->moduleList();
	Q_ASSERT(mods.count() > 0);

	ListCSwordModuleInfo::iterator end_it = mods.end();
	for (ListCSwordModuleInfo::iterator it(mods.begin()); it != end_it; ++it) {
		//   qWarning("adding module %s (%s)", (*it)->name().toLatin1(), (*it)->config(CSwordModuleInfo::AbsoluteDataPath).toLatin1());
		bool isUpdate = false;

		CSwordModuleInfo* const installedModule = local_backend->findModuleByName((*it)->name());
		if (installedModule) {
			//    qWarning("module is already installed in %s", installedModule->config(CSwordModuleInfo::AbsoluteDataPath).toLatin1());
		}
		//     Q_ASSERT(installedModule);

		if (installedModule) { //module already installed?
			//check whether it's an uodated module or just the same
			const SWVersion installedVersion(
				installedModule->config(CSwordModuleInfo::ModuleVersion).toLatin1()
			);

			const SWVersion newVersion(
				(*it)->config(CSwordModuleInfo::ModuleVersion).toLatin1()
			);

			isUpdate = (newVersion > installedVersion);
			if (!isUpdate) {
				//     qWarning("    mod %s is not an update", (*it)->name().toLatin1());
				continue;
			}
		}

		//   Q_ASSERT(installedModule);
		m_installModuleListView->addModule((*it), installedModule ? installedModule->config(CSwordModuleInfo::ModuleVersion) : QString::null);
	}
	m_installModuleListView->finish();
	return true;
}

void CSwordSetupDialog::slot_connectToSource() {
	if (!m_installModuleListPage) { //the widgets are not yet created
		m_installModuleListPage = new QWidget(0);

		QGridLayout* layout = new QGridLayout(m_installModuleListPage);
		layout->setMargin(5);
		layout->setSpacing(10);

		QLabel* installLabel = CToolClass::explanationLabel(m_installModuleListPage,
			i18n("Install/update works - Step 2"),
			i18n("Please choose the works which should be installed and/or updated and click the install button."));
			
		layout->addWidget(installLabel, 0, 0);
		layout->setRowStretch(0,0);

		m_installStackedWidget->addWidget(m_installModuleListPage);
		m_installModuleListPage->setMinimumSize(500,400);

		//insert a list box which contains all available remote modules
		BTInstallMgr iMgr;
		sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, currentInstallSource());

		m_installModuleListView = new CSwordSetupModuleListView(m_installModuleListPage, true, &is);
		layout->addWidget( m_installModuleListView, 1, 0);
		layout->setColumnStretch(0,5);
		layout->setRowStretch(1,5);

		connect(m_installModuleListView, SIGNAL(selectedModulesChanged()), SLOT(slot_installModulesChanged()));
	}

	if (populateInstallModuleListView( currentInstallSource() ) ){

		//code valid for already existing and newly created widgets
		disconnect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_connectToSource()));
		connect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_installModules()));

		m_installContinueButton->setText(i18n("Install works"));
		m_installContinueButton->setEnabled(false);

		m_installStackedWidget->setCurrentWidget(m_installModuleListPage);

		connect( m_installBackButton, SIGNAL(clicked()), this, SLOT(slot_showInstallSourcePage()));
		m_installBackButton->setEnabled(true);
	}
}

void CSwordSetupDialog::slot_installAddSource() {

	sword::InstallSource newSource = CSwordSetupInstallSourcesDialog::getSource();

	if ( !((QString)newSource.type.c_str()).isEmpty() ) { // we have a valid source to add
		BTInstallMgr::Tool::RemoteConfig::addSource( &newSource );
	}

	populateInstallCombos(); //make sure the items are updated
}



void CSwordSetupDialog::slot_installModulesChanged() {
	// This function enabled the Install modules button if modules are chosen
	// If an item was clicked to be not chosen look if there are other selected items
	// If the item was clicked to be chosen enable the button without looking at the other items

	const int moduleCount = m_installModuleListView->selectedModules().count();
	m_installContinueButton->setEnabled(moduleCount > 0);
}

void CSwordSetupDialog::slot_installModules() {
	qWarning("CSwordSetupDialog::slot_installModules()");
	//   m_installContinueButton->setEnabled(false);
	//   m_installBackButton->setEnabled(false);

	//first get all chosen modules
	QStringList moduleList = m_installModuleListView->selectedModules();
	Q_ASSERT(moduleList.count() != 0);
	if (moduleList.count() == 0) { // no modules selected
		return;
	}

	const QString message = i18n("You selected the following works: ")
		.append(moduleList.join(", "))
		.append("\n\n")
		.append(i18n("Do you really want to install them on your system?"));

	if ((QMessageBox::question(this, i18n("Confirmation"), message, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)) {  //Yes was pressed.
		BTInstallMgr iMgr;
		m_currentInstallMgr = &iMgr;
		sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, currentInstallSource());

		QString target = m_targetCombo->currentText();

		//make sure target/mods.d and target/modules exist
		QDir dir(target.toLatin1());
		if (!dir.exists()) {
			dir.mkdir(target);
		}
		if (!dir.exists("modules")) {
			dir.mkdir("modules");
		}
		if (!dir.exists("mods.d")) {
			dir.mkdir("mods.d");
		}

		sword::SWMgr lMgr( target.toLatin1() );

		//module are removed in this section of code
		m_installedModuleCount = 0;
		m_progressDialog = new QProgressDialog("", i18n("Cancel"), 0, 100 * moduleList.count(), this);
		m_progressDialog->setWindowTitle(i18n("Download of work(s)"));
		m_progressDialog->setMinimumDuration(0);

		connect(m_progressDialog, SIGNAL(canceled()), SLOT(slot_installProgressCancelClicked()));
		connect(&iMgr, SIGNAL(completed(const int, const int)), SLOT(installCompleted(const int, const int)));

		for (QStringList::iterator it = moduleList.begin(); it != moduleList.end() && !m_progressDialog->wasCanceled(); ++it, ++m_installedModuleCount ){

			qDebug() << "installing" << (*it);
			m_installingModule = *it;

			//check whether it's an update. If yes, remove existing module first
			CSwordModuleInfo* m = backend()->findModuleByName(*it);
			if (m) { //module found?
				QString prefixPath = m->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
				QString dataPath = m->config(CSwordModuleInfo::DataPath);
				if (dataPath.left(2) == "./") {
					dataPath = dataPath.mid(2);
				}

				if (prefixPath.contains(dataPath)) {
					prefixPath.remove( prefixPath.indexOf(dataPath), dataPath.length() ); //complicated to work with Qt 3.0
					//prefixPath = prefixPath.replace(dataPath, ""); //old code working with Qt 3.2
				}
				else {
					prefixPath = QString::fromLatin1(backend()->prefixPath);
				}

				sword::SWMgr mgr(prefixPath.toLatin1());
				iMgr.removeModule(&mgr, m->name().toLatin1());
			}

			if (!m_progressDialog->wasCanceled() && BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)) {
				qDebug() << "calling install";
				int status = iMgr.installModule(&lMgr, 0, (*it).toLatin1(), &is);
				qWarning() << "status: " << status;
			}
			else if (!m_progressDialog->wasCanceled()) { //local source
				iMgr.installModule(&lMgr, is.directory.c_str(), (*it).toLatin1());
			}
		}

		delete m_progressDialog;
		m_progressDialog = 0;

		//reload our backend because modules may have changed
		backend()->reloadModules();
		populateInstallModuleListView( currentInstallSource() ); //rebuild the tree
		populateRemoveModuleListView();
	}

	m_currentInstallMgr = 0;
	m_installBackButton->setEnabled(true);
	slot_installModulesChanged();
}

void CSwordSetupDialog::installCompleted( const int total, const int /* file */) {
	if (m_progressDialog) {
		m_progressDialog->setValue(total+100*m_installedModuleCount);
		m_progressDialog->setLabelText( QString("[%1]: %2%").arg(m_installingModule).arg(total) );
	}
	KApplication::kApplication()->processEvents();
}

void CSwordSetupDialog::slot_showInstallSourcePage() {
	connect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_connectToSource()));
	disconnect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_installModules()));
	m_installBackButton->setEnabled(false);

	m_installContinueButton->setText(i18n("Connect to library"));
	m_installContinueButton->setEnabled(true);

	m_installStackedWidget->setCurrentWidget(m_installSourcePage);
}

void CSwordSetupDialog::slot_swordEditClicked() {
	if (QTreeWidgetItem* i = m_swordPathListBox->currentItem()) {
		QDir dir = QFileDialog::getExistingDirectory(this, i18n("Choose directory"), i->text(0));
		if (dir.isReadable()) {
			const QFileInfo fi( dir.canonicalPath() );
			if (!fi.exists() || !fi.isWritable()) {
				const int result = QMessageBox::warning(this, i18n("Confirmation"), i18n("This directory is not writable, so works can not be installed here using BibleTime. Do you want to use this directory instead of the previous value?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
				if (result != QMessageBox::Yes) return;
			}
			i->setText(0, dir.canonicalPath());
			m_swordSetupChanged = true;
			writeSwordConfig(); //to make sure other parts work with the new setting
			populateInstallCombos(); //update target list bof on install page
			populateRemoveModuleListView();
		}
	}
}

void CSwordSetupDialog::slot_swordAddClicked() {
	QDir dir = QFileDialog::getExistingDirectory(this, i18n("Choose directory"), "");
	if (dir.isReadable()) {
		const QFileInfo fi( dir.canonicalPath() );
		if (!fi.exists() || !fi.isWritable()) {
			const int result = QMessageBox::warning(this, i18n("Warning"), i18n("This directory is not writable, so works can not be installed here using BibleTime. Do you still want to add it to the list of module directories?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
			if (result != QMessageBox::Yes) {
				return;
			}
		}
		new QTreeWidgetItem(m_swordPathListBox, QStringList(dir.canonicalPath()) );
		m_swordSetupChanged = true;
		writeSwordConfig(); //to make sure other parts work with the new setting
		populateInstallCombos();     //update target list bof on install page
		populateRemoveModuleListView();
	}
}

void CSwordSetupDialog::slot_swordRemoveClicked() {
	QTreeWidgetItem* i = m_swordPathListBox->currentItem();
	if (i) {
		delete i;

		m_swordSetupChanged = true;
		writeSwordConfig(); //to make sure other parts work with the new setting
		populateInstallCombos(); //update target list bof on install page
		populateRemoveModuleListView();
	}
}

void CSwordSetupDialog::setupSwordPathListBox() {
	QStringList targets = BTInstallMgr::Tool::LocalConfig::targetList();
	m_swordPathListBox->clear();

	for (QStringList::iterator it = targets.begin(); it != targets.end(); ++it)  {
		if ((*it).isEmpty()) continue;
		new QTreeWidgetItem(m_swordPathListBox, QStringList(*it) );
	}
	//m_swordPathListBox->setCurrentItem( m_swordPathListBox->firstChild() );
}

void CSwordSetupDialog::slot_swordPathSelected() {
	m_swordEditPathButton->setEnabled( m_swordPathListBox->currentItem() );
}

const QString CSwordSetupDialog::currentInstallSource() {
	QString source = m_sourceCombo->currentText();
	QString rep = i18n("[Local]") + " ";
	int i = source.indexOf(rep);
	if (i>=0) {
		source.remove(i, rep.length());
	}
	rep = i18n("[Remote]") + " ";
	i = source.indexOf(rep);
	if (i>=0) {
		source.remove(i, rep.length());
	}
	return source;
}

void CSwordSetupDialog::slot_installProgressCancelClicked() {
	//cancel possible active module installation
	Q_ASSERT(m_currentInstallMgr);
	if (m_currentInstallMgr) {
		m_currentInstallMgr->terminate();
	}
}

void CSwordSetupDialog::slot_installDeleteSource() {
	BTInstallMgr iMgr;
	sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source( &iMgr, currentInstallSource() );
	BTInstallMgr::Tool::RemoteConfig::removeSource( &iMgr, &is );
	
	populateInstallCombos();
}

void CSwordSetupDialog::slot_moduleRefreshProgressCancelClicked() {
	Q_ASSERT(m_currentInstallMgr);
	if (m_currentInstallMgr) {
		m_currentInstallMgr->terminate();
	}
	KApplication::kApplication()->processEvents();
}

void CSwordSetupDialog::slot_moduleRefreshCompleted(const int /*total*/, const int current) {
	if (m_progressDialog) {
		m_progressDialog->setValue(current);
	}
	KApplication::kApplication()->processEvents();
}

} // NAMESPACE
