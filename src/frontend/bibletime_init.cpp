/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bibletime.h"

#include <QActionGroup>
#include <QDebug>
#include <QDockWidget>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPointer>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include "../backend/config/btconfig.h"
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "../util/cresmgr.h"
#include "bibletimeapp.h"
#include "btbookshelfdockwidget.h"
#include "btopenworkaction.h"
#include "cinfodisplay.h"
#include "cmdiarea.h"
#include "display/btfindwidget.h"
#include "displaywindow/btmodulechooserbar.h"
#include "keychooser/ckeychooser.h"
#include "bookmarks/cbookmarkindex.h"

// Sword includes:
#include <swmgr.h>


using namespace InfoDisplay;

BibleTime::ActionCollection::ActionCollection(
        QAction * const showBookshelfAction,
        QAction * const showBookmarksAction,
        QAction * const showMagAction,
        QObject * const parent)
    : BtActionCollection(parent)
{
    // File menu:

    file.openWork =
            new BtOpenWorkAction(
                btConfig(),
                QStringLiteral("GUI/mainWindow/openWorkAction/grouping"),
                this);

    file.quit = new QAction(this);
    file.quit->setIcon(CResMgr::mainMenu::file::quit::icon());
    file.quit->setMenuRole(QAction::QuitRole);
    file.quit->setShortcut(tr("Ctrl+Q", "File|Quit"));
    addAction(QStringLiteral("quit"), file.quit);

    // View menu:

    view.fullscreen = new QAction(this);
    view.fullscreen->setCheckable(true);
    view.fullscreen->setIcon(CResMgr::mainMenu::view::showFullscreen::icon());
    view.fullscreen->setShortcut(tr("F11", "View|Full screen"));
    addAction(QStringLiteral("toggleFullscreen"), view.fullscreen);

    view.showBookshelf = showBookshelfAction;
    view.showBookshelf->setIcon(CResMgr::mainMenu::view::showBookshelf::icon());
    addAction(QStringLiteral("showBookshelf"), view.showBookshelf);

    view.showBookmarks = showBookmarksAction;
    view.showBookmarks->setIcon(CResMgr::mainMenu::view::showBookmarks::icon());
    addAction(QStringLiteral("showBookmarks"), view.showBookmarks);

    view.showMag = showMagAction;
    view.showMag->setIcon(CResMgr::mainMenu::view::showMag::icon());
    view.showMag->setShortcut(tr("F8", "View|Show mag"));
    addAction(QStringLiteral("showMag"), view.showMag);

    view.showParallelTextHeadings = new QAction(this);
    view.showParallelTextHeadings->setCheckable(true);
    addAction(QStringLiteral("showParallelTextHeaders"),
              view.showParallelTextHeadings);

    view.toolbars.showMainToolbar = new QAction(this);
    view.toolbars.showMainToolbar->setCheckable(true);
    view.toolbars.showMainToolbar->setShortcut(
                tr("F6", "View|Show main toolbar"));
    addAction(QStringLiteral("showToolbar"), view.toolbars.showMainToolbar);

    view.toolbars.showNavigationToolbar = new QAction(this);
    view.toolbars.showNavigationToolbar->setCheckable(true);
    addAction(QStringLiteral("showNavigation"),
              view.toolbars.showNavigationToolbar);

    view.toolbars.showWorksToolbar = new QAction(this);
    view.toolbars.showWorksToolbar->setCheckable(true);
    addAction(QStringLiteral("showWorks"), view.toolbars.showWorksToolbar);

    view.toolbars.showToolsToolbar = new QAction(this);
    view.toolbars.showToolsToolbar->setCheckable(true);
    addAction(QStringLiteral("showTools"), view.toolbars.showToolsToolbar);

    view.toolbars.showToolbarsInTextWindows = new QAction(this);
    view.toolbars.showToolbarsInTextWindows->setCheckable(true);
    addAction(QStringLiteral("showToolbarsInTextWindows"),
              view.toolbars.showToolbarsInTextWindows);

    view.scroll.autoScrollUp = new QAction(this);
    view.scroll.autoScrollUp->setShortcut(
                tr("Shift+Up", "View|Scroll|Auto scroll up"));
    addAction(QStringLiteral("autoScrollUp"), view.scroll.autoScrollUp);

    view.scroll.autoScrollDown = new QAction(this);
    view.scroll.autoScrollDown->setShortcut(
                tr("Shift+Down", "View|Scroll|Auto scroll down"));
    addAction(QStringLiteral("autoScrollDown"), view.scroll.autoScrollDown);

    view.scroll.pauseAutoScroll = new QAction(this);
    view.scroll.pauseAutoScroll->setDisabled(true);
    view.scroll.pauseAutoScroll->setShortcut(
                tr("Space", "View|Scroll|Auto scroll pause"));
    addAction(QStringLiteral("autoScrollPause"), view.scroll.pauseAutoScroll);

    // Search menu:

    search.searchOpenWorks = new QAction(this);
    search.searchOpenWorks->setIcon(
                CResMgr::mainMenu::mainIndex::search::icon());
    search.searchOpenWorks->setShortcut(
                tr("Ctrl+O", "Search|Search in open works"));
    addAction(QStringLiteral("searchOpenWorks"), search.searchOpenWorks);

    search.searchStandardBible = new QAction(this);
    search.searchStandardBible->setIcon(
                CResMgr::mainMenu::mainIndex::searchdefaultbible::icon());
    search.searchStandardBible->setShortcut(
                tr("Ctrl+Alt+F", "Search|Search in the default Bible"));
    addAction(QStringLiteral("searchStdBible"), search.searchStandardBible);

    // Windows menu:

    windows.closeWindow = new QAction(this);
    windows.closeWindow->setIcon(CResMgr::mainMenu::window::close::icon());
    windows.closeWindow->setShortcut(tr("Ctrl+W", "Windows|Close window"));
    addAction(QStringLiteral("closeWindow"), windows.closeWindow);

    windows.closeAllWindows = new QAction(this);
    windows.closeAllWindows->setIcon(
                CResMgr::mainMenu::window::closeAll::icon());
    windows.closeAllWindows->setShortcut(
                tr("Ctrl+Alt+W", "Windows|Close all windows"));
    addAction(QStringLiteral("closeAllWindows"), windows.closeAllWindows);

    windows.tileVertically = new QAction(this);
    windows.tileVertically->setIcon(
                CResMgr::mainMenu::window::tileVertical::icon());
    windows.tileVertically->setShortcut(
                tr("Ctrl+G", "Windows|Tile vertically"));
    addAction(QStringLiteral("tileVertically"), windows.tileVertically);

    windows.tileHorizontally = new QAction(this);
    windows.tileHorizontally->setIcon(
                CResMgr::mainMenu::window::tileHorizontal::icon());
    windows.tileHorizontally->setShortcut(
                tr("Ctrl+H", "Windows|Tile horizontally"));
    addAction(QStringLiteral("tileHorizontally"), windows.tileHorizontally);

    windows.tile = new QAction(this);
    windows.tile->setIcon(CResMgr::mainMenu::window::tile::icon());
    windows.tile->setShortcut(tr("Ctrl+I", "Windows|Tile"));
    addAction(QStringLiteral("tile"), windows.tile);

    windows.cascade = new QAction(this);
    windows.cascade->setIcon(CResMgr::mainMenu::window::cascade::icon());
    windows.cascade->setShortcut(tr("Ctrl+J", "Windows|Cascade"));
    addAction(QStringLiteral("cascade"), windows.cascade);

    namespace AM = CResMgr::mainMenu::window::arrangementMode;

    windows.arrangementMode.manual = new QAction(this);
    windows.arrangementMode.manual->setCheckable(true);
    windows.arrangementMode.manual->setIcon(AM::manual::icon());
    windows.arrangementMode.manual->setShortcut(
                tr("Ctrl+Alt+M", "Windows|Arrangement mode|Manual"));
    addAction(QStringLiteral("manualArrangement"),
              windows.arrangementMode.manual);

    windows.arrangementMode.tabbed = new QAction(this);
    windows.arrangementMode.tabbed->setCheckable(true);
    windows.arrangementMode.tabbed->setIcon(AM::autoTabbed::icon());
    windows.arrangementMode.tabbed->setShortcut(
                tr("Ctrl+Alt+T", "Windows|Arrangement mode|Tabbed"));
    addAction(QStringLiteral("autoTabbed"), windows.arrangementMode.tabbed);

    windows.arrangementMode.autoTileVertically = new QAction(this);
    windows.arrangementMode.autoTileVertically->setCheckable(true);
    windows.arrangementMode.autoTileVertically->setIcon(
                AM::autoTileVertical::icon());
    windows.arrangementMode.autoTileVertically->setShortcut(
                tr("Ctrl+Alt+G",
                   "Windows|Arrangement mode|Auto tile vertically"));
    addAction(QStringLiteral("autoVertical"),
              windows.arrangementMode.autoTileVertically);

    windows.arrangementMode.autoTileHorizontally = new QAction(this);
    windows.arrangementMode.autoTileHorizontally->setCheckable(true);
    windows.arrangementMode.autoTileHorizontally->setIcon(
                AM::autoTileHorizontal::icon());
    windows.arrangementMode.autoTileHorizontally->setShortcut(
                tr("Ctrl+Alt+H",
                   "Windows|Arrangement mode|Auto tile horizontally"));
    addAction(QStringLiteral("autoHorizontal"),
              windows.arrangementMode.autoTileHorizontally);

    windows.arrangementMode.autoTile = new QAction(this);
    windows.arrangementMode.autoTile->setCheckable(true);
    windows.arrangementMode.autoTile->setIcon(AM::autoTile::icon());
    windows.arrangementMode.autoTile->setShortcut(
                tr("Ctrl+Alt+I", "Windows|Arrangement mode|Auto tile"));
    addAction(QStringLiteral("autoTile"), windows.arrangementMode.autoTile);

    windows.arrangementMode.autoCascade = new QAction(this);
    windows.arrangementMode.autoCascade->setCheckable(true);
    windows.arrangementMode.autoCascade->setIcon(AM::autoCascade::icon());
    windows.arrangementMode.autoCascade->setShortcut(
                tr("Ctrl+Alt+J", "Windows|Arrangement mode|Auto cascade"));
    addAction(QStringLiteral("autoCascade"),
              windows.arrangementMode.autoCascade);

    windows.saveAsNewSession = new QAction(this);
    windows.saveAsNewSession->setIcon(
                CResMgr::mainMenu::window::saveToNewProfile::icon());
    windows.saveAsNewSession->setShortcut(
                tr("Ctrl+Alt+S", "Windows|Save as new session..."));
    addAction(QStringLiteral("saveNewSession"), windows.saveAsNewSession);

    settings.bookshelfManager = new QAction(this);
    settings.bookshelfManager->setIcon(
                CResMgr::mainMenu::settings::swordSetupDialog::icon());
    settings.bookshelfManager->setShortcut(
                tr("F4", "Settings|Bookshelf Manager..."));
    addAction(QStringLiteral("bookshelfWizard"), settings.bookshelfManager);

    settings.configureBibleTime = new QAction(this);
    settings.configureBibleTime->setMenuRole(QAction::PreferencesRole);
    settings.configureBibleTime->setIcon(
                CResMgr::mainMenu::settings::configureDialog::icon());
    addAction(QStringLiteral("setPreferences"), settings.configureBibleTime);

    help.handbook = new QAction(this);
    help.handbook->setMenuRole(QAction::ApplicationSpecificRole);
    help.handbook->setIcon(CResMgr::mainMenu::help::handbook::icon());
    help.handbook->setShortcut(tr("F1", "Help|Handbook"));
    addAction(QStringLiteral("openHandbook"), help.handbook);

    help.bibleStudyHowto = new QAction(this);
    help.bibleStudyHowto->setIcon(
                CResMgr::mainMenu::help::bibleStudyHowTo::icon());
    help.bibleStudyHowto->setShortcut(tr("F2", "Help|Bible Study Howto"));
    addAction(QStringLiteral("bibleStudyHowto"), help.bibleStudyHowto);

    help.tipOfTheDay = new QAction(this);
    help.tipOfTheDay->setIcon(CResMgr::mainMenu::help::tipOfTheDay::icon());
    help.tipOfTheDay->setShortcut(tr("F3", "Help|Tip of the day..."));
    addAction(QStringLiteral("tipOfTheDay"), help.tipOfTheDay);

    help.aboutBibleTime = new QAction(this);
    help.aboutBibleTime->setMenuRole(QAction::AboutRole);
    help.aboutBibleTime->setIcon(
                CResMgr::mainMenu::help::aboutBibleTime::icon());
    addAction(QStringLiteral("aboutBibleTime"), help.aboutBibleTime);

    retranslateUi();
}

