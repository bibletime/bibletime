/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//own includes
#include "bibletime.h"
#include "config.h"

#include "backend/cswordversekey.h"

#include "util/ctoolclass.h"

#include "frontend/cmdiarea.h"
#include "frontend/cprofilemgr.h"
#include "frontend/cprofile.h"
#include "frontend/cprofilewindow.h"
#include "frontend/coptionsdialog.h"
#include "frontend/cswordsetupdialog.h"
#include "frontend/cbtconfig.h"
#include "frontend/cinputdialog.h"
#include "frontend/cinfodisplay.h"
#include "frontend/mainindex/cmainindex.h"
#include "frontend/mainindex/cindexitem.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cbiblereadwindow.h"
#include "frontend/searchdialog/csearchdialog.h"

#include <errno.h>

//QT includes
#include <qprogressdialog.h>
#include <qlistview.h>
#include <qvaluelist.h>
#include <qclipboard.h>
#include <qinputdialog.h>

//KDE includes
#include <kaction.h>
#include <kapplication.h>
#include <kaboutkde.h>
#include <kaboutdialog.h>
#include <kbugreport.h>
#include <kaboutapplication.h>
#include <kstandarddirs.h>
#include <kmenubar.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <klocale.h>
#include <kedittoolbar.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>
#include <ktip.h>

//Sword includes
#include <versekey.h>

using namespace Profile;

/* An action which stores a user defined pointer to a widget.
 * @author Joachim Ansorg
 */
class KUserDataAction : public KToggleAction {
public:
	KUserDataAction( QString caption, const KShortcut& shortcut, const QObject* receiver, const char* slot, KActionCollection* actionCollection)
: KToggleAction(caption, shortcut, receiver, slot, actionCollection), m_userData(0) { /* no impl */
	};

	void setUserData(QWidget* const data) {
		m_userData = data;
	};
	QWidget* const getUserData() const {
		return m_userData;
	};

private:
	QWidget* m_userData;
};

/** Opens the optionsdialog of BibleTime. */
void BibleTime::slotSettingsOptions() {
	COptionsDialog *dlg = new COptionsDialog(this, "COptionsDialog", actionCollection());
	connect(dlg, SIGNAL(signalSettingsChanged()), SLOT(slotSettingsChanged()) );

	dlg->exec();
	dlg->delayedDestruct();
}

/** Is called when settings in the optionsdialog were changed (ok or apply) */
void BibleTime::slotSettingsChanged() {
	const QString language = CBTConfig::get
								 (CBTConfig::language);
	m_backend->booknameLanguage(language);

	QListViewItemIterator it( m_mainIndex );
	CItemBase* item = 0;
	for ( ; it.current(); ++it ) {
		if ( (item = dynamic_cast<CItemBase*>(it.current())) ) {
			item->update();
		}
	}

	refreshDisplayWindows();
	refreshProfileMenus();
}

/** Opens the sword setup dialog of BibleTime. */
void BibleTime::slotSwordSetupDialog() {
	BookshelfManager::CSwordSetupDialog *dlg = new BookshelfManager::CSwordSetupDialog(this, "CSwordSetupDialog");
	connect(dlg, SIGNAL(signalSwordSetupChanged()), SLOT(slotSwordSetupChanged()) );

	dlg->exec();
	dlg->delayedDestruct();
}

/** Is called when settings in the sword setup dialog were changed (ok or apply) */
void BibleTime::slotSwordSetupChanged() {
	/*
	  Refresh everything here what might have changed
	  these are the mainindex, the searchdialog, the displaywindows
	  But at first we have to reset the Sword backend to reload the modules
	*/
	m_mainIndex->saveBookmarks();

	CPointers::deleteBackend();
	m_backend = new CSwordBackend();
	CPointers::setBackend(m_backend);
	m_backend->initModules();

	m_mainIndex->reloadSword();

	//  refresh display windows
	refreshDisplayWindows();
}

/** Shows the daily tip */
void BibleTime::slotHelpTipOfDay() {
	KTipDialog::setShowOnStart( CBTConfig::get
									(CBTConfig::tips) );
	KTipDialog::showTip(this, "bibletime/tips", true);
}

