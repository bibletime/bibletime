/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
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
#include <QProcess>
#include <QtGlobal>
#include <QToolBar>
#include <QUrl>
#include "backend/config/btconfig.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/btaboutdialog.h"
#include "frontend/cinfodisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "frontend/display/btfindwidget.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/messagedialog.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "frontend/settingsdialogs/cconfigurationdialog.h"
#include "frontend/tips/bttipdialog.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/directory.h"


/** Opens the optionsdialog of BibleTime. */
void BibleTime::slotSettingsOptions() {
    qDebug() << "BibleTime::slotSettingsOptions";
    CConfigurationDialog *dlg = new CConfigurationDialog(this, m_actionCollection);
    BT_CONNECT(dlg,  SIGNAL(signalSettingsChanged()),
               this, SLOT(slotSettingsChanged()) );

    dlg->show();
}

/** Save the settings, used when no settings have been saved before **/
void BibleTime::saveConfigSettings() {
    CConfigurationDialog* dlg = new CConfigurationDialog(this, nullptr);
    dlg->save();
    delete dlg;
}

/** Is called when settings in the optionsdialog were changed (ok or apply) */
void BibleTime::slotSettingsChanged() {
    qDebug() << "BibleTime::slotSettingsChanged";
    const QString language = btConfig().value<QString>("GUI/booknameLanguage", QLocale::system().name());
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

    m_actionCollection->readShortcuts("Application shortcuts");
    refreshDisplayWindows();
    refreshProfileMenus();
    qDebug() << "BibleTime::slotSettingsChanged";
}

/** Opens the bookshelf wizard. */
void BibleTime::slotBookshelfWizard() {
    BtBookshelfWizard dlg(BibleTime::instance());
    dlg.exec();
}

/** Is called just before the window menu is shown. */
void BibleTime::slotWindowMenuAboutToShow() {
    BT_ASSERT(m_windowMenu);

    const int numSubWindows = m_mdi->subWindowList().count();
    m_windowCloseAction->setEnabled(numSubWindows);
    m_windowCloseAllAction->setEnabled(numSubWindows);
    m_openWindowsMenu->setEnabled(numSubWindows);

    const bool enableManualArrangeActions = numSubWindows > 1;
    m_windowCascadeAction->setEnabled(enableManualArrangeActions);
    m_windowTileAction->setEnabled(enableManualArrangeActions);
    m_windowTileVerticalAction->setEnabled(enableManualArrangeActions);
    m_windowTileHorizontalAction->setEnabled(enableManualArrangeActions);
}

/** Is called just before the open windows menu is shown. */
void BibleTime::slotOpenWindowsMenuAboutToShow() {
    BT_ASSERT(m_openWindowsMenu);

    m_openWindowsMenu->clear();
    Q_FOREACH (QMdiSubWindow * const window, m_mdi->usableWindowList()) {
        QAction *openWindowAction = m_openWindowsMenu->addAction(window->windowTitle());
        openWindowAction->setCheckable(true);
        openWindowAction->setChecked(window == m_mdi->activeSubWindow());
        BT_CONNECT(openWindowAction, SIGNAL(triggered()),
                   m_windowMapper,   SLOT(map()));
        m_windowMapper->setMapping(openWindowAction, window);
    }
}

