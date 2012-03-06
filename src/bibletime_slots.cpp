/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QInputDialog>
#include <QList>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QtGlobal>
#include <QToolBar>
#include <QUrl>
#include "backend/config/btconfig.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/btaboutdialog.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/bookshelfmanager/btmodulemanagerdialog.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "frontend/tips/bttipdialog.h"
#include "util/directory.h"


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
    const QString language = btConfig().value<QString>("language", QLocale::system().name());
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

/** Is called just before the window menu is shown. */
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

/** Is called just before the open windows menu is shown. */
void BibleTime::slotOpenWindowsMenuAboutToShow() {
    Q_ASSERT(m_openWindowsMenu);

    QList<QMdiSubWindow*> windows = m_mdi->usableWindowList();
    m_openWindowsMenu->clear();
    Q_FOREACH (QMdiSubWindow * const window, windows) {
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
        btConfig().setValue("GUI/alignmentMode", manual);
    }
    else if (clickedAction == m_windowAutoTileVerticalAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTileVertical );
        btConfig().setValue("GUI/alignmentMode", autoTileVertical);
    }
    else if (clickedAction == m_windowAutoTileHorizontalAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTileHorizontal );
        btConfig().setValue("GUI/alignmentMode", autoTileHorizontal);
    }
    else if (clickedAction == m_windowAutoTileAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTile );
        btConfig().setValue("GUI/alignmentMode", autoTile);
    }
    else if (clickedAction == m_windowAutoTabbedAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoCascadeAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeTabbed );
        btConfig().setValue("GUI/alignmentMode", autoTabbed);
    }
    else if (clickedAction == m_windowAutoCascadeAction) {
        m_windowManualModeAction->setChecked(false);
        m_windowAutoTileHorizontalAction->setChecked(false);
        m_windowAutoTileVerticalAction->setChecked(false);
        m_windowAutoTileAction->setChecked(false);
        m_windowAutoTabbedAction->setChecked(false);
        m_mdi->enableWindowMinMaxFlags(false);
        m_mdi->setMDIArrangementMode( CMDIArea::ArrangementModeCascade );
        btConfig().setValue("GUI/alignmentMode", autoCascade);
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
    bool currentState = btConfig().value<bool>("GUI/showMainToolbar", true);
    btConfig().setValue("GUI/showMainToolbar", !currentState);
    if ( m_showMainWindowToolbarAction->isChecked()) {
        m_mainToolBar->show();
    }
    else {
        m_mainToolBar->hide();
    }
}

void BibleTime::slotToggleTextWindowHeader() {
    bool currentState = btConfig().value<bool>("GUI/showTextWindowHeaders", true);
    btConfig().setValue("GUI/showTextWindowHeaders", !currentState);
    emit toggledTextWindowHeader(!currentState);
}

