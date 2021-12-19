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
#include "../backend/config/btconfig.h"
#include "../backend/keys/cswordversekey.h"
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "../util/directory.h"
#include "btaboutdialog.h"
#include "cinfodisplay.h"
#include "cmdiarea.h"
#include "bookshelfwizard/btbookshelfwizard.h"
#include "display/btfindwidget.h"
#include "display/btmodelviewreaddisplay.h"
#include "displaywindow/btmodulechooserbar.h"
#include "displaywindow/cdisplaywindow.h"
#include "messagedialog.h"
#include "searchdialog/csearchdialog.h"
#include "settingsdialogs/cconfigurationdialog.h"
#include "tips/bttipdialog.h"


/** Opens the optionsdialog of BibleTime. */
void BibleTime::slotSettingsOptions() {
    qDebug() << "BibleTime::slotSettingsOptions";
    CConfigurationDialog *dlg = new CConfigurationDialog(this);
    BT_CONNECT(dlg,  &BtConfigDialog::signalSettingsChanged,
               [this]{
                   qDebug() << "BibleTime::slotSettingsChanged";
                   CSwordBackend::instance()->setBooknameLanguage(
                               btConfig().booknameLanguage());

                   /** \todo update the bookmarks after Bible bookname language
                             has been changed. */
                   /* for (QTreeWidgetItemIterator it = m_mainIndex; *it; ++it)
                       if (auto * citem = dynamic_cast<CIndexItemBase*>(*it))
                           citem->update(); */

                   m_actionCollection->readShortcuts("Application shortcuts");
                   refreshDisplayWindows();
                   refreshProfileMenus();
                   m_infoDisplay->updateColors();
                   qDebug() << "BibleTime::slotSettingsChanged";
               });

    dlg->show();
}

/** Save the settings, used when no settings have been saved before **/
void BibleTime::saveConfigSettings()
{ CConfigurationDialog(this).save(); }

/** Opens the bookshelf wizard. */
void BibleTime::slotBookshelfWizard()
{ BtBookshelfWizard(this).exec(); }

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
    for (auto * const window : m_mdi->usableWindowList()) {
        QAction *openWindowAction = m_openWindowsMenu->addAction(window->windowTitle());
        openWindowAction->setCheckable(true);
        openWindowAction->setChecked(window == m_mdi->activeSubWindow());
        BT_CONNECT(openWindowAction, &QAction::triggered,
                   [this, window]{ slotSetActiveSubWindow(window); });
    }
}

/** This slot is connected with the windowAutoTileAction object */
void BibleTime::slotUpdateWindowArrangementActions(QAction * trigerredAction) {
    BT_ASSERT(trigerredAction);

    auto guiConfig = btConfig().session().group("GUI");
    if (trigerredAction == m_windowAutoTileVerticalAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTileVertical);
        guiConfig.setValue("alignmentMode", autoTileVertical);
    }
    else if (trigerredAction == m_windowAutoTileHorizontalAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTileHorizontal);
        guiConfig.setValue("alignmentMode", autoTileHorizontal);
    }
    else if (trigerredAction == m_windowAutoTileAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTile);
        guiConfig.setValue("alignmentMode", autoTile);
    }
    else if (trigerredAction == m_windowAutoTabbedAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTabbed);
        guiConfig.setValue("alignmentMode", autoTabbed);
    }
    else if (trigerredAction == m_windowAutoCascadeAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeCascade);
        guiConfig.setValue("alignmentMode", autoCascade);
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
        guiConfig.setValue("alignmentMode", manual);

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
    auto guiConf = btConfig().session().group("GUI");
    bool const currentState = guiConf.value<bool>("showMainToolbar", true);
    guiConf.setValue("showMainToolbar", !currentState);
    if ( m_showMainWindowToolbarAction->isChecked()) {
        m_mainToolBar->show();
    }
    else {
        m_mainToolBar->hide();
    }
}