void BibleTime::ActionCollection::retranslateUi() {
    file.quit->setText(tr("&Quit"));
    file.quit->setToolTip(tr("Quit BibleTime"));

    view.fullscreen->setText(tr("&Full screen"));
    view.fullscreen->setToolTip(tr("Display BibleTime in full screen"));

    view.showBookshelf->setText(tr("Show bookshelf"));
    view.showBookshelf->setToolTip(
                tr("Toggle visibility of the bookshelf window"));

    view.showBookmarks->setText(tr("Show bookmarks"));
    view.showBookmarks->setToolTip(
                tr("Toggle visibility of the bookmarks window"));

    view.showMag->setText(tr("Show mag"));
    view.showMag->setToolTip(tr("Toggle visibility of the mag window"));

    view.showParallelTextHeadings->setText(tr("Show parallel text headers"));
    view.showParallelTextHeadings->setToolTip(
                tr("Toggle the works header row visibility in windows"));

    view.toolbars.showMainToolbar->setText(tr("Show main toolbar"));
    view.toolbars.showMainToolbar->setToolTip(
                tr("Toggle the main toolbar visibility"));

    view.toolbars.showNavigationToolbar->setText(tr("Show navigation toolbar"));
    view.toolbars.showNavigationToolbar->setToolTip(
                tr("Toggle the navigation toolbar visibility"));

    view.toolbars.showWorksToolbar->setText(tr("Show works toolbar"));
    view.toolbars.showWorksToolbar->setToolTip(
                tr("Toggle the works toolbar visibility"));

    view.toolbars.showToolsToolbar->setText(tr("Show tools toolbar"));
    view.toolbars.showToolsToolbar->setToolTip(
                tr("Toggle the window tools toolbar visibility"));

    view.toolbars.showToolbarsInTextWindows->setText(
                tr("Show toolbars in text windows"));
    view.toolbars.showToolbarsInTextWindows->setToolTip(
                tr("Toggle the location of subwindow-specific toolbars between "
                   "the main window and subwindows"));

    view.scroll.autoScrollUp->setText(tr("Auto scroll up"));
    view.scroll.autoScrollUp->setToolTip(
                tr("Adjust automatic scrolling speed in the up direction"));

    view.scroll.autoScrollDown->setText(tr("Auto scroll down"));
    view.scroll.autoScrollDown->setToolTip(
                tr("Adjust automatic scrolling speed in the down direction"));

    view.scroll.pauseAutoScroll->setText(tr("Pause auto scroll"));
    view.scroll.pauseAutoScroll->setToolTip(tr("Pause/resume auto scrolling"));

    search.searchOpenWorks->setText(tr("Search in &open works..."));
    search.searchOpenWorks->setToolTip(
                tr("Search in all works that are currently open"));

    search.searchStandardBible->setText(tr("Search in standard &Bible..."));
    search.searchStandardBible->setToolTip(tr("Search in the standard Bible"));

    windows.closeWindow->setText(tr("Close &window"));
    windows.closeWindow->setToolTip(tr("Close the current open window"));

    windows.closeAllWindows->setText(tr("Cl&ose all windows"));
    windows.closeAllWindows->setToolTip(
                tr("Close all open windows inside BibleTime"));

    windows.tileVertically->setText(tr("Tile &vertically"));
    windows.tileVertically->setToolTip(
                tr("Vertically tile (arrange side by side) the open windows"));

    windows.tileHorizontally->setText(tr("Tile &horizontally"));
    windows.tileHorizontally->setToolTip(
                tr("Horizontally tile (arrange on top of each other) the open "
                   "windows"));

    windows.tile->setText(tr("&Tile"));
    windows.tile->setToolTip(tr("Tile the open windows"));

    windows.cascade->setText(tr("&Cascade"));
    windows.cascade->setToolTip(tr("Cascade the open windows"));

    windows.arrangementMode.manual->setText(tr("&Manual mode"));
    windows.arrangementMode.manual->setToolTip(
                tr("Manually arrange the open windows"));

    windows.arrangementMode.tabbed->setText(tr("Ta&bbed"));
    windows.arrangementMode.tabbed->setToolTip(
                tr("Automatically tab the open windows"));

    windows.arrangementMode.autoTileVertically->setText(
                tr("Auto-tile &vertically"));
    windows.arrangementMode.autoTileVertically->setToolTip(
                tr("Automatically tile the open windows vertically (arrange "
                   "side by side)"));

    windows.arrangementMode.autoTileHorizontally->setText(
                tr("Auto-tile &horizontally"));
    windows.arrangementMode.autoTileHorizontally->setToolTip(
                tr("Automatically tile the open windows horizontally (arrange "
                   "on top of each other)"));

    windows.arrangementMode.autoTile->setText(tr("Auto-&tile"));
    windows.arrangementMode.autoTile->setToolTip(
                tr("Automatically tile the open windows"));

    windows.arrangementMode.autoCascade->setText(tr("Auto-&cascade"));
    windows.arrangementMode.autoCascade->setToolTip(
                tr("Automatically cascade the open windows"));

    windows.saveAsNewSession->setText(tr("Save as &new session..."));
    windows.saveAsNewSession->setToolTip(tr("Create and save a new session"));

    settings.bookshelfManager->setText(tr("Bookshelf Manager..."));
    settings.bookshelfManager->setToolTip(
                tr("Configure your bookshelf and install/update/remove/index "
                   "works"));

    settings.configureBibleTime->setText(tr("&Configure BibleTime..."));
    settings.configureBibleTime->setToolTip(tr("Set BibleTime's preferences"));

    help.handbook->setText(tr("&Handbook"));
    help.handbook->setToolTip(tr("Open BibleTime's handbook"));

    help.bibleStudyHowto->setText(tr("&Bible Study Howto"));
    help.bibleStudyHowto->setToolTip(
                tr("Open the Bible study HowTo included with BibleTime.<br/>"
                   "This HowTo is an introduction on how to study the Bible in "
                   "an efficient way."));

    help.tipOfTheDay->setText(tr("&Tip of the day..."));
    help.tipOfTheDay->setToolTip(tr("Show tips about BibleTime"));

    help.aboutBibleTime->setText(tr("&About BibleTime"));
    help.aboutBibleTime->setToolTip(
                tr("Information about the BibleTime program"));
}

