/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "bibletime.h"
#include "config.h"
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"
#include "backend/managers/btstringmgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/clanguagemgr.h"
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

//QT includes
#include <QSplitter>
#include <QPointer>
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
#include <khelpmenu.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kmenu.h>


//Sword includes
#include <swlog.h>
#include <stringmgr.h>


using namespace InfoDisplay;
using namespace Profile;



/**Initializes the view of this widget*/
void BibleTime::initView()
{
	KStartupLogo::setStatusMessage(i18n("Creating BibleTime's GUI") + QString("..."));

	m_mainSplitter = new QSplitter(this);
	m_mainSplitter->setChildrenCollapsible(false);
	setCentralWidget(m_mainSplitter);

	m_leftPaneSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
	m_leftPaneSplitter->setChildrenCollapsible(false);
	m_mainSplitter->addWidget(m_leftPaneSplitter);
	
	m_mainIndex = new CMainIndex(this);

	m_infoDisplay = new CInfoDisplay(m_leftPaneSplitter);
	m_leftPaneSplitter->addWidget(m_mainIndex);
	m_leftPaneSplitter->addWidget(m_infoDisplay);
	CPointers::setInfoDisplay(m_infoDisplay);

	m_mdi = new CMDIArea(m_mainSplitter);
	m_mainSplitter->addWidget(m_mdi);

	m_mdi->setMinimumSize(100, 100);
	m_mdi->setFocusPolicy(Qt::ClickFocus);

	m_helpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData(), true, actionCollection());
}

void BibleTime::initAction(KAction* action, QString text, QString icon, KShortcut accel, QString tooltip, const char* actionName, const char* slot )
{
	action->setText(text);
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(icon));
	action->setShortcut(accel);
	if (tooltip != QString::null) {
		action->setToolTip(tooltip);
	}
	actionCollection()->addAction(actionName, action);
	if (slot) {
		QObject::connect( action, SIGNAL(triggered()), this, slot );
	}
}

