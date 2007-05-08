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

#include "backend/cswordbackend.h"
#include "backend/cswordmoduleinfo.h"
// #include "btinstallmgr.h"

#include "cmanageindiceswidget.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"
#include "util/scoped_resource.h"

//QT includes
#include <qdir.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qwidgetstack.h>
#include <qfileinfo.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qdict.h>

//KDE includes
#include <kapplication.h>
#include <kconfig.h>
#include <kdirselectdialog.h>
#include <keditlistbox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <kurl.h>

//Sword includes
#include <installmgr.h>
#include <swmodule.h>
#include <swversion.h>

using namespace sword;

namespace BookshelfManager {

	CSwordSetupDialog::CSwordSetupDialog(QWidget *parent, const char *name )
: KDialogBase(IconList, i18n("Bookshelf Manager"), Ok, Ok, parent, name, true, true, QString::null, QString::null, QString::null),
	m_removeModuleListView(0),
	m_installModuleListPage(0),
	m_installModuleListView(0),
	m_progressDialog(0),
	m_refreshedRemoteSources(false) {
		setIconListAllVisible(true);
		m_swordSetupChanged = false;

		initSwordConfig();
		initInstall();
		initRemove();
		initManageIndices();
	}

	void CSwordSetupDialog::initSwordConfig() {
		QFrame* page = m_swordConfigPage = addPage(i18n("Bookshelf path(s)"), QString::null, SmallIcon("bt_swordconfig",32));
		page->setMinimumSize(500,400);

		QGridLayout* layout = new QGridLayout(page, 6, 4);
		layout->setMargin(5);

		layout->setSpacing(10);
		layout->setColStretch(0,1);
		layout->setRowStretch(5,1);

		QLabel* mainLabel = CToolClass::explanationLabel(page,
							i18n("Configure bookshelf path(s)"),
							i18n("You can store your bookshelfs in one or more directories, which you can specify here.")
														);
		layout->addMultiCellWidget(mainLabel, 0, 0, 0, 3);


		QString swordConfPath = BTInstallMgr::Tool::LocalConfig::swordConfigFilename();
		QLabel* confPathLabel = new QLabel(i18n("Your bookshelf configuration file is <b>%1</b>").arg(swordConfPath), page);
		layout->addMultiCellWidget(confPathLabel, 1,1,0,3);

		m_swordPathListBox = new QListView(page);
		//   m_swordPathListBox->setFullWidth(true);
		m_swordPathListBox->addColumn(i18n("Path to bookshelf"));
		connect(m_swordPathListBox, SIGNAL(selectionChanged()), this, SLOT(slot_swordPathSelected()));
		layout->addMultiCellWidget(m_swordPathListBox, 2,5,0,1);

		m_swordEditPathButton = new QPushButton(i18n("Edit Entry"), page);
		m_swordEditPathButton->setIconSet(SmallIcon("edit", 16));
		connect(m_swordEditPathButton, SIGNAL(clicked()), this, SLOT(slot_swordEditClicked()));
		layout->addWidget(m_swordEditPathButton, 2, 3);

		m_swordAddPathButton = new QPushButton(i18n("Add Entry"), page);
		m_swordAddPathButton->setIconSet(SmallIcon("edit_add", 16));
		connect(m_swordAddPathButton, SIGNAL(clicked()), this, SLOT(slot_swordAddClicked()));
		layout->addWidget(m_swordAddPathButton, 3,3);

		m_swordRemovePathButton = new QPushButton(i18n("Remove Entry"), page);
		m_swordRemovePathButton->setIconSet(SmallIcon("editdelete", 16));
		connect(m_swordRemovePathButton, SIGNAL(clicked()), this, SLOT(slot_swordRemoveClicked()));
		layout->addWidget(m_swordRemovePathButton, 4,3);

		setupSwordPathListBox();
	}

