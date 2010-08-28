/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QInputDialog>
#include <QList>
#include <QMdiSubWindow>
#include <QMenu>
#include <QProcess>
#include <QtGlobal>
#include <QToolBar>
#include <QUrl>
#include "backend/config/cbtconfig.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/btaboutdialog.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/profile/cprofilemgr.h"
#include "frontend/profile/cprofile.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "util/directory.h"


using namespace Profile;


/** Opens the optionsdialog of BibleTime. */
void BibleTime::slotSettingsOptions() {
    qDebug() << "BibleTime::slotSettingsOptions";
    CConfigurationDialog *dlg = new CConfigurationDialog(this, m_actionCollection);
    QObject::connect(dlg, SIGNAL(signalSettingsChanged()), this, SLOT(slotSettingsChanged()) );

    dlg->show();
}

/** Save the settings, used when no settings have been saved before **/
void BibleTime::saveConfigSettings() {
    CConfigurationDialog* dlg = new CConfigurationDialog(this, 0);
    dlg->save();
    delete dlg;
}

/** Is called when settings in the optionsdialog were changed (ok or apply) */
void BibleTime::slotSettingsChanged() {
    qDebug() << "BibleTime::slotSettingsChanged";
    const QString language = CBTConfig::get(CBTConfig::language);
    CSwordBackend::instance()->booknameLanguage(language);

// \todo update the bookmarks after Bible bookname language has been changed
//     QTreeWidgetItemIterator it(m_mainIndex);
//     while (*it) {
//         CIndexItemBase* citem = dynamic_cast<CIndexItemBase*>(*it);
//         if (citem) {
//             citem->update();
//         }
//         ++it;
//     }

    refreshBibleTimeAccel();
    refreshDisplayWindows();
    refreshProfileMenus();
    qDebug() << "BibleTime::slotSettingsChanged";
}

/** Opens the sword setup dialog of BibleTime. */
void BibleTime::slotSwordSetupDialog() {
    BtModuleManagerDialog *dlg = BtModuleManagerDialog::getInstance(this);

    dlg->showNormal();
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
}

/** Is called just before the window menu is ahown. */
void BibleTime::slotWindowMenuAboutToShow() {
    Q_ASSERT(m_windowMenu);

    if ( m_mdi->subWindowList().isEmpty() ) {
        m_windowCascadeAction->setEnabled(false);
        m_windowTileVerticalAction->setEnabled(false);
        m_windowTileHorizontalAction->setEnabled(false);
        m_windowCloseAction->setEnabled(false);
        m_windowCloseAllAction->setEnabled(false);
        m_openWindowsMenu->setEnabled(false);
    }
    else if (m_mdi->subWindowList().count() == 1) {
        m_windowTileVerticalAction->setEnabled(false);
        m_windowTileHorizontalAction->setEnabled(false);
        m_windowCascadeAction->setEnabled(false);
        m_windowCloseAction->setEnabled(true);
        m_windowCloseAllAction->setEnabled(true);
        m_openWindowsMenu->setEnabled(true);
        //   m_windowMenu->insertSeparator();
    }
    else {
        slotUpdateWindowArrangementActions(0); //update the window tile/cascade states
        m_windowCloseAction->setEnabled(true);
        m_windowCloseAllAction->setEnabled(true);
        m_openWindowsMenu->setEnabled(true);
    }
}

/** Is called just before the open windows menu is ahown. */
void BibleTime::slotOpenWindowsMenuAboutToShow() {
    Q_ASSERT(m_openWindowsMenu);

    QList<QMdiSubWindow*> windows = m_mdi->usableWindowList();
    m_openWindowsMenu->clear();
    foreach (QMdiSubWindow *window, windows) {
        QAction *openWindowAction = m_openWindowsMenu->addAction(window->windowTitle());
        openWindowAction->setCheckable(true);
        openWindowAction->setChecked(window == m_mdi->activeSubWindow());
        connect(openWindowAction, SIGNAL(triggered()), m_windowMapper, SLOT(map()));
        m_windowMapper->setMapping(openWindowAction, window);
    }
}

