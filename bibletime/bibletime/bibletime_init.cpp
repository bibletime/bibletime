/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "bibletime.h"

#include "backend/btstringmgr.h"
#include "backend/cswordbackend.h"

#include "frontend/mainindex/cmainindex.h"
#include "frontend/cprofilemgr.h"
#include "frontend/cprofile.h"
#include "frontend/cmdiarea.h"
#include "frontend/kstartuplogo.h"
#include "frontend/cswordsetupdialog.h"
#include "frontend/cprinter.h"
#include "frontend/cmanageindiceswidget.h"

#include "backend/clanguagemgr.h"


#include "config.h"
#include "frontend/cbtconfig.h"
#include "frontend/cinfodisplay.h"

#include "util/cpointers.h"
#include "util/cresmgr.h"

//system includes
#include <stdlib.h>

//QT includes
#include <qpopupmenu.h>
#include <qsplitter.h>
#include <qguardedptr.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>

//KDE includes
#include <kaboutdata.h>
#include <kaccel.h>
#include <kaction.h>
#include <kapplication.h>
#include <kconfigbase.h>
#include <kdeversion.h>
#include <kglobal.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kstddirs.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kdeversion.h>

//Sword includes
#include <swlog.h>

using namespace InfoDisplay;
using namespace Profile;

/**Initializes the view of this widget*/
void BibleTime::initView() {
	KStartupLogo::setStatusMessage(i18n("Creating BibleTime's GUI") + QString("..."));

	m_mainSplitter = new QSplitter(this, "mainsplitter");
	m_mainSplitter->setChildrenCollapsible(false);
	setCentralWidget(m_mainSplitter);

	m_leftPaneSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
	m_leftPaneSplitter->setChildrenCollapsible(false);
	
	QVBox* vBox = new QVBox(m_leftPaneSplitter);
	vBox->setMinimumSize(100, 100);
	
	QLabel* bookshelfLabel = new QLabel( i18n("Bookshelf"), vBox );
	bookshelfLabel->setMargin(5);

	m_mainIndex = new CMainIndex(vBox);

	m_infoDisplay = new CInfoDisplay(m_leftPaneSplitter);
	CPointers::setInfoDisplay(m_infoDisplay);

	m_mdi = new CMDIArea(m_mainSplitter, "mdiarea" );
	m_mdi->setMinimumSize(100, 100);
	m_mdi->setFocusPolicy(ClickFocus);

	m_helpMenu = new KHelpMenu(this, KGlobal::instance()->aboutData(), true, actionCollection());
}