/**Initializes the view of this widget*/
void BibleTime::initView() {

    // Create menu and toolbar before the mdi area
    createMenuAndToolBar();

    createCentralWidget();

    m_bookshelfDock = new BtBookshelfDockWidget(this);
    m_bookshelfDock->setObjectName("bookshelfDock");
    addDockWidget(Qt::LeftDockWidgetArea, m_bookshelfDock);

    m_bookmarksDock = new QDockWidget(this);
    m_bookmarksDock->setObjectName("bookmarksDock");
    m_bookmarksPage = new CBookmarkIndex(this);
    m_bookmarksDock->setWidget(m_bookmarksPage);
    addDockWidget(Qt::LeftDockWidgetArea, m_bookmarksDock);
    tabifyDockWidget(m_bookmarksDock, m_bookshelfDock);
    m_bookshelfDock->loadBookshelfState();

    m_magDock = new QDockWidget(this);
    m_magDock->setObjectName("magDock");
    m_infoDisplay = new CInfoDisplay(this);
    m_infoDisplay->resize(150, 150);
    m_magDock->setWidget(m_infoDisplay);
    addDockWidget(Qt::LeftDockWidgetArea, m_magDock);

    BT_CONNECT(m_bookshelfDock, &BtBookshelfDockWidget::moduleHovered,
               m_infoDisplay,
               qOverload<CSwordModuleInfo *>(&CInfoDisplay::setInfo));
    BT_CONNECT(m_bookmarksPage, &CBookmarkIndex::magInfoProvided,
               m_infoDisplay,
               qOverload<Rendering::InfoType, QString const &>(
                   &CInfoDisplay::setInfo));

    m_mdi->setMinimumSize(100, 100);
    m_mdi->setFocusPolicy(Qt::ClickFocus);

    BT_CONNECT(&m_autoScrollTimer, &QTimer::timeout,
               this, &BibleTime::slotAutoScroll);
}

