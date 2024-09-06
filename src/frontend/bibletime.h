/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include <QMainWindow>

#include <QList>
#include <QPointer>
#ifdef BUILD_TEXT_TO_SPEECH
#include <QTextToSpeech>
#endif
#include <QTimer>
#include "../backend/drivers/btmodulelist.h"
#include "../backend/drivers/cswordmoduleinfo.h"


namespace InfoDisplay { class CInfoDisplay; }
namespace Search { class CSearchDialog; }
class BibleTimeApp;
class BtActionClass;
class BtActionCollection;
class BtBookshelfDockWidget;
class BtFindWidget;
class BtModuleChooserBar;
class BtModelViewReadDisplay;
class BtOpenWorkAction;
class CBookmarkIndex;
class CDisplayWindow;
class CKeyChooser;
class CMDIArea;
class QAction;
class QMdiSubWindow;
class QMenu;
class QActionGroup;
class QToolBar;
class QSplitter;

/**
  * @page backend The structure of the backend
  * <p>
  * The backend implementation for Sword is called CSwordBackend, the classes we use
  * to work with keys are called CSwordVerseKey and CSwordLDKey, both are derived from
  * the class CSwordKey.
  * The CSwordKey derived classes used for Sword do also inherit the classes
  * VerseKey (CSwordVerseKey)
  * and SWKey (CSwordLDKey).
  * </p>
  * <p>
  * The classes used to handle all module based stuff are derived from CSwordModuleInfo.
  * The module classes are: CSwordModuleInfo (for Sword modules),
  * CSwordBibleModuleInfo (for bibles), CSwordCommentaryModuleInfo (for commentaries) and
  * CSwordLexiconModuleInfo (for lexicons).
  * Have a look at the class documentation of the mentioned classes to learn how the
  * structure of them looks like and which class inherits which other class.
  * </p>
  * <p>
  * The first objects which should be created in the application is the backend
  * (for Sword the class is called CSwordBackend).
  * Then create all the different module classes for the correct Sword modules.
  * Have a look at
  * BibleTime::initBackend() to see how it's done in BibleTime.@br
  * Later you can work with them for example by using the CSwordKey and
  * CSwordModuleInfo derived class.
  * </p>
  */

/**
  * @page frontend The structure of the frontend
  *
  * <p>
  * The frontend contains the classes which interact with the user. For example the
  * display windows, the searchdialog or the other parts.
  * </p><p>
  * The display windows which provide functionality are CBibleReadWindow for
  * Bibles, CBookReadWindow for books, CCommentaryReadWindow for commentaries and CLexiconReadWindow for
  * lexicon and dictionaries. CHTMLWriteWindow and CPlainWriteWindows are used for editing the Personal Commentary.
  * </p><p>
  *
  * The class CDisplayWindow is the class that various views with in the windows are derived.
  * </p><p>
  *
  * Another important part of the frontend are the keychoosers.
  * They provide an interface to choose a key of a module.
  * The interface for different module types is different.
  * The base class is CKeyChooser which is the factory for the derived classes.
  * Use the function CKeyChooser::createInstance to get the correct
  * keychooser implementation for the desired module.<br/>
  * </p><p>
  * Some major toolbar widgets are CKeyChooser and BtDisplaySettingsButton.
  * </p>
  */

/** @mainpage BibleTime - Source Code Documentation
 * BibleTime main page.
 * <p>This is the source code documentation of BibleTime, a Bible study tool.
 * <p>
 * The main class of BibleTime is called @ref BibleTime, which is the main window
 * and initializes all important parts at startup.
 * </p><p>
 * BibleTime is divided in two major parts, the backend and the frontend.
 * The text display windows belong to the @ref frontend.
 * The backend is mainly a wrapper around Sword's classes to use Qt functionality
 * to allow easy access to it's functionality and to have it in a (more or less :)
 * object oriented structure.</p>
 * <p>
 *       - Introduction to the frontend: @ref frontend<br/>
 *       - Introduction to the backend: @ref backend<br/>
 *       - Introduction to the model/view display: @ref modelviewmain<br/>
 * </p>
 */

/** BibleTime's main class.
 * The main class of BibleTime. Here are the main widgets created.
 *
 * This is the main class of BibleTime! This class creates the GUI, the QAction objects
 * and connects to some slots. Please insert the creation of actions in initActions,
 * the creation of widgets into initView and the connect(...) calls into initConnections.
 * Reading from a config file on creation time should go into readSettings(), saving into
 * saveSettings().
 * This is the general way of all BibleTime classes.
 */
class BibleTime final : public QMainWindow {

    Q_OBJECT

    friend class CDisplayWindow;

public:

    BibleTime(BibleTimeApp & app,
              QWidget * parent = nullptr,
              Qt::WindowFlags flags = Qt::WindowFlags());

    ~BibleTime() override;

    static BibleTime * instance() noexcept { return m_instance; }

    /** Saves the configuration dialog settings, doesn't open dialog. */
    void saveConfigSettings();

    /** \returns a pointer to the Navigation toolbar. */
    QToolBar * navToolBar() const noexcept { return m_navToolBar; }

