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
#include "util/cpointers.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"
#include "backend/managers/btstringmgr.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/clanguagemgr.h"
#include "frontend/mainindex/cmainindex.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/profile/cprofilemgr.h"
#include "frontend/profile/cprofile.h"
#include "frontend/cmdiarea.h"
#include "frontend/cprinter.h"
#include "backend/config/cbtconfig.h"
#include "frontend/cinfodisplay.h"
#include "frontend/mainindex/bookshelf/cbookshelfindex.h"
#include "frontend/mainindex/bookmarks/cbookmarkindex.h"

// Qt includes
#include <QSplitter>
#include <QPointer>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QApplication>
#include <QDebug>
#include <QDockWidget>

// Sword includes
#include <swlog.h>

using namespace InfoDisplay;
using namespace Profile;

/**Initializes the view of this widget*/
void BibleTime::initView()
{
	m_mdi = new CMDIArea(this);
	setCentralWidget(m_mdi);

    m_bookshelfDock = new QDockWidget(tr("Bookshelf"), this);
    m_bookshelfDock->setObjectName("BookshelfDock");
	m_bookshelfPage = new CBookshelfIndex(0);
    m_bookshelfDock->setWidget(m_bookshelfPage);
    addDockWidget(Qt::LeftDockWidgetArea, m_bookshelfDock);
	
    m_bookmarksDock = new QDockWidget(tr("Bookmarks"), this);
    m_bookmarksDock->setObjectName("BookmarksDock");
	m_bookmarksPage = new CBookmarkIndex(0);
    m_bookmarksDock->setWidget(m_bookmarksPage);
    addDockWidget(Qt::LeftDockWidgetArea, m_bookmarksDock);
    tabifyDockWidget(m_bookmarksDock, m_bookshelfDock);
	
    m_magDock = new QDockWidget(tr("Mag"), this);
    m_magDock->setObjectName("MagDock");
	m_infoDisplay = new CInfoDisplay(this);
	m_infoDisplay->resize(150,150);
    m_magDock->setWidget(m_infoDisplay);
    addDockWidget(Qt::LeftDockWidgetArea, m_magDock);
	
	CPointers::setInfoDisplay(m_infoDisplay);
	m_mdi->setMinimumSize(100, 100);
	m_mdi->setFocusPolicy(Qt::ClickFocus);
}

QAction* BibleTime::initAction(QAction* action, QString text, QString icon, QKeySequence accel, 
	const QString& tooltip, const QString& actionName, const char* slot )
{
	action->setText(text);
	if ( ! icon.isEmpty() )
		action->setIcon(util::filesystem::DirectoryUtil::getIcon(icon));
	action->setShortcut(accel);
	if (tooltip != QString::null) action->setToolTip(tooltip);
 	actionCollection()->addAction(actionName, action);
	if (slot) QObject::connect( action, SIGNAL(triggered()), this, slot );
	return action;
}