static QToolBar* createToolBar(QWidget* parent, bool visible) {
    QToolBar* bar = new QToolBar(parent);
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
}

CKeyChooser* BibleTime::keyChooser() const {
    BT_ASSERT(m_navToolBar);

    return m_navToolBar->findChild<CKeyChooser *>();
}

void BibleTime::createMenuAndToolBar()
{
    // Create menubar
    menuBar();

    m_mainToolBar = createToolBar(this, true);
    addToolBar(m_mainToolBar);

    // Set visibility of main window toolbars based on config
    bool visible =
            !btConfig().session().value<bool>(
                QStringLiteral("GUI/showToolbarsInEachWindow"),
                true);

    m_navToolBar = createToolBar(this, visible);
    addToolBar(m_navToolBar);

    m_worksToolBar = new BtModuleChooserBar(this);
    m_worksToolBar->setVisible(visible);
    addToolBar(m_worksToolBar);

    m_toolsToolBar = createToolBar(this, visible);
    addToolBar(m_toolsToolBar);
}

void BibleTime::createCentralWidget()
{
    m_mdi = new CMDIArea(this);
    m_findWidget = new BtFindWidget(this);
    m_findWidget->setVisible(false);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_mdi);
    layout->addWidget(m_findWidget);

    QWidget* widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    BT_CONNECT(m_findWidget, &BtFindWidget::findNext,
               m_mdi,        &CMDIArea::findNextTextInActiveWindow);

    BT_CONNECT(m_findWidget, &BtFindWidget::findPrevious,
               m_mdi,        &CMDIArea::findPreviousTextInActiveWindow);

    BT_CONNECT(m_findWidget, &BtFindWidget::highlightText,
               m_mdi,        &CMDIArea::highlightTextInActiveWindow);

    BT_CONNECT(m_mdi, &CMDIArea::subWindowActivated,
               this,  &BibleTime::slotActiveWindowChanged);
}