void BibleTime::slotToggleNavigatorToolbar() {
    bool currentState = btConfig().value<bool>("GUI/showTextWindowNavigator", true);
    btConfig().setValue("GUI/showTextWindowNavigator", !currentState);
    showOrHideToolBars();
    if (btConfig().value<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowNavigator(!currentState);
    else
        emit toggledTextWindowNavigator(false);
}

void BibleTime::slotToggleToolsToolbar() {
    bool currentState = btConfig().value<bool>("GUI/showTextWindowToolButtons", true);
    btConfig().setValue("GUI/showTextWindowToolButtons", !currentState);
    showOrHideToolBars();
    if (btConfig().value<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowToolButtons(!currentState);
    else
        emit toggledTextWindowToolButtons(false);
}

void BibleTime::slotToggleWorksToolbar() {
    bool currentState = btConfig().value<bool>("GUI/showTextWindowModuleSelectorButtons", true);
    btConfig().setValue("GUI/showTextWindowModuleSelectorButtons", !currentState);
    showOrHideToolBars();
    if (btConfig().value<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowModuleChooser(!currentState);
    else
        emit toggledTextWindowModuleChooser(false);
}

void BibleTime::slotToggleFormatToolbar() {
    bool currentState = btConfig().value<bool>("GUI/showFormatToolbarButtons", true);
    btConfig().setValue("GUI/showFormatToolbarButtons", !currentState);
    showOrHideToolBars();
    if (btConfig().value<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowFormatToolbar(!currentState);
    else
        emit toggledTextWindowFormatToolbar(false);
}

void BibleTime::slotToggleToolBarsInEachWindow() {
    bool currentState = btConfig().value<bool>("GUI/showToolbarsInEachWindow", true);
    btConfig().setValue("GUI/showToolbarsInEachWindow", !currentState);
    showOrHideToolBars();

    if (!currentState) {
        emit toggledTextWindowNavigator(btConfig().value<bool>("GUI/showTextWindowNavigator", true));
        emit toggledTextWindowModuleChooser(btConfig().value<bool>("GUI/showTextWindowModuleSelectorButtons", true));
        emit toggledTextWindowToolButtons(btConfig().value<bool>("GUI/showTextWindowToolButtons", true));
        emit toggledTextWindowFormatToolbar(btConfig().value<bool>("GUI/showFormatToolbarButtons", true));
    }
    else {
        emit toggledTextWindowNavigator(false);
        emit toggledTextWindowToolButtons(false);
        emit toggledTextWindowModuleChooser(false);
        emit toggledTextWindowFormatToolbar(false);
    }


}

void BibleTime::showOrHideToolBars() {
    if (btConfig().value<bool>("GUI/showToolbarsInEachWindow", true)) {
        m_navToolBar->setVisible(false);
        m_worksToolBar->setVisible(false);
        m_toolsToolBar->setVisible(false);
        m_formatToolBar->setVisible(false);
    }
    else {
        m_navToolBar->setVisible(btConfig().value<bool>("GUI/showTextWindowNavigator", true));
        m_worksToolBar->setVisible(btConfig().value<bool>("GUI/showTextWindowModuleSelectorButtons", true));
        m_toolsToolBar->setVisible(btConfig().value<bool>("GUI/showTextWindowToolButtons", true));
        m_formatToolBar->setVisible(btConfig().value<bool>("GUI/showFormatToolbarButtons", true));
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

    Q_FOREACH (const QMdiSubWindow * const subWindow, m_mdi->subWindowList()) {
        const CDisplayWindow * const w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
        if (w != 0) {
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
    CSwordModuleInfo* bible = btConfig().getDefaultSwordModuleByType("standardBible");
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

void BibleTime::slotOpenTipDialog() {
    BtTipDialog* dlg = new BtTipDialog(this);
    dlg->show();
}

void BibleTime::saveProfile() {
    // Save main window settings:
    BtConfig & conf = btConfig();
    conf.setSessionValue("MainWindow/geometry", saveGeometry());
    conf.setSessionValue("MainWindow/state", saveState());
    conf.setSessionValue("MainWindow/MDIArrangementMode", static_cast<int>(m_mdi->getMDIArrangementMode()));

    QStringList windowsList;
    Q_FOREACH (const QMdiSubWindow * const w,
               m_mdi->subWindowList(QMdiArea::StackingOrder))
    {
        CDisplayWindow * const displayWindow = dynamic_cast<CDisplayWindow*>(w->widget());
        if (!displayWindow)
            continue;

        const QString windowKey = QString::number(windowsList.size());
        windowsList.append(windowKey);
        const QString windowGroup = "window/" + windowKey + '/';
        displayWindow->storeProfileSettings(windowGroup);
    }
    conf.setSessionValue("windowsList", windowsList);
}

void BibleTime::loadProfile(QAction * action) {
    Q_ASSERT(action);
    QVariant keyProperty = action->property("ProfileKey");
    Q_ASSERT(keyProperty.type() == QVariant::String);
    Q_ASSERT(btConfig().sessionNames().contains(keyProperty.toString()));
    loadProfile(keyProperty.toString());
}

void BibleTime::loadProfile(const QString & profileKey) {
    Q_ASSERT(btConfig().sessionNames().contains(profileKey));

    // Save old profile:
    saveProfile();

    // Close all open windows BEFORE switching profile:
    m_mdi->closeAllSubWindows();

    // Switch profile Activate profile:
    btConfig().setCurrentSession(profileKey);
    reloadProfile();
    refreshProfileMenus();
}

namespace {

/// Helper object for reloadProfile()
struct WindowLoadStatus {
    inline WindowLoadStatus() : window(0) {}
    QStringList failedModules;
    QList<CSwordModuleInfo*> okModules;
    CDisplayWindow * window;
};

} // anonymous namespace

void BibleTime::reloadProfile() {
    typedef CMDIArea::MDIArrangementMode MAM;
    typedef CWriteWindow::WriteWindowType WWT;

    // Cache pointer to config:
    BtConfig & conf = btConfig();

    // Disable updates while doing big changes:
    setUpdatesEnabled(false);

    // Close all open windows:
    m_mdi->closeAllSubWindows();

    // Reload main window settings:
    restoreGeometry(conf.sessionValue<QByteArray>("MainWindow/geometry"));
    restoreState(conf.sessionValue<QByteArray>("MainWindow/state"));
    m_windowFullscreenAction->setChecked(isFullScreen());
    m_mdi->setMDIArrangementMode(static_cast<MAM>(conf.sessionValue<int>("MainWindow/MDIArrangementMode")));

    QWidget * focusWindow = 0;
    QMap<QString, WindowLoadStatus> failedWindows;
    Q_FOREACH (const QString & w,
               conf.sessionValue<QStringList>("windowsList"))
    {
        const QString windowGroup = "window/" + w + '/';

        // Try to determine window modules:
        WindowLoadStatus wls;
        Q_FOREACH (const QString &moduleName,
                   conf.sessionValue<QStringList>(windowGroup + "modules"))
        {
            CSwordModuleInfo * const m = CSwordBackend::instance()->findModuleByName(moduleName);
            if (m) {
                wls.okModules.append(m);
            } else {
                wls.failedModules.append(moduleName);
            }
        }

        // Check whether the window totally failed (no modules can be loaded):
        if (wls.okModules.isEmpty()) {
            failedWindows.insert(w, wls);
            continue;
        }

        // Check whether the window partially failed:
        if (!wls.failedModules.isEmpty())
            failedWindows.insert(w, wls);

        // Try to respawn the window:
        Q_ASSERT(!wls.window);
        const QString key = conf.sessionValue<QString>(windowGroup + "key");
        WWT wwt = static_cast<WWT>(conf.sessionValue<int>(windowGroup + "writeWindowType", 0));
        if (wwt > 0) {
            // Note, that we *might* lose the rest of wls.okModules here:
            if (wls.okModules.size() > 1)
                qWarning() << "Got more modules for a \"write window\" than expected from the profile!";

            wls.window = createWriteDisplayWindow(wls.okModules.first(), key, wwt);
        } else {
            wls.window = createReadDisplayWindow(wls.okModules, key);
        }

        if (wls.window) {
            wls.window->applyProfileSettings(windowGroup);
            if (conf.sessionValue<bool>(windowGroup + "hasFocus", false))
                focusWindow = wls.window;
        } else {
            failedWindows.insert(w, wls);
        }
    }

    // Re-arrange MDI:
    m_mdi->triggerWindowUpdate();

    // Activate focused window:
    if (focusWindow)
        focusWindow->setFocus();

    // Re-enable updates and repaint:
    setUpdatesEnabled(true);
    repaint(); /// \bug The main window (except decors) is all black without this (not even hover over toolbar buttons works)

    /// \todo For windows in failedWindows ask whether to keep the settings / close windows etc
}

void BibleTime::deleteProfile(QAction* action) {
    Q_ASSERT(action);
    QVariant keyProperty = action->property("ProfileKey");
    Q_ASSERT(keyProperty.type() == QVariant::String);
    Q_ASSERT(btConfig().sessionNames().contains(keyProperty.toString()));

    /// \todo Ask for confirmation
    btConfig().deleteSession(keyProperty.toString());
    refreshProfileMenus();
}

void BibleTime::toggleFullscreen() {
    setWindowState(windowState() ^ Qt::WindowFullScreen);
    m_mdi->triggerWindowUpdate();
}

/** Saves current settings into a new profile. */
void BibleTime::saveToNewProfile() {
    BtConfig & conf = btConfig();

    // Get new unique name:
    QString name;
    for (;;) {
        bool ok;
        name = QInputDialog::getText(
                   this, tr("New Session"),
                   tr("Please enter a name for the new session."),
                   QLineEdit::Normal, name, &ok);
        if (!ok)
            return;

        if (!name.isEmpty()) {
            // Check whether name already exists:
            if (conf.sessionNames().values().contains(name)) {
                QMessageBox::information(this, tr("Session already exists"),
                                         tr("Session with the name \"%1\" "
                                            "already exists. Please provide a "
                                            "different name.").arg(name));
            } else {
                break;
            }
        }
    }

    // Also save old profile:
    saveProfile();

    // Save new profile:
    conf.setCurrentSession(conf.addSession(name));
    saveProfile();

    // Refresh profile menus:
    refreshProfileMenus();
}

/** Slot to refresh the save profile and load profile menus. */
void BibleTime::refreshProfileMenus() {
    typedef BtConfig::SessionNamesHashMap SNHM;
    typedef SNHM::const_iterator SNHMCI;

    m_windowLoadProfileMenu->clear();
    m_windowDeleteProfileMenu->clear();

    BtConfig & conf = btConfig();
    const BtConfig::SessionNamesHashMap &sessions = conf.sessionNames();

    const bool enableActions = sessions.size() > 1;
    m_windowLoadProfileMenu->setEnabled(enableActions);
    m_windowDeleteProfileMenu->setEnabled(enableActions);


    if (enableActions) {
        for (SNHMCI it = sessions.constBegin(); it != sessions.constEnd(); ++it) {
            if (it.key() == conf.currentSessionKey())
                continue;

            QAction * a;
            a = m_windowLoadProfileMenu->addAction(it.value());
            a->setProperty("ProfileKey", it.key());
            a = m_windowDeleteProfileMenu->addAction(it.value());
            a->setProperty("ProfileKey", it.key());
        }
    }
}

// Quit from BibleTime
void BibleTime::quit() {
    Search::CSearchDialog::closeDialog();
    close();
}