	void CSwordSetupDialog::initInstall() {
		m_installPage = addPage(i18n("Install/Update works"), QString::null, SmallIcon("bt_bible",32));

		QVBoxLayout* vboxlayout = new QVBoxLayout(m_installPage);
		QHBoxLayout* hboxlayout = new QHBoxLayout();
		hboxlayout->setAutoAdd( true );

		vboxlayout->addLayout(hboxlayout);

		m_installWidgetStack = new QWidgetStack(m_installPage);
		hboxlayout->addWidget(m_installWidgetStack);

		m_installSourcePage = new QWidget(0);
		m_installWidgetStack->addWidget(m_installSourcePage);

		//  m_installSourcePage->setMinimumSize(500,400);

		QGridLayout* layout = new QGridLayout(m_installSourcePage, 7, 3);
		layout->setMargin(5);
		layout->setSpacing(10);
		layout->setRowStretch(6,5);
		layout->setColStretch(0,5);

		QLabel* installLabel = CToolClass::explanationLabel(m_installSourcePage,
							   i18n("Install/update works - Step 1"),
							   i18n("Please choose a (local or remote) library and a bookshelf path to install the work(s) to. \
After that step click on the connect button.<br/>\
<b>WARNING: If you live in a persecuted country and do not wish to risk detection you should NOT use \
the module remote installation feature!</b>")
														   );
		layout->addMultiCellWidget(installLabel, 0,0,0,2);

		QLabel* sourceHeadingLabel = new QLabel(QString("<b>%1</b>").arg(i18n("Select library")), m_installSourcePage);
		layout->addMultiCellWidget(sourceHeadingLabel, 1,1,0,1);

		m_sourceCombo = new QComboBox(m_installSourcePage);
		layout->addWidget(m_sourceCombo, 2, 0);

		QPushButton* deleteSourceButton = new QPushButton(i18n("Delete library"), m_installSourcePage);
		deleteSourceButton->setIconSet(SmallIcon("remove", 16));
		connect(deleteSourceButton, SIGNAL(clicked()), SLOT(slot_installDeleteSource()));
		layout->addWidget(deleteSourceButton, 2, 1, Qt::AlignLeft);

		QPushButton* addSourceButton = new QPushButton(i18n("Add library"), m_installSourcePage);
		addSourceButton->setIconSet(SmallIcon("folder_new", 16));
		connect(addSourceButton, SIGNAL(clicked()), SLOT(slot_installAddSource()));
		layout->addWidget(addSourceButton, 2, 2, Qt::AlignLeft);

		m_sourceLabel = new QLabel(m_installSourcePage);
		layout->addMultiCellWidget(m_sourceLabel, 3,3,0,1);

		QLabel* targetHeadingLabel = new QLabel(QString("<b>%1</b>").arg(i18n("Select bookshelf path")), m_installSourcePage);
		layout->addMultiCellWidget(targetHeadingLabel, 4,4,0,1);

		m_targetCombo = new QComboBox(m_installSourcePage);
		layout->addWidget(m_targetCombo, 5, 0);

		m_targetLabel = new QLabel(m_installSourcePage);
		layout->addMultiCellWidget(m_targetLabel, 6,6,0,0,Qt::AlignTop);

		//part beloew main layout with the back/next buttons
		QHBoxLayout* myHBox = new QHBoxLayout();
		vboxlayout->addLayout(myHBox);

		m_installBackButton = new QPushButton(i18n("Back"), m_installPage);
		m_installBackButton->setIconSet(SmallIcon("back",16));
		myHBox->addWidget(m_installBackButton);

		myHBox->addSpacing(10);
		myHBox->addStretch(5);

		m_installContinueButton = new QPushButton(i18n("Connect to library"), m_installPage);
		m_installContinueButton->setIconSet(SmallIcon("forward",16));
		connect(m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_connectToSource()));
		myHBox->addWidget(m_installContinueButton);

		m_installBackButton->setEnabled(false);

		connect(m_sourceCombo, SIGNAL( highlighted(const QString&) ), SLOT( slot_sourceSelected( const QString&) ));
		connect(m_targetCombo, SIGNAL( highlighted(const QString&) ), SLOT( slot_targetSelected( const QString&) ));
		populateInstallCombos();

