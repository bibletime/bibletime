/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BIBLETIME_H
#define BIBLETIME_H

#include <QMainWindow>

#include <QList>
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/profile/cprofile.h"
#include "frontend/profile/cprofilemgr.h"
#include <QSignalMapper>
#ifdef BT_DEBUG
#include <QMutex>
#endif


namespace InfoDisplay {
class CInfoDisplay;
}
class BtActionClass;
class BtBookshelfDockWidget;
class BtOpenWorkAction;
class CBookmarkIndex;
class CDisplayWindow;
class CMDIArea;
class CSwordModuleInfo;
class QAction;
class QLabel;
class QMenu;
class QToolBar;
class QSplitter;
class QSignalMapper;

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
  * BibleTime::initBackens() to see how it's done in BibleTime.@br
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
  * The class CDisplay is the class that various views with in the windows are derived. 
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

/** @mainpage BibleTime - sourcecode documentation
 * BibleTime main page.
 * <p>This is the sourcecode documentation of BibleTime, a Bible study tool.
 * <p>
 * The main class of BibleTime is called @ref BibleTime, which is the main window
 * and initializes all important parts at startup. 
 * </p><p>
 * BibleTime is divided in two major parts, the backend and the frontend.
 * The text display windows belong to the @ref frontend.
 * The backend is mainly a wrapper around Sword's classes to use Qt functionality
 * to allow easy access to it's functionality and to have it in a (more or less :)
 * object oriented structure.</p><br/>
 * <p>
 *       -Introduction to the frontend: @ref frontend.<br/>
 *       -Introduction to the backend: @ref backend<br/>
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
class BibleTime : public QMainWindow {
        friend class CDisplayWindow;
        friend class BibleTimeDBusAdaptor;
        Q_OBJECT

    public:

        BibleTime(QWidget *parent = 0, Qt::WindowFlags flags = 0);

        ~BibleTime();

        static inline BibleTime *instance() { return m_instance; }

        /**
        * Reads the settings from the configfile and sets the right properties.
        */
        void readSettings();
        /**
        * Saves the settings of this class
        */
        void saveSettings();
        /**
        * Restores the workspace if the flaf for this is set in the config.
        */
        void restoreWorkspace();
        /**
        * Apply the settings given by the profile p
        */
        void applyProfileSettings( Profile::CProfile* p );
        /**
        * Stores the settings of the mainwindow in the profile p
        */
        void storeProfileSettings( Profile::CProfile* p );
        /**
        *  Save the configuration dialog settings, don't open dialog
        */
        void saveConfigSettings();
        /**
        * Get pointer to Navigation toolbar
        */
        inline QToolBar *navToolBar() const {
            return m_navToolBar;
        }
        /**
        * Get pointer to Works toolbar
        */
        inline BtModuleChooserBar *worksToolBar() const {
            return m_worksToolBar;
        }
        /**
        * Get pointer to Tools toolbar
        */
        inline QToolBar *toolsToolBar() const {
            return m_toolsToolBar;
        }
        /**
        * Get pointer to Format toolbar
        */
        inline QToolBar *formatToolBar() const {
            return m_formatToolBar;
        }

        /**
          \returns a pointer to the info display.
        */
        inline InfoDisplay::CInfoDisplay *infoDisplay() const {
            return m_infoDisplay;
        }

        /**
        * Clears the actions of the MDI related toolbars
        */
        void clearMdiToolBars();

        /**
          Displays a dialog which asks the user an unlock key for the given module and tries
          to unlock the module. If an invalid unlock key is given, a warning message is
          issued and the user is again asked for the key.
          \param[in] module The module to unlock.
          \param[in] parent The parent widget for the unlock dialogs.
          \returns whether the module was successfully unlocked.
        */
        static bool moduleUnlock(CSwordModuleInfo *module, QWidget *parent = 0);

    public slots:
        /**
        * Opens the optionsdialog of BibleTime.
        */
        void slotSettingsOptions();
        /**
        * Opens the optionsdialog of BibleTime.
        */
        void slotSwordSetupDialog();
        /**
        * Opens the handbook.
        */
        void openOnlineHelp_Handbook();
        /**
        * Opens the bible study howto.
        */
        void openOnlineHelp_Howto();

        /**
          Processes the command-line options given to BibleTime.
          \param[in] args the command-line arguments given to BibleTime.
        */
        void processCommandline(const QStringList &args);

        /**
        * Creates QAction's that have keyboard shortcuts
        */
        static void insertKeyboardActions( BtActionCollection* const a );

    protected: // Protected methods
        /**
        * Catch QMainWindow events
        */
        bool event(QEvent* event);
        /**
        * Create the main window menu and toolbar
        */
        void createMenuAndToolBar();
        /**
        * Initializes the sword.conf in the $HOME\Sword directory
        */
        void initSwordConfigFile();
        /**
        * Initializes the view of this widget
        */
        void initView();
        /**
        * Initializes the menubar of BibleTime.
        */
        void initMenubar();
        /**
        * Initializes the SIGNAL / SLOT connections
        */
        void initConnections();
        /**
        * Initializes the backend
        */
        void initBackends();
        /**
        * Initializes the action objects of the GUI
        */
        void initActions();
        /**
          Initializes the toolbars.
        */
        void initToolbars();
        /**
          Retranslates the UI.
        */
        void retranslateUi();
        /**
          Retranslates the UI actions.
        */
        static void retranslateUiActions(BtActionCollection* ac);
        /**
        * Initializes one action object
        */
        QAction* initAction(QAction* action, QString text, QString icon, QKeySequence accel,
                            const QString& tooltip, const QString& actionName, const char* slot );
        /**
        * Refreshes all presenter supporting at least in of the features given as parameter.
        */
        void refreshDisplayWindows();
        /**
        * Refresh main window accelerators
        */
        void refreshBibleTimeAccel();

        /**
        * Reimplemented from QWidget.
        */
        void closeEvent(QCloseEvent *event);

    protected slots:
        /**
         * Creates a new presenter in the MDI area according to the type of the module.
         */
        CDisplayWindow* createReadDisplayWindow(QList<CSwordModuleInfo*> modules, const QString& key);
        CDisplayWindow* createReadDisplayWindow(CSwordModuleInfo* module, const QString& key = QString::null);
        CDisplayWindow* createWriteDisplayWindow(CSwordModuleInfo* module, const QString& key, const CDisplayWindow::WriteWindowType& type);
        CDisplayWindow* moduleEditPlain(CSwordModuleInfo *module);
        CDisplayWindow* moduleEditHtml(CSwordModuleInfo *module);
        void searchInModule(CSwordModuleInfo *module);
        void slotModuleUnlock(CSwordModuleInfo *module);
        void moduleAbout(CSwordModuleInfo *module);
        void quit();

        /**
         * Is called when the window menu is about to show ;-)
         */
        void slotWindowMenuAboutToShow();
        /**
         * Is called when the open windows menu is about to show ;-)
         */
        void slotOpenWindowsMenuAboutToShow();
        /**
         * This slot is connected with the windowAutoTileVerticalAction object
         */
        void slotAutoTileVertical();
        /**
         * This slot is connected with the windowAutoTileHorizontalAction object
         */
        void slotAutoTileHorizontal();
        /**
         * This slot is connected with the windowAutoTileAction object
         */
        void slotAutoTile();
        /**
         * This slot is connected with the windowAutoTabbedAction object
         */
        void slotAutoTabbed();
        /**
         * This slot is connected with the windowAutoCascadeAction object
         */
        void slotAutoCascade();
        void slotUpdateWindowArrangementActions( QAction* );

        void slotCascade();
        void slotTile();
        void slotTileVertical();
        void slotTileHorizontal();

        void slotManualArrangementMode();

        /**
         * Shows/hides the main toolbar
         */
        void slotToggleMainToolbar();
        void slotToggleToolsToolbar();
        void slotToggleNavigatorToolbar();
        void slotToggleWorksToolbar();
        void slotToggleFormatToolbar();

        void slotToggleToolBarsInEachWindow();
        
        /**
        * Shows/hides the text window text area headers and sets
        * configuration that newly opened windows don't user headers.
        */
        void slotToggleTextWindowHeader();

        /**
         * Used to set the active menu
         */
        void slotSetActiveSubWindow(QWidget* window);
        /**
        * Saves to the profile with the menu id ID
        */
        void saveProfile(QAction* action);
        /**
        * Saves the current settings into the currently activatred profile.
        */
        void saveProfile(Profile::CProfile* p);
        /**
        * Deletes the chosen session from the menu and from disk.
        */
        void deleteProfile(QAction* action);
        /**
        * Loads the profile with the menu id ID
        */
        void loadProfile(QAction* action);
        /**
        * Loads the profile with the menu ID id
        */
        void loadProfile(Profile::CProfile* p);
        /**
        * Toggles between normal and fullscreen mode.
        */
        void toggleFullscreen();
        /**
        * Is called when settings in the optionsdialog have been
        * changed (ok or apply)
        */
        void slotSettingsChanged();

        /**
         * Called when search button is pressed
         **/
        void slotSearchModules();
        /**
         * Called for search default bible
         **/
        void slotSearchDefaultBible();
        /**
         Saves current settings into a new profile.
        */
        void saveToNewProfile();
        /**
        * Slot to refresh the save profile and load profile menus.
        */
        void refreshProfileMenus();

        /**
        * Open the About Dialog
        */
        void slotOpenAboutDialog();

    signals:
        void toggledTextWindowHeader(bool newState);
        void toggledTextWindowNavigator(bool newState);
        void toggledTextWindowToolButtons(bool newState);
        void toggledTextWindowModuleChooser(bool newState);
        void toggledTextWindowFormatToolbar(bool newState);

    private:
        static BibleTime *m_instance;

        //  True if window was maximized before last toggle to full screen.
        bool m_WindowWasMaximizedBeforeFullScreen;

        // Docking widgets and their respective content widgets:
        BtBookshelfDockWidget* m_bookshelfDock;
        QDockWidget* m_bookmarksDock;
        CBookmarkIndex* m_bookmarksPage;
        QDockWidget* m_magDock;
        InfoDisplay::CInfoDisplay* m_infoDisplay;

        QToolBar* m_mainToolBar;
        QToolBar* m_navToolBar;
        BtModuleChooserBar* m_worksToolBar;
        QToolBar* m_toolsToolBar;
        QToolBar* m_formatToolBar;

        // File menu:
        QMenu *m_fileMenu;
        BtOpenWorkAction *m_openWorkAction;
        QAction *m_quitAction;

        // View menu:
        QMenu *m_viewMenu;
        QAction* m_viewToolbarAction;
        QAction *m_showBookshelfAction;
        QAction *m_showBookmarksAction;
        QAction *m_showMagAction;
        QMenu *m_toolBarsMenu;
        QAction *m_showTextAreaHeadersAction;
        QAction *m_showTextWindowNavigationAction;
        QAction *m_showTextWindowModuleChooserAction;
        QAction *m_showTextWindowToolButtonsAction;
        QAction *m_showFormatToolbarAction;
        QAction *m_toolbarsInEachWindow;

        // Search menu:
        QMenu *m_searchMenu;
        QAction *m_searchOpenWorksAction;
        QAction *m_searchStandardBibleAction;

        // Window menu:
        QMenu *m_windowMenu;
        QMenu *m_openWindowsMenu;
        QAction *m_windowCascadeAction;
        QAction *m_windowTileAction;
        QAction *m_windowTileHorizontalAction;
        QAction *m_windowTileVerticalAction;
        QAction *m_windowManualModeAction;
        QMenu *m_windowArrangementMenu;
        QAction *m_windowAutoCascadeAction;
        QAction *m_windowAutoTileAction;
        QAction *m_windowAutoTabbedAction;
        QAction *m_windowAutoTileVerticalAction;
        QAction *m_windowAutoTileHorizontalAction;
        QAction *m_windowCloseAction;
        QAction *m_windowCloseAllAction;

        // Settings menu:
        QMenu *m_settingsMenu;
        QAction *m_setPreferencesAction;
        QAction *m_bookshelfManagerAction;

        // Help menu:
        QMenu *m_helpMenu;
        QAction *m_openHandbookAction;
        QAction *m_bibleStudyHowtoAction;
        QAction *m_aboutBibleTimeAction;

        BtActionCollection* m_actionCollection;

        QMenu* m_windowSaveProfileMenu;
        QAction* m_windowSaveToNewProfileAction;
        QMenu* m_windowLoadProfileMenu;
        QMenu* m_windowDeleteProfileMenu;
        QAction* m_windowFullscreenAction;

        /**
         * Signal mapper to map windows to menu items.
         */
        QSignalMapper* m_windowMapper;
        /// \todo remove?
        // QList<QAction*> m_windowOpenWindowsList;

        CMDIArea* m_mdi;

        Profile::CProfileMgr m_profileMgr;


    protected: //DBUS interface implementation
        void closeAllModuleWindows();
        void syncAllBibles(const QString& key);
        void syncAllCommentaries(const QString& key);
        void syncAllLexicons(const QString& key);
        void syncAllVerseBasedModules(const QString& key);
        void openWindow(const QString& moduleName, const QString& key);
        void openDefaultBible(const QString& key);
        QString getCurrentReference();
        QStringList searchInModule(const QString& module, const QString& searchText);
        QStringList searchInOpenModules(const QString& searchText);
        QStringList searchInDefaultBible(const QString& searchText);
        QStringList getModulesOfType(const QString& type);

        // Helper function
        void syncAllModulesByType(const CSwordModuleInfo::ModuleType type, const QString& key);

    private:
        void showOrHideToolBars();
#ifdef BT_DEBUG
        void deleteDebugWindow();
    private slots:
        void slotDebugWindowClosing();
        void slotDebugTimeout();
        void slotShowDebugWindow(bool);
    private:
        QAction *m_debugWidgetAction;
        static QLabel *m_debugWindow;
        static QMutex m_debugWindowLock;
#endif
};

#endif