// Creates QAction's for all actions that can have keyboard shortcuts
// Used in creating the main window and by the configuration dialog for setting shortcuts
void BibleTime::insertKeyboardActions( BtActionCollection* const a )
{
	QAction* action = new QAction(a);
	action->setText(tr("&Quit"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon("exit.svg"));
	action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	action->setToolTip(tr("Quit BibleTime"));
 	a->addAction("quit", action);

	action = new QAction(a);
	action->setText(tr("&Fullscreen mode"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::showFullscreen::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::showFullscreen::accel));
	action->setToolTip(tr("Toggle fullscreen mode of the main window"));
 	a->addAction("toggleFullscreen", action);

	action = new QAction(a);
	action->setText(tr("&Show toolbar"));
	action->setShortcut(QKeySequence(Qt::Key_F6));
 	a->addAction("showToolbar", action);

	action = new QAction(a);
	action->setText(tr("Search in &open works..."));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::mainIndex::search::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::mainIndex::search::accel));
	action->setToolTip(tr("Search in all works that are currently open"));
 	a->addAction("searchOpenWorks", action);

	action = new QAction(a);
	action->setText(tr("Search in standard &Bible..."));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::mainIndex::searchdefaultbible::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::mainIndex::searchdefaultbible::accel));
	action->setToolTip(tr("Search in the standard Bible"));
 	a->addAction("searchStdBible", action);

	action = new QAction(a);
	action->setText(tr("Save as &new session..."));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::saveToNewProfile::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::saveToNewProfile::accel));
	action->setToolTip(tr("Create and save a new session"));
 	a->addAction("saveNewSession", action);

	action = new QAction(a);
	action->setText(tr("&Manual mode"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::arrangementMode::manual::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::manual::accel));
	action->setToolTip(tr("Manually arrange the open windows"));
 	a->addAction("manualArrangement", action);

	action = new QAction(a);
	action->setText(tr("Auto-tile &vertically"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::arrangementMode::autoTileVertical::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoTileVertical::accel));
	action->setToolTip(tr("Automatically tile the open windows vertically (arrange side by side)"));
 	a->addAction("autoVertical", action);

	action = new QAction(a);
	action->setText(tr("Auto-tile &horizontally"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel));
	action->setToolTip(tr("Automatically tile the open windows horizontally (arrange on top of each other)"));
 	a->addAction("autoHorizontal", action);

	action = new QAction(a);
	action->setText(tr("Auto-&cascade"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::arrangementMode::autoCascade::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoCascade::accel));
	action->setToolTip(tr("Automatically cascade the open windows"));
 	a->addAction("autoCascade", action);

	action = new QAction(a);
	action->setText(tr("&Cascade"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::cascade::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::cascade::accel));
	action->setToolTip(tr("Cascade the open windows"));
 	a->addAction("cascade", action);

	action = new QAction(a);
	action->setText(tr("Tile &vertically"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::tileVertical::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::tileVertical::accel));
	action->setToolTip(tr("Vertically tile (arrange side by side) the open windows"));
 	a->addAction("tileVertically", action);

	action = new QAction(a);
	action->setText(tr("Tile &horizontally"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::tileHorizontal::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::tileHorizontal::accel));
	action->setToolTip(tr("Horizontally tile (arrange on top of each other) the open windows"));
 	a->addAction("tileHorizontally", action);

	action = new QAction(a);
	action->setText(tr("Cl&ose all windows"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::window::closeAll::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::window::closeAll::accel));
	action->setToolTip(tr("Close all open windows inside BibleTime"));
 	a->addAction("closeAllWindows", action);

	action = new QAction(a);
	action->setText(tr("&Configure BibleTime..."));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon("configure.svg"));
	action->setToolTip(tr("Set BibleTime's preferences"));
 	a->addAction("setPreferences", action);

	action = new QAction(a);
	action->setText(tr("Bookshelf &Manager..."));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::settings::swordSetupDialog::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::settings::swordSetupDialog::accel));
	action->setToolTip(tr("Configure your bookshelf and install/update/remove/index works"));
 	a->addAction("bookshelfManager", action);

	action = new QAction(a);
	action->setText(tr("&Handbook"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::help::handbook::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::help::handbook::accel));
	action->setToolTip(tr("Open BibleTime's handbook"));
 	a->addAction("openHandbook", action);

	action = new QAction(a);
	action->setText(tr("&Bible Study Howto"));
	action->setIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::mainMenu::help::bibleStudyHowTo::icon));
	action->setShortcut(QKeySequence(CResMgr::mainMenu::help::bibleStudyHowTo::accel));
	action->setToolTip(tr("Open the Bible study HowTo included with BibleTime.<BR>This HowTo is an introduction on how to study the Bible in an efficient way."));
 	a->addAction("bibleStudyHowto", action);

	action = new QAction(a);
	action->setText(tr("&About BibleTime"));
	action->setToolTip(tr("Information about the BibleTime program"));
 	a->addAction("aboutBibleTime", action);
}

