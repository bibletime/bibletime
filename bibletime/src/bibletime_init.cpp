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
#include <QMenuBar>
#include <QToolBar>
#include <QApplication>

//KDE includes
#include <kaboutdata.h>
#include <klocale.h>

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

//	m_helpMenu = new KHelpMenu(this, KGlobal::mainComponent().aboutData(), true, actionCollection());
}

QAction* BibleTime::initAction(QAction* action, QString text, QString icon, QKeySequence accel, QString tooltip, const char* slot )
{
	action->setText(text);
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(icon));
	action->setShortcut(accel);
	if (tooltip != QString::null) action->setToolTip(tooltip);
// 	actionCollection()->addAction(actionName, action);
	if (slot) QObject::connect( action, SIGNAL(triggered()), this, slot );
	return action;
}

/** Initializes the action objects of the GUI */
void BibleTime::initActions()
{
	KStartupLogo::setStatusMessage(i18n("Initializing menu- and toolbars") + QString("..."));

	QMenu* fileMenu = menuBar()->addMenu(i18n("&File"));
	QMenu* viewMenu = menuBar()->addMenu(i18n("&View"));
	QMenu* searchMenu = menuBar()->addMenu(i18n("&Search"));
	m_windowMenu = menuBar()->addMenu(i18n("&Window"));
	QMenu* settingsMenu = menuBar()->addMenu(i18n("Se&ttings"));
	QMenu* helpMenu = menuBar()->addMenu(i18n("&Help"));
	
	m_mainToolBar = addToolBar(i18n("BibleTime"));
	m_mainToolBar->setFloatable(false);
	m_mainToolBar->setMovable(false);

	QAction* tmp = new QAction(this);
	initAction(
		tmp,
		i18n("Quit"),
		QString("exit.svg"),
		QKeySequence(Qt::CTRL + Qt::Key_Q),
		i18n("Exit BibleTime"),
		SLOT( close() )
	);
	fileMenu->addAction(tmp);
	m_mainToolBar->addAction(tmp);
	m_mainToolBar->addSeparator();
	
	m_viewToolbar_action = new QAction(this);
	m_viewToolbar_action->setCheckable(true);
	m_viewToolbar_action->setChecked(true);
	viewMenu->addAction(initAction(
		m_viewToolbar_action,
		i18n("&Show toolbar"),
		"",
		QKeySequence(),
		"",
		SLOT(slotToggleToolbar())));
	
	viewMenu->addSeparator();
 
	m_viewMainIndex_action = new QAction(this);
	m_viewMainIndex_action->setCheckable(true);
	viewMenu->addAction(initAction(
		m_viewMainIndex_action,
		i18n("&Show bookshelf"),
		CResMgr::mainMenu::view::showMainIndex::icon,
		CResMgr::mainMenu::view::showMainIndex::accel,
		CResMgr::mainMenu::view::showMainIndex::tooltip,
		SLOT(slotToggleMainIndex())));
	m_mainToolBar->addAction(m_viewMainIndex_action);

	m_viewInfoDisplay_action = new QAction(this);
	m_viewInfoDisplay_action->setCheckable(true);
	viewMenu->addAction(initAction(
		m_viewInfoDisplay_action,
		i18n("Show &mag"),
		CResMgr::mainMenu::view::showInfoDisplay::icon,
		CResMgr::mainMenu::view::showInfoDisplay::accel,
		CResMgr::mainMenu::view::showInfoDisplay::tooltip,
		SLOT(slotToggleInfoDisplay())
	));
	viewMenu->addSeparator();
	m_mainToolBar->addAction(m_viewInfoDisplay_action);
	m_mainToolBar->addSeparator();
	
	m_windowFullscreen_action = new QAction(this);
	m_windowFullscreen_action->setCheckable(true);
	viewMenu->addAction(initAction(
		m_windowFullscreen_action,
		i18n("&Fullscreen mode"),
		CResMgr::mainMenu::window::showFullscreen::icon,
		CResMgr::mainMenu::window::showFullscreen::accel,
		CResMgr::mainMenu::window::showFullscreen::tooltip,
		SLOT(toggleFullscreen()))
	);
	m_mainToolBar->addAction(m_windowFullscreen_action);

	tmp = initAction(
		new QAction(this),
		i18n("Search in &open work(s)"),
		CResMgr::mainMenu::mainIndex::search::icon,
		CResMgr::mainMenu::mainIndex::search::accel,
		CResMgr::mainMenu::mainIndex::search::tooltip,
		SLOT( slotSearchModules() )
	);
	searchMenu->addAction(tmp);
	m_mainToolBar->addAction(tmp);
	m_mainToolBar->addSeparator();

	searchMenu->addAction(initAction(
		new QAction(this),
		i18n("Search in standard &Bible"),
		CResMgr::mainMenu::mainIndex::searchdefaultbible::icon,
		CResMgr::mainMenu::mainIndex::searchdefaultbible::accel,
		CResMgr::mainMenu::mainIndex::searchdefaultbible::tooltip,
		SLOT(slotSearchDefaultBible())));

	m_windowSaveProfileMenu = new QMenu(i18n("&Save session"));
	m_windowMenu->addMenu(m_windowSaveProfileMenu);

	m_windowSaveToNewProfile_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowSaveToNewProfile_action,
		i18n("Save as &new session"),
		CResMgr::mainMenu::window::saveToNewProfile::icon,
		CResMgr::mainMenu::window::saveToNewProfile::accel,
		CResMgr::mainMenu::window::saveToNewProfile::tooltip,
		SLOT( saveToNewProfile() ))
	);	

	m_windowLoadProfileMenu = new QMenu(i18n("&Load session"));
	m_windowMenu->addMenu(m_windowLoadProfileMenu);

	m_windowDeleteProfileMenu = new QMenu(i18n("&Delete session"));
	m_windowMenu->addMenu(m_windowDeleteProfileMenu);
	
	QObject::connect(m_windowLoadProfileMenu, SIGNAL(triggered(QAction*)), SLOT(loadProfile(QAction*)));
	QObject::connect(m_windowSaveProfileMenu, SIGNAL(triggered(QAction*)), SLOT(saveProfile(QAction*)));
	QObject::connect(m_windowDeleteProfileMenu, SIGNAL(triggered(QAction*)), SLOT(deleteProfile(QAction*)));

	refreshProfileMenus();

	m_windowMenu->addSeparator();