/** This slot is connected with the windowAutoTileAction object */
void BibleTime::slotUpdateWindowArrangementActions(QAction * trigerredAction) {
    BT_ASSERT(trigerredAction);

    if (trigerredAction == m_windowAutoTileVerticalAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTileVertical);
        btConfig().setSessionValue("GUI/alignmentMode", autoTileVertical);
    }
    else if (trigerredAction == m_windowAutoTileHorizontalAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTileHorizontal);
        btConfig().setSessionValue("GUI/alignmentMode", autoTileHorizontal);
    }
    else if (trigerredAction == m_windowAutoTileAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTile);
        btConfig().setSessionValue("GUI/alignmentMode", autoTile);
    }
    else if (trigerredAction == m_windowAutoTabbedAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTabbed);
        btConfig().setSessionValue("GUI/alignmentMode", autoTabbed);
    }
    else if (trigerredAction == m_windowAutoCascadeAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeCascade);
        btConfig().setSessionValue("GUI/alignmentMode", autoCascade);
    }
    else {
        BT_ASSERT(trigerredAction == m_windowManualModeAction
                 || trigerredAction == m_windowTileAction
                 || trigerredAction == m_windowCascadeAction
                 || trigerredAction == m_windowTileVerticalAction
                 || trigerredAction == m_windowTileHorizontalAction);

        if (trigerredAction != m_windowManualModeAction)
            m_windowManualModeAction->setChecked(true);

        m_mdi->enableWindowMinMaxFlags(true);
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeManual);
        btConfig().setSessionValue("GUI/alignmentMode", manual);

        if (trigerredAction == m_windowTileAction)
            m_mdi->myTile();
        else if (trigerredAction == m_windowCascadeAction)
            m_mdi->myCascade();
        else if (trigerredAction == m_windowTileVerticalAction)
            m_mdi->myTileVertical();
        else if (trigerredAction == m_windowTileHorizontalAction)
            m_mdi->myTileHorizontal();

        return;
    }

    m_mdi->enableWindowMinMaxFlags(false);
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

/** Shows/hides the toolbar */
void BibleTime::slotToggleMainToolbar() {
    BT_ASSERT(m_mainToolBar);
    bool currentState = btConfig().sessionValue<bool>("GUI/showMainToolbar", true);
    btConfig().setSessionValue("GUI/showMainToolbar", !currentState);
    if ( m_showMainWindowToolbarAction->isChecked()) {
        m_mainToolBar->show();
    }
    else {
        m_mainToolBar->hide();
    }
}

void BibleTime::slotToggleTextWindowHeader() {
    bool currentState = btConfig().sessionValue<bool>("GUI/showTextWindowHeaders", true);
    btConfig().setSessionValue("GUI/showTextWindowHeaders", !currentState);
    emit toggledTextWindowHeader(!currentState);
}

