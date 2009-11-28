/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
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


namespace InfoDisplay {
class CInfoDisplay;
}
class BtActionClass;
class BtBookshelfDockWidget;
class CBookmarkIndex;
class CDisplayWindow;
class CMDIArea;
class CSwordModuleInfo;
class QAction;
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
  * The classes used to handle all module based stuff are derived from CModuleInfo.
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
  * The frontend contains the classes which interact with the user. For example the main index,
  * the display windows, the searchdialog or the other parts.
  * </p><p>
  * The main index is implemented in the class CGroupManager, the items of the
  * main index are implemented in the class CGroupManagerItem.
  * Each CGroupManagerItem has a type() function which returns the type of
  * the object (Module, Bookmark or Group).<br/>
  * The display windows are all derived from the base class CPresenter.
  * The display windows which handle Sword modules are all derived from the
  * CSwordPresenter class.
  * The display windows which provide functionality are CBiblePresenter for
  * Bibles, CCommentaryPresenter for commentaries and CLexiconPresenter for
  * lexicon and dictionaries.
  * CSwordPresenter provides the essential base functions which are
  * reimplemented in the derived classes (for example CSwordPresenter::lookup).<br/>
  * </p><p>
  * Another important part of the frontend are the keychoosers.
  * They provide an interface to choose a key of a module.
  * The interface for different module types is different.
  * The base class is CKeyChooser which is the factory for the derived classes.
  * Use the function CKeyChooser::createInstance to get the correct
  * keychooser implementation for the desired module.<br/>
  * </p>
  */

/** @mainpage BibleTime - sourcecode documentation
 * BibleTime main page.
 * <p>This is the sourcecode documentation of BibleTime, a Bible study tool for KDE/Linux.
 * BibleTime is devided in two major parts, the backend and the frontend.
 * The backend is mainly a wrapper around Sword's classes to use Qt functionality
 * to allow easy access to it's functionality and to have it in a (more or less :)
 * object oriented structure.</p><br/>
 * <p>
 *       -Introduction to the backend: @ref backend<br/>
 *       -Introduction to the frontend: @ref frontend.<br/>
 * The main class of BibleTime is called @ref BibleTime, which is the main window
 * and initializes all important parts at startup. The text display windows
 * belong to the @ref frontend.
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
        /**
         * construtor of BibleTime
         */
        BibleTime();
        /**
         * destructor of BibleTime
         */
        ~BibleTime();

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
        * Processes the commandline options given to BibleTime.
        */
        void processCommandline();
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
        * Called before a window is closed
        */
        bool queryClose();

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
        void moduleUnlock(CSwordModuleInfo *module);
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
         * This slot is connected with the windowAutoTile_action object
         */
        void slotAutoTileVertical();
        /**
         * This slot is connected with the windowAutoTile_action object
         */
        void slotAutoTileHorizontal();
        /**
         * This slot is connected with the windowAutoCascade_action object
         */
        void slotAutoCascade();
        void slotUpdateWindowArrangementActions( QAction* );

        void slotCascade();
        void slotTileVertical();
        void slotTileHorizontal();

        void slotManualArrangementMode();

        /**
        * Is called when a client was selected in the window menu
        */
        void slotWindowMenuActivated();
        /**
         * Shows/hides the toolbar
         */
        void slotToggleToolbar();
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
        * Is called when settings in the sword setup dialog have been
        * changed (ok or apply)
        */
        void slotSwordSetupChanged();
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
        * Called before quit.
        */
        void slot_aboutToQuit();
        /**
        * Open the About Dialog
        */
        void slotOpenAboutDialog();

    private:
        //  True if window was maximized before last toggle to full screen.
        bool m_WindowWasMaximizedBeforeFullScreen;

        // Docking widgets and their respective content widgets:
        BtBookshelfDockWidget* m_bookshelfDock;
        QDockWidget* m_bookmarksDock;
        CBookmarkIndex* m_bookmarksPage;
        QDockWidget* m_magDock;
        InfoDisplay::CInfoDisplay* m_infoDisplay;

        QToolBar* m_mainToolBar;
        // VIEW menu actions
        QAction* m_viewToolbar_action;
        QMenu* m_windowMenu;
        QMenu* m_openWindowsMenu;
        /** WINDOW menu actions */
        QAction* m_windowCascade_action;
        QAction* m_windowTileHorizontal_action;
        QAction* m_windowTileVertical_action;
        QAction* m_windowManualMode_action;
        QAction* m_windowAutoCascade_action;
        QAction* m_windowAutoTileVertical_action;
        QAction* m_windowAutoTileHorizontal_action;
        QAction* m_windowClose_action;
        QAction* m_windowCloseAll_action;
        BtActionCollection* m_actionCollection;

        QMenu* m_windowSaveProfileMenu;
        QAction* m_windowSaveToNewProfile_action;
        QMenu* m_windowLoadProfileMenu;
        QMenu* m_windowDeleteProfileMenu;
        QAction* m_windowFullscreen_action;

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
        void reloadModules();
        //helper function
        void syncAllModulesByType(const CSwordModuleInfo::ModuleType type, const QString& key);
};

#endif
