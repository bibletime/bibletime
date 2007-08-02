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
#include "config.h"

#include "util/cpointers.h"
#include "util/cresmgr.h"

#include "backend/btstringmgr.h"
#include "backend/cswordbackend.h"
#include "backend/clanguagemgr.h"

#include "frontend/mainindex/cmainindex.h"

#include "frontend/profile/cprofilemgr.h"
#include "frontend/profile/cprofile.h"

#include "frontend/bookshelfmanager/cswordsetupdialog.h"
#include "frontend/bookshelfmanager/cmanageindiceswidget.h"

#include "frontend/cmdiarea.h"
#include "frontend/kstartuplogo.h"
#include "frontend/cprinter.h"
#include "frontend/cbtconfig.h"
#include "frontend/cinfodisplay.h"


//system includes
#include <stdlib.h>

//QT includes
//#include <q3popupmenu.h>
#include <QSplitter>
#include <QPointer>
//#include <q3listview.h>
//#include <qlayout.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenu>



//KDE includes
#include <kaboutdata.h>
#include <kaction.h>
#include <kstdaction.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <kconfigbase.h>
#include <kglobal.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kstandarddirs.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kmenu.h>
//#include <kdeversion.h>

//Sword includes
#include <swlog.h>
#include <stringmgr.h>

using namespace InfoDisplay;
using namespace Profile;

/**Initializes the view of this widget*/
void BibleTime::initView() {
	KStartupLogo::setStatusMessage(i18n("Creating BibleTime's GUI") + QString("..."));

	m_mainSplitter = new QSplitter(this);
	m_mainSplitter->setChildrenCollapsible(false);
	setCentralWidget(m_mainSplitter);

	m_leftPaneSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
	m_leftPaneSplitter->setChildrenCollapsible(false);
	
	QVBoxLayout* layout = new QVBoxLayout(m_leftPaneSplitter);
	QLabel* bookshelfLabel = new QLabel( i18n("Bookshelf"));
	bookshelfLabel->setMargin(5);
	layout->addWidget(bookshelfLabel);
	m_mainIndex = new CMainIndex(m_leftPaneSplitter);
	layout->addWidget(m_mainIndex);
	
	QWidget* mainindexwidget = new QWidget;
	mainindexwidget->setLayout(layout);
	mainindexwidget->setMinimumSize(100, 100);

	m_infoDisplay = new CInfoDisplay(m_leftPaneSplitter);
	CPointers::setInfoDisplay(m_infoDisplay);

	m_mdi = new CMDIArea(m_mainSplitter);
	m_mdi->setMinimumSize(100, 100);
	m_mdi->setFocusPolicy(Qt::ClickFocus);

	m_helpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData(), true, actionCollection());
}

void BibleTime::initAction(KAction* action, QString text, QString icon, KShortcut accel, QString tooltip, const char* actionName, const char* slot )
{
	action->setText(text);
	action->setIcon(KIcon(icon));
	action->setShortcut(accel);
	if (tooltip != QString::null) {
		action->setToolTip(tooltip);
	}
	actionCollection()->addAction(actionName, action);
	if (slot) {
		QObject::connect( action, SIGNAL(triggered), this, slot );
	}
}