/** Initializes the action objects of the GUI */
void BibleTime::initActions()
{
    m_actionCollection = new BtActionCollection(this);

	insertKeyboardActions(m_actionCollection);

	// Main menus
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
	QMenu* searchMenu = menuBar()->addMenu(tr("&Search"));
	m_windowMenu = menuBar()->addMenu(tr("&Window"));
	QMenu* settingsMenu = menuBar()->addMenu(tr("Se&ttings"));
	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	
	// Name of the main toolbar
	m_mainToolBar = addToolBar(tr("Main Toolbar"));
	m_mainToolBar->setObjectName("MainToolBar");
	m_mainToolBar->setFloatable(false);
	m_mainToolBar->setMovable(false);

	QAction* tmp = m_actionCollection->action("quit");
	fileMenu->addAction(tmp);
	m_mainToolBar->addAction(tmp);
	m_mainToolBar->addSeparator();
	connect(tmp, SIGNAL(triggered()), this, SLOT(close()) );

	m_windowFullscreen_action = m_actionCollection->action("toggleFullscreen");
	m_windowFullscreen_action->setCheckable(true);
	viewMenu->addAction(m_windowFullscreen_action);
	m_mainToolBar->addAction(m_windowFullscreen_action);
	connect(m_windowFullscreen_action, SIGNAL(triggered()), this, SLOT(toggleFullscreen()) );

	m_viewToolbar_action = m_actionCollection->action("showToolbar");
	m_viewToolbar_action->setCheckable(true);
	m_viewToolbar_action->setChecked(true);
	viewMenu->addAction(m_viewToolbar_action);
	connect(m_viewToolbar_action, SIGNAL(triggered()), this, SLOT(slotToggleToolbar()) );

	
    QAction* action = m_bookshelfDock->toggleViewAction();
	action->setText(tr("Show Bookshelf"));
	viewMenu->addAction(action);
	
    action = m_bookmarksDock->toggleViewAction();
	action->setText(tr("Show Bookmarks"));
	viewMenu->addAction(action);
	
    action = m_magDock->toggleViewAction();
	action->setText(tr("Show Mag"));
	viewMenu->addAction(action);
	
	m_mainToolBar->addSeparator();

	tmp = m_actionCollection->action("searchOpenWorks");
	searchMenu->addAction(tmp);
	m_mainToolBar->addAction(tmp);
	m_mainToolBar->addSeparator();
	connect(tmp, SIGNAL(triggered()), this, SLOT(slotSearchModules()) );

	tmp = m_actionCollection->action("searchStdBible");
	searchMenu->addAction(tmp);
	connect(tmp, SIGNAL(triggered()), this, SLOT(slotSearchDefaultBible()) );

	m_windowSaveProfileMenu = new QMenu(tr("&Save session"));
	m_windowMenu->addMenu(m_windowSaveProfileMenu);

	m_windowSaveToNewProfile_action = m_actionCollection->action("saveNewSession");
	m_windowMenu->addAction(m_windowSaveToNewProfile_action);
	connect(m_windowSaveToNewProfile_action, SIGNAL(triggered()), this, SLOT(saveToNewProfile()) );


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

	m_windowManualMode_action = m_actionCollection->action("manualArrangement");
	m_windowManualMode_action->setCheckable(true);
	arrangementMenu->addAction(m_windowManualMode_action);
	connect(m_windowManualMode_action, SIGNAL(triggered()), this, SLOT(slotManualArrangementMode()) );

	//: Vertical tiling means that windows are vertical, placed side by side
	m_windowAutoTileVertical_action = m_actionCollection->action("autoVertical");
	m_windowAutoTileVertical_action->setCheckable(true);
	arrangementMenu->addAction(m_windowAutoTileVertical_action);
	connect(m_windowAutoTileVertical_action, SIGNAL(triggered()), this, SLOT(slotAutoTileVertical()) );

	//: Horizontal tiling means that windows are horizontal, placed on top of each other
	m_windowAutoTileHorizontal_action = m_actionCollection->action("autoHorizontal");
	m_windowAutoTileHorizontal_action->setCheckable(true);
	arrangementMenu->addAction(m_windowAutoTileHorizontal_action);
	connect(m_windowAutoTileHorizontal_action, SIGNAL(triggered()), this, SLOT(slotAutoTileHorizontal()) );

	m_windowAutoCascade_action = m_actionCollection->action("autoCascade");
	m_windowAutoCascade_action->setCheckable(true);
	arrangementMenu->addAction(m_windowAutoCascade_action);
	connect(m_windowAutoCascade_action, SIGNAL(triggered()), this, SLOT(slotAutoCascade()) );

	m_windowCascade_action = m_actionCollection->action("cascade");
	m_windowMenu->addAction(m_windowCascade_action);
	connect(m_windowCascade_action, SIGNAL(triggered()), this, SLOT(slotCascade()) );

	m_windowTileVertical_action = m_actionCollection->action("tileVertically");
	m_windowMenu->addAction(m_windowTileVertical_action);
	connect(m_windowTileVertical_action, SIGNAL(triggered()), this, SLOT(slotTileVertical()) );

	m_windowTileHorizontal_action = m_actionCollection->action("tileHorizontally");
	m_windowMenu->addAction(m_windowTileHorizontal_action);
	connect(m_windowTileHorizontal_action, SIGNAL(triggered()), this, SLOT(slotTileHorizontal()) );

	m_windowCloseAll_action = m_actionCollection->action("closeAllWindows");
	m_windowMenu->addAction(m_windowCloseAll_action);
	connect(m_windowCloseAll_action, SIGNAL(triggered()), m_mdi, SLOT(deleteAll()));


	tmp = m_actionCollection->action("setPreferences");
 	settingsMenu->addAction(tmp);
	connect(tmp, SIGNAL(triggered()), this, SLOT(slotSettingsOptions()) );

	settingsMenu->addSeparator();
 
	tmp = m_actionCollection->action("bookshelfManager");
 	settingsMenu->addAction(tmp);
	connect(tmp, SIGNAL(triggered()), this, SLOT(slotSwordSetupDialog()) );

	tmp = m_actionCollection->action("openHandbook");
	helpMenu->addAction(tmp);
	m_mainToolBar->addAction(tmp);
	connect(tmp, SIGNAL(triggered()), this, SLOT(openOnlineHelp_Handbook()) );

	tmp = m_actionCollection->action("bibleStudyHowto");
	helpMenu->addAction(tmp);
	connect(tmp, SIGNAL(triggered()), this, SLOT(openOnlineHelp_Howto()) );

 	helpMenu->addSeparator();

	tmp = m_actionCollection->action("aboutBibleTime");
	helpMenu->addAction(tmp);
	connect(tmp, SIGNAL(triggered()), this, SLOT(slotOpenAboutDialog()) );
}

