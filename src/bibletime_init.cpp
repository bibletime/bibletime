/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"

#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPointer>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include "backend/config/cbtconfig.h"
#include "backend/managers/btstringmgr.h"
#include "backend/managers/clanguagemgr.h"
#include "backend/managers/cswordbackend.h"
#include "frontend/btbookshelfdockwidget.h"
#include "frontend/btopenworkaction.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/cprinter.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/bookmarks/cbookmarkindex.h"
#include "frontend/profile/cprofile.h"
#include "frontend/profile/cprofilemgr.h"
#include "util/cresmgr.h"
#include "util/directory.h"

// Sword includes:
#include <swlog.h>

#ifdef BT_DEBUG
#include <QLabel>
#include <QMetaObject>
#include <QMutexLocker>
#include <QTimer>
#endif


using namespace InfoDisplay;
using namespace Profile;

/**Initializes the view of this widget*/
void BibleTime::initView() {

    // Create menu and toolbar before the mdi area
    createMenuAndToolBar();

    m_mdi = new CMDIArea(this);
    setCentralWidget(m_mdi);

    m_bookshelfDock = new BtBookshelfDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_bookshelfDock);

    m_bookmarksDock = new QDockWidget(this);
    m_bookmarksDock->setObjectName("BookmarksDock");
    m_bookmarksPage = new CBookmarkIndex(0);
    m_bookmarksDock->setWidget(m_bookmarksPage);
    addDockWidget(Qt::LeftDockWidgetArea, m_bookmarksDock);
    tabifyDockWidget(m_bookmarksDock, m_bookshelfDock);

    m_magDock = new QDockWidget(this);
    m_magDock->setObjectName("MagDock");
    m_infoDisplay = new CInfoDisplay(this);
    m_infoDisplay->resize(150, 150);
    m_magDock->setWidget(m_infoDisplay);
    addDockWidget(Qt::LeftDockWidgetArea, m_magDock);

    m_mdi->setMinimumSize(100, 100);
    m_mdi->setFocusPolicy(Qt::ClickFocus);
}

QAction* BibleTime::initAction(QAction* action, QString text, QString icon,
                               QKeySequence accel, const QString& tooltip,
                               const QString& actionName, const char* slot) {
    namespace DU = util::directory;

    action->setText(text);
    if ( ! icon.isEmpty() )
        action->setIcon(DU::getIcon(icon));
    action->setShortcut(accel);
    if (tooltip != QString::null) action->setToolTip(tooltip);
    m_actionCollection->addAction(actionName, action);
    if (slot) QObject::connect( action, SIGNAL(triggered()), this, slot );
    return action;
}