//--------------------------Window arrangement actions---------------------------------------

	QMenu* arrangementMenu = new QMenu(i18n("&Arrangement mode"));
	m_windowMenu->addMenu(arrangementMenu);

	m_windowManualMode_action = new QAction(this);
	m_windowManualMode_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowManualMode_action,
		i18n("&Manual mode"),
		CResMgr::mainMenu::window::arrangementMode::manual::icon,
		CResMgr::mainMenu::window::arrangementMode::manual::accel,
		CResMgr::mainMenu::window::arrangementMode::manual::tooltip,
		SLOT( slotManualArrangementMode() ))
	);

	m_windowAutoTileVertical_action = new QAction(this);
	m_windowAutoTileVertical_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowAutoTileVertical_action,
		i18n("Auto-tile &vertically"),
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::icon,
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::accel,
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::tooltip,
		SLOT( slotAutoTileVertical() ))
	);

	m_windowAutoTileHorizontal_action = new QAction(this);
	m_windowAutoTileHorizontal_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowAutoTileHorizontal_action,
		i18n("Auto-tile &horizontally"),
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::tooltip,
		SLOT( slotAutoTileHorizontal() ))
	);

	m_windowAutoCascade_action = new QAction(this);
	m_windowAutoCascade_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowAutoCascade_action,
		i18n("Auto-&cascade"),
		CResMgr::mainMenu::window::arrangementMode::autoCascade::icon,
		CResMgr::mainMenu::window::arrangementMode::autoCascade::accel,
		CResMgr::mainMenu::window::arrangementMode::autoCascade::tooltip,
		SLOT( slotAutoCascade() ))
	);

	m_windowCascade_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowCascade_action,
		i18n("&Cascade"),
		CResMgr::mainMenu::window::cascade::icon,
		CResMgr::mainMenu::window::cascade::accel,
		CResMgr::mainMenu::window::cascade::tooltip,
		SLOT( slotCascade() ))
	);

	m_windowTileVertical_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowTileVertical_action,
		i18n("Tile &vertically"),
		CResMgr::mainMenu::window::tileVertical::icon,
		CResMgr::mainMenu::window::tileVertical::accel,
		CResMgr::mainMenu::window::tileVertical::tooltip,
		SLOT( slotTileVertical() ))
	);

	m_windowTileHorizontal_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowTileHorizontal_action,
		i18n("Tile &horizontally"),
		CResMgr::mainMenu::window::tileHorizontal::icon,
		CResMgr::mainMenu::window::tileHorizontal::accel,
		CResMgr::mainMenu::window::tileHorizontal::tooltip,
		SLOT( slotTileHorizontal() ))
	);

	m_windowCloseAll_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowCloseAll_action,
		i18n("Cl&ose all"),
		CResMgr::mainMenu::window::closeAll::icon,
		CResMgr::mainMenu::window::closeAll::accel,
		CResMgr::mainMenu::window::closeAll::tooltip,
		0)
	);
	QObject::connect(m_windowCloseAll_action, SIGNAL(triggered()), m_mdi, SLOT( deleteAll() ) );

 	settingsMenu->addAction(initAction(
		new QAction(this),
		i18n("&Configure BibleTime"),
		"configure.svg",
		QKeySequence(),
		"",
		SLOT( slotSettingsOptions() )));
	settingsMenu->addSeparator();
 
 	settingsMenu->addAction(initAction(
		new QAction(this),
		i18n("Bookshelf &Manager"),
		CResMgr::mainMenu::settings::swordSetupDialog::icon,
		CResMgr::mainMenu::settings::swordSetupDialog::accel,
		CResMgr::mainMenu::settings::swordSetupDialog::tooltip,
		SLOT( slotSwordSetupDialog() )));

	tmp = initAction(
		new QAction(this),
		i18n("&Handbook"),
		CResMgr::mainMenu::help::handbook::icon,
		CResMgr::mainMenu::help::handbook::accel,
		CResMgr::mainMenu::help::handbook::tooltip,
		SLOT( openOnlineHelp_Handbook() )
	);
	helpMenu->addAction(tmp);
	m_mainToolBar->addAction(tmp);

	helpMenu->addAction(initAction(
		new QAction(this),
		i18n("&Bible Study Howto"),
		CResMgr::mainMenu::help::bibleStudyHowTo::icon,
		CResMgr::mainMenu::help::bibleStudyHowTo::accel,
		CResMgr::mainMenu::help::bibleStudyHowTo::tooltip,
		SLOT( openOnlineHelp_Howto() ))
	);
	