/** Initializes the action objects of the GUI */
void BibleTime::initActions() {
	KStartupLogo::setStatusMessage(i18n("Initializing menu- and toolbars") + QString("..."));
	KAction* action = 0;

	action = KStdAction::quit(kapp, SLOT( quit() ), actionCollection());
	action->setToolTip( CResMgr::mainMenu::file::quit::tooltip );

	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	action = new KAction(i18n("Search in &open work(s)"),
						 CResMgr::mainMenu::mainIndex::search::icon,
						 CResMgr::mainMenu::mainIndex::search::accel,
						 this, SLOT(slotSearchModules()), actionCollection(),
						 CResMgr::mainMenu::mainIndex::search::actionName
						);
	action->setToolTip( CResMgr::mainMenu::mainIndex::search::tooltip );

	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	action = new KAction(i18n("Search in standard &Bible"),
						 CResMgr::mainMenu::mainIndex::searchdefaultbible::icon,
						 CResMgr::mainMenu::mainIndex::searchdefaultbible::accel,
						 this, SLOT(slotSearchDefaultBible()), actionCollection(),
						 CResMgr::mainMenu::mainIndex::searchdefaultbible::actionName
						);
	action->setToolTip( CResMgr::mainMenu::mainIndex::searchdefaultbible::tooltip );

	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	m_viewToolbar_action = KStdAction::showToolbar(this, SLOT( slotToggleToolbar() ), actionCollection());
	m_viewToolbar_action->setToolTip( CResMgr::mainMenu::view::showToolBar::tooltip );

	#if KDE_VERSION_MINOR < 1

	m_viewToolbar_action->plugAccel( accel() );
	#endif


	m_viewMainIndex_action = new KToggleAction(i18n("&Show bookshelf"),
							 CResMgr::mainMenu::view::showMainIndex::icon,
							 CResMgr::mainMenu::view::showMainIndex::accel,
							 this, SLOT(slotToggleMainIndex()), actionCollection(),
							 CResMgr::mainMenu::view::showMainIndex::actionName);
	m_viewMainIndex_action->setToolTip( CResMgr::mainMenu::view::showMainIndex::tooltip );

	#if KDE_VERSION_MINOR < 1

	m_viewMainIndex_action->plugAccel( accel() );
	#endif

	m_viewInfoDisplay_action = new KToggleAction(i18n("Show &mag"),
							   CResMgr::mainMenu::view::showInfoDisplay::icon,
							   CResMgr::mainMenu::view::showInfoDisplay::accel,
							   this, SLOT(slotToggleInfoDisplay()), actionCollection(),
							   CResMgr::mainMenu::view::showInfoDisplay::actionName);
	m_viewMainIndex_action->setToolTip( CResMgr::mainMenu::view::showInfoDisplay::tooltip );

	#if KDE_VERSION_MINOR < 1

	m_viewInfoDisplay_action->plugAccel( accel() );
	#endif

	action = KStdAction::preferences(this, SLOT( slotSettingsOptions() ), actionCollection());
	action->setToolTip( CResMgr::mainMenu::settings::optionsDialog::tooltip );

	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	action = new KAction(i18n("Bookshelf &Manager"),
						 CResMgr::mainMenu::settings::swordSetupDialog::icon,
						 CResMgr::mainMenu::settings::swordSetupDialog::accel,
						 this, SLOT( slotSwordSetupDialog() ), actionCollection(),
						 CResMgr::mainMenu::settings::swordSetupDialog::actionName
						);
	action->setToolTip( CResMgr::mainMenu::settings::swordSetupDialog::tooltip );
	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	//delete help action if KDE created it
	if ( actionCollection()->action( KStdAction::stdName(KStdAction::ConfigureToolbars) )) {
		actionCollection()->remove
		(actionCollection()->action(
			 KStdAction::stdName(KStdAction::ConfigureToolbars))
		);
	}
	action = KStdAction::configureToolbars(this, SLOT( slotSettingsToolbar() ), actionCollection());
	action->setToolTip( CResMgr::mainMenu::settings::editToolBar::tooltip );
	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	m_windowArrangementMode_action = new KActionMenu(i18n("&Arrangement mode"),
							 CResMgr::mainMenu::window::arrangementMode::icon,
							 actionCollection(),
							 CResMgr::mainMenu::window::arrangementMode::actionName);
	m_windowArrangementMode_action->setDelayed( false );

	m_windowManualMode_action = new KToggleAction(i18n("&Manual mode"),
								CResMgr::mainMenu::window::arrangementMode::manual::icon,
								CResMgr::mainMenu::window::arrangementMode::manual::accel,
								this, SLOT(slotManualArrangementMode()), actionCollection(),
								CResMgr::mainMenu::window::arrangementMode::manual::actionName
												 );
	m_windowManualMode_action->setToolTip(
		CResMgr::mainMenu::window::arrangementMode::manual::tooltip
	);
	#if KDE_VERSION_MINOR < 1

	m_windowManualMode_action->plugAccel( accel() );
	#endif

	m_windowArrangementMode_action->insert( m_windowManualMode_action );


	m_windowAutoTileVertical_action = new KToggleAction(i18n("Auto-tile &vertically"),
									  CResMgr::mainMenu::window::arrangementMode::autoTileVertical::icon,
									  CResMgr::mainMenu::window::arrangementMode::autoTileVertical::accel,
									  this, SLOT(slotAutoTileVertical()), actionCollection(),
									  CResMgr::mainMenu::window::arrangementMode::autoTileVertical::actionName
													   );
	m_windowAutoTileVertical_action->setToolTip(
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::tooltip
	);
	#if KDE_VERSION_MINOR < 1

	m_windowAutoTileVertical_action->plugAccel( accel() );
	#endif

	m_windowArrangementMode_action->insert( m_windowAutoTileVertical_action );


	m_windowAutoTileHorizontal_action = new KToggleAction(i18n("Auto-tile &horizontally"),
										CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon,
										CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel,
										this, SLOT(slotAutoTileHorizontal()), actionCollection(),
										CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::actionName
														 );
	m_windowAutoTileHorizontal_action->setToolTip(
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::tooltip
	);
	#if KDE_VERSION_MINOR < 1

	m_windowAutoTileHorizontal_action->plugAccel( accel() );
	#endif

	m_windowArrangementMode_action->insert( m_windowAutoTileHorizontal_action );


	m_windowAutoCascade_action  = new KToggleAction(i18n("Auto-&cascade"),
								  CResMgr::mainMenu::window::arrangementMode::autoCascade::icon,
								  CResMgr::mainMenu::window::arrangementMode::autoCascade::accel,
								  this, SLOT(slotAutoCascade()), actionCollection(),
								  CResMgr::mainMenu::window::arrangementMode::autoCascade::actionName
												   );
	m_windowAutoCascade_action->setToolTip(
		CResMgr::mainMenu::window::arrangementMode::autoCascade::tooltip
	);
	#if KDE_VERSION_MINOR < 1
	//   qWarning("Plug accel");
	m_windowAutoCascade_action->plugAccel( accel() );
	#endif

	m_windowArrangementMode_action->insert( m_windowAutoCascade_action );

	m_windowCascade_action = new KAction(i18n("&Cascade"),
										 CResMgr::mainMenu::window::cascade::icon,
										 CResMgr::mainMenu::window::cascade::accel,
										 this, SLOT(slotCascade()), actionCollection(),
										 CResMgr::mainMenu::window::cascade::actionName
										);
	m_windowCascade_action->setToolTip( CResMgr::mainMenu::window::cascade::tooltip );

	#if KDE_VERSION_MINOR < 1

	m_windowCascade_action->plugAccel( accel() );
	#endif


	m_windowTileVertical_action = new KAction(i18n("Tile &vertically"),
								  CResMgr::mainMenu::window::tileVertical::icon,
								  CResMgr::mainMenu::window::tileVertical::accel,
								  this, SLOT(slotTileVertical()), actionCollection(),
								  CResMgr::mainMenu::window::tileVertical::actionName
											 );
	m_windowTileVertical_action->setToolTip( CResMgr::mainMenu::window::tileVertical::tooltip );
	#if KDE_VERSION_MINOR < 1

	m_windowTileVertical_action->plugAccel( accel() );
	#endif

	m_windowTileHorizontal_action = new KAction(i18n("Tile &horizontally"),
									CResMgr::mainMenu::window::tileHorizontal::icon,
									CResMgr::mainMenu::window::tileHorizontal::accel,
									this, SLOT(slotTileHorizontal()), actionCollection(),
									CResMgr::mainMenu::window::tileHorizontal::actionName
											   );
	m_windowTileHorizontal_action->setToolTip( CResMgr::mainMenu::window::tileHorizontal::tooltip );

	#if KDE_VERSION_MINOR < 1

	m_windowTileHorizontal_action->plugAccel( accel() );
	#endif


	m_windowCloseAll_action = new KAction(i18n("Cl&ose all"),
										  CResMgr::mainMenu::window::closeAll::icon,
										  CResMgr::mainMenu::window::closeAll::accel,
										  m_mdi, SLOT(deleteAll()), actionCollection(),
										  CResMgr::mainMenu::window::closeAll::actionName
										 );
	m_windowCloseAll_action->setToolTip( CResMgr::mainMenu::window::closeAll::tooltip );
	#if KDE_VERSION_MINOR < 1

	m_windowCloseAll_action->plugAccel( accel() );
	#endif


	m_windowSaveProfile_action = new KActionMenu(i18n("&Save session"),
								 CResMgr::mainMenu::window::saveProfile::icon,
								 actionCollection(),
								 CResMgr::mainMenu::window::saveProfile::actionName
												);
	m_windowSaveProfile_action->setToolTip( CResMgr::mainMenu::window::saveProfile::tooltip );
	m_windowSaveProfile_action->setDelayed( false );
	#if KDE_VERSION_MINOR < 1

	m_windowSaveProfile_action->plugAccel( accel() );
	#endif


	m_windowSaveToNewProfile_action = new KAction(i18n("Save as &new session"),
									  CResMgr::mainMenu::window::saveToNewProfile::icon,
									  CResMgr::mainMenu::window::saveToNewProfile::accel,
									  this, SLOT(saveToNewProfile()), actionCollection(),
									  CResMgr::mainMenu::window::saveToNewProfile::actionName
												 );
	m_windowSaveToNewProfile_action->setToolTip( CResMgr::mainMenu::window::saveToNewProfile::tooltip );
	#if KDE_VERSION_MINOR < 1

	m_windowSaveToNewProfile_action->plugAccel( accel() );
	#endif


	m_windowLoadProfile_action = new KActionMenu(i18n("&Load session"),
								 CResMgr::mainMenu::window::loadProfile::icon,
								 actionCollection(),
								 CResMgr::mainMenu::window::loadProfile::actionName
												);
	m_windowLoadProfile_action->setToolTip(	CResMgr::mainMenu::window::loadProfile::tooltip	);
	m_windowLoadProfile_action->setDelayed( false );
	#if KDE_VERSION_MINOR < 1

	m_windowLoadProfile_action->plugAccel( accel() );
	#endif

	m_windowDeleteProfile_action = new KActionMenu(i18n("&Delete session"),
								   CResMgr::mainMenu::window::deleteProfile::icon,
								   actionCollection(),
								   CResMgr::mainMenu::window::deleteProfile::actionName
												  );
	m_windowLoadProfile_action->setToolTip( CResMgr::mainMenu::window::deleteProfile::tooltip );
	m_windowLoadProfile_action->setDelayed( false );
	#if KDE_VERSION_MINOR < 1

	m_windowDeleteProfile_action->plugAccel( accel() );
	#endif

	m_windowFullscreen_action = new KToggleAction(i18n("&Fullscreen mode"),
								CResMgr::mainMenu::window::showFullscreen::icon,
								CResMgr::mainMenu::window::showFullscreen::accel,
								this, SLOT(toggleFullscreen()), actionCollection(),
								CResMgr::mainMenu::window::showFullscreen::actionName
												 );
	m_windowFullscreen_action->setToolTip( CResMgr::mainMenu::window::showFullscreen::tooltip );
	m_windowFullscreen_action->setShortcutConfigurable(true);
	#if KDE_VERSION_MINOR < 1

	m_windowFullscreen_action->plugAccel( accel() );
	#endif

	//connect the profile popups to the right slots
	KPopupMenu* loadPopup = m_windowLoadProfile_action->popupMenu();
	KPopupMenu* savePopup = m_windowSaveProfile_action->popupMenu();
	KPopupMenu* deletePopup = m_windowDeleteProfile_action->popupMenu();

	connect(loadPopup, SIGNAL(activated(int)), SLOT(loadProfile(int)));
	connect(savePopup, SIGNAL(activated(int)), SLOT(saveProfile(int)));
	connect(deletePopup, SIGNAL(activated(int)), SLOT(deleteProfile(int)));

	refreshProfileMenus();

	if ( actionCollection()->action( KStdAction::stdName(KStdAction::HelpContents) )) {  //delete help action if KDE created it
		actionCollection()->remove
		(actionCollection()->action(KStdAction::stdName(KStdAction::HelpContents)) );
	}

	action = new KAction(i18n("&Handbook"),
						 CResMgr::mainMenu::help::handbook::icon,
						 CResMgr::mainMenu::help::handbook::accel,
						 this, SLOT(openOnlineHelp_Handbook()), actionCollection(),
						 CResMgr::mainMenu::help::handbook::actionName
						);
	action->setToolTip( CResMgr::mainMenu::help::handbook::tooltip );

	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	action = new KAction(i18n("&Bible Study Howto"),
						 CResMgr::mainMenu::help::bibleStudyHowTo::icon,
						 CResMgr::mainMenu::help::bibleStudyHowTo::accel,
						 this, SLOT(openOnlineHelp_Howto()), actionCollection(),
						 CResMgr::mainMenu::help::bibleStudyHowTo::actionName
						);
	action->setToolTip( CResMgr::mainMenu::help::bibleStudyHowTo::tooltip );

	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	if ( actionCollection()->action( KStdAction::stdName( KStdAction::WhatsThis ) ) ) {  //delete "What's this" action if KDE created it already
		KAction* action = actionCollection()->action(KStdAction::stdName( KStdAction::WhatsThis ));
		actionCollection()->remove
		( action );
	}


	if ( actionCollection()->action( KStdAction::stdName( KStdAction::ReportBug ) ) ) { //delete Report Bug action if KDE created it
		actionCollection()->remove
		(actionCollection()->action(KStdAction::stdName( KStdAction::ReportBug )));
	}
	action = KStdAction::reportBug(m_helpMenu, SLOT(reportBug()), actionCollection());
	action->setToolTip(CResMgr::mainMenu::help::bugreport::tooltip);
	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	action = new KAction(i18n("&Daily tip"),
						 CResMgr::mainMenu::help::dailyTip::icon,
						 CResMgr::mainMenu::help::dailyTip::accel,
						 this, SLOT(slotHelpTipOfDay()), actionCollection(),
						 CResMgr::mainMenu::help::dailyTip::actionName
						);
	action->setToolTip(CResMgr::mainMenu::help::dailyTip::tooltip);
	#if KDE_VERSION_MINOR < 1

	action->plugAccel( accel() );
	#endif

	if ( actionCollection()->action( KStdAction::stdName( KStdAction::AboutApp ) ) ) {  //delete About KDE action if KDE created it
		actionCollection()->remove
		(actionCollection()->action(KStdAction::stdName( KStdAction::AboutApp )));
	}
	action = KStdAction::aboutApp(m_helpMenu, SLOT(aboutApplication()), actionCollection());
	action->setToolTip(CResMgr::mainMenu::help::aboutBibleTime::tooltip);

	//delete About KDE action if KDE created it already
	if ( actionCollection()->action( KStdAction::stdName( KStdAction::AboutKDE ) ) ) {  //delete About KDE action if KDE created it
		actionCollection()->remove
		(actionCollection()->action(KStdAction::stdName( KStdAction::AboutKDE )));
	}
	action = KStdAction::aboutKDE(m_helpMenu, SLOT(aboutKDE()), actionCollection());
	action->setToolTip(CResMgr::mainMenu::help::aboutKDE::tooltip);
}

