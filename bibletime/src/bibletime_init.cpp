/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
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
#include "frontend/cmdiarea.h"
#include "frontend/kstartuplogo.h"
#include "frontend/cprinter.h"
#include "backend/config/cbtconfig.h"
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
#include <QDebug>

//Sword includes
#include <swlog.h>

using namespace InfoDisplay;
using namespace Profile;

/**Initializes the view of this widget*/
void BibleTime::initView()
{
	KStartupLogo::setStatusMessage(tr("Creating BibleTime's GUI") + QString("..."));

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
	KStartupLogo::setStatusMessage(tr("Initializing menu- and toolbars") + QString("..."));

	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
	QMenu* searchMenu = menuBar()->addMenu(tr("&Search"));
	m_windowMenu = menuBar()->addMenu(tr("&Window"));
	QMenu* settingsMenu = menuBar()->addMenu(tr("Se&ttings"));
	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	
	m_mainToolBar = addToolBar(tr("BibleTime"));
	m_mainToolBar->setFloatable(false);
	m_mainToolBar->setMovable(false);

	QAction* tmp = new QAction(this);
	initAction(
		tmp,
		tr("Quit"),
		QString("exit.svg"),
		QKeySequence(Qt::CTRL + Qt::Key_Q),
		tr("Exit BibleTime"),
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
		tr("&Show toolbar"),
		"",
		QKeySequence(),
		"",
		SLOT(slotToggleToolbar())));
	
	viewMenu->addSeparator();
 
	m_viewMainIndex_action = new QAction(this);
	m_viewMainIndex_action->setCheckable(true);
	viewMenu->addAction(initAction(
		m_viewMainIndex_action,
		tr("&Show bookshelf"),
		CResMgr::mainMenu::view::showMainIndex::icon,
		CResMgr::mainMenu::view::showMainIndex::accel,
		CResMgr::mainMenu::view::showMainIndex::tooltip,
		SLOT(slotToggleMainIndex())));
	m_mainToolBar->addAction(m_viewMainIndex_action);

	m_viewInfoDisplay_action = new QAction(this);
	m_viewInfoDisplay_action->setCheckable(true);
	viewMenu->addAction(initAction(
		m_viewInfoDisplay_action,
		tr("Show &mag"),
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
		tr("&Fullscreen mode"),
		CResMgr::mainMenu::window::showFullscreen::icon,
		CResMgr::mainMenu::window::showFullscreen::accel,
		CResMgr::mainMenu::window::showFullscreen::tooltip,
		SLOT(toggleFullscreen()))
	);
	m_mainToolBar->addAction(m_windowFullscreen_action);

	tmp = initAction(
		new QAction(this),
		tr("Search in &open work(s)"),
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
		tr("Search in standard &Bible"),
		CResMgr::mainMenu::mainIndex::searchdefaultbible::icon,
		CResMgr::mainMenu::mainIndex::searchdefaultbible::accel,
		CResMgr::mainMenu::mainIndex::searchdefaultbible::tooltip,
		SLOT(slotSearchDefaultBible())));

	m_windowSaveProfileMenu = new QMenu(tr("&Save session"));
	m_windowMenu->addMenu(m_windowSaveProfileMenu);

	m_windowSaveToNewProfile_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowSaveToNewProfile_action,
		tr("Save as &new session"),
		CResMgr::mainMenu::window::saveToNewProfile::icon,
		CResMgr::mainMenu::window::saveToNewProfile::accel,
		CResMgr::mainMenu::window::saveToNewProfile::tooltip,
		SLOT( saveToNewProfile() ))
	);	

	m_windowLoadProfileMenu = new QMenu(tr("&Load session"));
	m_windowMenu->addMenu(m_windowLoadProfileMenu);

	m_windowDeleteProfileMenu = new QMenu(tr("&Delete session"));
	m_windowMenu->addMenu(m_windowDeleteProfileMenu);
	
	QObject::connect(m_windowLoadProfileMenu, SIGNAL(triggered(QAction*)), SLOT(loadProfile(QAction*)));
	QObject::connect(m_windowSaveProfileMenu, SIGNAL(triggered(QAction*)), SLOT(saveProfile(QAction*)));
	QObject::connect(m_windowDeleteProfileMenu, SIGNAL(triggered(QAction*)), SLOT(deleteProfile(QAction*)));

	refreshProfileMenus();

	m_windowMenu->addSeparator();