// 	helpMenu->addSeparator();
// 
// 	helpMenu->addAction(initAction(
// 		new QAction(this),
// 		i18n("&Report a bug"),
// 		"",
// 		QKeySequence(),
// 		"",
// 		SLOT(reportBug()))
// 	);
// 	
// 	helpMenu->addSeparator();
// 
// 	helpMenu->addAction(initAction(
// 		new QAction(this),
// 		i18n("&About BibleTime"),
// 		"",
// 		QKeySequence(),
// 		"",
// 		SLOT(aboutApplication()))
// 	);
}

/** Initializes the SIGNAL / SLOT connections */
void BibleTime::initConnections() {
 	QObject::connect(m_mdi, SIGNAL(sigSetToplevelCaption(const QString&)),
 			this, SLOT(setPlainCaption(const QString&)));
	QObject::connect(m_mdi, SIGNAL(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)),
			this, SLOT(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)));

	if (m_windowMenu) {
		QObject::connect(m_windowMenu, SIGNAL(aboutToShow()), this, SLOT(slotWindowMenuAboutToShow()));
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

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slot_aboutToQuit()));
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

// 	if (m_initialized) { //on startup KDE sets the main geometry
		//see polish(), where m_initialized is set and the KDE methods are called for window resize
		//first Main Window state
		m_windowFullscreen_action->setChecked( p->fullscreen() );  //set the fullscreen button state
		toggleFullscreen(); //either showFullscreen or showNormal
		if (p->maximized()) QMainWindow::showMaximized(); //if maximized, then also call showMaximized
		//Then Main Window geometry
		QMainWindow::resize( p->geometry().size() ); //Don't use QMainWindowInterface::resize
		QMainWindow::move( p->geometry().topLeft() );//Don't use QMainWindowInterface::move
// 	}
}

/** Stores the settings of the mainwindow in the profile p */
void BibleTime::storeProfileSettings( CProfile* p ) {
	Q_ASSERT(p && m_windowFullscreen_action);
	if (!p || !m_windowFullscreen_action) return;
	
	p->setFullscreen( m_windowFullscreen_action->isChecked() );
	p->setMaximized( this->QMainWindow::isMaximized() );

	QRect geometry;
	geometry.setTopLeft(pos());
	geometry.setSize(size());
	p->setGeometry(geometry);
}