/** This slot is connected with the windowAutoTileAction object */
void BibleTime::slotUpdateWindowArrangementActions( QAction* clickedAction ) {
    /* If a toggle action was clicked we see if it is checked or unchecked and
    * enable/disable the simple cascade and tile options accordingly
    */
    m_windowTileVerticalAction->setEnabled( m_windowManualModeAction->isChecked() );
    m_windowTileHorizontalAction->setEnabled( m_windowManualModeAction->isChecked() );
    m_windowCascadeAction->setEnabled( m_windowManualModeAction->isChecked() );
    m_windowTileAction->setEnabled( m_windowManualModeAction->isChecked() );

    if (clickedAction) {
        m_windowManualModeAction->setEnabled(
            m_windowManualModeAction != clickedAction
            && m_windowTileHorizontalAction != clickedAction
            && m_windowTileVerticalAction != clickedAction
            && m_windowCascadeAction != clickedAction
            && m_windowTileAction != clickedAction
        );
        m_windowAutoTileVerticalAction->setEnabled( m_windowAutoTileVerticalAction != clickedAction );
        m_windowAutoTileHorizontalAction->setEnabled( m_windowAutoTileHorizontalAction != clickedAction );
        m_windowAutoCascadeAction->setEnabled( m_windowAutoCascadeAction != clickedAction );
        m_windowAutoTileAction->setEnabled( m_windowAutoTileAction != clickedAction );
        m_windowAutoTabbedAction->setEnabled( m_windowAutoTabbedAction != clickedAction );
    }

    if (clickedAction == m_windowManualModeAction) {
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(true);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
    }
    else if (clickedAction == m_windowAutoTileVerticalAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTileVertical );
    }
    else if (clickedAction == m_windowAutoTileHorizontalAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTileHorizontal );
    }
    else if (clickedAction == m_windowAutoTileAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTile );
    }
    else if (clickedAction == m_windowAutoTabbedAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTabbed );
    }
    else if (clickedAction == m_windowAutoCascadeAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeCascade );
    }
    else if (clickedAction == m_windowTileAction) {
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
        m_mdi->myTile();
    }
    else if (clickedAction == m_windowCascadeAction) {
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
        m_mdi->myCascade();
    }
    else if (clickedAction == m_windowTileVerticalAction) {
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
        m_mdi->myTileVertical();
    }
    else if (clickedAction == m_windowTileHorizontalAction) {
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeManual );
        m_mdi->myTileHorizontal();
    }
}

void BibleTime::slotManualArrangementMode() {
    slotUpdateWindowArrangementActions( m_windowManualModeAction );
}

/** This slot is connected with the windowAutoTileAction object */
void BibleTime::slotAutoTileHorizontal() {
    slotUpdateWindowArrangementActions( m_windowAutoTileHorizontalAction );
}

/** This slot is connected with the windowAutoTileAction object */
void BibleTime::slotAutoTileVertical() {
    slotUpdateWindowArrangementActions( m_windowAutoTileVerticalAction );
}

/** This slot is connected with the windowAutoTileAction object */
void BibleTime::slotAutoTile() {
    slotUpdateWindowArrangementActions( m_windowAutoTileAction );
}

/** This slot is connected with the windowAutoTabbedAction object */
void BibleTime::slotAutoTabbed() {
    slotUpdateWindowArrangementActions( m_windowAutoTabbedAction );
}

void BibleTime::slotTile() {
    slotUpdateWindowArrangementActions( m_windowTileAction );
}

void BibleTime::slotCascade() {
    slotUpdateWindowArrangementActions( m_windowCascadeAction );
}

void BibleTime::slotTileVertical() {
    slotUpdateWindowArrangementActions( m_windowTileVerticalAction );
}