		slot_sourceSelected( m_sourceCombo->currentText() );
	}

	void CSwordSetupDialog::initRemove() {
		QFrame* page = m_removePage = addPage(i18n("Remove works"), QString::null, SmallIcon("edittrash",32));

		page->setMinimumSize(500,400);

		QGridLayout* layout = new QGridLayout(page, 4, 4);
		layout->setMargin(5);

		layout->setSpacing(10);
		layout->setColStretch(1,1);
		layout->setRowStretch(2,1);

		QLabel* mainLabel= CToolClass::explanationLabel(page,
						   i18n("Remove installed work(s)"),
						   i18n("This dialog lets you remove installed works from your system. Choose the modules and then click on the remove button.")
													   );
		layout->addMultiCellWidget(mainLabel, 0, 0, 0, 3);

		QLabel* headingLabel = new QLabel(QString("<b>%1</b>").arg(i18n("Select works to be uninstalled")), page);
		layout->addMultiCellWidget(headingLabel, 1, 1, 0, 3);

		m_removeModuleListView = new CSwordSetupModuleListView(page, false);
		layout->addMultiCellWidget( m_removeModuleListView, 2,2,0,3);

		m_removeRemoveButton = new QPushButton(i18n("Remove selected work(s)"), page);
		m_removeRemoveButton->setIconSet( SmallIcon("edittrash", 16) );
		layout->addWidget(m_removeRemoveButton, 3, 3, Qt::AlignRight);

		connect(m_removeRemoveButton, SIGNAL(clicked()),
				this, SLOT(slot_doRemoveModules()));

		populateRemoveModuleListView();
	}

	void CSwordSetupDialog::initManageIndices()
	{
		QFrame* page = m_manageIndiciesPage = addPage(i18n("Manage search indicies"),
			QString::null, SmallIcon("filefind",32));

		page->setMinimumSize(500,400);
		QVBoxLayout* box = new QVBoxLayout(page, 4, 4);
		CManageIndicesWidget* mi = new CManageIndicesWidget(page);
		box->addWidget(mi);
	}

	void CSwordSetupDialog::slotOk() {
		writeSwordConfig();
		KDialogBase::slotOk();
		emit signalSwordSetupChanged( );
	}

	void CSwordSetupDialog::writeSwordConfig() {
		if (m_swordSetupChanged && m_swordPathListBox->childCount()) {
			QStringList targets;
			QListViewItemIterator it( m_swordPathListBox );
			while ( it.current() ) {
				QListViewItem *item = it.current();
				if (!item->text(0).isEmpty()) {
					targets << item->text(0);
				}
				++it;
			}

			BTInstallMgr::Tool::LocalConfig::setTargetList(targets); //creates new Sword config
		}
	}

	const bool CSwordSetupDialog::showPart( CSwordSetupDialog::Parts ID ) {
		bool ret = false;
		switch (ID) {
			case CSwordSetupDialog::Sword:
			showPage( pageIndex(m_swordConfigPage) );
			break;
			case CSwordSetupDialog::Install:
			showPage( pageIndex(m_installPage) );
			break;
			case CSwordSetupDialog::Remove:
			showPage( pageIndex(m_removePage) );
			break;
			default:
			break;
		}
		return ret;
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
				m_sourceCombo->insertItem( i18n("[Remote]") + " " + *it ); //remote source
			}
			else { // local source
				QFileInfo fi( is.directory.c_str() );
				if (fi.isDir() && fi.isReadable()) {
					m_sourceCombo->insertItem( i18n("[Local]") + " " + *it );
				}
			}
		}

		//Fill in the targets in the targets combobox
		//list = (m_targetCombo->count()) ? m_swordPathListBox : BTInstallMgr::Tool::LocalConfig::targetList();
		if (m_targetCombo->count()) { //we already read in the list once, we have to use the Sword paths list items now because this list is newer
			list.clear();
			QListViewItemIterator it2( m_swordPathListBox );
			while (it2.current()) {
				list << it2.current()->text(0);

				++it2;
			}
		}
		else {
			list = BTInstallMgr::Tool::LocalConfig::targetList();
		}

		m_targetCombo->clear();
		for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
			QFileInfo fi(*it);
			if (fi.isDir() && fi.isWritable()) {
				m_targetCombo->insertItem( *it );
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
		int i = source.find(rep);
		if (i>=0) {
			source.remove(i, rep.length());
		}
		rep = i18n("[Remote]") + " ";
		i = source.find(rep);
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

		const QString message = i18n("You selected the following work(s): %1.\n\n"
									 "Do you really want to remove them from your system?").arg(moduleList.join(", "));

		if ((KMessageBox::warningYesNo(0, message, i18n("Warning")) == KMessageBox::Yes)) {  //Yes was pressed.
			sword::InstallMgr installMgr;
			QDict<sword::SWMgr> mgrDict; //maps config paths to SWMgr objects

			for ( QStringList::Iterator it = moduleList.begin(); it != moduleList.end(); ++it ) {
				if (CSwordModuleInfo* m = backend()->findModuleByName(*it)) { //module found?
					QString prefixPath = m->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
					QString dataPath = m->config(CSwordModuleInfo::DataPath);

					if (dataPath.left(2) == "./") {
						dataPath = dataPath.mid(2);
					}

					if (prefixPath.contains(dataPath)) { //remove module part to get the prefix path
						prefixPath = prefixPath.remove( prefixPath.find(dataPath), dataPath.length() );
					}
					else { //fall back to default Sword config path
						prefixPath = QString::fromLatin1(backend()->prefixPath);
					}

					sword::SWMgr* mgr = mgrDict[ prefixPath ];
					if (!mgr) { //create new mgr if it's not yet available
						mgrDict.insert(prefixPath, new sword::SWMgr(prefixPath.local8Bit()));
						mgr = mgrDict[ prefixPath ];
					}

					installMgr.removeModule(mgr, m->name().latin1());
				}
			}

			CPointers::backend()->reloadModules();
			populateRemoveModuleListView(); //rebuild the tree
			populateInstallModuleListView( currentInstallSource() ); //rebuild the tree

			//delete all mgrs
			mgrDict.setAutoDelete(true);
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
			m_removeModuleListView->addModule(
				(*it),
				(*it)->config(CSwordModuleInfo::ModuleVersion)
			);
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

		m_progressDialog = new KProgressDialog(0,0,i18n("Download"), QString::null, true);

		connect(
			m_progressDialog, SIGNAL(cancelClicked()),
			SLOT(slot_moduleRefreshProgressCancelClicked())
		);
		connect(
			&iMgr, SIGNAL(completed(const int, const int)),
			SLOT(slot_moduleRefreshCompleted(const int, const int))
		);

		m_progressDialog->progressBar()->setTotalSteps(100);
		m_progressDialog->setMinimumDuration(0); //show immediately
		m_progressDialog->setLabel( i18n("Downloading library information...") );

		if (BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)) {
			//   int errorCode = 0;
			if (!m_refreshedRemoteSources) {
				if (!iMgr.refreshRemoteSource( &is ) ) { //make sure the sources were updates sucessfully
					m_refreshedRemoteSources = true;
					success = true;
					m_progressDialog->progressBar()->setProgress(100); //make sure the dialog closes
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
		Q_ASSERT(m_installModuleListView);
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
			//   qWarning("adding module %s (%s)", (*it)->name().latin1(), (*it)->config(CSwordModuleInfo::AbsoluteDataPath).latin1());
			bool isUpdate = false;

			CSwordModuleInfo* const installedModule = local_backend->findModuleByName((*it)->name());
			if (installedModule) {
				//    qWarning("module is already installed in %s", installedModule->config(CSwordModuleInfo::AbsoluteDataPath).latin1());
			}
			//     Q_ASSERT(installedModule);

			if (installedModule) { //module already installed?
				//check whether it's an uodated module or just the same
				const SWVersion installedVersion(
					installedModule->config(CSwordModuleInfo::ModuleVersion).latin1()
				);

				const SWVersion newVersion(
					(*it)->config(CSwordModuleInfo::ModuleVersion).latin1()
				);

				isUpdate = (newVersion > installedVersion);
				if (!isUpdate) {
					//     qWarning("    mod %s is not an update", (*it)->name().latin1());
					continue;
				}
			}

			//   Q_ASSERT(installedModule);
			m_installModuleListView->addModule(
				(*it),
				installedModule
				? installedModule->config(CSwordModuleInfo::ModuleVersion)
				: QString::null
			);
		}
		m_installModuleListView->finish();
		return true;
	}

	void CSwordSetupDialog::slot_connectToSource() {
		if (!m_installModuleListPage) { //the widgets are not yet created
			m_installModuleListPage = new QWidget(0);

			QGridLayout* layout = new QGridLayout(m_installModuleListPage, 7, 2);
			layout->setMargin(5);
			layout->setSpacing(10);

			QLabel* installLabel = CToolClass::explanationLabel(m_installModuleListPage,
								   i18n("Install/update works - Step 2"),
								   i18n("Please choose the works which should be installed and/or updated and click the install button.")
															   );
			layout->addMultiCellWidget(installLabel, 0,0,0,1);
			layout->setRowStretch(0,0);

			m_installWidgetStack->addWidget(m_installModuleListPage);
			m_installModuleListPage->setMinimumSize(500,400);

			//insert a list box which contains all available remote modules
			BTInstallMgr iMgr;
			sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, currentInstallSource());

			m_installModuleListView = new CSwordSetupModuleListView(m_installModuleListPage, true, &is);
			layout->addMultiCellWidget( m_installModuleListView, 1,6,0,1);
			layout->setColStretch(0,5);
			layout->setRowStretch(1,5);

			connect(m_installModuleListView, SIGNAL(selectedModulesChanged()), SLOT(slot_installModulesChanged()));
		}

		if (populateInstallModuleListView( currentInstallSource() ) ){

			//code valid for already existing and newly created widgets
			disconnect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_connectToSource()));
			connect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_installModules()));
	
			m_installContinueButton->setText(i18n("Install works"));
			m_installContinueButton->setEnabled(false);
	
			m_installWidgetStack->raiseWidget(m_installModuleListPage);
	
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

	void CSwordSetupDialog::slot_installDeleteSource() {

		BTInstallMgr iMgr;
		sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source( &iMgr, currentInstallSource() );
		BTInstallMgr::Tool::RemoteConfig::removeSource( &iMgr, &is );

		populateInstallCombos();
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

		const QString message = i18n("You selected the following works: %1.\n\n\
 Do you really want to install them on your system?").arg(moduleList.join(", "));

		if ((KMessageBox::warningYesNo(0, message, i18n("Warning")) == KMessageBox::Yes)) {  //Yes was pressed.
			BTInstallMgr iMgr;
			m_currentInstallMgr = &iMgr;
			sword::InstallSource is = BTInstallMgr::Tool::RemoteConfig::source(&iMgr, currentInstallSource());

			qWarning("installing from %s/%s", is.source.c_str(), is.directory.c_str());
			QString target = m_targetCombo->currentText();

			//make sure target/mods.d and target/modules exist
			QDir dir(target.latin1());
			if (!dir.exists()) {
				dir.mkdir(target, true);
			}
			if (!dir.exists("modules")) {
				dir.mkdir("modules");
			}
			if (!dir.exists("mods.d")) {
				dir.mkdir("mods.d");
			}

			sword::SWMgr lMgr( target.latin1() );

			//module are removed in this section of code
			m_installedModuleCount = 0;
			m_progressDialog = new KProgressDialog(0,0,i18n("Download of work(s)"), QString::null, true);

			connect(
				m_progressDialog, SIGNAL(cancelClicked()),
				SLOT(slot_installProgressCancelClicked())
			);
			connect(
				&iMgr, SIGNAL(completed(const int, const int)),
				SLOT(installCompleted(const int, const int))
			);

			m_progressDialog->progressBar()->setTotalSteps(100 * moduleList.count());
			m_progressDialog->setMinimumDuration(0); //show immediately


			for ( QStringList::Iterator it = moduleList.begin(); (it != moduleList.end()) && !m_progressDialog->wasCancelled(); ++it, ++m_installedModuleCount ) {

				qWarning("installing %s", (*it).latin1());
				m_installingModule = *it;

				//check whether it's an update. If yes, remove existing module first
				CSwordModuleInfo* m = backend()->findModuleByName(*it);
				Q_ASSERT(!m);
				if (m) { //module found?
					QString prefixPath = m->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
					QString dataPath = m->config(CSwordModuleInfo::DataPath);
					if (dataPath.left(2) == "./") {
						dataPath = dataPath.mid(2);
					}

					if (prefixPath.contains(dataPath)) {
						prefixPath.remove( prefixPath.find(dataPath), dataPath.length() ); //complicated to work with Qt 3.0
						//prefixPath = prefixPath.replace(dataPath, ""); //old code working with Qt 3.2
					}
					else {
						prefixPath = QString::fromLatin1(backend()->prefixPath);
					}

					sword::SWMgr mgr(prefixPath.latin1());
					iMgr.removeModule(&mgr, m->name().latin1());
				}

				if (!m_progressDialog->wasCancelled()
						&& BTInstallMgr::Tool::RemoteConfig::isRemoteSource(&is)) {
					//        qWarning("calling install");
					int status = iMgr.installModule(&lMgr, 0, (*it).latin1(), &is);
					//         qWarning("status: %d", status);
					Q_ASSERT(status != -1);
				}
				else if (!m_progressDialog->wasCancelled()) { //local source
					iMgr.installModule(&lMgr, is.directory.c_str(), (*it).latin1());
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
			m_progressDialog->progressBar()->setProgress(total+100*m_installedModuleCount);
			m_progressDialog->setLabel( i18n("[%1]: %2% complete").arg(m_installingModule).arg(total) );
		}
	}

	void CSwordSetupDialog::slot_showInstallSourcePage() {
		connect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_connectToSource()));
		disconnect( m_installContinueButton, SIGNAL(clicked()), this, SLOT(slot_installModules()));
		m_installBackButton->setEnabled(false);

		m_installContinueButton->setText(i18n("Connect to library"));
		m_installContinueButton->setEnabled(true);

		m_installWidgetStack->raiseWidget(m_installSourcePage);
	}

	void CSwordSetupDialog::slot_swordEditClicked() {
		if (QListViewItem* i = m_swordPathListBox->currentItem()) {
			KURL url = KDirSelectDialog::selectDirectory(i->text(0), true);
			if (url.isValid()) {
				const QFileInfo fi( url.path() );
				if (!fi.exists() || !fi.isWritable()) {
					const int result = KMessageBox::warningYesNo(this, i18n("This directory is not writable, so works \
   can not be installed here using BibleTime. \
   Do you want to use this directory instead of the previous value?"));
					if (result == KMessageBox::No) {
						return;
					}
				}
				i->setText(0, url.path());
				m_swordSetupChanged = true;
				writeSwordConfig(); //to make sure other parts work with the new setting
				populateInstallCombos(); //update target list bof on install page
				populateRemoveModuleListView();
			}
		}
	}

	void CSwordSetupDialog::slot_swordAddClicked() {
		KURL url = KDirSelectDialog::selectDirectory(QString::null, true);
		if (url.isValid()) {
			const QFileInfo fi( url.path() );
			if (!fi.exists() || !fi.isWritable()) {
				const int result = KMessageBox::warningYesNo(this, i18n("This directory is not writable, \
   so works can not be installed here using BibleTime. \
   Do you want to add it to the list of module directories?"));
				if (result == KMessageBox::No) {
					return;
				}
			}
			(void)new QListViewItem(m_swordPathListBox, url.path());
			m_swordSetupChanged = true;
			writeSwordConfig(); //to make sure other parts work with the new setting
			populateInstallCombos();     //update target list bof on install page
			populateRemoveModuleListView();
		}
	}

	void CSwordSetupDialog::slot_swordRemoveClicked() {
		QListViewItem* i = m_swordPathListBox->currentItem();
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
			if ((*it).isEmpty()) {
				continue;
			}
			new QListViewItem(m_swordPathListBox, *it);
		}
		m_swordPathListBox->setCurrentItem( m_swordPathListBox->firstChild() );
	}

	void CSwordSetupDialog::slot_swordPathSelected() {
		m_swordEditPathButton->setEnabled( m_swordPathListBox->currentItem() );
	}

	const QString CSwordSetupDialog::currentInstallSource() {
		QString source = m_sourceCombo->currentText();
		QString rep = i18n("[Local]") + " ";
		int i = source.find(rep);
		if (i>=0) {
			source.remove(i, rep.length());
		}
		rep = i18n("[Remote]") + " ";
		i = source.find(rep);
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

	void CSwordSetupDialog::slot_moduleRefreshProgressCancelClicked() {
		Q_ASSERT(m_currentInstallMgr);
		if (m_currentInstallMgr) {
			m_currentInstallMgr->terminate();
		}
	}

	void CSwordSetupDialog::slot_moduleRefreshCompleted(const int /*total*/, const int current) {
		if (m_progressDialog) {
			m_progressDialog->progressBar()->setProgress(current);
		}
	}

} // NAMESPACE