/** Initializes the menubar of BibleTime. */
void BibleTime::initMenubar() {
	//get the window and edit menus using the actions and their properties
	m_windowMenu = dynamic_cast<QPopupMenu*>(m_windowCloseAll_action->container(0));
}

/** Initializes the SIGNAL / SLOT connections */
void BibleTime::initConnections() {
	connect(m_mdi, SIGNAL(sigSetToplevelCaption(const QString&)),
			this, SLOT(setPlainCaption(const QString&)));
	connect(m_mdi, SIGNAL(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)),
			this, SLOT(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)));

	Q_ASSERT(m_windowMenu);
	if (m_windowMenu) {
		connect(m_windowMenu, SIGNAL(aboutToShow()),
				this, SLOT(slotWindowMenuAboutToShow()));
	}
	else {
		qWarning("Main window: can't find window menu");
	}


	connect(
		m_mainIndex, SIGNAL(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)),
		this, SLOT(createReadDisplayWindow(ListCSwordModuleInfo,const QString&))
	);
	connect(
		m_mainIndex, SIGNAL(createWriteDisplayWindow(CSwordModuleInfo*, const QString&, const CDisplayWindow::WriteWindowType&)),
		this, SLOT(createWriteDisplayWindow(CSwordModuleInfo*,const QString&, const CDisplayWindow::WriteWindowType&))
	);
	connect(m_mainIndex, SIGNAL(signalSwordSetupChanged()), this, SLOT(slotSwordSetupChanged()));
}