// Creates QAction's for all actions that can have keyboard shortcuts
// Used in creating the main window and by the configuration dialog for setting shortcuts
void BibleTime::insertKeyboardActions( BtActionCollection* const a ) {
    namespace DU = util::directory;

    QAction* action = new QAction(a);
    action->setText(tr("&Quit"));
    action->setIcon(DU::getIcon("exit.svg"));
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    action->setToolTip(tr("Quit BibleTime"));
    a->addAction("quit", action);

    action = new QAction(a);
    action->setText(tr("&Fullscreen mode"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::showFullscreen::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::showFullscreen::accel));
    action->setToolTip(tr("Toggle fullscreen mode of the main window"));
    a->addAction("toggleFullscreen", action);

    action = new QAction(a);
    action->setText(tr("&Show toolbar"));
    action->setShortcut(QKeySequence(Qt::Key_F6));
    a->addAction("showToolbar", action);

    action = new QAction(a);
    action->setText(tr("Search in &open works..."));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::mainIndex::search::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::mainIndex::search::accel));
    action->setToolTip(tr("Search in all works that are currently open"));
    a->addAction("searchOpenWorks", action);

    action = new QAction(a);
    action->setText(tr("Search in standard &Bible..."));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::mainIndex::searchdefaultbible::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::mainIndex::searchdefaultbible::accel));
    action->setToolTip(tr("Search in the standard Bible"));
    a->addAction("searchStdBible", action);

    action = new QAction(a);
    action->setText(tr("Save as &new session..."));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::saveToNewProfile::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::saveToNewProfile::accel));
    action->setToolTip(tr("Create and save a new session"));
    a->addAction("saveNewSession", action);

    action = new QAction(a);
    action->setText(tr("&Manual mode"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::arrangementMode::manual::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::manual::accel));
    action->setToolTip(tr("Manually arrange the open windows"));
    a->addAction("manualArrangement", action);

    action = new QAction(a);
    action->setText(tr("Auto-tile &vertically"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::arrangementMode::autoTileVertical::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoTileVertical::accel));
    action->setToolTip(tr("Automatically tile the open windows vertically (arrange side by side)"));
    a->addAction("autoVertical", action);

    action = new QAction(a);
    action->setText(tr("Auto-tile &horizontally"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoTileHorizontal::accel));
    action->setToolTip(tr("Automatically tile the open windows horizontally (arrange on top of each other)"));
    a->addAction("autoHorizontal", action);

    action = new QAction(a);
    action->setText(tr("Auto-&tile"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::arrangementMode::autoTile::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoTile::accel));
    action->setToolTip(tr("Automatically tile the open windows"));
    a->addAction("autoTile", action);

    action = new QAction(a);
    action->setText(tr("Ta&bbed"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::arrangementMode::autoTabbed::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoTabbed::accel));
    action->setToolTip(tr("Automatically tab the open windows"));
    a->addAction("autoTabbed", action);

    action = new QAction(a);
    action->setText(tr("Auto-&cascade"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::arrangementMode::autoCascade::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::arrangementMode::autoCascade::accel));
    action->setToolTip(tr("Automatically cascade the open windows"));
    a->addAction("autoCascade", action);

    action = new QAction(a);
    action->setText(tr("&Cascade"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::cascade::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::cascade::accel));
    action->setToolTip(tr("Cascade the open windows"));
    a->addAction("cascade", action);

    action = new QAction(a);
    action->setText(tr("&Tile"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::tile::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::tile::accel));
    action->setToolTip(tr("Tile the open windows"));
    a->addAction("tile", action);

    action = new QAction(a);
    action->setText(tr("Tile &vertically"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::tileVertical::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::tileVertical::accel));
    action->setToolTip(tr("Vertically tile (arrange side by side) the open windows"));
    a->addAction("tileVertically", action);

    action = new QAction(a);
    action->setText(tr("Tile &horizontally"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::tileHorizontal::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::tileHorizontal::accel));
    action->setToolTip(tr("Horizontally tile (arrange on top of each other) the open windows"));
    a->addAction("tileHorizontally", action);

    action = new QAction(a);
    action->setText(tr("Close &window"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::close::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::close::accel));
    action->setToolTip(tr("Close the current open window"));
    a->addAction("closeWindow", action);

    action = new QAction(a);
    action->setText(tr("Cl&ose all windows"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::window::closeAll::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::window::closeAll::accel));
    action->setToolTip(tr("Close all open windows inside BibleTime"));
    a->addAction("closeAllWindows", action);

    action = new QAction(a);
    action->setText(tr("&Configure BibleTime..."));
    action->setIcon(DU::getIcon("configure.svg"));
    action->setToolTip(tr("Set BibleTime's preferences"));
    a->addAction("setPreferences", action);

    action = new QAction(a);
    action->setText(tr("Bookshelf &Manager..."));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::settings::swordSetupDialog::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::settings::swordSetupDialog::accel));
    action->setToolTip(tr("Configure your bookshelf and install/update/remove/index works"));
    a->addAction("bookshelfManager", action);

    action = new QAction(a);
    action->setText(tr("&Handbook"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::help::handbook::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::help::handbook::accel));
    action->setToolTip(tr("Open BibleTime's handbook"));
    a->addAction("openHandbook", action);

    action = new QAction(a);
    action->setText(tr("&Bible Study Howto"));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::help::bibleStudyHowTo::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::help::bibleStudyHowTo::accel));
    action->setToolTip(tr("Open the Bible study HowTo included with BibleTime.<br/>This HowTo is an introduction on how to study the Bible in an efficient way."));
    a->addAction("bibleStudyHowto", action);

    action = new QAction(a);
    action->setText(tr("&About BibleTime"));
    action->setToolTip(tr("Information about the BibleTime program"));
    a->addAction("aboutBibleTime", action);

    action = new QAction(a);
    action->setText(tr("&Tip of the day..."));
    action->setIcon(DU::getIcon(CResMgr::mainMenu::help::tipOfTheDay::icon));
    action->setShortcut(QKeySequence(CResMgr::mainMenu::help::tipOfTheDay::accel));
    action->setToolTip(tr("Show tips about BibleTime"));
    a->addAction("tipOfTheDay", action);

    action = new QAction(a);
    a->addAction("showToolbarsInTextWindows", action);

    action = new QAction(a);
    a->addAction("showNavigation", action);

    action = new QAction(a);
    a->addAction("showWorks", action);

    action = new QAction(a);
    a->addAction("showTools", action);

    action = new QAction(a);
    a->addAction("showFormat", action);

    action = new QAction(a);
    a->addAction("showParallelTextHeaders", action);

    action = new QAction(a);
    a->addAction("showBookshelf", action);

    action = new QAction(a);
    a->addAction("showBookmarks", action);

    action = new QAction(a);
    a->addAction("showMag", action);

    retranslateUiActions(a);
}

static QToolBar* createToolBar(const QString& name, QWidget* parent, bool visible) {
    QToolBar* bar = new QToolBar(parent);
    bar->setObjectName(name);
    bar->setFloatable(false);
    bar->setMovable(true);
    bar->setVisible(visible);
    return bar;
}

void BibleTime::clearMdiToolBars() {
    // Clear main window toolbars
    m_navToolBar->clear();
    m_worksToolBar->clear();
    m_toolsToolBar->clear();
    m_formatToolBar->clear();
}

void BibleTime::createMenuAndToolBar()
{
    // Create menubar
    menuBar();

    m_mainToolBar = createToolBar("MainToolBar", this, true);
    addToolBar(m_mainToolBar);

    // Set visibility of main window toolbars based on config
    bool visible = ! CBTConfig::get(CBTConfig::showToolbarsInEachWindow);

    m_navToolBar = createToolBar("NavToolBar", this, visible);
    addToolBar(m_navToolBar);

    m_worksToolBar = new BtModuleChooserBar(this);
    m_worksToolBar->setObjectName("WorksToolBar");
    m_worksToolBar->setVisible(visible);
    addToolBar(m_worksToolBar);

    m_toolsToolBar = createToolBar("ToolsToolBar", this, visible);
    addToolBar(m_toolsToolBar);

    m_formatToolBar = createToolBar("FormatToolBar", this, visible);
    addToolBar(m_formatToolBar);
}

