/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//own includes
#include "bibletime.h"
#include "config.h"

#include "backend/keys/cswordversekey.h"

#include "util/ctoolclass.h"
#include "util/directoryutil.h"

#include "frontend/cmdiarea.h"
#include "frontend/profile/cprofilemgr.h"
#include "frontend/profile/cprofile.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "frontend/cbtconfig.h"
#include "frontend/cinputdialog.h"
#include "frontend/cinfodisplay.h"
#include "frontend/mainindex/cmainindex.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cbiblereadwindow.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"

//QT includes
#include <QClipboard>
#include <QInputDialog>
#include <QList>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QApplication>
#include <QProcess>
#include <QMdiSubWindow>

//KDE includes
#include <kbugreport.h>
#include <kaboutapplicationdialog.h>
#include <kaboutdata.h>

using namespace Profile;

// /* An action which stores a user defined pointer to a widget.
//  * @author Joachim Ansorg
//  */
// class KUserDataAction : public KToggleAction {
// public:
// 	KUserDataAction( QString caption, const KShortcut& shortcut, const QObject* receiver, const char* slot, KActionCollection* actionCollection)
// 		: KToggleAction(caption, actionCollection), m_userData(0) 
// 	{
// 		setShortcut(shortcut);
// 		QObject::connect(this, SIGNAL(triggered()), receiver, slot);
// 	};
// 
// 	void setUserData(QWidget* const data) {
// 		m_userData = data;
// 	};
// 	QWidget* const getUserData() const {
// 		return m_userData;
// 	};
// 
// private:
// 	QWidget* m_userData;
// };

/** Opens the optionsdialog of BibleTime. */
void BibleTime::slotSettingsOptions() {
	qDebug("BibleTime::slotSettingsOptions");
	CConfigurationDialog *dlg = new CConfigurationDialog(this, 0 /*actionCollection()*/);
	QObject::connect(dlg, SIGNAL(signalSettingsChanged()), this, SLOT(slotSettingsChanged()) );

	dlg->exec();
	dlg->delayedDestruct();
	// TODO: comment out exec and destruct, use dlg->show();
}

/** Is called when settings in the optionsdialog were changed (ok or apply) */
void BibleTime::slotSettingsChanged() {
	qDebug("BibleTime::slotSettingsChanged");
	const QString language = CBTConfig::get
								 (CBTConfig::language);
	//m_backend->booknameLanguage(language);
	CPointers::backend()->booknameLanguage(language);
// TODO: update the bookmarks after Bible bookname language has been changed
// 	QTreeWidgetItemIterator it(m_mainIndex);
// 	while (*it) {
// 		CIndexItemBase* citem = dynamic_cast<CIndexItemBase*>(*it);
// 		if (citem) {
// 			citem->update();
// 		}
// 		++it;
// 	}

	refreshDisplayWindows();
	refreshProfileMenus();
	qDebug("BibleTime::slotSettingsChanged");
}

/** Opens the sword setup dialog of BibleTime. */
void BibleTime::slotSwordSetupDialog() {
	//TODO: nonmodal dialog, memory management (one instance only!
	//BtModuleManagerDialog *dlg = new BtModuleManagerDialog(this);
	BtModuleManagerDialog* dlg = BtModuleManagerDialog::getInstance(this);
	//disconnect first because it may be connected already
	//QObject::disconnect(dlg, SIGNAL(swordSetupChanged()), this, SLOT(slotSwordSetupChanged()) );
	//connect(dlg, SIGNAL(swordSetupChanged()), SLOT(slotSwordSetupChanged()) );

	dlg->showNormal();
	dlg->show();
	dlg->raise();
	dlg->activateWindow();
}

/** Is called when settings in the sword setup dialog were changed (ok or apply) */
void BibleTime::slotSwordSetupChanged() {
	/*
	  Refresh everything here what might have changed
	  these are the mainindex, the searchdialog, the displaywindows
	  But at first we have to reset the Sword backend to reload the modules

	TODO: should bookshelf manager be updated?
	Should there be different signals/slots for visual changes,
	i.e. grouping/hiding?

	*/


	//CPointers::deleteBackend();
	//m_backend = new CSwordBackend();
	//CPointers::setBackend(new CSwordBackend());
	//CPointers::backend()->reloadModules();

	//m_mainIndex->reloadSword();

	//  refresh display windows
	//refreshDisplayWindows();
}