void BibleTime::slotTileHorizontal() {
    slotUpdateWindowArrangementActions( m_windowTileHorizontalAction );
}

/** This slot is connected with the windowAutoCascadeAction object */
void BibleTime::slotAutoCascade() {
    slotUpdateWindowArrangementActions( m_windowAutoCascadeAction );
}

/** Shows/hides the toolbar */
void BibleTime::slotToggleMainToolbar() {
    Q_ASSERT(m_mainToolBar);
    if (m_viewToolbarAction->isChecked()) {
        m_mainToolBar->show();
    }
    else {
        m_mainToolBar->hide();
    }
}

void BibleTime::slotToggleTextWindowHeader() {
    bool currentState = CBTConfig::get(CBTConfig::showTextWindowHeaders);
    CBTConfig::set(CBTConfig::showTextWindowHeaders, !currentState);
    emit toggledTextWindowHeader(!currentState);
}

void BibleTime::slotToggleNavigatorToolbar() {
    bool currentState = CBTConfig::get(CBTConfig::showTextWindowNavigator);
    CBTConfig::set(CBTConfig::showTextWindowNavigator, !currentState);
    showOrHideToolBars();
    if (CBTConfig::get(CBTConfig::showToolbarsInEachWindow))
        emit toggledTextWindowNavigator(!currentState);
    else
        emit toggledTextWindowNavigator(false);
}

void BibleTime::slotToggleToolsToolbar() {
    bool currentState = CBTConfig::get(CBTConfig::showTextWindowToolButtons);
    CBTConfig::set(CBTConfig::showTextWindowToolButtons, !currentState);
    showOrHideToolBars();
    if (CBTConfig::get(CBTConfig::showToolbarsInEachWindow))
        emit toggledTextWindowToolButtons(!currentState);
    else
        emit toggledTextWindowToolButtons(false);
}

void BibleTime::slotToggleWorksToolbar() {
    bool currentState = CBTConfig::get(CBTConfig::showTextWindowModuleSelectorButtons);
    CBTConfig::set(CBTConfig::showTextWindowModuleSelectorButtons, !currentState);
    showOrHideToolBars();
    if (CBTConfig::get(CBTConfig::showToolbarsInEachWindow))
        emit toggledTextWindowModuleChooser(!currentState);
    else
        emit toggledTextWindowModuleChooser(false);
}

void BibleTime::slotToggleFormatToolbar() {
    bool currentState = CBTConfig::get(CBTConfig::showFormatToolbarButtons);
    CBTConfig::set(CBTConfig::showFormatToolbarButtons, !currentState);
    showOrHideToolBars();
    if (CBTConfig::get(CBTConfig::showToolbarsInEachWindow))
        emit toggledTextWindowFormatToolbar(!currentState);
    else
        emit toggledTextWindowFormatToolbar(false);
}

void BibleTime::slotToggleToolBarsInEachWindow() {
    bool currentState = CBTConfig::get(CBTConfig::showToolbarsInEachWindow);
    CBTConfig::set(CBTConfig::showToolbarsInEachWindow, !currentState);
    showOrHideToolBars();

    if (!currentState) {
        emit toggledTextWindowNavigator(CBTConfig::get(CBTConfig::showTextWindowNavigator));
        emit toggledTextWindowToolButtons(CBTConfig::get(CBTConfig::showTextWindowToolButtons));
        emit toggledTextWindowModuleChooser(CBTConfig::get(CBTConfig::showTextWindowModuleSelectorButtons));
        emit toggledTextWindowFormatToolbar(CBTConfig::get(CBTConfig::showFormatToolbarButtons));
    }
    else {
        emit toggledTextWindowNavigator(false);
        emit toggledTextWindowToolButtons(false);
        emit toggledTextWindowModuleChooser(false);
        emit toggledTextWindowFormatToolbar(false);
    }


}