/** Initializes the action objects of the GUI */
void BibleTime::initActions() {
    m_actionCollection = new BtActionCollection(this);
    insertKeyboardActions(m_actionCollection);

    // Create the window to signal mapper and connect it up:
    m_windowMapper = new QSignalMapper(this);
    connect(m_windowMapper, SIGNAL(mapped(QWidget*)),
            this,           SLOT(slotSetActiveSubWindow(QWidget*)));

    // File menu actions:
    m_openWorkAction = new BtOpenWorkAction("GUI/MainWindow/OpenWorkAction/grouping", this);
    connect(m_openWorkAction, SIGNAL(triggered(CSwordModuleInfo*)),
            this,             SLOT(createReadDisplayWindow(CSwordModuleInfo*)));

    m_quitAction = m_actionCollection->action("quit");
    Q_ASSERT(m_quitAction != 0);
    connect(m_quitAction, SIGNAL(triggered()),
            this,         SLOT(quit()));


    // View menu actions:
    m_windowFullscreenAction = m_actionCollection->action("toggleFullscreen");
    Q_ASSERT(m_windowFullscreenAction != 0);
    m_windowFullscreenAction->setCheckable(true);
    connect(m_windowFullscreenAction, SIGNAL(triggered()),
            this,                     SLOT(toggleFullscreen()));

    // Special case these actions, overwrite those already in collection
    namespace DU = util::directory;
    m_showBookshelfAction = m_bookshelfDock->toggleViewAction();
    m_showBookshelfAction->setIcon(DU::getIcon(CResMgr::mainMenu::view::showBookshelf::icon));
    m_showBookshelfAction->setToolTip(tr("Toggle visibility of the bookshelf window"));
    m_actionCollection->addAction("showBookshelf", m_showBookshelfAction);
    m_showBookmarksAction = m_bookmarksDock->toggleViewAction();
    m_showBookmarksAction->setIcon(DU::getIcon(CResMgr::mainMenu::view::showBookmarks::icon));
    m_showBookmarksAction->setToolTip(tr("Toggle visibility of the bookmarks window"));
    m_actionCollection->addAction("showBookmarks", m_showBookmarksAction);
    m_showMagAction = m_magDock->toggleViewAction();
    m_showMagAction->setIcon(DU::getIcon(CResMgr::mainMenu::view::showMag::icon));
    m_showMagAction->setToolTip(tr("Toggle visibility of the mag window"));
    m_actionCollection->addAction("showMag", m_showMagAction);

    m_showTextAreaHeadersAction = m_actionCollection->action("showParallelTextHeaders");
    Q_ASSERT(m_showTextAreaHeadersAction != 0);
    m_showTextAreaHeadersAction->setCheckable(true);
    m_showTextAreaHeadersAction->setChecked(CBTConfig::get(CBTConfig::showTextWindowHeaders));
    connect(m_showTextAreaHeadersAction, SIGNAL(toggled(bool)),
            this,                        SLOT(slotToggleTextWindowHeader()));

    m_showMainWindowToolbarAction = m_actionCollection->action("showToolbar");
    Q_ASSERT(m_showMainWindowToolbarAction != 0);
    m_showMainWindowToolbarAction->setCheckable(true);
    m_showMainWindowToolbarAction->setChecked(CBTConfig::get(CBTConfig::showMainWindowToolbar));
    connect( m_showMainWindowToolbarAction, SIGNAL(triggered()),
            this,                SLOT(slotToggleMainToolbar()));

    m_showTextWindowNavigationAction = m_actionCollection->action("showNavigation");
    Q_ASSERT(m_showTextWindowNavigationAction != 0);
    m_showTextWindowNavigationAction->setCheckable(true);
    m_showTextWindowNavigationAction->setChecked(CBTConfig::get(CBTConfig::showTextWindowNavigator));
    connect(m_showTextWindowNavigationAction, SIGNAL(toggled(bool)),
            this,                             SLOT(slotToggleNavigatorToolbar()));

    m_showTextWindowModuleChooserAction = m_actionCollection->action("showWorks");
    Q_ASSERT(m_showTextWindowModuleChooserAction != 0);
    m_showTextWindowModuleChooserAction->setCheckable(true);
    m_showTextWindowModuleChooserAction->setChecked(CBTConfig::get(CBTConfig::showTextWindowModuleSelectorButtons));
    connect(m_showTextWindowModuleChooserAction, SIGNAL(toggled(bool)),
            this,                                SLOT(slotToggleWorksToolbar()));

    m_showTextWindowToolButtonsAction = m_actionCollection->action("showTools");
    Q_ASSERT(m_showTextWindowToolButtonsAction != 0);
    m_showTextWindowToolButtonsAction->setCheckable(true);
    m_showTextWindowToolButtonsAction->setChecked(CBTConfig::get(CBTConfig::showTextWindowToolButtons));
    connect(m_showTextWindowToolButtonsAction, SIGNAL(toggled(bool)),
            this,                              SLOT(slotToggleToolsToolbar()));

    m_showFormatToolbarAction = m_actionCollection->action("showFormat");
    Q_ASSERT(m_showFormatToolbarAction != 0);
    m_showFormatToolbarAction->setCheckable(true);
    m_showFormatToolbarAction->setChecked(CBTConfig::get(CBTConfig::showFormatToolbarButtons));
    bool ok = connect(m_showFormatToolbarAction, SIGNAL(toggled(bool)),
                      this,                      SLOT(slotToggleFormatToolbar()));

    m_toolbarsInEachWindow = m_actionCollection->action("showToolbarsInTextWindows");
    Q_ASSERT(m_toolbarsInEachWindow != 0);
    m_toolbarsInEachWindow->setCheckable(true);
    m_toolbarsInEachWindow->setChecked(CBTConfig::get(CBTConfig::showToolbarsInEachWindow));
    ok = connect(m_toolbarsInEachWindow, SIGNAL(toggled(bool)),
                      this,                   SLOT(slotToggleToolBarsInEachWindow()));
    Q_ASSERT(ok);

    // Search menu actions:
    m_searchOpenWorksAction = m_actionCollection->action("searchOpenWorks");
    Q_ASSERT(m_searchOpenWorksAction != 0);
    connect(m_searchOpenWorksAction, SIGNAL(triggered()),
            this,                    SLOT(slotSearchModules()));

    m_searchStandardBibleAction = m_actionCollection->action("searchStdBible");
    Q_ASSERT(m_searchStandardBibleAction != 0);
    connect(m_searchStandardBibleAction, SIGNAL(triggered()),
            this,                        SLOT(slotSearchDefaultBible()));

    // Window menu actions:
    m_windowCloseAction = m_actionCollection->action("closeWindow");
    Q_ASSERT(m_windowCloseAction != 0);
    connect(m_windowCloseAction, SIGNAL(triggered()),
            m_mdi,                SLOT(closeActiveSubWindow()));

    m_windowCloseAllAction = m_actionCollection->action("closeAllWindows");
    Q_ASSERT(m_windowCloseAllAction != 0);
    connect(m_windowCloseAllAction, SIGNAL(triggered()),
            m_mdi,                   SLOT(closeAllSubWindows()));

    m_windowCascadeAction = m_actionCollection->action("cascade");
    Q_ASSERT(m_windowCascadeAction != 0);
    connect(m_windowCascadeAction, SIGNAL(triggered()),
            this,                   SLOT(slotCascade()));

    m_windowTileAction = m_actionCollection->action("tile");
    Q_ASSERT(m_windowTileAction != 0);
    connect(m_windowTileAction, SIGNAL(triggered()),
            this,                SLOT(slotTile()));

    m_windowTileVerticalAction = m_actionCollection->action("tileVertically");
    Q_ASSERT(m_windowTileVerticalAction != 0);
    connect(m_windowTileVerticalAction, SIGNAL(triggered()),
            this,                        SLOT(slotTileVertical()));

    m_windowTileHorizontalAction = m_actionCollection->action("tileHorizontally");
    Q_ASSERT(m_windowTileHorizontalAction != 0);
    connect(m_windowTileHorizontalAction, SIGNAL(triggered()),
            this,                          SLOT(slotTileHorizontal()));

    m_windowManualModeAction = m_actionCollection->action("manualArrangement");
    Q_ASSERT(m_windowManualModeAction != 0);
    m_windowManualModeAction->setCheckable(true);
    m_windowManualModeAction->setChecked(true);
    connect(m_windowManualModeAction, SIGNAL(triggered()),
            this,                      SLOT(slotManualArrangementMode()));

    m_windowAutoTabbedAction = m_actionCollection->action("autoTabbed");
    Q_ASSERT(m_windowAutoTabbedAction != 0);
    m_windowAutoTabbedAction->setCheckable(true);
    if(CBTConfig::get(CBTConfig::autoTabbed) == true) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(true);
    }
    connect(m_windowAutoTabbedAction, SIGNAL(triggered()),
            this,                      SLOT(slotAutoTabbed()));

    //: Vertical tiling means that windows are vertical, placed side by side
    m_windowAutoTileVerticalAction = m_actionCollection->action("autoVertical");
    Q_ASSERT(m_windowAutoTileVerticalAction != 0);
    m_windowAutoTileVerticalAction->setCheckable(true);
    if(CBTConfig::get(CBTConfig::autoTileVertical) == true) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(true);
    }
    connect(m_windowAutoTileVerticalAction, SIGNAL(triggered()),
            this,                            SLOT(slotAutoTileVertical()));

    //: Horizontal tiling means that windows are horizontal, placed on top of each other
    m_windowAutoTileHorizontalAction = m_actionCollection->action("autoHorizontal");
    Q_ASSERT(m_windowAutoTileHorizontalAction != 0);
    m_windowAutoTileHorizontalAction->setCheckable(true);
    if(CBTConfig::get(CBTConfig::autoTileHorizontal) == true) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(true);
    }
    connect(m_windowAutoTileHorizontalAction, SIGNAL(triggered()),
            this,                              SLOT(slotAutoTileHorizontal()));

    m_windowAutoTileAction = m_actionCollection->action("autoTile");
    Q_ASSERT(m_windowAutoTileAction != 0);
    m_windowAutoTileAction->setCheckable(true);
    if(CBTConfig::get(CBTConfig::autoTile) == true) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileAction->setChecked(true);
    }
    connect(m_windowAutoTileAction, SIGNAL(triggered()),
            this,                    SLOT(slotAutoTile()));

    m_windowAutoCascadeAction = m_actionCollection->action("autoCascade");
    Q_ASSERT(m_windowAutoCascadeAction != 0);
    m_windowAutoCascadeAction->setCheckable(true);
    if(CBTConfig::get(CBTConfig::autoCascade) == true) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(true);
    }
    connect(m_windowAutoCascadeAction, SIGNAL(triggered()),
            this,                       SLOT(slotAutoCascade()));

    /*
     * All actions related to arrangement modes have to be initialized before calling a slot on them,
     * thus we call them afterwards now.
     */
    if(m_windowAutoTabbedAction->isChecked() == true)
        slotAutoTabbed();
    else if(m_windowAutoTileVerticalAction->isChecked() == true)
        slotAutoTileVertical();
    else if(m_windowAutoTileHorizontalAction->isChecked() == true)
        slotAutoTileHorizontal();
    else if(m_windowAutoTileAction->isChecked() == true)
        slotAutoTile();
    else if(m_windowAutoCascadeAction->isChecked() == true)
        slotAutoCascade();
    else
        slotManualArrangementMode();

    m_windowSaveToNewProfileAction = m_actionCollection->action("saveNewSession");
    Q_ASSERT(m_windowSaveToNewProfileAction != 0);
    connect(m_windowSaveToNewProfileAction, SIGNAL(triggered()),
            this,                            SLOT(saveToNewProfile()));

    m_setPreferencesAction = m_actionCollection->action("setPreferences");
    Q_ASSERT(m_setPreferencesAction != 0);
    connect(m_setPreferencesAction, SIGNAL(triggered()),
            this,                   SLOT(slotSettingsOptions()));

    m_bookshelfManagerAction = m_actionCollection->action("bookshelfManager");
    Q_ASSERT(m_bookshelfManagerAction != 0);
    connect(m_bookshelfManagerAction, SIGNAL(triggered()),
            this,                     SLOT(slotSwordSetupDialog()));

    m_openHandbookAction = m_actionCollection->action("openHandbook");
    Q_ASSERT(m_openHandbookAction != 0);
    connect(m_openHandbookAction, SIGNAL(triggered()),
            this,                 SLOT(openOnlineHelp_Handbook()));

    m_bibleStudyHowtoAction = m_actionCollection->action("bibleStudyHowto");
    Q_ASSERT(m_bibleStudyHowtoAction != 0);
    connect(m_bibleStudyHowtoAction, SIGNAL(triggered()),
            this,                    SLOT(openOnlineHelp_Howto()));

    m_aboutBibleTimeAction = m_actionCollection->action("aboutBibleTime");
    Q_ASSERT(m_aboutBibleTimeAction != 0);
    connect(m_aboutBibleTimeAction,  SIGNAL(triggered()),
            this,                    SLOT(slotOpenAboutDialog()) );

    m_tipOfTheDayAction = m_actionCollection->action("tipOfTheDay");
    Q_ASSERT(m_tipOfTheDayAction != 0);
    connect(m_tipOfTheDayAction,  SIGNAL(triggered()),
            this,                    SLOT(slotOpenTipDialog()) );

    #ifdef BT_DEBUG
    m_debugWidgetAction = new QAction(this);
    m_debugWidgetAction->setCheckable(true);
    connect(m_debugWidgetAction, SIGNAL(triggered(bool)),
            this,                 SLOT(slotShowDebugWindow(bool)));
    #endif

    retranslateUiActions(m_actionCollection);
}