/** Is called just before the window menu is ahown. */
void BibleTime::slotWindowMenuAboutToShow() {
	Q_ASSERT(m_windowMenu);
	if (!m_windowMenu) {
		return;
	}

	if ( m_mdi->windowList().isEmpty() ) {
		m_windowCascade_action->setEnabled(false);
		m_windowTileVertical_action->setEnabled(false);
		m_windowTileHorizontal_action->setEnabled(false);
		m_windowCloseAll_action->setEnabled(false);
	}
	else if (m_mdi->windowList().count() == 1) {
		m_windowTileVertical_action->setEnabled( false );
		m_windowTileHorizontal_action->setEnabled( false );
		m_windowCascade_action->setEnabled( false );
		m_windowCloseAll_action->setEnabled( true );
		//   m_windowMenu->insertSeparator();
	}
	else {
		slotUpdateWindowArrangementActions(0); //update the window tile/cascade states
		m_windowCloseAll_action->setEnabled( true );
	}
	
#if QT_VERSION >= 0x030200
	QPtrList<KAction>::iterator end = m_windowOpenWindowsList.end();
	for (QPtrList<KAction>::iterator it = m_windowOpenWindowsList.begin(); it != end; ++it ) {
		(*it)->unplugAll();
	}
#else
	QPtrListIterator<KAction> it(m_windowOpenWindowsList);
	while (it.current() != 0){
		it.current()->unplugAll();
		++it;
	}
#endif
	m_windowOpenWindowsList.setAutoDelete(true);
	m_windowOpenWindowsList.clear();

	if (!m_windowActionCollection) {
		m_windowActionCollection = new KActionCollection(this);
	}

	QWidgetList windows = m_mdi->windowList();
	const int count = windows.count();
	for ( int i = 0; i < count; ++i ) {
		QWidget* w = windows.at(i);
		Q_ASSERT(w);

		KUserDataAction* action = new KUserDataAction(w->caption(), KShortcut(), this, SLOT(slotWindowMenuActivated()), m_windowActionCollection);
		Q_ASSERT(action);
		action->setUserData(w);

		m_windowOpenWindowsList.append(action);
		action->setChecked( w == m_mdi->activeWindow() );
		action->plug(m_windowMenu);
	}
}

/** This slot is connected with the windowAutoTile_action object */
void BibleTime::slotUpdateWindowArrangementActions( KAction* clickedAction ) {
	/* If a toggle action was clicked we see if it checked ot unchecked and
	* enable/disable the simple cascade and tile options accordingly
	*/
	m_windowTileVertical_action->setEnabled( m_windowManualMode_action->isChecked() );
	m_windowTileHorizontal_action->setEnabled( m_windowManualMode_action->isChecked() );
	m_windowCascade_action->setEnabled( m_windowManualMode_action->isChecked() );

	if (clickedAction) {
		m_windowManualMode_action->setEnabled(
			m_windowManualMode_action != clickedAction
			&& m_windowTileHorizontal_action != clickedAction
			&& m_windowTileVertical_action != clickedAction
			&& m_windowCascade_action != clickedAction
		);
		m_windowAutoTileVertical_action->setEnabled( m_windowAutoTileVertical_action != clickedAction );
		m_windowAutoTileHorizontal_action->setEnabled( m_windowAutoTileHorizontal_action != clickedAction );
		m_windowAutoCascade_action->setEnabled( m_windowAutoCascade_action != clickedAction );
	}

	if (clickedAction == m_windowManualMode_action) {
		m_windowAutoTileVertical_action->setChecked(false);
		m_windowAutoTileHorizontal_action->setChecked(false);
		m_windowAutoCascade_action->setChecked(false);

		m_mdi->setGUIOption( CMDIArea::Nothing );
	}
	else if (clickedAction == m_windowAutoTileVertical_action) {
		m_windowManualMode_action->setChecked(false);
		m_windowAutoTileHorizontal_action->setChecked(false);
		m_windowAutoCascade_action->setChecked(false);

		m_mdi->setGUIOption( CMDIArea::autoTileVertical );
	}
	else if (clickedAction == m_windowAutoTileHorizontal_action) {
		m_windowManualMode_action->setChecked(false);
		m_windowAutoTileVertical_action->setChecked(false);
		m_windowAutoCascade_action->setChecked(false);

		m_mdi->setGUIOption( CMDIArea::autoTileHorizontal );
	}
	else if (clickedAction == m_windowAutoCascade_action) {
		m_windowManualMode_action->setChecked(false);
		m_windowAutoTileHorizontal_action->setChecked(false);
		m_windowAutoTileVertical_action->setChecked(false);

		m_mdi->setGUIOption( CMDIArea::autoCascade );
	}
	else if (clickedAction == m_windowCascade_action) {
		m_mdi->setGUIOption( CMDIArea::Nothing );
		m_mdi->myCascade();
	}
	else if (clickedAction == m_windowTileVertical_action) {
		m_mdi->setGUIOption( CMDIArea::Nothing );
		m_mdi->myTileVertical();
	}
	else if (clickedAction == m_windowTileHorizontal_action) {
		m_mdi->setGUIOption( CMDIArea::Nothing );
		m_mdi->myTileHorizontal();
	}
}

void BibleTime::slotManualArrangementMode() {
	slotUpdateWindowArrangementActions( m_windowManualMode_action );
}