/** Initializes the SIGNAL / SLOT connections */
void BibleTime::initConnections() 
{
 	QObject::connect(m_mdi, SIGNAL(sigSetToplevelCaption(const QString&)),
 		this, SLOT(setPlainCaption(const QString&)));
	QObject::connect(m_mdi, SIGNAL(createReadDisplayWindow(QList<CSwordModuleInfo*>, const QString&)),
		this, SLOT(createReadDisplayWindow(QList<CSwordModuleInfo*>, const QString&)));

	if (m_windowMenu) {
		QObject::connect(m_windowMenu, SIGNAL(aboutToShow()), this, SLOT(slotWindowMenuAboutToShow()));
	}
	else {
		qWarning() << "Main window: can't find window menu";
	}

	bool ok;
	ok = connect(m_bookmarksPage, SIGNAL(createReadDisplayWindow(QList<CSwordModuleInfo*>, const QString&)),
				 this, SLOT(createReadDisplayWindow(QList<CSwordModuleInfo*>,const QString&)));
	Q_ASSERT(ok);
	ok = connect(m_bookshelfPage, SIGNAL(createReadDisplayWindow(QList<CSwordModuleInfo*>, const QString&)),
				 this, SLOT(createReadDisplayWindow(QList<CSwordModuleInfo*>,const QString&)));
	Q_ASSERT(ok);
	ok = connect(m_bookshelfPage, SIGNAL(createWriteDisplayWindow(CSwordModuleInfo*, const QString&, const CDisplayWindow::WriteWindowType&)),
				 this, SLOT(createWriteDisplayWindow(CSwordModuleInfo*,const QString&, const CDisplayWindow::WriteWindowType&)));
	Q_ASSERT(ok);

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slot_aboutToQuit()));
}

/** Initializes the backend */
void BibleTime::initBackends() 
{
	qDebug("BibleTime::initBackends");
	
	sword::StringMgr::setSystemStringMgr( new BTStringMgr() );
	sword::SWLog::getSystemLog()->setLogLevel(1);

	CSwordBackend* backend = new CSwordBackend();
	backend->booknameLanguage(CBTConfig::get(CBTConfig::language) );

	CPointers::setBackend(backend);
	const CSwordBackend::LoadError errorCode = CPointers::backend()->initModules(CSwordBackend::OtherChange);

	if (errorCode != CSwordBackend::NoError) {
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

	//This function will 
	// - delete all orphaned indexes (no module present) if autoDeleteOrphanedIndices is true
	// - delete all indices of modules where hasIndex() returns false
	//BookshelfManager::CManageIndicesWidget::deleteOrphanedIndices();
	//TODO: //backend::deleteOrphanedIndices();
	
}

void BibleTime::applyProfileSettings( CProfile* p ) 
{
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
	restoreState(p->getMainwindowState());
	
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

void BibleTime::storeProfileSettings( CProfile* p ) 
{
	Q_ASSERT(p && m_windowFullscreen_action);
	if (!p || !m_windowFullscreen_action) return;
	
	p->setMainwindowState(saveState());
	p->setFullscreen( m_windowFullscreen_action->isChecked() );
	p->setMaximized( this->QMainWindow::isMaximized() );

	QRect geometry;
	geometry.setTopLeft(pos());
	geometry.setSize(size());
	p->setGeometry(geometry);
	
	p->setMDIArrangementMode(m_mdi->getMDIArrangementMode());
}