void BibleTime::initMenubar() {
    // File menu:
    m_fileMenu = new QMenu(this);
    m_fileMenu->addAction(m_openWorkAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_quitAction);
    menuBar()->addMenu(m_fileMenu);

    // View menu:
    m_viewMenu = new QMenu(this);
    m_viewMenu->addAction(m_windowFullscreenAction);
    m_viewMenu->addAction(m_showBookshelfAction);
    m_viewMenu->addAction(m_showBookmarksAction);
    m_viewMenu->addAction(m_showMagAction);
    m_viewMenu->addAction(m_showTextAreaHeadersAction);
    m_viewMenu->addSeparator();
    m_toolBarsMenu = new QMenu(this);
    m_toolBarsMenu->addAction( m_showMainWindowToolbarAction);
    m_toolBarsMenu->addAction(m_showTextWindowNavigationAction);
    m_toolBarsMenu->addAction(m_showTextWindowModuleChooserAction);
    m_toolBarsMenu->addAction(m_showTextWindowToolButtonsAction);
    m_toolBarsMenu->addAction(m_showFormatToolbarAction);
    m_toolBarsMenu->addSeparator();
    m_toolBarsMenu->addAction(m_toolbarsInEachWindow);
    m_viewMenu->addMenu(m_toolBarsMenu);
    menuBar()->addMenu(m_viewMenu);

    // Search menu:
    m_searchMenu = new QMenu(this);
    m_searchMenu->addAction(m_searchOpenWorksAction);
    m_searchMenu->addAction(m_searchStandardBibleAction);
    menuBar()->addMenu(m_searchMenu);

    // Window menu:
    m_windowMenu = new QMenu(this);
    m_openWindowsMenu = new QMenu(this);
    QObject::connect(m_openWindowsMenu, SIGNAL(aboutToShow()),
                     this,              SLOT(slotOpenWindowsMenuAboutToShow()));
    m_windowMenu->addMenu(m_openWindowsMenu);
    m_windowMenu->addAction(m_windowCloseAction);
    m_windowMenu->addAction(m_windowCloseAllAction);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_windowCascadeAction);
    m_windowMenu->addAction(m_windowTileAction);
    m_windowMenu->addAction(m_windowTileVerticalAction);
    m_windowMenu->addAction(m_windowTileHorizontalAction);
    m_windowArrangementMenu = new QMenu(this);
    m_windowArrangementMenu->addAction(m_windowManualModeAction);
    m_windowArrangementMenu->addAction(m_windowAutoTabbedAction);
    m_windowArrangementMenu->addAction(m_windowAutoTileVerticalAction);
    m_windowArrangementMenu->addAction(m_windowAutoTileHorizontalAction);
    m_windowArrangementMenu->addAction(m_windowAutoTileAction);
    m_windowArrangementMenu->addAction(m_windowAutoCascadeAction);
    m_windowMenu->addMenu(m_windowArrangementMenu);
    m_windowMenu->addSeparator();
    m_windowSaveProfileMenu = new QMenu(this);
    m_windowMenu->addMenu(m_windowSaveProfileMenu);
    m_windowMenu->addAction(m_windowSaveToNewProfileAction);
    m_windowLoadProfileMenu = new QMenu(this);
    m_windowMenu->addMenu(m_windowLoadProfileMenu);
    m_windowDeleteProfileMenu = new QMenu(this);
    m_windowMenu->addMenu(m_windowDeleteProfileMenu);
    connect(m_windowLoadProfileMenu, SIGNAL(triggered(QAction*)),
            this,                    SLOT(loadProfile(QAction*)));
    connect(m_windowSaveProfileMenu, SIGNAL(triggered(QAction*)),
            this,                    SLOT(saveProfile(QAction*)));
    connect(m_windowDeleteProfileMenu, SIGNAL(triggered(QAction*)),
            this,                      SLOT(deleteProfile(QAction*)));
    refreshProfileMenus();
    menuBar()->addMenu(m_windowMenu);
    connect(m_windowMenu, SIGNAL(aboutToShow()),
            this,         SLOT(slotWindowMenuAboutToShow()));

    // Settings menu:
    m_settingsMenu = new QMenu(this);
    m_settingsMenu->addAction(m_setPreferencesAction);
    m_settingsMenu->addSeparator();
    m_settingsMenu->addAction(m_bookshelfManagerAction);
    menuBar()->addMenu(m_settingsMenu);

    // Help menu:
    m_helpMenu = new QMenu(this);
    m_helpMenu->addAction(m_openHandbookAction);
    m_helpMenu->addAction(m_bibleStudyHowtoAction);
    m_helpMenu->addAction(m_tipOfTheDayAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutBibleTimeAction);
    #ifdef BT_DEBUG
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_debugWidgetAction);
    #endif
    menuBar()->addMenu(m_helpMenu);
}