void BibleTime::slotToggleNavigatorToolbar() {
    bool currentState = btConfig().sessionValue<bool>("GUI/showTextWindowNavigator", true);
    btConfig().setSessionValue("GUI/showTextWindowNavigator", !currentState);
    if (btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowNavigator(!currentState);
    else
        m_navToolBar->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowNavigator", true));
}

void BibleTime::slotToggleToolsToolbar() {
    bool currentState = btConfig().sessionValue<bool>("GUI/showTextWindowToolButtons", true);
    btConfig().setSessionValue("GUI/showTextWindowToolButtons", !currentState);
    if (btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowToolButtons(!currentState);
    else
        m_toolsToolBar->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowToolButtons", true));
}

void BibleTime::slotToggleWorksToolbar() {
    bool currentState = btConfig().sessionValue<bool>("GUI/showTextWindowModuleSelectorButtons", true);
    btConfig().setSessionValue("GUI/showTextWindowModuleSelectorButtons", !currentState);
    if (btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowModuleChooser(!currentState);
    else
        m_worksToolBar->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowModuleSelectorButtons", true));
}

void BibleTime::slotToggleFormatToolbar() {
    bool currentState = btConfig().sessionValue<bool>("GUI/showFormatToolbarButtons", true);
    btConfig().setSessionValue("GUI/showFormatToolbarButtons", !currentState);
    if (btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true))
        emit toggledTextWindowFormatToolbar(!currentState);
    else
        m_formatToolBar->setVisible(!currentState);
}

void BibleTime::slotToggleToolBarsInEachWindow() {
    bool currentState = btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true);
    btConfig().setSessionValue("GUI/showToolbarsInEachWindow", !currentState);
    showOrHideToolBars();
}

void BibleTime::showOrHideToolBars() {
    if (btConfig().sessionValue<bool>("GUI/showToolbarsInEachWindow", true)) {
        // set main window widgets invisible
        m_navToolBar->setVisible(false);
        m_worksToolBar->setVisible(false);
        m_toolsToolBar->setVisible(false);
        m_formatToolBar->setVisible(false);
        // set state of sub window widets
        emit toggledTextWindowNavigator(btConfig().sessionValue<bool>("GUI/showTextWindowNavigator", true));
        emit toggledTextWindowModuleChooser(btConfig().sessionValue<bool>("GUI/showTextWindowModuleSelectorButtons", true));
        emit toggledTextWindowToolButtons(btConfig().sessionValue<bool>("GUI/showTextWindowToolButtons", true));
        emit toggledTextWindowFormatToolbar(btConfig().sessionValue<bool>("GUI/showFormatToolbarButtons", true));
    }
    else {
        // set state of main window widgets
        m_navToolBar->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowNavigator", true));
        m_worksToolBar->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowModuleSelectorButtons", true));
        m_toolsToolBar->setVisible(btConfig().sessionValue<bool>("GUI/showTextWindowToolButtons", true));
        m_formatToolBar->setVisible(btConfig().sessionValue<bool>("GUI/showFormatToolbarButtons", true));
        //set sub window widgets invisible
        emit toggledTextWindowNavigator(false);
        emit toggledTextWindowToolButtons(false);
        emit toggledTextWindowModuleChooser(false);
        emit toggledTextWindowFormatToolbar(false);
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
    BtConstModuleList modules;

    Q_FOREACH (const QMdiSubWindow * const subWindow, m_mdi->subWindowList()) {
        const CDisplayWindow * const w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
        if (w != nullptr) {
            modules << w->modules();
        }
    }
    Search::CSearchDialog::openDialog(modules, QString::null);
}

void BibleTime::slotActiveWindowChanged(QMdiSubWindow* window)
{
    if (window == nullptr)
        m_findWidget->setVisible(false);
}

/* Search default Bible slot
 * Call CSearchDialog::openDialog with only the default bible module
 */
void BibleTime::slotSearchDefaultBible() {
    BtConstModuleList module;
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

    conf.setSessionValue("FindIsVisible", m_findWidget->isVisibleTo(this));

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
    BT_ASSERT(action);
    QVariant keyProperty = action->property("ProfileKey");
    BT_ASSERT(keyProperty.type() == QVariant::String);
    BT_ASSERT(btConfig().sessionNames().contains(keyProperty.toString()));
    loadProfile(keyProperty.toString());
}

void BibleTime::loadProfile(const QString & profileKey) {
    BT_ASSERT(btConfig().sessionNames().contains(profileKey));

    // do nothing if requested session is the current session
    if (profileKey == btConfig().currentSessionKey())
        return;

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
    inline WindowLoadStatus() : window(nullptr) {}
    QStringList failedModules;
    QList<CSwordModuleInfo*> okModules;
    CDisplayWindow * window;
};

} // anonymous namespace

void BibleTime::reloadProfile() {
    using MAM = CMDIArea::MDIArrangementMode;
    using WWT = CPlainWriteWindow::WriteWindowType;
    using message::setQActionCheckedNoTrigger;

    // Cache pointer to config:
    BtConfig & conf = btConfig();

    // Disable updates while doing big changes:
    setUpdatesEnabled(false);

    // Close all open windows:
    m_mdi->closeAllSubWindows();

    // Reload main window settings:
    restoreGeometry(conf.sessionValue<QByteArray>("MainWindow/geometry"));
    restoreState(conf.sessionValue<QByteArray>("MainWindow/state"));

    /*
     * restoreState includes visibility of child widgets, the manually added
     * qactions (so not including bookmark, bookshelf and mag) are not restored
     * though, so we restore their state here.
     */
    setQActionCheckedNoTrigger(m_windowFullscreenAction, isFullScreen());
    setQActionCheckedNoTrigger(m_showTextAreaHeadersAction, conf.sessionValue<bool>("GUI/showTextWindowHeaders", true));
    setQActionCheckedNoTrigger(m_showMainWindowToolbarAction, conf.sessionValue<bool>("GUI/showMainToolbar", true));
    setQActionCheckedNoTrigger(m_showTextWindowNavigationAction, conf.sessionValue<bool>("GUI/showTextWindowNavigator", true));
    setQActionCheckedNoTrigger(m_showTextWindowModuleChooserAction, conf.sessionValue<bool>("GUI/showTextWindowModuleSelectorButtons", true));
    setQActionCheckedNoTrigger(m_showTextWindowToolButtonsAction, conf.sessionValue<bool>("GUI/showTextWindowToolButtons", true));
    setQActionCheckedNoTrigger(m_showFormatToolbarAction, conf.sessionValue<bool>("GUI/showFormatToolbarButtons", true));
    setQActionCheckedNoTrigger(m_toolbarsInEachWindow, conf.sessionValue<bool>("GUI/showToolbarsInEachWindow", true));

    m_mdi->setMDIArrangementMode(static_cast<MAM>(
                                     conf.sessionValue<int>("MainWindow/MDIArrangementMode",CMDIArea::ArrangementModeTile)));

    m_findWidget->setVisible(conf.sessionValue<bool>("FindIsVisible", false));

    QWidget * focusWindow = nullptr;
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
        BT_ASSERT(!wls.window);
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

    /* This call is necessary to restore the visibility of the toolbars in the child
     * windows, since their state is not saved automatically.
     */
    showOrHideToolBars();

    // Re-arrange MDI:
    m_mdi->triggerWindowUpdate();

    // Activate focused window:
    if (focusWindow)
        focusWindow->setFocus();

    // Re-enable updates and repaint:
    setUpdatesEnabled(true);
    repaint(); /// \bug The main window (except decors) is all black without this (not even hover over toolbar buttons work)
    raise(); /// \bug The main window would not refresh at all. A call to this function or adjustSize() seems to fix this

    /// \todo For windows in failedWindows ask whether to keep the settings / close windows etc
}

void BibleTime::deleteProfile(QAction* action) {
    BT_ASSERT(action);
    QVariant keyProperty = action->property("ProfileKey");
    BT_ASSERT(keyProperty.type() == QVariant::String);
    BT_ASSERT(btConfig().sessionNames().contains(keyProperty.toString()));

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
                message::showInformation(this, tr("Session already exists"),
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

/** Slot to refresh the saved profile and load profile menus. */
void BibleTime::refreshProfileMenus() {
    using SNHM = BtConfig::SessionNamesHashMap;
    using SNHMCI = SNHM::const_iterator;

    m_windowLoadProfileMenu->clear();
    m_windowDeleteProfileMenu->clear();

    BtConfig & conf = btConfig();
    const BtConfig::SessionNamesHashMap &sessions = conf.sessionNames();

    const bool enableActions = sessions.size() > 1;
    m_windowLoadProfileMenu->setEnabled(enableActions);
    m_windowDeleteProfileMenu->setEnabled(enableActions);


    if (enableActions) {
        for (SNHMCI it = sessions.constBegin(); it != sessions.constEnd(); ++it) {
            QAction * a;

            a = m_windowLoadProfileMenu->addAction(it.value());
            a->setProperty("ProfileKey", it.key());
            a->setActionGroup(m_windowLoadProfileActionGroup);
            a->setCheckable(true);
            if (it.key() == conf.currentSessionKey())
                a->setChecked(true);

            a = m_windowDeleteProfileMenu->addAction(it.value());
            a->setProperty("ProfileKey", it.key());
            if (it.key() == conf.currentSessionKey())
                a->setDisabled(true);
        }
    }
}

// Quit from BibleTime
void BibleTime::quit() {
    Search::CSearchDialog::closeDialog();
    close();
}