void BibleTime::slotToggleTextWindowHeader() {
    auto guiConfig = btConfig().session().group("GUI");
    bool const currentState =
            guiConfig.value<bool>("showTextWindowHeaders", true);
    guiConfig.setValue("showTextWindowHeaders", !currentState);
    Q_EMIT toggledTextWindowHeader(!currentState);
}

void BibleTime::slotToggleNavigatorToolbar() {
    auto guiConfig = btConfig().session().group("GUI");
    bool const currentState =
            guiConfig.value<bool>("showTextWindowNavigator", true);
    guiConfig.setValue("showTextWindowNavigator", !currentState);
    if (guiConfig.value<bool>("showToolbarsInEachWindow", true))
        Q_EMIT toggledTextWindowNavigator(!currentState);
    else
        m_navToolBar->setVisible(guiConfig.value<bool>("showTextWindowNavigator", true));
}

void BibleTime::slotToggleToolsToolbar() {
    auto guiConfig = btConfig().session().group("GUI");
    bool const currentState =
            guiConfig.value<bool>("showTextWindowToolButtons", true);
    guiConfig.setValue("showTextWindowToolButtons", !currentState);
    if (guiConfig.value<bool>("showToolbarsInEachWindow", true))
        Q_EMIT toggledTextWindowToolButtons(!currentState);
    else
        m_toolsToolBar->setVisible(guiConfig.value<bool>("showTextWindowToolButtons", true));
}

void BibleTime::slotToggleWorksToolbar() {
    auto guiConfig = btConfig().session().group("GUI");
    bool const currentState =
            guiConfig.value<bool>("showTextWindowModuleSelectorButtons", true);
    guiConfig.setValue("showTextWindowModuleSelectorButtons", !currentState);
    if (guiConfig.value<bool>("showToolbarsInEachWindow", true))
        Q_EMIT toggledTextWindowModuleChooser(!currentState);
    else
        m_worksToolBar->setVisible(guiConfig.value<bool>("showTextWindowModuleSelectorButtons", true));
}

void BibleTime::slotToggleToolBarsInEachWindow() {
    auto guiConfig = btConfig().session().group("GUI");
    bool const currentState =
            guiConfig.value<bool>("showToolbarsInEachWindow", true);
    guiConfig.setValue("showToolbarsInEachWindow", !currentState);
    showOrHideToolBars();
}

void BibleTime::showOrHideToolBars() {
    auto const guiConfig = btConfig().session().group("GUI");
    if (guiConfig.value<bool>("showToolbarsInEachWindow", true)) {
        // set main window widgets invisible
        m_navToolBar->setVisible(false);
        m_worksToolBar->setVisible(false);
        m_toolsToolBar->setVisible(false);
        // set state of sub window widets
        Q_EMIT toggledTextWindowNavigator(guiConfig.value<bool>("showTextWindowNavigator", true));
        Q_EMIT toggledTextWindowModuleChooser(guiConfig.value<bool>("showTextWindowModuleSelectorButtons", true));
        Q_EMIT toggledTextWindowToolButtons(guiConfig.value<bool>("showTextWindowToolButtons", true));
    }
    else {
        // set state of main window widgets
        m_navToolBar->setVisible(guiConfig.value<bool>("showTextWindowNavigator", true));
        m_worksToolBar->setVisible(guiConfig.value<bool>("showTextWindowModuleSelectorButtons", true));
        m_toolsToolBar->setVisible(guiConfig.value<bool>("showTextWindowToolButtons", true));
        //set sub window widgets invisible
        Q_EMIT toggledTextWindowNavigator(false);
        Q_EMIT toggledTextWindowToolButtons(false);
        Q_EMIT toggledTextWindowModuleChooser(false);
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

    for (auto const * const subWindow : m_mdi->subWindowList()) {
        const CDisplayWindow * const w = dynamic_cast<CDisplayWindow*>(subWindow->widget());
        if (w != nullptr) {
            modules << w->modules();
        }
    }
    openSearchDialog(std::move(modules));
}

void BibleTime::slotActiveWindowChanged(QMdiSubWindow* window)
{
    if (window == nullptr)
        m_findWidget->setVisible(false);
    autoScrollStop();
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
    openSearchDialog(std::move(module));
}

void BibleTime::openOnlineHelp_Handbook() {
    auto url(util::directory::getHandbook());
    if (url.isEmpty()) {
        message::showCritical(
                    this,
                    tr("Error locating handbook!"),
                    tr("A suitable installed handbook could not be found!"));
        return;
    }
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(url)))
        message::showCritical(
                    this,
                    tr("Error opening handbook!"),
                    tr("The installed handbook could not be opened!"));
}