    /** \returns a pointer to the Works toolbar. */
    BtModuleChooserBar * worksToolBar() const noexcept
    { return m_worksToolBar; }

    /** \returns a pointer to the Tools toolbar. */
    QToolBar * toolsToolBar() const noexcept { return m_toolsToolBar; }

    /** \returns a pointer to the info display. */
    InfoDisplay::CInfoDisplay * infoDisplay() const noexcept
    { return m_infoDisplay; }

    /** Clears the actions of the MDI related toolbars. */
    void clearMdiToolBars();

    /** \returns the main window CKeyChooser. */
    CKeyChooser * keyChooser() const;

    /**
      Displays a dialog which asks the user an unlock key for the given module
      and tries to unlock the module. If an invalid unlock key is given, a
      warning message is issued and the user is again asked for the key.
      \param[in] module The module to unlock.
      \param[in] parent The parent widget for the unlock dialogs.
      \returns whether the module was successfully unlocked.
    */
    static bool moduleUnlock(CSwordModuleInfo * module,
                             QWidget * parent = nullptr);

    /** \returns a pointer to the module associated with the current window. */
    CSwordModuleInfo const * getCurrentModule();

    /** \returns a pointer to the display associated with the current window. */
    BtModelViewReadDisplay * getCurrentDisplay();

    /** Opens the BtFindWidget below the MDI area. */
    void openFindWidget();

    /** Sets the focus on the display widget. */
    void setDisplayFocus();

    void openSearchDialog(BtConstModuleList modules,
                          QString const & searchText = {});

#ifdef BUILD_TEXT_TO_SPEECH
    /**
      Speaks the given text.
      \param[in] text The text to speak.
     */
    void speakText(const QString& text);

    /**
      Creates a QTextToSpeech instance, taking text-to-speech
      settings into account.
      \returns The QTextToSpeech instance.
     */
    static std::unique_ptr<QTextToSpeech> createTextToSpeechInstance();
#endif

public Q_SLOTS:

    /** Opens the optionsdialog of BibleTime. */
    void slotSettingsOptions();

    /** Opens the bookshelf wizard. */
    void slotBookshelfWizard();

    /** Opens the handbook. */
    void openOnlineHelp_Handbook();

    /** Opens the bible study howto. */
    void openOnlineHelp_Howto();

    /** Opens the Tip Dialog. */
    void slotOpenTipDialog();

    /**
      Processes the command-line options given to BibleTime.
      \param[in] ignoreSession Specifies whether --ignore-session was used.
      \param[in] bibleKey If --open-default-bible was used, the bible key
                          specified, or null otherwise.
    */
    void processCommandline(bool ignoreSession, QString const & bibleKey);

    /** Creates QAction's that have keyboard shortcuts. */
    static void insertKeyboardActions(BtActionCollection * const a);

    void autoScrollStop();

Q_SIGNALS:

    void toggledTextWindowHeader(bool newState);
    void toggledTextWindowNavigator(bool newState);
    void toggledTextWindowToolButtons(bool newState);
    void toggledTextWindowModuleChooser(bool newState);
    void colorThemeChanged();

private: // methods:

    bool event(QEvent * event) override;

    /** Creates the main window menu and toolbar. */
    void createMenuAndToolBar();

    /** Creates MDI and and BtFindWidget. */
    void createCentralWidget();

    /** Initializes the view of this widget. */
    void initView();

    /** Initializes the menubar of BibleTime. */
    void initMenubar();

    /** Initializes the SIGNAL / SLOT connections. */
    void initConnections();

    /** Initializes the action objects of the GUI. */
    void initActions();

    /** Initializes the toolbars. */
    void initToolbars();

    /** Retranslates the UI. */
    void retranslateUi();

    /** Retranslates the UI actions. */
    static void retranslateUiActions(BtActionCollection * ac);

    /**
      Refreshes all presenter supporting at least in of the features given as
      parameter.
    */
    void refreshDisplayWindows() const;

    template <bool goingUp>
    void autoScroll();

    /**
      Set the visibility of all tool bars according to the configuration taking
      the toolbarsInEachWindow setting into account.
    */
    void showOrHideToolBars();

    void slotShowDebugWindow(bool);

private Q_SLOTS:

    /**
      Creates a new presenter in the MDI area according to the type of the
      module.
    */
    CDisplayWindow * createReadDisplayWindow(QList<CSwordModuleInfo *> modules,
                                             QString const & key);
    CDisplayWindow * createReadDisplayWindow(CSwordModuleInfo * module,
                                             QString const & key = {});
    void slotModuleUnlock(CSwordModuleInfo * module);
    void moduleAbout(CSwordModuleInfo * module);

    /** Automatically scrolls the display. */
    void slotAutoScroll();

    /** Called when the window menu is about to show. */
    void slotWindowMenuAboutToShow();

    /** Called when the open windows menu is about to show. */
    void slotOpenWindowsMenuAboutToShow();

    void slotUpdateWindowArrangementActions(QAction * trigerredAction);

    void slotCascade();
    void slotTile();
    void slotTileVertical();
    void slotTileHorizontal();