/** Initializes the action objects of the GUI */
void BibleTime::initActions() {
    m_actions = new ActionCollection(m_bookshelfDock->toggleViewAction(),
                                     m_bookmarksDock->toggleViewAction(),
                                     m_magDock->toggleViewAction(),
                                     this);

    // File menu actions:
    BT_CONNECT(m_actions->file.openWork, &BtOpenWorkAction::triggered,
               [this](CSwordModuleInfo * const module)
               { createReadDisplayWindow(module); });
    BT_CONNECT(m_actions->file.quit, &QAction::triggered,
               this, &BibleTime::close);

    // View menu actions:
    BT_CONNECT(m_actions->view.fullscreen, &QAction::triggered,
               this, &BibleTime::toggleFullscreen);

    auto const sessionGuiConf =
            btConfig().session().group(QStringLiteral("GUI"));

    m_actions->view.showParallelTextHeadings->setChecked(
                sessionGuiConf.value<bool>(
                    QStringLiteral("showTextWindowHeaders"),
                    true));
    BT_CONNECT(m_actions->view.showParallelTextHeadings, &QAction::toggled,
               this, &BibleTime::slotToggleTextWindowHeader);

    m_actions->view.toolbars.showMainToolbar->setChecked(
                sessionGuiConf.value<bool>(QStringLiteral("showMainToolbar"),
                                           true));
    BT_CONNECT(m_actions->view.toolbars.showMainToolbar, &QAction::triggered,
               this, &BibleTime::slotToggleMainToolbar);

    m_actions->view.toolbars.showNavigationToolbar->setChecked(
                sessionGuiConf.value<bool>(
                    QStringLiteral("showTextWindowNavigator"), true));
    BT_CONNECT(m_actions->view.toolbars.showNavigationToolbar,
               &QAction::toggled,
               this, &BibleTime::slotToggleNavigatorToolbar);

    m_actions->view.toolbars.showWorksToolbar->setChecked(
                sessionGuiConf.value<bool>(
                    QStringLiteral("showTextWindowModuleSelectorButtons"),
                    true));
    BT_CONNECT(m_actions->view.toolbars.showWorksToolbar, &QAction::toggled,
               this, &BibleTime::slotToggleWorksToolbar);

    m_actions->view.toolbars.showToolsToolbar->setChecked(
                sessionGuiConf.value<bool>(
                    QStringLiteral("showTextWindowToolButtons"),
                    true));
    BT_CONNECT(m_actions->view.toolbars.showToolsToolbar, &QAction::toggled,
               this, &BibleTime::slotToggleToolsToolbar);

    m_actions->view.toolbars.showToolbarsInTextWindows->setChecked(
                sessionGuiConf.value<bool>(
                    QStringLiteral("showToolbarsInEachWindow"),
                    true));
    BT_CONNECT(m_actions->view.toolbars.showToolbarsInTextWindows,
               &QAction::toggled,
               this, &BibleTime::slotToggleToolBarsInEachWindow);

    BT_CONNECT(m_actions->view.scroll.autoScrollUp, &QAction::triggered,
               this, &BibleTime::autoScroll<true>);
    BT_CONNECT(m_actions->view.scroll.autoScrollDown, &QAction::triggered,
               this, &BibleTime::autoScroll<false>);
    BT_CONNECT(m_actions->view.scroll.pauseAutoScroll, &QAction::triggered,
               this, &BibleTime::autoScrollPause);

    // Search menu actions:
    BT_CONNECT(m_actions->search.searchOpenWorks, &QAction::triggered,
               this, &BibleTime::slotSearchModules);
    BT_CONNECT(m_actions->search.searchStandardBible,
               &QAction::triggered,
               this, &BibleTime::slotSearchDefaultBible);

    // Windows menu actions:
    BT_CONNECT(m_actions->windows.closeWindow, &QAction::triggered,
               m_mdi, &CMDIArea::closeActiveSubWindow);
    BT_CONNECT(m_actions->windows.closeAllWindows, &QAction::triggered,
               m_mdi, &CMDIArea::closeAllSubWindows);
    BT_CONNECT(m_actions->windows.tileVertically, &QAction::triggered,
               this, &BibleTime::slotTileVertical);
    BT_CONNECT(m_actions->windows.tileHorizontally, &QAction::triggered,
               this, &BibleTime::slotTileHorizontal);
    BT_CONNECT(m_actions->windows.tile, &QAction::triggered,
               this, &BibleTime::slotTile);
    BT_CONNECT(m_actions->windows.cascade, &QAction::triggered,
               this, &BibleTime::slotCascade);

    alignmentMode alignment =
            sessionGuiConf.value<alignmentMode>(QStringLiteral("alignmentMode"),
                                                autoTileVertical);

    /*
     * All actions related to arrangement modes have to be initialized before calling a slot on them,
     * thus we call them afterwards now.
     */
    {

        QAction * alignmentAction;
        auto const & modeActions = m_actions->windows.arrangementMode;
        switch (alignment) {
            case autoTabbed:
                alignmentAction = modeActions.tabbed; break;
            case autoTileVertical:
                alignmentAction = modeActions.autoTileVertically; break;
            case autoTileHorizontal:
                alignmentAction = modeActions.autoTileHorizontally; break;
            case autoTile:
                alignmentAction = modeActions.autoTile; break;
            case autoCascade:
                alignmentAction = modeActions.autoCascade; break;
            case manual:
            default:
                alignmentAction = modeActions.manual; break;
        }
        alignmentAction->setChecked(true);
        slotUpdateWindowArrangementActions(alignmentAction);
    }
    m_windowArrangementActionGroup = new QActionGroup(this);
    m_windowArrangementActionGroup->addAction(
                m_actions->windows.arrangementMode.manual);
    m_windowArrangementActionGroup->addAction(
                m_actions->windows.arrangementMode.tabbed);
    m_windowArrangementActionGroup->addAction(
                m_actions->windows.arrangementMode.autoTileVertically);
    m_windowArrangementActionGroup->addAction(
                m_actions->windows.arrangementMode.autoTileHorizontally);
    m_windowArrangementActionGroup->addAction(
                m_actions->windows.arrangementMode.autoTile);
    m_windowArrangementActionGroup->addAction(
                m_actions->windows.arrangementMode.autoCascade);
    BT_CONNECT(m_windowArrangementActionGroup, &QActionGroup::triggered,
               this, &BibleTime::slotUpdateWindowArrangementActions);

    BT_CONNECT(m_actions->windows.saveAsNewSession, &QAction::triggered,
               this, &BibleTime::saveToNewProfile);

    // Settings menu actions:

    BT_CONNECT(m_actions->settings.configureBibleTime, &QAction::triggered,
               this, &BibleTime::slotSettingsOptions);
    BT_CONNECT(m_actions->settings.bookshelfManager, &QAction::triggered,
               this, &BibleTime::slotBookshelfWizard);

    // Help menu actions:

    BT_CONNECT(m_actions->help.handbook, &QAction::triggered,
               this, &BibleTime::openOnlineHelp_Handbook);
    BT_CONNECT(m_actions->help.bibleStudyHowto, &QAction::triggered,
               this, &BibleTime::openOnlineHelp_Howto);
    BT_CONNECT(m_actions->help.tipOfTheDay, &QAction::triggered,
               this, &BibleTime::slotOpenTipDialog);
    BT_CONNECT(m_actions->help.aboutBibleTime, &QAction::triggered,
               this, &BibleTime::slotOpenAboutDialog);

    if (btApp->debugMode()) {
        m_debugWidgetAction = new QAction(this);
        m_debugWidgetAction->setCheckable(true);
        BT_CONNECT(m_debugWidgetAction, &QAction::triggered,
                   this,                &BibleTime::slotShowDebugWindow);
    }
}