/** Initializes the backend */
void BibleTime::initBackends() {
	KStartupLogo::setStatusMessage(i18n("Initializing Sword") + QString("..."));

	StringMgr::setSystemStringMgr( new BTStringMgr() );
	SWLog::getSystemLog()->setLogLevel(1);

	m_backend = new CSwordBackend();
	m_backend->booknameLanguage( CBTConfig::get
									 (CBTConfig::language) );

	CPointers::setBackend(m_backend);
	const CSwordBackend::LoadError errorCode = m_backend->initModules();

	m_moduleList = 0;
	if ( errorCode == CSwordBackend::NoError ) { //no error
		m_moduleList = &(m_backend->moduleList());
	}
	else {
		m_moduleList = 0;
		//show error message that initBackend failed
		switch (errorCode) {
			case CSwordBackend::NoSwordConfig: //mods.d or mods.conf missing
			{
				KStartupLogo::hideSplash();
				BookshelfManager::CSwordSetupDialog dlg;
				dlg.showPart( BookshelfManager::CSwordSetupDialog::Sword );
				dlg.exec();
				break;
			}

			case CSwordBackend::NoModules: //no modules installed, but config exists
			{
				KStartupLogo::hideSplash();
				BookshelfManager::CSwordSetupDialog dlg;
				dlg.showPart( BookshelfManager::CSwordSetupDialog::Install );
				dlg.exec();
				break;
			}

			default: //unknown error
			{
				KStartupLogo::hideSplash();
				BookshelfManager::CSwordSetupDialog dlg;
				dlg.showPart( BookshelfManager::CSwordSetupDialog::Sword );
				dlg.exec();
				break;
			}
		}
	}

	KStartupLogo::setStatusMessage(i18n("Checking indices") + QString("..."));
	//This function will 
	// - delete all orphaned indexes (no module present) if autoDeleteOrphanedIndices is true
	// - delete all indices of modules where hasIndex() returns false
	BookshelfManager::CManageIndicesWidget::deleteOrphanedIndices();
	
}