void BibleTime::initToolbars() {
    QToolButton *openWorkButton = new QToolButton(this);
    openWorkButton->setDefaultAction(m_openWorkAction);
    openWorkButton->setPopupMode(QToolButton::InstantPopup);
    m_mainToolBar->addWidget(openWorkButton);

    m_mainToolBar->addAction(m_windowFullscreenAction);
    QAction *a = m_actionCollection->action("showBookshelf");
    Q_ASSERT(a != 0);
    m_mainToolBar->addAction(a);
    a = m_actionCollection->action("showBookmarks");
    Q_ASSERT(a != 0);
    m_mainToolBar->addAction(a);
    a = m_actionCollection->action("showMag");
    Q_ASSERT(a != 0);
    m_mainToolBar->addAction(a);
    m_mainToolBar->addAction(m_searchOpenWorksAction);
    m_mainToolBar->addAction(m_openHandbookAction);
}

void BibleTime::retranslateUi() {
    m_bookmarksDock->setWindowTitle(tr("Bookmarks"));
    m_magDock->setWindowTitle(tr("Mag"));
    m_mainToolBar->setWindowTitle(tr("Main toolbar"));
    m_navToolBar->setWindowTitle(tr("Navigation toolbar"));
    m_worksToolBar->setWindowTitle(tr("Works toolbar"));
    m_toolsToolBar->setWindowTitle(tr("Tools toolbar"));
    m_formatToolBar->setWindowTitle(tr("Format toolbar"));

    m_fileMenu->setTitle(tr("&File"));
    m_viewMenu->setTitle(tr("&View"));
        m_toolBarsMenu->setTitle(tr("Toolbars"));

    m_searchMenu->setTitle(tr("&Search"));
    m_windowMenu->setTitle(tr("&Window"));
        m_openWindowsMenu->setTitle(tr("O&pen windows"));
        m_windowArrangementMenu->setTitle(tr("&Arrangement mode"));
        m_windowSaveProfileMenu->setTitle(tr("&Save session"));
        m_windowLoadProfileMenu->setTitle(tr("&Load session"));
        m_windowDeleteProfileMenu->setTitle(tr("&Delete session"));
    m_settingsMenu->setTitle(tr("Se&ttings"));
    m_helpMenu->setTitle(tr("&Help"));

    #ifdef BT_DEBUG
    m_debugWidgetAction->setText(tr("Show \"Whats this widget\" dialog"));
    #endif

    retranslateUiActions(m_actionCollection);
}