void BibleTime::initMenubar() {
    // File menu:
    m_fileMenu = new QMenu(this);
    m_fileMenu->addAction(m_actions->file.openWork);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_actions->file.quit);
    menuBar()->addMenu(m_fileMenu);

    // View menu:
    m_viewMenu = new QMenu(this);
    m_viewMenu->addAction(m_actions->view.fullscreen);
    m_viewMenu->addAction(m_actions->view.showBookshelf);
    m_viewMenu->addAction(m_actions->view.showBookmarks);
    m_viewMenu->addAction(m_actions->view.showMag);
    m_viewMenu->addAction(m_actions->view.showParallelTextHeadings);
    m_viewMenu->addSeparator();

    m_toolBarsMenu = new QMenu(this);
    m_toolBarsMenu->addAction(m_actions->view.toolbars.showMainToolbar);
    m_toolBarsMenu->addAction(m_actions->view.toolbars.showNavigationToolbar);
    m_toolBarsMenu->addAction(m_actions->view.toolbars.showWorksToolbar);
    m_toolBarsMenu->addAction(m_actions->view.toolbars.showToolsToolbar);
    m_toolBarsMenu->addSeparator();
    m_toolBarsMenu->addAction(
                m_actions->view.toolbars.showToolbarsInTextWindows);
    m_viewMenu->addMenu(m_toolBarsMenu);
    m_viewMenu->addSeparator();

    m_scrollMenu= new QMenu(this);
    m_scrollMenu->addAction(m_actions->view.scroll.autoScrollUp);
    m_scrollMenu->addAction(m_actions->view.scroll.autoScrollDown);
    m_scrollMenu->addAction(m_actions->view.scroll.pauseAutoScroll);
    m_viewMenu->addMenu(m_scrollMenu);

    menuBar()->addMenu(m_viewMenu);

    // Search menu:
    m_searchMenu = new QMenu(this);
    m_searchMenu->addAction(m_actions->search.searchOpenWorks);
    m_searchMenu->addAction(m_actions->search.searchStandardBible);
    menuBar()->addMenu(m_searchMenu);

    // Window menu:
    m_windowsMenu = new QMenu(this);
    m_openWindowsMenu = new QMenu(this);
    BT_CONNECT(m_openWindowsMenu, &QMenu::aboutToShow,
               this,              &BibleTime::slotOpenWindowsMenuAboutToShow);
    m_windowsMenu->addMenu(m_openWindowsMenu);
    m_windowsMenu->addAction(m_actions->windows.closeWindow);
    m_windowsMenu->addAction(m_actions->windows.closeAllWindows);
    m_windowsMenu->addSeparator();
    m_windowsMenu->addAction(m_actions->windows.tileVertically);
    m_windowsMenu->addAction(m_actions->windows.tileHorizontally);
    m_windowsMenu->addAction(m_actions->windows.tile);
    m_windowsMenu->addAction(m_actions->windows.cascade);
    m_windowArrangementMenu = new QMenu(this);
    m_windowArrangementMenu->addAction(
                m_actions->windows.arrangementMode.manual);
    m_windowArrangementMenu->addAction(
                m_actions->windows.arrangementMode.tabbed);
    m_windowArrangementMenu->addAction(
                m_actions->windows.arrangementMode.autoTileVertically);
    m_windowArrangementMenu->addAction(
                m_actions->windows.arrangementMode.autoTileHorizontally);
    m_windowArrangementMenu->addAction(
                m_actions->windows.arrangementMode.autoTile);
    m_windowArrangementMenu->addAction(
                m_actions->windows.arrangementMode.autoCascade);

    m_windowsMenu->addMenu(m_windowArrangementMenu);
    m_windowsMenu->addSeparator();
    m_windowsMenu->addAction(m_actions->windows.saveAsNewSession);
    m_windowLoadProfileMenu = new QMenu(this);
    m_windowLoadProfileActionGroup = new QActionGroup(m_windowLoadProfileMenu);
    m_windowsMenu->addMenu(m_windowLoadProfileMenu);
    m_windowDeleteProfileMenu = new QMenu(this);
    m_windowsMenu->addMenu(m_windowDeleteProfileMenu);
    BT_CONNECT(m_windowLoadProfileMenu, &QMenu::triggered,
               this, qOverload<QAction *>(&BibleTime::loadProfile));
    BT_CONNECT(m_windowDeleteProfileMenu, &QMenu::triggered,
               this,                      &BibleTime::deleteProfile);
    refreshProfileMenus();
    menuBar()->addMenu(m_windowsMenu);
    BT_CONNECT(m_windowsMenu, &QMenu::aboutToShow,
               this,          &BibleTime::slotWindowMenuAboutToShow);

    m_settingsMenu = new QMenu(this);
    m_settingsMenu->addAction(m_actions->settings.bookshelfManager);
    m_settingsMenu->addSeparator();
    m_settingsMenu->addAction(m_actions->settings.configureBibleTime);
    menuBar()->addMenu(m_settingsMenu);

    // Help menu:
    m_helpMenu = new QMenu(this);
    m_helpMenu->addAction(m_actions->help.handbook);
    m_helpMenu->addAction(m_actions->help.bibleStudyHowto);
    m_helpMenu->addAction(m_actions->help.tipOfTheDay);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_actions->help.aboutBibleTime);
    if (m_debugWidgetAction) {
        m_helpMenu->addSeparator();
        m_helpMenu->addAction(m_debugWidgetAction);
    }
    menuBar()->addMenu(m_helpMenu);
}