void BibleTime::openOnlineHelp_Howto() {
    auto url(util::directory::getHowto());
    if (url.isEmpty()) {
        message::showCritical(
                    this,
                    tr("Error locating howto!"),
                    tr("A suitable installed howto could not be found!"));
        return;
    }
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(url)))
        message::showCritical(
                    this,
                    tr("Error opening howto!"),
                    tr("The installed howto could not be opened!"));
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
    auto conf = btConfig().session();
    conf.setValue("MainWindow/geometry", saveGeometry());
    conf.setValue("MainWindow/state", saveState());
    conf.setValue("MainWindow/MDIArrangementMode", static_cast<int>(m_mdi->getMDIArrangementMode()));

    conf.setValue("FindIsVisible", m_findWidget->isVisibleTo(this));

    QStringList windowsList;
    for (auto const * const w : m_mdi->subWindowList(QMdiArea::StackingOrder)) {
        CDisplayWindow * const displayWindow = dynamic_cast<CDisplayWindow*>(w->widget());
        if (!displayWindow)
            continue;

        const QString windowKey = QString::number(windowsList.size());
        windowsList.append(windowKey);
        auto windowConf = conf.group("window/" + windowKey);
        displayWindow->storeProfileSettings(windowConf);
    }
    conf.setValue("windowsList", windowsList);
}

void BibleTime::loadProfile(QAction * action) {
    BT_ASSERT(action);
    QVariant keyProperty = action->property("ProfileKey");
    BT_ASSERT(keyProperty.type() == QVariant::String);
    BT_ASSERT(btConfig().sessionNames().contains(keyProperty.toString()));
    loadProfile(keyProperty.toString());
}

void BibleTime::loadProfile(const QString & profileKey) {
    auto & conf = btConfig();
    BT_ASSERT(conf.sessionNames().contains(profileKey));

    // do nothing if requested session is the current session
    if (profileKey == conf.currentSessionKey())
        return;

    // Save old profile:
    saveProfile();

    // Close all open windows BEFORE switching profile:
    m_mdi->closeAllSubWindows();

    // Switch profile Activate profile:
    conf.setCurrentSession(profileKey);
    reloadProfile();
    refreshProfileMenus();
}