/** retranslation for actions used in this class
*   This is called for two different collections of actions
*   One set is for the actual use in the menus, etc
*   The second is used during the use of the configuration shortcut editor
*/
void BibleTime::retranslateUiActions(BtActionCollection* ac) {
    QAction *a = ac->action("showToolbarsInTextWindows");
    Q_ASSERT(a != 0);
    a->setText(tr("Show toolbars in text windows"));
    a = ac->action("showToolbar");
    Q_ASSERT(a != 0);
    a->setText(tr("Show main toolbar"));
    a = ac->action("showNavigation");
    Q_ASSERT(a != 0);
    a->setText(tr("Show navigation bar"));
    a = ac->action("showWorks");
    Q_ASSERT(a != 0);
    a->setText(tr("Show works toolbar"));
    a = ac->action("showTools");
    Q_ASSERT(a != 0);
    a->setText(tr("Show tools toolbar"));
    a = ac->action("showFormat");
    Q_ASSERT(a != 0);
    a->setText(tr("Show formatting toolbar"));
    a = ac->action("showBookshelf");
    Q_ASSERT(a != 0);
    a->setText(tr("Show bookshelf"));
    a = ac->action("showBookmarks");
    Q_ASSERT(a != 0);
    a->setText(tr("Show bookmarks"));
    a = ac->action("showMag");
    Q_ASSERT(a != 0);
    a->setText(tr("Show mag"));
    a = ac->action("showParallelTextHeaders");
    Q_ASSERT(a != 0);
    a->setText(tr("Show parallel text headers"));
}