//--------------------------Window arrangement actions---------------------------------------

	QMenu* arrangementMenu = new QMenu(tr("&Arrangement mode"));
	m_windowMenu->addMenu(arrangementMenu);

	m_windowManualMode_action = new QAction(this);
	m_windowManualMode_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowManualMode_action,
		tr("&Manual mode"),
		CResMgr::mainMenu::window::arrangementMode::manual::icon,
		CResMgr::mainMenu::window::arrangementMode::manual::accel,
		CResMgr::mainMenu::window::arrangementMode::manual::tooltip,
		SLOT( slotManualArrangementMode() ))
	);

	m_windowAutoTileVertical_action = new QAction(this);
	m_windowAutoTileVertical_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowAutoTileVertical_action,
		tr("Auto-tile &vertically"),
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::icon,
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::accel,
		CResMgr::mainMenu::window::arrangementMode::autoTileVertical::tooltip,
		SLOT( slotAutoTileVertical() ))
	);

	m_windowAutoTileHorizontal_action = new QAction(this);
	m_windowAutoTileHorizontal_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowAutoTileHorizontal_action,
		tr("Auto-tile &horizontally"),
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel,
		CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::tooltip,
		SLOT( slotAutoTileHorizontal() ))
	);

	m_windowAutoCascade_action = new QAction(this);
	m_windowAutoCascade_action->setCheckable(true);
	arrangementMenu->addAction(initAction(
		m_windowAutoCascade_action,
		tr("Auto-&cascade"),
		CResMgr::mainMenu::window::arrangementMode::autoCascade::icon,
		CResMgr::mainMenu::window::arrangementMode::autoCascade::accel,
		CResMgr::mainMenu::window::arrangementMode::autoCascade::tooltip,
		SLOT( slotAutoCascade() ))
	);

	m_windowCascade_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowCascade_action,
		tr("&Cascade"),
		CResMgr::mainMenu::window::cascade::icon,
		CResMgr::mainMenu::window::cascade::accel,
		CResMgr::mainMenu::window::cascade::tooltip,
		SLOT( slotCascade() ))
	);

	m_windowTileVertical_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowTileVertical_action,
		tr("Tile &vertically"),
		CResMgr::mainMenu::window::tileVertical::icon,
		CResMgr::mainMenu::window::tileVertical::accel,
		CResMgr::mainMenu::window::tileVertical::tooltip,
		SLOT( slotTileVertical() ))
	);

	m_windowTileHorizontal_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowTileHorizontal_action,
		tr("Tile &horizontally"),
		CResMgr::mainMenu::window::tileHorizontal::icon,
		CResMgr::mainMenu::window::tileHorizontal::accel,
		CResMgr::mainMenu::window::tileHorizontal::tooltip,
		SLOT( slotTileHorizontal() ))
	);

	m_windowCloseAll_action = new QAction(this);
	m_windowMenu->addAction(initAction(
		m_windowCloseAll_action,
		tr("Cl&ose all"),
		CResMgr::mainMenu::window::closeAll::icon,
		CResMgr::mainMenu::window::closeAll::accel,
		CResMgr::mainMenu::window::closeAll::tooltip,
		0)
	);
	QObject::connect(m_windowCloseAll_action, SIGNAL(triggered()), m_mdi, SLOT( deleteAll() ) );

 	settingsMenu->addAction(initAction(
		new QAction(this),
		tr("&Configure BibleTime"),
		"configure.svg",
		QKeySequence(),
		"",
		SLOT( slotSettingsOptions() )));
	settingsMenu->addSeparator();
 
 	settingsMenu->addAction(initAction(
		new QAction(this),
		tr("Bookshelf &Manager"),
		CResMgr::mainMenu::settings::swordSetupDialog::icon,
		CResMgr::mainMenu::settings::swordSetupDialog::accel,
		CResMgr::mainMenu::settings::swordSetupDialog::tooltip,
		SLOT( slotSwordSetupDialog() )));

	tmp = initAction(
		new QAction(this),
		tr("&Handbook"),
		CResMgr::mainMenu::help::handbook::icon,
		CResMgr::mainMenu::help::handbook::accel,
		CResMgr::mainMenu::help::handbook::tooltip,
		SLOT( openOnlineHelp_Handbook() )
	);
	helpMenu->addAction(tmp);
	m_mainToolBar->addAction(tmp);

	helpMenu->addAction(initAction(
		new QAction(this),
		tr("&Bible Study Howto"),
		CResMgr::mainMenu::help::bibleStudyHowTo::icon,
		CResMgr::mainMenu::help::bibleStudyHowTo::accel,
		CResMgr::mainMenu::help::bibleStudyHowTo::tooltip,
		SLOT( openOnlineHelp_Howto() ))
	);
	
// 	helpMenu->addSeparator();
// 
// 	helpMenu->addAction(initAction(
// 		new QAction(this),
// 		tr("&Report a bug"),
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
// 		tr("&About BibleTime"),
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
		qWarning() << "Main window: can't find window menu";
	}

	QObject::connect(m_mainIndex, SIGNAL(createReadDisplayWindow(ListCSwordModuleInfo, const QString&)),
		this, SLOT(createReadDisplayWindow(ListCSwordModuleInfo,const QString&))
	);
	QObject::connect(m_mainIndex, SIGNAL(createWriteDisplayWindow(CSwordModuleInfo*, const QString&, const CDisplayWindow::WriteWindowType&)),
		this, SLOT(createWriteDisplayWindow(CSwordModuleInfo*,const QString&, const CDisplayWindow::WriteWindowType&))
	);
	//QObject::connect(m_mainIndex, SIGNAL(signalSwordSetupChanged()), this, SLOT(slotSwordSetupChanged()));

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slot_aboutToQuit()));
}