/** Apply the settings given by the profile p*/
void BibleTime::applyProfileSettings( CProfile* p ) {
	Q_ASSERT(p);
	if (!p) return;

	if (m_initialized) { //on startup KDE sets the main geometry
		//see polish(), where m_initialized is set and the KDE methods are called for window resize
		
		//first Main Window state
		m_windowFullscreen_action->setChecked( p->fullscreen() );  //set the fullscreen button state
		toggleFullscreen(); //either showFullscreen or showNormal
		if (p->maximized()) KMainWindow::showMaximized(); //if maximized, then also call showMaximized
	
		//Then Main Window geometry
		KMainWindow::resize( p->geometry().size() ); //Don't use KMainWindowInterface::resize
		KMainWindow::move( p->geometry().topLeft() );//Don't use KMainWindowInterface::move
}
}

/** Stores the settings of the mainwindow in the profile p */
void BibleTime::storeProfileSettings( CProfile* p ) {
	Q_ASSERT(p && m_windowFullscreen_action);
	if (!p || !m_windowFullscreen_action) return;
	
	p->setFullscreen( m_windowFullscreen_action->isChecked() );
	p->setMaximized( this->KMainWindow::isMaximized() );

	QRect geometry;
	geometry.setTopLeft(pos());
	geometry.setSize(size());
	p->setGeometry(geometry);
}