/** This slot is connected with the windowAutoTile_action object */
void BibleTime::slotAutoTileHorizontal() {
	slotUpdateWindowArrangementActions( m_windowAutoTileHorizontal_action );
}

/** This slot is connected with the windowAutoTile_action object */
void BibleTime::slotAutoTileVertical() {
	slotUpdateWindowArrangementActions( m_windowAutoTileVertical_action );
}

void BibleTime::slotCascade() {
	slotUpdateWindowArrangementActions( m_windowCascade_action );
}

void BibleTime::slotTileVertical() {
	slotUpdateWindowArrangementActions( m_windowTileVertical_action );
}

void BibleTime::slotTileHorizontal() {
	slotUpdateWindowArrangementActions( m_windowTileHorizontal_action );
}

/** This slot is connected with the windowAutoCascade_action object */
void BibleTime::slotAutoCascade() {
	slotUpdateWindowArrangementActions( m_windowAutoCascade_action );
}

void BibleTime::slotWindowMenuActivated() {
	if (!m_windowMenu) {
		return;
	}

	const KUserDataAction* action = dynamic_cast<const KUserDataAction*>(sender());
	Q_ASSERT(action);
	if (action) {
		QWidget* const window = action->getUserData();
		Q_ASSERT(window);
		if ( window ) {
			window->setFocus();
		}
	}
}


/** Shows/hides the toolbar */
void BibleTime::slotToggleToolbar() {
	Q_ASSERT(toolBar("mainToolBar"));

	if (m_viewToolbar_action->isChecked()) {
		toolBar("mainToolBar")->show();
	}
	else {
		toolBar("mainToolBar")->hide();
	}
}

/** Shows or hides the groupmanager. 
*/
void BibleTime::slotToggleMainIndex() {
	//we use the parent widget because the main index is enclosed in a layout which adds the label at the top
	if (m_viewMainIndex_action->isChecked()) {
		m_mainIndex->parentWidget()->show();
	}
	else {
		m_mainIndex->parentWidget()->hide();
	}
}

/** Shows or hides the groupmanager. */
void BibleTime::slotToggleInfoDisplay() {
	if (m_viewInfoDisplay_action->isChecked()) {
		m_infoDisplay->show();
	}
	else {
		m_infoDisplay->hide();
	}
}

/** Opens a toolbar editor */
void BibleTime::slotSettingsToolbar() {
	KEditToolbar dlg(actionCollection());
	if (dlg.exec()) {
		createGUI();
	}
}

void BibleTime::slotSearchModules() {
	//get the modules of the open windows
	ListCSwordModuleInfo modules;

	QWidgetList windows = m_mdi->windowList();
	for ( int i = 0; i < static_cast<int>(windows.count()); ++i ) {
		if (CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(windows.at(i))) {
			ListCSwordModuleInfo windowModules = w->modules();

			ListCSwordModuleInfo::iterator end_it = windowModules.end();
			for (ListCSwordModuleInfo::iterator it(windowModules.begin()); it != end_it; ++it) {
				modules.append(*it);
			};
		};
	};

	Search::CSearchDialog::openDialog(modules, QString::null);
}

/* Search default Bible slot
 * Call CSearchDialog::openDialog with only the default bible module
 */
void BibleTime::slotSearchDefaultBible() {
	ListCSwordModuleInfo module;
	CSwordModuleInfo* bible = CBTConfig::get
								  (CBTConfig::standardBible);
	if (bible) {
		module.append(bible);
	}

	Search::CSearchDialog::openDialog(module, QString::null);
}

void BibleTime::openOnlineHelp_Handbook() {
	kapp->invokeHelp("", "bibletime/handbook/");
}

void BibleTime::openOnlineHelp_Howto() {
	kapp->invokeHelp("", "bibletime/howto/");
}

/** Saves the current settings into the currently activatred profile. */
void BibleTime::saveProfile(int ID) {
	m_mdi->setUpdatesEnabled(false);

	KPopupMenu* popup = m_windowSaveProfile_action->popupMenu();
	const QString profileName = popup->text(ID).remove("&");
	CProfile* p = m_profileMgr.profile( profileName );
	Q_ASSERT(p);
	if ( p ) {
		saveProfile(p);
	}

	m_mdi->setUpdatesEnabled(true);
}

void BibleTime::saveProfile(CProfile* profile) {
	if (!profile) {
		return;
	}

	//save mainwindow settings
	storeProfileSettings(profile);

	QWidgetList windows = m_mdi->windowList();
	QPtrList<CProfileWindow> profileWindows;
	for (QWidget* w = windows.first(); w; w = windows.next()) {
		CDisplayWindow* displayWindow = dynamic_cast<CDisplayWindow*>(w);
		if (!displayWindow) {
			continue;
		}

		CProfileWindow* profileWindow = new CProfileWindow();
		displayWindow->storeProfileSettings(profileWindow);
		profileWindows.append(profileWindow);
	}
	profile->save(profileWindows);

	//clean up memory - delete all created profile windows
	profileWindows.setAutoDelete(true);
	profileWindows.clear();
}