/** Is called just before the window menu is ahown. */
void BibleTime::slotWindowMenuAboutToShow() {
	Q_ASSERT(m_windowMenu);
	if (!m_windowMenu) {
		return;
	}

	if ( m_mdi->subWindowList().isEmpty() ) {
		m_windowCascade_action->setEnabled(false);
		m_windowTileVertical_action->setEnabled(false);
		m_windowTileHorizontal_action->setEnabled(false);
		m_windowCloseAll_action->setEnabled(false);
	}
	else if (m_mdi->subWindowList().count() == 1) {
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
	
	QList<QAction*>::iterator end = m_windowOpenWindowsList.end();
	for (QList<QAction*>::iterator it = m_windowOpenWindowsList.begin(); it != end; ++it ) {
		//(*it)->unplugAll(); //see kde porting doc
		foreach (QWidget *w, (*it)->associatedWidgets() ) {
    		w->removeAction(*it);
		}
	}

	//m_windowOpenWindowsList.setAutoDelete(true);
	qDeleteAll(m_windowOpenWindowsList);
	m_windowOpenWindowsList.clear();

// 	if (!m_windowActionCollection) {
// 		m_windowActionCollection = new KActionCollection(this, KComponentData());
// 	}

// 	QList<QWidget*> windows = m_mdi->windowList();
// 	const int count = windows.count();
// 	for ( int i = 0; i < count; ++i ) {
// 		QWidget* w = windows.at(i);
// 		Q_ASSERT(w);
// 
// 		KUserDataAction* action = new KUserDataAction(w->windowTitle(), KShortcut(), this, SLOT(slotWindowMenuActivated()), m_windowActionCollection);
// 		Q_ASSERT(action);
// 		action->setUserData(w);
// 
// 		m_windowOpenWindowsList.append(action);
// 		action->setChecked( w == m_mdi->activeWindow() );
// 		m_windowMenu->addAction(action);
// 	}
}

/** This slot is connected with the windowAutoTile_action object */
void BibleTime::slotUpdateWindowArrangementActions( QAction* clickedAction ) {
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

		m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
	}
	else if (clickedAction == m_windowAutoTileVertical_action) {
		m_windowManualMode_action->setChecked(false);
		m_windowAutoTileHorizontal_action->setChecked(false);
		m_windowAutoCascade_action->setChecked(false);

		m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTileVertical );
	}
	else if (clickedAction == m_windowAutoTileHorizontal_action) {
		m_windowManualMode_action->setChecked(false);
		m_windowAutoTileVertical_action->setChecked(false);
		m_windowAutoCascade_action->setChecked(false);

		m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTileHorizontal );
	}
	else if (clickedAction == m_windowAutoCascade_action) {
		m_windowManualMode_action->setChecked(false);
		m_windowAutoTileHorizontal_action->setChecked(false);
		m_windowAutoTileVertical_action->setChecked(false);

		m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeCascade );
	}
	else if (clickedAction == m_windowCascade_action) {
		m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
		m_mdi->myCascade();
	}
	else if (clickedAction == m_windowTileVertical_action) {
		m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
		m_mdi->myTileVertical();
	}
	else if (clickedAction == m_windowTileHorizontal_action) {
		m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
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

/*	const KUserDataAction* action = dynamic_cast<const KUserDataAction*>(sender());
	Q_ASSERT(action);
	if (action) {
		QWidget* const window = action->getUserData();
		Q_ASSERT(window);
		if ( window ) {
			window->setFocus();
		}
	}*/
}

/** Shows/hides the toolbar */
void BibleTime::slotToggleToolbar() {
	Q_ASSERT(m_mainToolBar);
	if (m_viewToolbar_action->isChecked()) {
		m_mainToolBar->show();
	}
	else {
		m_mainToolBar->hide();
	}
}