void BibleTime::showOrHideToolBars() {
    if (CBTConfig::get(CBTConfig::showToolbarsInEachWindow)) {
        m_navToolBar->setVisible(false);
        m_worksToolBar->setVisible(false);
        m_toolsToolBar->setVisible(false);
        m_formatToolBar->setVisible(false);
    }
    else {
        m_navToolBar->setVisible(CBTConfig::get(CBTConfig::showTextWindowNavigator));
        m_worksToolBar->setVisible(CBTConfig::get(CBTConfig::showTextWindowModuleSelectorButtons));
        m_toolsToolBar->setVisible(CBTConfig::get(CBTConfig::showTextWindowToolButtons));
        m_formatToolBar->setVisible(CBTConfig::get(CBTConfig::showFormatToolbarButtons));
    }
}

/** Sets the active window. */
void BibleTime::slotSetActiveSubWindow(QWidget* window) {
    if (!window)
        return;
    m_mdi->setActiveSubWindow(dynamic_cast<QMdiSubWindow*>(window));
}

void BibleTime::slotSearchModules() {
    //get the modules of the open windows
    QList<const CSwordModuleInfo*> modules;

    foreach(QMdiSubWindow* subWindow, m_mdi->subWindowList()) {
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
    QList<const CSwordModuleInfo*> module;
    CSwordModuleInfo* bible = CBTConfig::get(CBTConfig::standardBible);
    if (bible) {
        module.append(bible);
    }
    Search::CSearchDialog::openDialog(module, QString::null);
}

void BibleTime::openOnlineHelp_Handbook() {
    QString filePath(util::directory::getHandbookDir().canonicalPath() + "/index.html");
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void BibleTime::openOnlineHelp_Howto() {
    QString filePath(util::directory::getHowtoDir().canonicalPath() + "/index.html");
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void BibleTime::slotOpenAboutDialog() {
    BtAboutDialog* dlg = new BtAboutDialog(this);
    dlg->show();
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
    foreach (QMdiSubWindow* w, m_mdi->subWindowList(QMdiArea::StackingOrder)) {
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
        m_mdi->closeAllSubWindows();
        loadProfile(p);
    }
}

void BibleTime::loadProfile(CProfile* p) {
    if (!p)
        return;

    QList<CProfileWindow*> windows = p->load();

    m_mdi->setUpdatesEnabled(false);//don't auto tile or auto cascade, this would mess up everything!!

    //load mainwindow setttings
    applyProfileSettings(p);

    QWidget* focusWindow = 0;

    //   for (CProfileWindow* w = windows.last(); w; w = windows.prev()) { //from the last one to make sure the order is right in the mdi area
    foreach (CProfileWindow* w, windows) {
        const QString key = w->key();
        QStringList usedModules = w->modules();

        QList<CSwordModuleInfo*> modules;
        for ( QStringList::Iterator it = usedModules.begin(); it != usedModules.end(); ++it ) {
            if (CSwordModuleInfo* m = CSwordBackend::instance()->findModuleByName(*it)) {
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
    if (m_windowFullscreenAction->isChecked()) {
        // set full screen mode
        m_WindowWasMaximizedBeforeFullScreen = isMaximized();
        showFullScreen();
    }
    else {
        // restore previous non-full screen mode
        if (m_WindowWasMaximizedBeforeFullScreen) {
            showMaximized();
        }
        else {
            showNormal();
        }
    }
    m_mdi->triggerWindowUpdate();
}

/** Saves current settings into a new profile. */
void BibleTime::saveToNewProfile() {
    bool ok = false;
    const QString name = QInputDialog::getText(this, tr("New Session"),
                         tr("Please enter a name for the new session."), QLineEdit::Normal, QString::null, &ok);
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

// Quit from BibleTime
void BibleTime::quit() {
    Search::CSearchDialog::closeDialog();
    close();
}