    /** Shows or hides the main toolbar. */
    void slotToggleMainToolbar();
    void slotToggleToolsToolbar();
    void slotToggleNavigatorToolbar();
    void slotToggleWorksToolbar();

    void slotToggleToolBarsInEachWindow();

    /**
      Shows or hides the text window text area headers and sets configuration
      that newly opened windows don't user headers.
    */
    void slotToggleTextWindowHeader();

    /** Used to set the active menu. */
    void slotSetActiveSubWindow(QWidget * window);

    void slotActiveWindowChanged(QMdiSubWindow * window);

    /** Saves the current settings into the currently activated profile. */
    void saveProfile();

    /** Deletes the chosen session from the menu and from disk. */
    void deleteProfile(QAction * action);

    /** Loads the profile with the menu id ID. */
    void loadProfile(QAction * action);

    /** Loads the profile with the given key. */
    void loadProfile(QString const & profileKey);

    /** Reloads the current profile. */
    void reloadProfile();

    /** Toggles between normal and fullscreen mode. */
    void toggleFullscreen();

    void autoScrollPause();
    bool autoScrollAnyKey();

    /** Called when search button is pressed. */
    void slotSearchModules();

    /** Called for search default bible. */
    void slotSearchDefaultBible();

    /** Saves current settings into a new profile. */
    void saveToNewProfile();

    /** Refreshes the save profile and load profile menus. */
    void refreshProfileMenus();

    /** Open the About Dialog. */
    void slotOpenAboutDialog();

private: // fields:

    static BibleTime * m_instance;

    // Docking widgets and their respective content widgets:
    BtBookshelfDockWidget * m_bookshelfDock;
    QDockWidget * m_bookmarksDock;
    CBookmarkIndex * m_bookmarksPage;
    QDockWidget * m_magDock;
    InfoDisplay::CInfoDisplay * m_infoDisplay;

    QToolBar * m_mainToolBar;
    QToolBar * m_navToolBar;
    BtModuleChooserBar * m_worksToolBar;
    QToolBar * m_toolsToolBar;

    // File menu:
    QMenu * m_fileMenu;
    BtOpenWorkAction * m_openWorkAction;
    QAction * m_quitAction;

    // View menu:
    QMenu * m_viewMenu;
    QAction * m_showBookshelfAction;
    QAction * m_showBookmarksAction;
    QAction * m_showMagAction;
    QMenu * m_toolBarsMenu;
    QMenu * m_scrollMenu;
    QAction * m_showMainWindowToolbarAction;
    QAction * m_showTextAreaHeadersAction;
    QAction * m_showTextWindowNavigationAction;
    QAction * m_showTextWindowModuleChooserAction;
    QAction * m_showTextWindowToolButtonsAction;
    QAction * m_toolbarsInEachWindow;

    // Search menu:
    QMenu * m_searchMenu;
    QAction * m_searchOpenWorksAction;
    QAction * m_searchStandardBibleAction;

    // Window menu:
    QMenu * m_windowMenu;
    QMenu * m_openWindowsMenu;
    QAction * m_windowCascadeAction;
    QAction * m_windowTileAction;
    QAction * m_windowTileHorizontalAction;
    QAction * m_windowTileVerticalAction;
    QAction * m_windowManualModeAction;
    QMenu * m_windowArrangementMenu;
    QActionGroup * m_windowArrangementActionGroup;
    QAction * m_windowAutoCascadeAction;
    QAction * m_windowAutoTileAction;
    QAction * m_windowAutoTabbedAction;
    QAction * m_windowAutoTileVerticalAction;
    QAction * m_windowAutoTileHorizontalAction;
    QAction * m_windowCloseAction;
    QAction * m_windowCloseAllAction;
    QAction * m_windowSaveToNewProfileAction;
    QMenu * m_windowLoadProfileMenu;
    QActionGroup * m_windowLoadProfileActionGroup;
    QMenu * m_windowDeleteProfileMenu;

    // Settings menu:
    QMenu * m_settingsMenu;
    QAction * m_setPreferencesAction;
    QAction * m_bookshelfManagerAction;
    QAction * m_bookshelfWizardAction;

    // Help menu:
    QMenu * m_helpMenu;
    QAction * m_openHandbookAction;
    QAction * m_bibleStudyHowtoAction;
    QAction * m_aboutBibleTimeAction;
    QAction * m_tipOfTheDayAction;

    BtActionCollection * m_actionCollection;

    QAction * m_autoScrollUpAction;
    QAction * m_autoScrollDownAction;
    QAction * m_autoScrollPauseAction;

    QAction * m_windowFullscreenAction;

    CMDIArea * m_mdi;
    BtFindWidget * m_findWidget;

    int m_autoScrollSpeed = 0;
    QTimer m_autoScrollTimer;
    QPointer<Search::CSearchDialog> m_searchDialog;

    QAction * m_debugWidgetAction = nullptr;
    QPointer<QWidget> m_debugWindow;

#ifdef BUILD_TEXT_TO_SPEECH
    std::unique_ptr<QTextToSpeech> m_textToSpeech;
#endif

};

extern template void BibleTime::autoScroll<true>();
extern template void BibleTime::autoScroll<false>();