/** Initializes the action objects of the GUI */
void BibleTime::initActions()
{
	KStartupLogo::setStatusMessage(i18n("Initializing menu- and toolbars") + QString("..."));
	KAction* action = 0;
	KActionCollection* ac = actionCollection();
	
	action = KStandardAction::quit(kapp, SLOT( quit() ), ac);
	action->setToolTip( CResMgr::mainMenu::file::quit::tooltip );


	initAction(
		new KAction(ac),
		i18n("Search in &open work(s)"),
		CResMgr::mainMenu::mainIndex::search::icon,
		CResMgr::mainMenu::mainIndex::search::accel,
		CResMgr::mainMenu::mainIndex::search::tooltip,
		CResMgr::mainMenu::mainIndex::search::actionName,
		SLOT( slotSearchModules() )
	);
	

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


	//--------------------------Window arrangement actions---------------------------------------

	m_windowArrangementMode_action = new KActionMenu(KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::arrangementMode::icon)), i18n("&Arrangement mode"), ac);
	ac->addAction(CResMgr::mainMenu::window::arrangementMode::actionName, m_windowArrangementMode_action);
	m_windowArrangementMode_action->setDelayed( false );


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


	m_windowAutoCascade_action = new KToggleAction(ac);
	initAction(
		m_windowAutoCascade_action,
		i18n("Auto-&cascade"),
		CResMgr::mainMenu::window::arrangementMode::autoCascade::icon,
		CResMgr::mainMenu::window::arrangementMode::autoCascade::accel,
		CResMgr::mainMenu::window::arrangementMode::autoCascade::tooltip,
		CResMgr::mainMenu::window::arrangementMode::autoCascade::actionName,
		SLOT( slotAutoCascade() )
	);
	m_windowArrangementMode_action->addAction( m_windowAutoCascade_action );


	m_windowCascade_action = new KAction(ac);
	initAction(
		m_windowCascade_action,
		i18n("&Cascade"),
		CResMgr::mainMenu::window::cascade::icon,
		CResMgr::mainMenu::window::cascade::accel,
		CResMgr::mainMenu::window::cascade::tooltip,
		CResMgr::mainMenu::window::cascade::actionName,
		SLOT( slotCascade() )
	);


	m_windowTileVertical_action = new KAction(ac);
	initAction(
		m_windowTileVertical_action,
		i18n("Tile &vertically"),
		CResMgr::mainMenu::window::tileVertical::icon,
		CResMgr::mainMenu::window::tileVertical::accel,
		CResMgr::mainMenu::window::tileVertical::tooltip,
		CResMgr::mainMenu::window::tileVertical::actionName,
		SLOT( slotTileVertical() )
	);


	m_windowTileHorizontal_action = new KAction(ac);
	initAction(
		m_windowTileHorizontal_action,
		i18n("Tile &horizontally"),
		CResMgr::mainMenu::window::tileHorizontal::icon,
		CResMgr::mainMenu::window::tileHorizontal::accel,
		CResMgr::mainMenu::window::tileHorizontal::tooltip,
		CResMgr::mainMenu::window::tileHorizontal::actionName,
		SLOT( slotTileHorizontal() )
	);


	m_windowCloseAll_action = new KAction(ac);
	initAction(
		m_windowCloseAll_action,
		i18n("Cl&ose all"),
		CResMgr::mainMenu::window::closeAll::icon,
		CResMgr::mainMenu::window::closeAll::accel,
		CResMgr::mainMenu::window::closeAll::tooltip,
		CResMgr::mainMenu::window::closeAll::actionName,
		0
	);
	QObject::connect(m_windowCloseAll_action, SIGNAL(triggered()), m_mdi, SLOT( deleteAll() ) );


	//--------------------------Profile/session actions----------------------------------------

	m_windowSaveProfile_action = new KActionMenu(KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::saveProfile::icon)), i18n("&Save session"), ac);
	ac->addAction(CResMgr::mainMenu::window::saveProfile::actionName, m_windowSaveProfile_action);
	m_windowSaveProfile_action->setDelayed( false );
	
	m_windowSaveToNewProfile_action = new KAction(ac);
	initAction(
		m_windowSaveToNewProfile_action,
		i18n("Save as &new session"),
		CResMgr::mainMenu::window::saveToNewProfile::icon,
		CResMgr::mainMenu::window::saveToNewProfile::accel,
		CResMgr::mainMenu::window::saveToNewProfile::tooltip,
		CResMgr::mainMenu::window::saveToNewProfile::actionName,
		SLOT( saveToNewProfile() )
	);	

	m_windowLoadProfile_action = new KActionMenu(KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::loadProfile::icon)), i18n("&Load session"), ac);
	ac->addAction(CResMgr::mainMenu::window::loadProfile::actionName, m_windowLoadProfile_action);
	m_windowLoadProfile_action->setDelayed( false );
	

	m_windowDeleteProfile_action = new KActionMenu(KIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::deleteProfile::icon)), i18n("&Delete session"), ac);
	ac->addAction(CResMgr::mainMenu::window::deleteProfile::actionName, m_windowDeleteProfile_action);
	m_windowLoadProfile_action->setDelayed( false );


	m_windowFullscreen_action = new KToggleAction(ac);
	initAction(
		m_windowFullscreen_action,
		i18n("&Fullscreen mode"),
		CResMgr::mainMenu::window::showFullscreen::icon,
		CResMgr::mainMenu::window::showFullscreen::accel,
		CResMgr::mainMenu::window::showFullscreen::tooltip,
		CResMgr::mainMenu::window::showFullscreen::actionName,
		SLOT(toggleFullscreen())
	);

	m_windowFullscreen_action->setShortcutConfigurable(true);
	

	//connect the profile popups to the right slots
	KMenu* loadPopup = m_windowLoadProfile_action->popupMenu();
	KMenu* savePopup = m_windowSaveProfile_action->popupMenu();
	KMenu* deletePopup = m_windowDeleteProfile_action->popupMenu();

	QObject::connect(loadPopup, SIGNAL(triggered(QAction*)), SLOT(loadProfile(QAction*)));
	QObject::connect(savePopup, SIGNAL(triggered(QAction*)), SLOT(saveProfile(QAction*)));
	QObject::connect(deletePopup, SIGNAL(triggered(QAction*)), SLOT(deleteProfile(QAction*)));

	refreshProfileMenus();

	if ( ac->action( KStandardAction::stdName(KStandardAction::HelpContents) )) {  //delete help action if KDE created it
		ac->removeAction
		(ac->action(KStandardAction::stdName(KStandardAction::HelpContents)) );
	}

	//----------------------------Help menu actions-------------------------------
	initAction(
		new KAction(ac),
		i18n("&Handbook"),
		CResMgr::mainMenu::help::handbook::icon,
		CResMgr::mainMenu::help::handbook::accel,
		CResMgr::mainMenu::help::handbook::tooltip,
		CResMgr::mainMenu::help::handbook::actionName,
		SLOT( openOnlineHelp_Handbook() )
	);

	initAction(
		new KAction(ac),
		i18n("&Bible Study Howto"),
		CResMgr::mainMenu::help::bibleStudyHowTo::icon,
		CResMgr::mainMenu::help::bibleStudyHowTo::accel,
		CResMgr::mainMenu::help::bibleStudyHowTo::tooltip,
		CResMgr::mainMenu::help::bibleStudyHowTo::actionName,
		SLOT( openOnlineHelp_Howto() )
	);

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
	
	if (ac->action(KStandardAction::stdName(KStandardAction::AboutApp) )) {  //delete About KDE action if KDE created it
		ac->removeAction
		(ac->action(KStandardAction::stdName( KStandardAction::AboutApp ) ));
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
	//Q_ASSERT(m_windowMenu);
}