void BibleTime::loadProfile(int ID) {
	KPopupMenu* popup = m_windowLoadProfile_action->popupMenu();
	//HACK: workaround the inserted & char by KPopupMenu
	const QString profileName = popup->text(ID).remove("&");
	CProfile* p = m_profileMgr.profile( profileName );
	//  qWarning("requesting popup: %s", popup->text(ID).latin1());
	Q_ASSERT(p);

	if ( p ) {
		m_mdi->deleteAll();
		loadProfile(p);
	}
}

void BibleTime::loadProfile(CProfile* p) {
	Q_ASSERT(p);
	if (!p) {
		return;
	}

	QPtrList<CProfileWindow> windows = p->load();
	Q_ASSERT(windows.count());

	//load mainwindow setttings
	applyProfileSettings(p);

	m_mdi->setUpdatesEnabled(false);//don't auto tile or auto cascade, this would mess up everything!!

	QWidget* focusWindow = 0;

	//   for (CProfileWindow* w = windows.last(); w; w = windows.prev()) { //from the last one to make sure the order is right in the mdi area
	for (CProfileWindow* w = windows.first(); w; w = windows.next()) {
		const QString key = w->key();
		QStringList usedModules = w->modules();

		ListCSwordModuleInfo modules;
		for ( QStringList::Iterator it = usedModules.begin(); it != usedModules.end(); ++it ) {
			if (CSwordModuleInfo* m = m_backend->findModuleByName(*it)) {
				modules.append(m);
			}
		}
		if (!modules.count()) { //are the modules still installed? If not continue wih next session window
			continue;
		}

		//is w->isWriteWindow is false we create a write window, otherwise a read window
		CDisplayWindow* displayWindow = 0;
		if (w->writeWindowType() > 0) { //create a write window
			displayWindow = createWriteDisplayWindow(modules.first(), key, CDisplayWindow::WriteWindowType(w->writeWindowType()) );
		}
		else { //create a read window
			displayWindow = createReadDisplayWindow(modules, key);
		}

		if (displayWindow) { //if a window was created initialize it.
			if (w->hasFocus()) {
				focusWindow = displayWindow;
			}

			displayWindow->applyProfileSettings(w);
		};
	}

	m_mdi->setUpdatesEnabled(true);

	if (focusWindow) {
		focusWindow->setFocus();
	}
}

void BibleTime::deleteProfile(int ID) {
	KPopupMenu* popup = m_windowDeleteProfile_action->popupMenu();
	//HACK: work around the inserted & char by KPopupMenu
	const QString profileName = popup->text(ID).remove("&");
	CProfile* p = m_profileMgr.profile( profileName );
	Q_ASSERT(p);

	if ( p ) {
		m_profileMgr.remove(p);
	}
	refreshProfileMenus();
}

void BibleTime::toggleFullscreen() {
	m_windowFullscreen_action->isChecked() ? showFullScreen() : showNormal();
	m_mdi->triggerWindowUpdate();
}

/** Saves current settings into a new profile. */
void BibleTime::saveToNewProfile() {
	bool ok = false;
	const QString name = QInputDialog::getText(i18n("Session name:"), i18n("Please enter a name for the new session."), QLineEdit::Normal, QString::null, &ok, this);
	if (ok && !name.isEmpty()) {
		CProfile* profile = m_profileMgr.create(name);
		saveProfile(profile);
	};

	refreshProfileMenus();
}

/** Slot to refresh the save profile and load profile menus. */
void BibleTime::refreshProfileMenus() {
	KPopupMenu* savePopup = m_windowSaveProfile_action->popupMenu();
	savePopup->clear();

	KPopupMenu* loadPopup = m_windowLoadProfile_action->popupMenu();
	loadPopup->clear();

	KPopupMenu* deletePopup = m_windowDeleteProfile_action->popupMenu();
	deletePopup->clear();

	//refresh the load, save and delete profile menus
	m_profileMgr.refresh();
	QPtrList<CProfile> profiles = m_profileMgr.profiles();

	const bool enableActions = bool(profiles.count() != 0);
	m_windowSaveProfile_action->setEnabled(enableActions);
	m_windowLoadProfile_action->setEnabled(enableActions);
	m_windowDeleteProfile_action->setEnabled(enableActions);

	for (CProfile* p = profiles.first(); p; p = profiles.next()) {
		savePopup->insertItem(p->name());
		loadPopup->insertItem(p->name());
		deletePopup->insertItem(p->name());
	}
}