void BibleTime::initToolbars() {
    QToolButton *openWorkButton = new QToolButton(this);
    openWorkButton->setDefaultAction(m_actions->file.openWork);
    openWorkButton->setPopupMode(QToolButton::InstantPopup);
    m_mainToolBar->addWidget(openWorkButton);

    m_mainToolBar->addAction(m_actions->view.fullscreen);
    m_mainToolBar->addAction(m_actions->view.showBookshelf);
    m_mainToolBar->addAction(m_actions->view.showBookmarks);
    m_mainToolBar->addAction(m_actions->view.showMag);
    m_mainToolBar->addAction(m_actions->search.searchOpenWorks);
    m_mainToolBar->addAction(m_actions->help.handbook);
}

void BibleTime::retranslateUi() {
    m_bookmarksDock->setWindowTitle(tr("Bookmarks"));
    m_magDock->setWindowTitle(tr("Mag"));
    m_mainToolBar->setWindowTitle(tr("Main toolbar"));
    m_navToolBar->setWindowTitle(tr("Navigation toolbar"));
    m_worksToolBar->setWindowTitle(tr("Works toolbar"));
    m_toolsToolBar->setWindowTitle(tr("Tools toolbar"));

    m_fileMenu->setTitle(tr("&File"));
    m_viewMenu->setTitle(tr("&View"));
    m_toolBarsMenu->setTitle(tr("Toolbars"));
    m_scrollMenu->setTitle(tr("Scroll"));

    m_searchMenu->setTitle(tr("&Search"));
    m_windowsMenu->setTitle(tr("&Windows"));
    m_openWindowsMenu->setTitle(tr("O&pen windows"));
    m_windowArrangementMenu->setTitle(tr("&Arrangement mode"));
    m_windowLoadProfileMenu->setTitle(tr("Sw&itch session"));
    m_windowDeleteProfileMenu->setTitle(tr("&Delete session"));

    m_settingsMenu->setTitle(tr("Se&ttings"));

    m_helpMenu->setTitle(tr("&Help"));

    if (m_debugWidgetAction)
        m_debugWidgetAction->setText(tr("Show \"What's this widget\" dialog"));

    m_actions->retranslateUi();
}