void BibleTime::reloadProfile() {
    using MAM = CMDIArea::MDIArrangementMode;
    using message::setQActionCheckedNoTrigger;

    // Disable updates while doing big changes:
    setUpdatesEnabled(false);

    // Close all open windows:
    m_mdi->closeAllSubWindows();

    // Reload main window settings:
    auto const sessionConf = btConfig().session();
    auto const mwConf = sessionConf.group("MainWindow");
    restoreGeometry(mwConf.value<QByteArray>("geometry"));
    restoreState(mwConf.value<QByteArray>("state"));

    /*
     * restoreState includes visibility of child widgets, the manually added
     * qactions (so not including bookmark, bookshelf and mag) are not restored
     * though, so we restore their state here.
     */
    auto const guiConf = sessionConf.group("GUI");
    setQActionCheckedNoTrigger(m_windowFullscreenAction, isFullScreen());
    setQActionCheckedNoTrigger(m_showTextAreaHeadersAction, guiConf.value<bool>("showTextWindowHeaders", true));
    setQActionCheckedNoTrigger(m_showMainWindowToolbarAction, guiConf.value<bool>("showMainToolbar", true));
    setQActionCheckedNoTrigger(m_showTextWindowNavigationAction, guiConf.value<bool>("showTextWindowNavigator", true));
    setQActionCheckedNoTrigger(m_showTextWindowModuleChooserAction, guiConf.value<bool>("showTextWindowModuleSelectorButtons", true));
    setQActionCheckedNoTrigger(m_showTextWindowToolButtonsAction, guiConf.value<bool>("showTextWindowToolButtons", true));
    setQActionCheckedNoTrigger(m_toolbarsInEachWindow, guiConf.value<bool>("showToolbarsInEachWindow", true));

    m_mdi->setMDIArrangementMode(
                static_cast<MAM>(
                    mwConf.value<int>("MDIArrangementMode",
                                      CMDIArea::ArrangementModeTile)));

    m_findWidget->setVisible(sessionConf.value<bool>("FindIsVisible", false));

    QWidget * focusWindow = nullptr;
    struct WindowLoadStatus {
        QStringList failedModules;
        QList<CSwordModuleInfo *> okModules;
        CDisplayWindow * window = nullptr;
    };
    QMap<QString, WindowLoadStatus> failedWindows;
    for (auto const & w : sessionConf.value<QStringList>("windowsList")) {
        BT_ASSERT(!w.endsWith('/'));
        auto const windowConf = sessionConf.group("window/" + w);

        // Try to determine window modules:
        WindowLoadStatus wls;
        for (auto const & moduleName
             : windowConf.value<QStringList>("modules"))
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
        auto const key = windowConf.value<QString>("key");
        wls.window = createReadDisplayWindow(wls.okModules, key);

        if (wls.window) {
            wls.window->applyProfileSettings(windowConf);
            if (windowConf.value<bool>("hasFocus", false))
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

void BibleTime::autoScrollUp() {
    setDisplayFocus();
    if (m_autoScroll.speed) {
        if (m_autoScroll.speed < 10) {
            ++m_autoScroll.speed;
        }
    } else {
        autoScrollEnablePauseAction(true);
        m_autoScroll.speed = 1;
    }
    setAutoScrollTimerInterval();
    if (m_autoScroll.speed)
        m_autoScrollTimer.start();
}

void BibleTime::autoScrollDown() {
    setDisplayFocus();
    if (m_autoScroll.speed){
        if (m_autoScroll.speed > -10) {
            --m_autoScroll.speed;
        }
    } else {
        autoScrollEnablePauseAction(true);
        m_autoScroll.speed = -1;
    }
    setAutoScrollTimerInterval();
    if (m_autoScroll.speed)
        m_autoScrollTimer.start();
}

void BibleTime::autoScrollPause() {
    if (!m_autoScroll.speed)
        return;
    setDisplayFocus();
    if (m_autoScrollTimer.isActive()) {
        m_autoScrollTimer.stop();
    } else {
        m_autoScrollTimer.start();
    }
}

bool BibleTime::autoScrollAnyKey(int /* key */) {
    if (!m_autoScroll.speed)
        return false;
    autoScrollStop();
    return true;
}

void BibleTime::autoScrollStop() {
    m_autoScrollTimer.stop();
    m_autoScroll.speed = 0;
    autoScrollEnablePauseAction(false);
}

void BibleTime::autoScrollEnablePauseAction(bool enable) {
    m_autoScrollPauseAction = &m_actionCollection->action("autoScrollPause");
    m_autoScrollPauseAction->setEnabled(enable);
}

void BibleTime::slotAutoScroll() {
    auto * display = getCurrentDisplay();
    if (display) {
        display->scroll(m_autoScroll.speed >0 ? -1 : 1 );
        display->updateReferenceText();
    }
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
    m_windowLoadProfileMenu->clear();
    m_windowDeleteProfileMenu->clear();

    BtConfig & conf = btConfig();
    auto const & sessions = conf.sessionNames();

    const bool enableActions = sessions.size() > 1;
    m_windowLoadProfileMenu->setEnabled(enableActions);
    m_windowDeleteProfileMenu->setEnabled(enableActions);


    if (enableActions) {
        for (auto it = sessions.constBegin(); it != sessions.constEnd(); ++it) {
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