/** Initializes the SIGNAL / SLOT connections */
void BibleTime::initConnections() {
    // Bookmarks page connections:
    connect(m_bookmarksPage, SIGNAL(createReadDisplayWindow(QList<CSwordModuleInfo*>, const QString&)),
            this,            SLOT(createReadDisplayWindow(QList<CSwordModuleInfo*>, const QString&)));

    // Bookshelf dock connections:
    connect(m_bookshelfDock, SIGNAL(moduleOpenTriggered(CSwordModuleInfo*)),
            this, SLOT(createReadDisplayWindow(CSwordModuleInfo*)));
    connect(m_bookshelfDock, SIGNAL(moduleSearchTriggered(CSwordModuleInfo*)),
            this, SLOT(searchInModule(CSwordModuleInfo*)));
    connect(m_bookshelfDock, SIGNAL(moduleEditPlainTriggered(CSwordModuleInfo*)),
            this, SLOT(moduleEditPlain(CSwordModuleInfo*)));
    connect(m_bookshelfDock, SIGNAL(moduleEditHtmlTriggered(CSwordModuleInfo*)),
            this, SLOT(moduleEditHtml(CSwordModuleInfo*)));
    connect(m_bookshelfDock, SIGNAL(moduleUnlockTriggered(CSwordModuleInfo*)),
            this, SLOT(slotModuleUnlock(CSwordModuleInfo*)));
    connect(m_bookshelfDock, SIGNAL(moduleAboutTriggered(CSwordModuleInfo*)),
            this, SLOT(moduleAbout(CSwordModuleInfo*)));
}

