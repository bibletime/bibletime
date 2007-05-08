/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef BIBLETIME_H
#define BIBLETIME_H

//Own includes
#include "bibletimeinterface.h"

//Frontend includes
#include "frontend/cprofilemgr.h"
#include "frontend/cprofile.h"
#include "frontend/displaywindow/cdisplaywindow.h"

//Backend includes
#include "backend/cswordmoduleinfo.h"

//KDE includes
#include <kmainwindow.h>

//forward: BT classes
class CMDIArea;
class CDisplayWindow;
class CMainIndex;

namespace InfoDisplay {
	class CInfoDisplay;
}

//KDE classes
class KToggleAction;
class KAccel;
class KPopupMenu;
class KAction;
class KActionMenu;
class KActionCollection;

//QT classes
class QPopupMenu;
class QProgressDialog;
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
  * the object (Module, Bookmark or Group).<BR>
  * The display windows are all derived from the base class CPresenter.
  * The display windows which handle Sword modules are all derived from the
  * CSwordPresenter class.
  * The display windows which provide functionality are CBiblePresenter for
  * Bibles, CCommentaryPresenter for commentaries and CLexiconPresenter for
  * lexicon and dictionaries.
  * CSwordPresenter provides the essential base functions which are
  * reimplemented in the derived classes (for example CSwordPresenter::lookup).<BR>
  * </p><p>
  * Another important part of the frontend are the keychoosers.
  * They provide an interface to choose a key of a module.
  * The interface for different module types is different.
  * The base class is CKeyChooser which is the factory for the derived classes.
  * Use the function CKeyChooser::createInstance to get the correct
  * keychooser implementation for the desired module.<BR>
  * </p>
  */

/** @mainpage BibleTime - sourcecode documentation
 * BibleTime main page.
 * <p>This is the sourcecode documentation of BibleTime, a Bible study tool for KDE/Linux.
 * BibleTime is devided in two major parts, the backend and the frontend.
 * The backend is mainly a wrapper around Sword's classes to use Qt functionality
 * to allow easy access to it's functionality and to have it in a (more or less :)
 * object oriented structure.</p><BR>
 * <p>
 *       -Introduction to the backend: @ref backend<BR>
 *       -Introduction to the frontend: @ref frontend.<BR>
 * The main class of BibleTime is called @ref BibleTime, which is the main window
 * and initializes all important parts at startup. The text display windows
 * belong to the @ref frontend.
 * </p>
 */

/** BibleTime's main class.
 * The main class of BibleTime. Here are the main widgets created.
 *
 * This is the main class of BibleTime! This class creates the GUI, the KAction objects
 * and connects to some slots. Please insert the creation of actions in initActions,
 * the creation of widgets into initView and the connect(...) calls into initConnections.
 * Reading from a config file on creation time should go into readSettings(), saving into
 * saveSettings().
 * This is the general way of all BibleTime classes.
 */
class BibleTime : public KMainWindow, virtual public BibleTimeInterface {
	friend class CMDIArea;
	Q_OBJECT
public:
	/**
	 * construtor of BibleTime
	 */
	BibleTime();
	/**
	 * destrutor of BibleTime
	 */
	virtual ~BibleTime();