/** Shows or hides the groupmanager. 
*/
void BibleTime::slotToggleMainIndex() {
	//we use the parent widget because the main index is enclosed in a layout which adds the label at the top
	if (m_viewMainIndex_action->isChecked()) {
		m_mainIndex->show();
	}
	else {
		m_mainIndex->hide();
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

void BibleTime::slotSearchModules() {
	//get the modules of the open windows
	ListCSwordModuleInfo modules;
 
	foreach(QMdiSubWindow* subWindow, m_mdi->subWindowList()){
		if (CDisplayWindow* w = dynamic_cast<CDisplayWindow*>(subWindow->widget())) {
			modules << w->modules();
		}
	}
	Search::CSearchDialog::openDialog(modules, QString::null);
}

/* Search default Bible slot
 * Call CSearchDialog::openDialog with only the default bible module
 */
void BibleTime::slotSearchDefaultBible() {
 	ListCSwordModuleInfo module;
 	CSwordModuleInfo* bible = CBTConfig::get(CBTConfig::standardBible);
 	if (bible) {
 		module.append(bible);
 	}
 	Search::CSearchDialog::openDialog(module, QString::null);
}

void BibleTime::openOnlineHelp_Handbook() {
	QStringList args;
	args << util::filesystem::DirectoryUtil::getHandbookDir().canonicalPath() +"/index.html";
	if (QProcess::startDetached("konqueror", args)) return;
	if (QProcess::startDetached("firefox", args)) return;
}

void BibleTime::openOnlineHelp_Howto() {
	QStringList args;
	args << util::filesystem::DirectoryUtil::getHowtoDir().canonicalPath() +"/index.html";
	if (QProcess::startDetached("konqueror", args)) return;
	if (QProcess::startDetached("firefox", args)) return;
}

/** Saves the current settings into the currently activated profile. */
void BibleTime::saveProfile(QAction* action) {
	m_mdi->setUpdatesEnabled(false);

	const QString profileName = action->text().remove("&");
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

	QList<CProfileWindow*> profileWindows;
	foreach (QMdiSubWindow* w, m_mdi->subWindowList()) {
		CDisplayWindow* displayWindow = dynamic_cast<CDisplayWindow*>(w->widget());
		if (!displayWindow) {
			continue;
		}

		CProfileWindow* profileWindow = new CProfileWindow();
		displayWindow->storeProfileSettings(profileWindow);
		profileWindows.append(profileWindow);
	}
	profile->save(profileWindows);

	//clean up memory - delete all created profile windows
	//profileWindows.setAutoDelete(true);
	qDeleteAll(profileWindows);
	profileWindows.clear();
}

void BibleTime::loadProfile(QAction* action) {
	const QString profileName = action->text().remove("&");
	CProfile* p = m_profileMgr.profile( profileName );
	Q_ASSERT(p);
	if ( p ) {
		m_mdi->deleteAll();
		loadProfile(p);
	}
}

void BibleTime::loadProfile(CProfile* p) {
	if (!p) return;

	QList<CProfileWindow*> windows = p->load();

	m_mdi->setUpdatesEnabled(false);//don't auto tile or auto cascade, this would mess up everything!!

	//load mainwindow setttings
	applyProfileSettings(p);

	QWidget* focusWindow = 0;

	//   for (CProfileWindow* w = windows.last(); w; w = windows.prev()) { //from the last one to make sure the order is right in the mdi area
	foreach (CProfileWindow* w, windows) {
		const QString key = w->key();
		QStringList usedModules = w->modules();

		ListCSwordModuleInfo modules;
		for ( QStringList::Iterator it = usedModules.begin(); it != usedModules.end(); ++it ) {
			if (CSwordModuleInfo* m = CPointers::backend()->findModuleByName(*it)) {
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
		}
	}

	m_mdi->setUpdatesEnabled(true);
	m_mdi->triggerWindowUpdate();

	if (focusWindow) {
		focusWindow->setFocus();
	}
}

void BibleTime::deleteProfile(QAction* action) {
	//HACK: work around the inserted & char by KPopupMenu
	const QString profileName = action->text().remove("&");
	CProfile* p = m_profileMgr.profile( profileName );
	Q_ASSERT(p);
	if ( p ) m_profileMgr.remove(p);
	refreshProfileMenus();
}

void BibleTime::toggleFullscreen() {
	m_windowFullscreen_action->isChecked() ? showFullScreen() : showNormal();
	m_mdi->triggerWindowUpdate();
}

/** Saves current settings into a new profile. */
void BibleTime::saveToNewProfile() {
	bool ok = false;
	const QString name = QInputDialog::getText(this, tr("Session name:"), tr("Please enter a name for the new session."), QLineEdit::Normal, QString::null, &ok);
	if (ok && !name.isEmpty()) {
		CProfile* profile = m_profileMgr.create(name);
		saveProfile(profile);
	}
	refreshProfileMenus();
}

/** Slot to refresh the save profile and load profile menus. */
void BibleTime::refreshProfileMenus() {
	m_windowSaveProfileMenu->clear();
	m_windowLoadProfileMenu->clear();
	m_windowDeleteProfileMenu->clear();

	//refresh the load, save and delete profile menus
	m_profileMgr.refresh();
	QList<CProfile*> profiles = m_profileMgr.profiles();

	const bool enableActions = bool(profiles.count() != 0);
	m_windowSaveProfileMenu->setEnabled(enableActions);
	m_windowLoadProfileMenu->setEnabled(enableActions);
	m_windowDeleteProfileMenu->setEnabled(enableActions);

	foreach (CProfile* p, profiles) {
		m_windowSaveProfileMenu->addAction(p->name());
		m_windowLoadProfileMenu->addAction(p->name());
		m_windowDeleteProfileMenu->addAction(p->name());
	}
}