void BibleTime::initSwordConfigFile() {
// On Windows the sword.conf must be created before the initialization of sword
// It will contain the LocalePath which is used for sword locales
// It also contains a DataPath to the %ALLUSERSPROFILE%\Sword directory
// If this is not done here, the sword locales.d won't be found
#ifdef Q_WS_WIN
    namespace DU = util::directory;
    QString configFile = util::directory::getUserHomeSwordDir().filePath("sword.conf");
    QFile file(configFile);
    if (file.exists()) {
        return;
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out << "\n";
    out << "[Install]\n";
    out << "DataPath="   << DU::convertDirSeparators( DU::getSharedSwordDir().absolutePath()) << "\n";
    out << "LocalePath=" << DU::convertDirSeparators(DU::getApplicationSwordDir().absolutePath()) << "\n";
    out << "\n";
    file.close();
#endif

#ifdef Q_WS_MAC
    namespace DU = util::directory;
    QString configFile = util::directory::getUserHomeSwordDir().filePath("sword.conf");
    QFile file(configFile);
    if (file.exists()) {
        return;
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out << "\n";
    out << "[Install]\n";
    out << "DataPath="   << DU::convertDirSeparators( DU::getUserHomeSwordDir().absolutePath()) << "\n";
    out << "\n";
    file.close();
#endif
}

/** Initializes the backend */
void BibleTime::initBackends() {
    qDebug() << "BibleTime::initBackends";

    initSwordConfigFile();

    sword::StringMgr::setSystemStringMgr( new BTStringMgr() );
    sword::SWLog::getSystemLog()->setLogLevel(sword::SWLog::LOG_ERROR);

    if (qApp->property("--debug").toBool()) {
        sword::SWLog::getSystemLog()->setLogLevel(sword::SWLog::LOG_DEBUG);
    }

#ifdef Q_WS_MAC
    // set a LocaleMgr with a fixed path to the locales.d of the DMG image on MacOS
    // note: this must be done after setting the BTStringMgr, because this will reset the LocaleMgr
    qDebug() << "Using sword locales dir: " << util::directory::getSwordLocalesDir().absolutePath().toUtf8();
    sword::LocaleMgr::setSystemLocaleMgr(new sword::LocaleMgr(util::directory::getSwordLocalesDir().absolutePath().toUtf8()));
#endif

    CSwordBackend *backend = CSwordBackend::createInstance();
    backend->booknameLanguage(CBTConfig::get(CBTConfig::language) );

    const CSwordBackend::LoadError errorCode = CSwordBackend::instance()->initModules(CSwordBackend::OtherChange);

    if (errorCode != CSwordBackend::NoError) {
        //show error message that initBackend failed
        /// \todo
//         switch (errorCode) {
//             case CSwordBackend::NoSwordConfig: //mods.d or mods.conf missing
//             {
//                 KStartupLogo::hideSplash();
//                 qDebug() << "case CSwordBackend::NoSwordConfig";
//                 BookshelfManager::CSwordSetupDialog dlg;
//                 dlg.showPart( BookshelfManager::CSwordSetupDialog::Sword );
//                 dlg.exec();
//                 break;
//             }
//
//             case CSwordBackend::NoModules: //no modules installed, but config exists
//             {
//                 KStartupLogo::hideSplash();
//                 qDebug() << "case CSwordBackend::NoModules";
//                 BookshelfManager::CSwordSetupDialog dlg;
//                 dlg.showPart( BookshelfManager::CSwordSetupDialog::Install );
//                 dlg.exec();
//                 break;
//             }
//
//             default: //unknown error
//             {
//                 KStartupLogo::hideSplash();
//                 qDebug() << "unknown error";
//                 BookshelfManager::CSwordSetupDialog dlg;
//                 dlg.showPart( BookshelfManager::CSwordSetupDialog::Sword );
//                 dlg.exec();
//                 break;
//             }
//         }
    }

    // This function will
    // - delete all orphaned indexes (no module present) if autoDeleteOrphanedIndices is true
    // - delete all indices of modules where hasIndex() returns false
    backend->deleteOrphanedIndices();

}

void BibleTime::applyProfileSettings( CProfile* p ) {
    qDebug() << "BibleTime::applyProfileSettings";
    Q_ASSERT(p);
    if (!p) return;

    //first Main Window geometry
    restoreGeometry(p->getMainwindowGeometry());
    restoreState(p->getMainwindowState());
    m_windowFullscreenAction->setChecked(isFullScreen());

    const CMDIArea::MDIArrangementMode newArrangementMode = p->getMDIArrangementMode();
    //make sure actions are updated by calling the slot functions
    //updatesEnabled in the MDI area is false atm, so changes won't actually be displayed yet
    switch (newArrangementMode) {
        case CMDIArea::ArrangementModeTileVertical:
            slotAutoTileVertical();
            break;
        case CMDIArea::ArrangementModeTileHorizontal:
            slotAutoTileHorizontal();
            break;
        case CMDIArea::ArrangementModeCascade:
            slotAutoCascade();
            break;
        case CMDIArea::ArrangementModeTile:
            slotAutoTile();
            break;
        case CMDIArea::ArrangementModeTabbed:
            slotAutoTabbed();
            break;
        case CMDIArea::ArrangementModeManual:
            slotManualArrangementMode();
            break;
        default:
            slotAutoTileVertical();
            break;
    }
    layout()->invalidate();
}

void BibleTime::storeProfileSettings( CProfile* p ) {
    Q_ASSERT(p && m_windowFullscreenAction);
    if (!p || !m_windowFullscreenAction) return;

    p->setMainwindowState(saveState());
    p->setMainwindowGeometry(saveGeometry());
    p->setMDIArrangementMode(m_mdi->getMDIArrangementMode());
}

#if BT_DEBUG

QLabel *BibleTime::m_debugWindow = 0;
QMutex BibleTime::m_debugWindowLock;

void BibleTime::slotShowDebugWindow(bool show) {
    if (show) {
        QMutexLocker lock(&m_debugWindowLock);
        if (m_debugWindow == 0) {
            m_debugWindow = new QLabel(0, Qt::Dialog);
            m_debugWindow->setAttribute(Qt::WA_DeleteOnClose);
            m_debugWindow->setTextFormat(Qt::RichText);
            m_debugWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            m_debugWindow->setWindowTitle(tr("Whats this widget?"));
        }
        m_debugWindow->show();
        connect(m_debugWindow, SIGNAL(destroyed()),
                this,          SLOT(slotDebugWindowClosing()), Qt::DirectConnection);
        QTimer::singleShot(0, this, SLOT(slotDebugTimeout()));
    } else {
        deleteDebugWindow();
    }
}

void BibleTime::deleteDebugWindow() {
    QMutexLocker lock(&m_debugWindowLock);
    if (m_debugWindow != 0) {
        m_debugWindow->disconnect(SIGNAL(destroyed()), this, SLOT(slotDebugWindowClosing()));
        delete m_debugWindow;
        m_debugWindow = 0;
    }
}

void BibleTime::slotDebugWindowClosing() {
    QMutexLocker lock(&m_debugWindowLock);
    m_debugWindow = 0;
    m_debugWidgetAction->setChecked(false);
}

void BibleTime::slotDebugTimeout() {
    QMutexLocker lock(&m_debugWindowLock);
    if (m_debugWindow == 0 || m_debugWindow->isVisible() == false) return;

    QTimer::singleShot(0, this, SLOT(slotDebugTimeout()));
    QObject *w = QApplication::widgetAt(QCursor::pos());
    if (w != 0) {
        QString objectHierarchy;
        do {
            const QMetaObject *m = w->metaObject();
            QString classHierarchy;
            do {
                if (!classHierarchy.isEmpty()) classHierarchy += ": ";
                classHierarchy += m->className();

                m = m->superClass();
            } while (m != 0);
            if (!objectHierarchy.isEmpty()) {
                objectHierarchy += "<br/><b>child of:</b> ";
            } else {
                objectHierarchy += "<b>This widget is:</b> ";
            }
            objectHierarchy += classHierarchy;
            w = w->parent();
        } while (w != 0);
        m_debugWindow->setText(objectHierarchy);
    } else {
        m_debugWindow->setText("No widget");
    }
    m_debugWindow->resize(m_debugWindow->minimumSizeHint());
}

#endif