/** Initializes the action objects of the GUI */
void BibleTime::initActions() {
	KStartupLogo::setStatusMessage(i18n("Initializing menu- and toolbars") + QString("..."));
	KAction* action = 0;
	KActionCollection* ac = actionCollection();
	
	action = KStandardAction::quit(kapp, SLOT( quit() ), ac);
	action->setToolTip( CResMgr::mainMenu::file::quit::tooltip );

	//action = new KAction(CResMgr::mainMenu::mainIndex::search::icon, i18n("Search in &open work(s)"), ac);
	//action->setShortcut(CResMgr::mainMenu::mainIndex::search::accel);
	//action->setToolTip( CResMgr::mainMenu::mainIndex::search::tooltip );
	//QObject::connect( action, SIGNAL(triggered), this, SLOT(slotSearchModules()) );
	//ac->addAction(CResMgr::mainMenu::mainIndex::search::actionName, action);
	
	initAction(
		new KAction(ac),
		i18n("Search in &open work(s)"),
		CResMgr::mainMenu::mainIndex::search::icon,
		CResMgr::mainMenu::mainIndex::search::accel,
		CResMgr::mainMenu::mainIndex::search::tooltip,
		CResMgr::mainMenu::mainIndex::search::actionName,
		SLOT( slotSearchModules() )
	);
	
	
	//action = new KAction(i18n("Search in standard &Bible"),
	//					 CResMgr::mainMenu::mainIndex::searchdefaultbible::icon,
	//					 CResMgr::mainMenu::mainIndex::searchdefaultbible::accel,
	//					 this, SLOT(slotSearchDefaultBible()), ac,
	//					 CResMgr::mainMenu::mainIndex::searchdefaultbible::actionName
	//					);
	//action->setToolTip( CResMgr::mainMenu::mainIndex::searchdefaultbible::tooltip );
	
	initAction(
		new KAction(ac),
		i18n("Search in standard &Bible"),
		CResMgr::mainMenu::mainIndex::searchdefaultbible::icon,
		CResMgr::mainMenu::mainIndex::searchdefaultbible::accel,
		CResMgr::mainMenu::mainIndex::searchdefaultbible::tooltip,
		CResMgr::mainMenu::mainIndex::searchdefaultbible::actionName,
		SLOT(slotSearchDefaultBible())
	);
	

	//I don't understand why this does not exist any more
	//m_viewToolbar_action = KStandardAction::showToolbar(this, SLOT( slotToggleToolbar() ), ac);
	m_viewToolbar_action = KStandardAction::create(KStandardAction::ShowToolbar, this, SLOT( slotToggleToolbar() ), ac);
	m_viewToolbar_action->setToolTip( CResMgr::mainMenu::view::showToolBar::tooltip );


	//m_viewMainIndex_action = new KToggleAction(i18n("&Show bookshelf"),
	//						 CResMgr::mainMenu::view::showMainIndex::icon,
	//						 CResMgr::mainMenu::view::showMainIndex::accel,
	//						 this, SLOT(slotToggleMainIndex()), ac,
	//						 CResMgr::mainMenu::view::showMainIndex::actionName);
	//m_viewMainIndex_action->setToolTip( CResMgr::mainMenu::view::showMainIndex::tooltip );
	
	m_viewMainIndex_action = new KToggleAction(ac);
	initAction(
		m_viewMainIndex_action,
		i18n("&Show bookshelf"),
		CResMgr::mainMenu::view::showMainIndex::icon,
		CResMgr::mainMenu::view::showMainIndex::accel,
		CResMgr::mainMenu::view::showMainIndex::tooltip,
		CResMgr::mainMenu::view::showMainIndex::actionName,
		SLOT(slotToggleMainIndex())
	);
	

	//m_viewInfoDisplay_action = new KToggleAction(i18n("Show &mag"),
	//						   CResMgr::mainMenu::view::showInfoDisplay::icon,
	//						   CResMgr::mainMenu::view::showInfoDisplay::accel,
	//						   this, SLOT(slotToggleInfoDisplay()), ac,
	//						   CResMgr::mainMenu::view::showInfoDisplay::actionName);
	//m_viewMainIndex_action->setToolTip( CResMgr::mainMenu::view::showInfoDisplay::tooltip );
	m_viewInfoDisplay_action = new KToggleAction(ac);
	initAction(
		m_viewInfoDisplay_action,
		i18n("Show &mag"),
		CResMgr::mainMenu::view::showInfoDisplay::icon,
		CResMgr::mainMenu::view::showInfoDisplay::accel,
		CResMgr::mainMenu::view::showInfoDisplay::tooltip,
		CResMgr::mainMenu::view::showInfoDisplay::actionName,
		SLOT(slotToggleInfoDisplay())
	);
	

	action = KStandardAction::preferences(this, SLOT( slotSettingsOptions() ), ac);
	action->setToolTip( CResMgr::mainMenu::settings::optionsDialog::tooltip );

	

	//action = new KAction(i18n("Bookshelf &Manager"),
	//					 CResMgr::mainMenu::settings::swordSetupDialog::icon,
	//					 CResMgr::mainMenu::settings::swordSetupDialog::accel,
	//					 this, SLOT( slotSwordSetupDialog() ), ac,
	//					 CResMgr::mainMenu::settings::swordSetupDialog::actionName
	//					);
	//action->setToolTip( CResMgr::mainMenu::settings::swordSetupDialog::tooltip );
	initAction(
		new KAction(ac),
		i18n("Bookshelf &Manager"),
		CResMgr::mainMenu::settings::swordSetupDialog::icon,
		CResMgr::mainMenu::settings::swordSetupDialog::accel,
		CResMgr::mainMenu::settings::swordSetupDialog::tooltip,
		CResMgr::mainMenu::settings::swordSetupDialog::actionName,
		SLOT( slotSwordSetupDialog() )
	);

	//delete help action if KDE created it
	if ( ac->action( KStandardAction::stdName(KStandardAction::ConfigureToolbars) )) {
		ac->removeAction
		(ac->action(
			 KStandardAction::stdName(KStandardAction::ConfigureToolbars))
		);
	}
	action = KStandardAction::configureToolbars(this, SLOT( slotSettingsToolbar() ), ac);
	action->setToolTip( CResMgr::mainMenu::settings::editToolBar::tooltip );


	//m_windowArrangementMode_action = new KActionMenu(i18n("&Arrangement mode"),
	//						 CResMgr::mainMenu::window::arrangementMode::icon,
	//						 ac,
	//						 CResMgr::mainMenu::window::arrangementMode::actionName);
	m_windowArrangementMode_action = new KActionMenu(KIcon(CResMgr::mainMenu::window::arrangementMode::icon), i18n("&Arrangement mode"), ac);
	ac->addAction(CResMgr::mainMenu::window::arrangementMode::actionName, action);
	m_windowArrangementMode_action->setDelayed( false );


	//m_windowManualMode_action = new KToggleAction(i18n("&Manual mode"),
	//							CResMgr::mainMenu::window::arrangementMode::manual::icon,
	//							CResMgr::mainMenu::window::arrangementMode::manual::accel,
	//							this, SLOT(slotManualArrangementMode()), ac,
	//							CResMgr::mainMenu::window::arrangementMode::manual::actionName
	//											 );
	//m_windowManualMode_action->setToolTip(
	//	CResMgr::mainMenu::window::arrangementMode::manual::tooltip
	//);
	m_windowManualMode_action = new KToggleAction(ac);
	initAction(
		m_windowManualMode_action,
		i18n("&Manual mode"),
		CResMgr::mainMenu::window::arrangementMode::manual::icon,
		CResMgr::mainMenu::window::arrangementMode::manual::accel,
		CResMgr::mainMenu::window::arrangementMode::manual::tooltip,
		CResMgr::mainMenu::window::arrangementMode::manual::actionName,
		SLOT( slotManualArrangementMode() )
	);

	m_windowArrangementMode_action->addAction( m_windowManualMode_action );


	//m_windowAutoTileVertical_action = new KToggleAction(i18n("Auto-tile &vertically"),
	//								  CResMgr::mainMenu::window::arrangementMode::autoTileVertical::icon,
	//								  CResMgr::mainMenu::window::arrangementMode::autoTileVertical::accel,
	//								  this, SLOT(slotAutoTileVertical()), ac,
	//								  CResMgr::mainMenu::window::arrangementMode::autoTileVertical::actionName
	//												   );
	//m_windowAutoTileVertical_action->setToolTip(
	//	CResMgr::mainMenu::window::arrangementMode::autoTileVertical::tooltip
	//);
	m_windowAutoTileVertical_action = new KToggleAction(ac);
	initAction(
		m_windowAutoTileVertical_action,
		i18n("Auto-tile &vertically"),
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::icon,
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::accel,
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::tooltip,
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::actionName,
		SLOT( slotAutoTileVertical() )
	);


	m_windowArrangementMode_action->addAction( m_windowAutoTileVertical_action );


	//m_windowAutoTileHorizontal_action = new KToggleAction(i18n("Auto-tile &horizontally"),
	//									CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon,
	//									CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel,
	//									this, SLOT(slotAutoTileHorizontal()), ac,
	//									CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::actionName
	//													 );
	//m_windowAutoTileHorizontal_action->setToolTip(
	//	CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::tooltip
	//);
	m_windowAutoTileHorizontal_action = new KToggleAction(ac);
	initAction(
		m_windowAutoTileHorizontal_action,
		i18n("Auto-tile &horizontally"),
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::tooltip,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::actionName,
		SLOT( slotAutoTileHorizontal() )
	);
	

	m_windowArrangementMode_action->addAction( m_windowAutoTileHorizontal_action );


	//m_windowAutoCascade_action  = new KToggleAction(i18n("Auto-&cascade"),
	//							  CResMgr::mainMenu::window::arrangementMode::autoCascade::icon,
	//							  CResMgr::mainMenu::window::arrangementMode::autoCascade::accel,
	//							  this, SLOT(slotAutoCascade()), ac,
	//							  CResMgr::mainMenu::window::arrangementMode::autoCascade::actionName
	//											   );
	//m_windowAutoCascade_action->setToolTip(
	//	CResMgr::mainMenu::window::arrangementMode::autoCascade::tooltip
	//);
	m_windowAutoTileHorizontal_action = new KToggleAction(ac);
	initAction(
		m_windowAutoTileHorizontal_action,
		i18n("Auto-&cascade"),
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::tooltip,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::actionName,
		SLOT( slotAutoTileHorizontal() )
	);

	m_windowArrangementMode_action->addAction( m_windowAutoCascade_action );

	//m_windowCascade_action = new KAction(i18n("&Cascade"),
	//									 CResMgr::mainMenu::window::cascade::icon,
	//									 CResMgr::mainMenu::window::cascade::accel,
	//									 this, SLOT(slotCascade()), ac,
	//									 CResMgr::mainMenu::window::cascade::actionName
	//									);
	//m_windowCascade_action->setToolTip( CResMgr::mainMenu::window::cascade::tooltip );

	

	//m_windowTileVertical_action = new KAction(i18n("Tile &vertically"),
	//							  CResMgr::mainMenu::window::tileVertical::icon,
	//							  CResMgr::mainMenu::window::tileVertical::accel,
	//							  this, SLOT(slotTileVertical()), ac,
	//							  CResMgr::mainMenu::window::tileVertical::actionName
	//										 );
	//m_windowTileVertical_action->setToolTip( CResMgr::mainMenu::window::tileVertical::tooltip );

	//m_windowTileHorizontal_action = new KAction(i18n("Tile &horizontally"),
	//								CResMgr::mainMenu::window::tileHorizontal::icon,
	//								CResMgr::mainMenu::window::tileHorizontal::accel,
	//								this, SLOT(slotTileHorizontal()), ac,
	//								CResMgr::mainMenu::window::tileHorizontal::actionName
	//										   );
	//m_windowTileHorizontal_action->setToolTip( CResMgr::mainMenu::window::tileHorizontal::tooltip );

	


	//m_windowCloseAll_action = new KAction(i18n("Cl&ose all"),
	//									  CResMgr::mainMenu::window::closeAll::icon,
	//									  CResMgr::mainMenu::window::closeAll::accel,
	//									  m_mdi, SLOT(deleteAll()), ac,
	//									  CResMgr::mainMenu::window::closeAll::actionName
	//									 );
	//m_windowCloseAll_action->setToolTip( CResMgr::mainMenu::window::closeAll::tooltip );
	

	//m_windowSaveProfile_action = new KActionMenu(i18n("&Save session"),
	//							 CResMgr::mainMenu::window::saveProfile::icon,
	//							 ac,
	//							 CResMgr::mainMenu::window::saveProfile::actionName
	//											);
	//m_windowSaveProfile_action->setToolTip( CResMgr::mainMenu::window::saveProfile::tooltip );
	
	m_windowSaveProfile_action->setDelayed( false );
	

	//m_windowSaveToNewProfile_action = new KAction(i18n("Save as &new session"),
	//								  CResMgr::mainMenu::window::saveToNewProfile::icon,
	//								  CResMgr::mainMenu::window::saveToNewProfile::accel,
	//								  this, SLOT(saveToNewProfile()), ac,
	//								  CResMgr::mainMenu::window::saveToNewProfile::actionName
	//											 );
	//m_windowSaveToNewProfile_action->setToolTip( CResMgr::mainMenu::window::saveToNewProfile::tooltip );
	

	//m_windowLoadProfile_action = new KActionMenu(i18n("&Load session"),
	//							 CResMgr::mainMenu::window::loadProfile::icon,
	//							 ac,
	//							 CResMgr::mainMenu::window::loadProfile::actionName
	//											);
	//m_windowLoadProfile_action->setToolTip(	CResMgr::mainMenu::window::loadProfile::tooltip	);
	
	m_windowLoadProfile_action->setDelayed( false );
	
	//m_windowDeleteProfile_action = new KActionMenu(i18n("&Delete session"),
	//							   CResMgr::mainMenu::window::deleteProfile::icon,
	//							   ac,
	//							   CResMgr::mainMenu::window::deleteProfile::actionName
	//											  );
	//m_windowLoadProfile_action->setToolTip( CResMgr::mainMenu::window::deleteProfile::tooltip );
	m_windowLoadProfile_action->setDelayed( false );


	//m_windowFullscreen_action = new KToggleAction(i18n("&Fullscreen mode"),
	//							CResMgr::mainMenu::window::showFullscreen::icon,
	//							CResMgr::mainMenu::window::showFullscreen::accel,
	//							this, SLOT(toggleFullscreen()), ac,
	//							CResMgr::mainMenu::window::showFullscreen::actionName
	//											 );
	//m_windowFullscreen_action->setToolTip( CResMgr::mainMenu::window::showFullscreen::tooltip );
	m_windowFullscreen_action->setShortcutConfigurable(true);
	

	//connect the profile popups to the right slots
	KMenu* loadPopup = m_windowLoadProfile_action->popupMenu();
	KMenu* savePopup = m_windowSaveProfile_action->popupMenu();
	KMenu* deletePopup = m_windowDeleteProfile_action->popupMenu();

	connect(loadPopup, SIGNAL(activated(int)), SLOT(loadProfile(int)));
	connect(savePopup, SIGNAL(activated(int)), SLOT(saveProfile(int)));
	connect(deletePopup, SIGNAL(activated(int)), SLOT(deleteProfile(int)));

	refreshProfileMenus();

	if ( ac->action( KStandardAction::stdName(KStandardAction::HelpContents) )) {  //delete help action if KDE created it
		ac->removeAction
		(ac->action(KStandardAction::stdName(KStandardAction::HelpContents)) );
	}

	//action = new KAction(i18n("&Handbook"),
	//					 CResMgr::mainMenu::help::handbook::icon,
	//					 CResMgr::mainMenu::help::handbook::accel,
	//					 this, SLOT(openOnlineHelp_Handbook()), ac,
	//					 CResMgr::mainMenu::help::handbook::actionName
	//					);
	//action->setToolTip( CResMgr::mainMenu::help::handbook::tooltip );

	
	//action = new KAction(i18n("&Bible Study Howto"),
	//					 CResMgr::mainMenu::help::bibleStudyHowTo::icon,
	//					 CResMgr::mainMenu::help::bibleStudyHowTo::accel,
	//					 this, SLOT(openOnlineHelp_Howto()), ac,
	//					 CResMgr::mainMenu::help::bibleStudyHowTo::actionName
	//					);
	//action->setToolTip( CResMgr::mainMenu::help::bibleStudyHowTo::tooltip );

	if ( ac->action( KStandardAction::stdName( KStandardAction::WhatsThis ) ) ) {  //delete "What's this" action if KDE created it already
		QAction* action = ac->action(KStandardAction::stdName( KStandardAction::WhatsThis ));
		ac->removeAction
		( action );
	}


	if ( ac->action( KStandardAction::stdName( KStandardAction::ReportBug ) ) ) { //delete Report Bug action if KDE created it
		ac->removeAction
		(ac->action(KStandardAction::stdName( KStandardAction::ReportBug )));
	}
	action = KStandardAction::reportBug(m_helpMenu, SLOT(reportBug()), ac);
	action->setToolTip(CResMgr::mainMenu::help::bugreport::tooltip);
	

	//action = new KAction(i18n("&Daily tip"),
	//					 CResMgr::mainMenu::help::dailyTip::icon,
	//					 CResMgr::mainMenu::help::dailyTip::accel,
	//					 this, SLOT(slotHelpTipOfDay()), ac,
	//					 CResMgr::mainMenu::help::dailyTip::actionName
	//					);
	//action->setToolTip(CResMgr::mainMenu::help::dailyTip::tooltip);
	

	if ( ac->action( KStandardAction::stdName( KStandardAction::AboutApp ) ) ) {  //delete About KDE action if KDE created it
		ac->removeAction
		(ac->action(KStandardAction::stdName( KStandardAction::AboutApp )));
	}
	action = KStandardAction::aboutApp(m_helpMenu, SLOT(aboutApplication()), ac);
	action->setToolTip(CResMgr::mainMenu::help::aboutBibleTime::tooltip);

	//delete About KDE action if KDE created it already
	if ( ac->action( KStandardAction::stdName( KStandardAction::AboutKDE ) ) ) {  //delete About KDE action if KDE created it
		ac->removeAction
		(ac->action(KStandardAction::stdName( KStandardAction::AboutKDE )));
	}
	action = KStandardAction::aboutKDE(m_helpMenu, SLOT(aboutKDE()), ac);
	action->setToolTip(CResMgr::mainMenu::help::aboutKDE::tooltip);
}

/** Initializes the menubar of BibleTime. */
void BibleTime::initMenubar() {
	//get the window and edit menus using the actions and their properties
	//action->associatedWidgets().value(0)
	m_windowMenu = dynamic_cast<QMenu*>(m_windowCloseAll_action->associatedWidgets().value(0));
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

	sword::StringMgr::setSystemStringMgr( new BTStringMgr() );
	sword::SWLog::getSystemLog()->setLogLevel(1);

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