	virtual void polish();
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

public slots:
	/**
	* Shows the daily tip
	*/
	void slotHelpTipOfDay();
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
	* Sets the plain caption of the main window
	*/
	virtual void setPlainCaption( const QString& );
	/**
	* Sets the caption of the mainwindow
	*/
	virtual void setCaption(const QString&);
	/**
	* Processes the commandline options given to BibleTime.
	*/
	void processCommandline();

protected: // Protected methods
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
	* Refreshes all presenter supporting at least in of the features given as parameter.
	*/
	void refreshDisplayWindows();
	/**
	* Called before a window is closed
	*/
	bool queryClose();
	/**
	* Called before quit.
	*/
	bool queryExit();
	/**
	* Reimplementation used for session management.
	*/
	void readProperties(KConfig* config);
	/**
	* Reimplementation used for sessions management.
	*/
	void saveProperties(KConfig* myConfig);

protected slots:
	/**
	 * Creates a new presenter in the MDI area according to the type of the module.
	 */
	CDisplayWindow* createReadDisplayWindow(ListCSwordModuleInfo modules, const QString& key);
	CDisplayWindow* createReadDisplayWindow(CSwordModuleInfo* module, const QString& key);
	CDisplayWindow* createWriteDisplayWindow(CSwordModuleInfo* module, const QString& key, const CDisplayWindow::WriteWindowType& type);
	/**
	 * Is called when the window menu is about to show ;-)
	 */
	void slotWindowMenuAboutToShow();
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
	void slotUpdateWindowArrangementActions( KAction* );

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
	* Opens a toolbar editor
	*/
	void slotSettingsToolbar();
	/**
	* Shows or hides the main index part
	*/
	void slotToggleMainIndex();
	/**
	* Shows or hides the info display part.
	*/
	void slotToggleInfoDisplay();
	/**
	* Saves to the profile with the menu id ID
	*/
	void saveProfile(int ID);
	/**
	* Saves the current settings into the currently activatred profile.
	*/
	void saveProfile(Profile::CProfile* p);
	/**
	* Deletes the chosen session from the menu and from disk.
	*/
	void deleteProfile(int ID);
	/**
	* Loads the profile with the menu id ID
	*/
	void loadProfile(int ID);
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


private:
	QPopupMenu* m_windowMenu;
	QPopupMenu* m_editMenu;

	/** VIEW menu actions */
	KToggleAction* m_viewToolbar_action;
	KToggleAction* m_viewMainIndex_action;
	KToggleAction* m_viewInfoDisplay_action;

	/** WINDOW menu actions */
	KAction* m_windowCascade_action;
	KAction* m_windowTileHorizontal_action;
	KAction* m_windowTileVertical_action;
	KActionMenu* m_windowArrangementMode_action;
	KToggleAction* m_windowManualMode_action;
	KToggleAction* m_windowAutoCascade_action;
	KToggleAction* m_windowAutoTileVertical_action;
	KToggleAction* m_windowAutoTileHorizontal_action;
	KAction* m_windowCloseAll_action;

	KActionCollection* m_windowActionCollection;

	KActionMenu* m_windowSaveProfile_action;
	KAction* m_windowSaveToNewProfile_action;
	KActionMenu* m_windowLoadProfile_action;
	KActionMenu* m_windowDeleteProfile_action;
	KToggleAction* m_windowFullscreen_action;

	QPtrList<KAction> m_windowOpenWindowsList;

	KHelpMenu* m_helpMenu;


	bool m_initialized;
	/**
	* The list of installed SWORD modules
	*/
	ListCSwordModuleInfo* m_moduleList;
	QProgressDialog* m_progress;

	Profile::CProfile* m_currentProfile;
	QSplitter* m_mainSplitter;
	QSplitter* m_leftPaneSplitter;
	CMDIArea* m_mdi;

	Profile::CProfileMgr m_profileMgr;
	CSwordBackend* m_backend;

	CMainIndex* m_mainIndex;

	InfoDisplay::CInfoDisplay* m_infoDisplay;

protected: //DCOP interface implementation
	//helper function
	void syncAllModulesByType(const CSwordModuleInfo::ModuleType type, const QString& key);

	//see bibletimeinterface.h for a documentation of these functions
	virtual void closeAllModuleWindows();
	virtual void syncAllBibles(const QString& key);
	virtual void syncAllCommentaries(const QString& key);
	virtual void syncAllLexicons(const QString& key);
	virtual void syncAllVerseBasedModules(const QString& key);
	virtual void openWindow(const QString& moduleName, const QString& key);
	virtual void openDefaultBible(const QString& key);
	virtual QString getCurrentReference();
	virtual QStringList searchInModule(const QString& module, const QString& searchText);
	virtual QStringList searchInOpenModules(const QString& searchText);
	virtual QStringList searchInDefaultBible(const QString& searchText);
	virtual QStringList getModulesOfType(const QString& type);
    virtual void reloadModules();
};

#endif