/** Initializes the SIGNAL / SLOT connections */
void BibleTime::initConnections() {
	QObject::connect(m_mdi, SIGNAL(sigSetToplevelCaption(const QString&)),
			this, SLOT(setPlainCaption(const QString&)));
	QObject::connect(m_mdi, SIGNAL(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)),
			this, SLOT(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)));

	if (m_windowMenu) {
		QObject::connect(m_windowMenu, SIGNAL(aboutToShow()),
				this, SLOT(slotWindowMenuAboutToShow()));
	}
	else {
		qWarning("Main window: can't find window menu");
	}


	QObject::connect(
		m_mainIndex, SIGNAL(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)),
		this, SLOT(createReadDisplayWindow(ListCSwordModuleInfo,const QString&))
	);
	QObject::connect(
		m_mainIndex, SIGNAL(createWriteDisplayWindow(CSwordModuleInfo*, const QString&, const CDisplayWindow::WriteWindowType&)),
		this, SLOT(createWriteDisplayWindow(CSwordModuleInfo*,const QString&, const CDisplayWindow::WriteWindowType&))
	);
	QObject::connect(m_mainIndex, SIGNAL(signalSwordSetupChanged()), this, SLOT(slotSwordSetupChanged()));
}

/** Initializes the backend */
void BibleTime::initBackends() {
	KStartupLogo::setStatusMessage(i18n("Initializing Sword") + QString("..."));
	qDebug("BibleTime::initBackends");
	
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
				qDebug("case CSwordBackend::NoSwordConfig");
				BookshelfManager::CSwordSetupDialog dlg;
				dlg.showPart( BookshelfManager::CSwordSetupDialog::Sword );
				dlg.exec();
				break;
			}

			case CSwordBackend::NoModules: //no modules installed, but config exists
			{
				KStartupLogo::hideSplash();
				qDebug("case CSwordBackend::NoModules");
				BookshelfManager::CSwordSetupDialog dlg;
				dlg.showPart( BookshelfManager::CSwordSetupDialog::Install );
				dlg.exec();
				break;
			}

			default: //unknown error
			{
				KStartupLogo::hideSplash();
				qDebug("unknown error");
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
	qDebug("BibleTime::applyProfileSettings");
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