/** Initializes the backend */
void BibleTime::initBackends() {
	KStartupLogo::setStatusMessage(tr("Initializing Sword") + QString("..."));
	qDebug("BibleTime::initBackends");
	
	sword::StringMgr::setSystemStringMgr( new BTStringMgr() );
	sword::SWLog::getSystemLog()->setLogLevel(1);

	CSwordBackend* backend = new CSwordBackend();
	backend->booknameLanguage(CBTConfig::get(CBTConfig::language) );

	CPointers::setBackend(backend);
	const CSwordBackend::LoadError errorCode = CPointers::backend()->initModules(CSwordBackend::OtherChange);

	m_moduleList = 0;
	if ( errorCode == CSwordBackend::NoError ) { //no error
		m_moduleList = &(CPointers::backend()->moduleList());
	}
	else {
		m_moduleList = 0;
		//show error message that initBackend failed
		//TODO:
// 		switch (errorCode) {
// 			case CSwordBackend::NoSwordConfig: //mods.d or mods.conf missing
// 			{
// 				KStartupLogo::hideSplash();
// 				qDebug("case CSwordBackend::NoSwordConfig");
// 				BookshelfManager::CSwordSetupDialog dlg;
// 				dlg.showPart( BookshelfManager::CSwordSetupDialog::Sword );
// 				dlg.exec();
// 				break;
// 			}
// 
// 			case CSwordBackend::NoModules: //no modules installed, but config exists
// 			{
// 				KStartupLogo::hideSplash();
// 				qDebug("case CSwordBackend::NoModules");
// 				BookshelfManager::CSwordSetupDialog dlg;
// 				dlg.showPart( BookshelfManager::CSwordSetupDialog::Install );
// 				dlg.exec();
// 				break;
// 			}
// 
// 			default: //unknown error
// 			{
// 				KStartupLogo::hideSplash();
// 				qDebug("unknown error");
// 				BookshelfManager::CSwordSetupDialog dlg;
// 				dlg.showPart( BookshelfManager::CSwordSetupDialog::Sword );
// 				dlg.exec();
// 				break;
// 			}
// 		}
	}

	KStartupLogo::setStatusMessage(tr("Checking indices") + QString("..."));
	//This function will 
	// - delete all orphaned indexes (no module present) if autoDeleteOrphanedIndices is true
	// - delete all indices of modules where hasIndex() returns false
	//BookshelfManager::CManageIndicesWidget::deleteOrphanedIndices();
	//TODO: //backend::deleteOrphanedIndices();
	
}

void BibleTime::applyProfileSettings( CProfile* p ) {
	qDebug("BibleTime::applyProfileSettings");
	Q_ASSERT(p);
	if (!p) return;

	//first Main Window state
	m_windowFullscreen_action->setChecked( p->fullscreen() );  //set the fullscreen button state
	toggleFullscreen(); //either showFullscreen or showNormal
	if (p->maximized()) QMainWindow::showMaximized(); //if maximized, then also call showMaximized
	//Then Main Window geometry
	QMainWindow::resize( p->geometry().size() ); //Don't use QMainWindowInterface::resize
	QMainWindow::move( p->geometry().topLeft() );//Don't use QMainWindowInterface::move
	
	const CMDIArea::MDIArrangementMode newArrangementMode = p->getMDIArrangementMode();
	//make sure actions are updated by calling the slot functions
	//updatesEnabled in the MDI area is false atm, so changes won't actually be displayed yet
	switch(newArrangementMode){
		case CMDIArea::ArrangementModeTileVertical: slotAutoTileVertical(); break;
		case CMDIArea::ArrangementModeTileHorizontal: slotAutoTileHorizontal(); break;
		case CMDIArea::ArrangementModeCascade: slotAutoCascade(); break;
		default: slotManualArrangementMode(); break;
	}
}

void BibleTime::storeProfileSettings( CProfile* p ) {
	Q_ASSERT(p && m_windowFullscreen_action);
	if (!p || !m_windowFullscreen_action) return;
	
	p->setFullscreen( m_windowFullscreen_action->isChecked() );
	p->setMaximized( this->QMainWindow::isMaximized() );

	QRect geometry;
	geometry.setTopLeft(pos());
	geometry.setSize(size());
	p->setGeometry(geometry);
	
	p->setMDIArrangementMode(m_mdi->getMDIArrangementMode());
}