/** Initializes the SIGNAL / SLOT connections */
void BibleTime::initConnections() {
    // Bookmarks page connections:
    BT_CONNECT(m_bookmarksPage, &CBookmarkIndex::createReadDisplayWindow,
               this,
               qOverload<QList<CSwordModuleInfo *>, QString const &>(
                   &BibleTime::createReadDisplayWindow));

    // Bookshelf dock connections:
    BT_CONNECT(m_bookshelfDock, &BtBookshelfDockWidget::moduleOpenTriggered,
               [this](CSwordModuleInfo * const module)
               { createReadDisplayWindow(module); });
    BT_CONNECT(m_bookshelfDock, &BtBookshelfDockWidget::moduleSearchTriggered,
               this,
               [this](CSwordModuleInfo * const m) { openSearchDialog({m}); });
    BT_CONNECT(m_bookshelfDock, &BtBookshelfDockWidget::moduleUnlockTriggered,
               this,            &BibleTime::slotModuleUnlock);
    BT_CONNECT(m_bookshelfDock, &BtBookshelfDockWidget::moduleAboutTriggered,
               this,            &BibleTime::moduleAbout);
    BT_CONNECT(m_bookshelfDock, &BtBookshelfDockWidget::installWorksClicked,
               this,            &BibleTime::slotBookshelfWizard);
}
