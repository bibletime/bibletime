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
#include <QCursor>
#include <QDesktopServices>
#include <QInputDialog>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMetaObject>
#include <QProcess>
#include <QtGlobal>
#include <QTextEdit>
#include <QTimerEvent>
#include <QToolBar>
#include <QUrl>
#include "../backend/config/btconfig.h"
#include "../backend/managers/cswordbackend.h"
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
#include "settingsdialogs/cconfigurationdialog.h"
#include "tips/bttipdialog.h"


namespace {

class DebugWindow : public QTextEdit {

public: // methods:

    DebugWindow()
        : QTextEdit(nullptr)
        , m_updateTimerId(startTimer(100))
    {
        setWindowFlags(Qt::Dialog);
        setAttribute(Qt::WA_DeleteOnClose);
        setReadOnly(true);
        retranslateUi();
        show();
    }

    void retranslateUi()
    { setWindowTitle(tr("What's this widget?")); }

    void timerEvent(QTimerEvent * const event) override {
        if (event->timerId() == m_updateTimerId) {
            if (QObject const * w = QApplication::widgetAt(QCursor::pos())) {
                QString objectHierarchy;
                do {
                    QMetaObject const * m = w->metaObject();
                    QString classHierarchy;
                    do {
                        if (!classHierarchy.isEmpty())
                            classHierarchy += QStringLiteral(": ");
                        classHierarchy += m->className();
                        m = m->superClass();
                    } while (m);
                    if (!objectHierarchy.isEmpty()) {
                        objectHierarchy
                                .append(QStringLiteral("<br/>"))
                                .append(tr("<b>child of:</b> %1").arg(
                                            classHierarchy));
                    } else {
                        objectHierarchy.append(
                                    tr("<b>This widget is:</b> %1").arg(
                                        classHierarchy));
                    }
                    w = w->parent();
                } while (w);
                setHtml(objectHierarchy);
            } else {
                setText(tr("No widget"));
            }
        } else {
            QTextEdit::timerEvent(event);
        }
    }

private: // fields:

    int const m_updateTimerId;

}; // class DebugWindow

} // anonymous namespace

/** Opens the optionsdialog of BibleTime. */
void BibleTime::slotSettingsOptions() {
    CConfigurationDialog *dlg = new CConfigurationDialog(this);
    BT_CONNECT(dlg,  &BtConfigDialog::signalSettingsChanged,
               [this]{
                   CSwordBackend::instance().setBooknameLanguage(
                               btConfig().booknameLanguage());

                   /** \todo update the bookmarks after Bible bookname language
                             has been changed. */
                   /* for (QTreeWidgetItemIterator it = m_mainIndex; *it; ++it)
                       if (auto * citem = dynamic_cast<CIndexItemBase*>(*it))
                           citem->update(); */

                   m_actionCollection->readShortcuts(
                               QStringLiteral("Application shortcuts"));
                   refreshDisplayWindows();
                   refreshProfileMenus();
                   m_infoDisplay->updateColors();

#ifdef BUILD_TEXT_TO_SPEECH
                   // reset current text-to-speech instance, will be recreated
                   // based on current config when needed
                   m_textToSpeech.reset();
#endif
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

    auto guiConfig = btConfig().session().group(QStringLiteral("GUI"));
    if (trigerredAction == m_windowAutoTileVerticalAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTileVertical);
        guiConfig.setValue(QStringLiteral("alignmentMode"), autoTileVertical);
    }
    else if (trigerredAction == m_windowAutoTileHorizontalAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTileHorizontal);
        guiConfig.setValue(QStringLiteral("alignmentMode"), autoTileHorizontal);
    }
    else if (trigerredAction == m_windowAutoTileAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTile);
        guiConfig.setValue(QStringLiteral("alignmentMode"), autoTile);
    }
    else if (trigerredAction == m_windowAutoTabbedAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeTabbed);
        guiConfig.setValue(QStringLiteral("alignmentMode"), autoTabbed);
    }
    else if (trigerredAction == m_windowAutoCascadeAction) {
        m_mdi->setMDIArrangementMode(CMDIArea::ArrangementModeCascade);
        guiConfig.setValue(QStringLiteral("alignmentMode"), autoCascade);
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
        guiConfig.setValue(QStringLiteral("alignmentMode"), manual);

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
    auto guiConf = btConfig().session().group(QStringLiteral("GUI"));
    bool const currentState =
            guiConf.value<bool>(QStringLiteral("showMainToolbar"), true);
    guiConf.setValue(QStringLiteral("showMainToolbar"), !currentState);
    if ( m_showMainWindowToolbarAction->isChecked()) {
        m_mainToolBar->show();
    }
    else {
        m_mainToolBar->hide();
    }
}

void BibleTime::slotToggleTextWindowHeader() {
    auto guiConfig = btConfig().session().group(QStringLiteral("GUI"));
    bool const currentState =
            guiConfig.value<bool>(QStringLiteral("showTextWindowHeaders"),
                                  true);
    guiConfig.setValue(QStringLiteral("showTextWindowHeaders"), !currentState);
    Q_EMIT toggledTextWindowHeader(!currentState);
}

void BibleTime::slotToggleNavigatorToolbar() {
    auto guiConfig = btConfig().session().group(QStringLiteral("GUI"));
    bool const currentState =
            guiConfig.value<bool>(QStringLiteral("showTextWindowNavigator"),
                                  true);
    guiConfig.setValue(QStringLiteral("showTextWindowNavigator"),
                       !currentState);
    if (guiConfig.value<bool>(QStringLiteral("showToolbarsInEachWindow"), true))
    {
        Q_EMIT toggledTextWindowNavigator(!currentState);
    } else {
        m_navToolBar->setVisible(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowNavigator"),
                        true));
    }
}

void BibleTime::slotToggleToolsToolbar() {
    auto guiConfig = btConfig().session().group(QStringLiteral("GUI"));
    bool const currentState =
            guiConfig.value<bool>(QStringLiteral("showTextWindowToolButtons"),
                                  true);
    guiConfig.setValue(QStringLiteral("showTextWindowToolButtons"),
                       !currentState);
    if (guiConfig.value<bool>(QStringLiteral("showToolbarsInEachWindow"), true))
    {
        Q_EMIT toggledTextWindowToolButtons(!currentState);
    } else {
        m_toolsToolBar->setVisible(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowToolButtons"),
                        true));
    }
}

void BibleTime::slotToggleWorksToolbar() {
    auto guiConfig = btConfig().session().group(QStringLiteral("GUI"));
    bool const currentState =
            guiConfig.value<bool>(
                QStringLiteral("showTextWindowModuleSelectorButtons"),
                true);
    guiConfig.setValue(QStringLiteral("showTextWindowModuleSelectorButtons"),
                       !currentState);
    if (guiConfig.value<bool>(QStringLiteral("showToolbarsInEachWindow"), true))
    {
        Q_EMIT toggledTextWindowModuleChooser(!currentState);
    } else {
        m_worksToolBar->setVisible(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowModuleSelectorButtons"),
                        true));
    }
}

void BibleTime::slotToggleToolBarsInEachWindow() {
    auto guiConfig = btConfig().session().group(QStringLiteral("GUI"));
    bool const currentState =
            guiConfig.value<bool>(QStringLiteral("showToolbarsInEachWindow"),
                                  true);
    guiConfig.setValue(QStringLiteral("showToolbarsInEachWindow"),
                       !currentState);
    showOrHideToolBars();
}

void BibleTime::showOrHideToolBars() {
    auto const guiConfig = btConfig().session().group(QStringLiteral("GUI"));
    if (guiConfig.value<bool>(QStringLiteral("showToolbarsInEachWindow"), true))
    {
        // set main window widgets invisible
        m_navToolBar->setVisible(false);
        m_worksToolBar->setVisible(false);
        m_toolsToolBar->setVisible(false);
        // set state of sub window widets
        Q_EMIT toggledTextWindowNavigator(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowNavigator"),
                        true));
        Q_EMIT toggledTextWindowModuleChooser(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowModuleSelectorButtons"),
                        true));
        Q_EMIT toggledTextWindowToolButtons(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowToolButtons"),
                        true));
    } else {
        // set state of main window widgets
        m_navToolBar->setVisible(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowNavigator"),
                        true));
        m_worksToolBar->setVisible(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowModuleSelectorButtons"),
                        true));
        m_toolsToolBar->setVisible(
                    guiConfig.value<bool>(
                        QStringLiteral("showTextWindowToolButtons"),
                        true));
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
            modules << w->constModules();
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
    if (auto const * const bible =
            btConfig().getDefaultSwordModuleByType(
                QStringLiteral("standardBible")))
        module.append(bible);
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
    conf.setValue(QStringLiteral("MainWindow/geometry"), saveGeometry());
    conf.setValue(QStringLiteral("MainWindow/state"), saveState());
    conf.setValue(QStringLiteral("MainWindow/MDIArrangementMode"),
                  static_cast<int>(m_mdi->getMDIArrangementMode()));

    conf.setValue(QStringLiteral("FindIsVisible"),
                  m_findWidget->isVisible());

    QStringList windowsList;
    for (auto const * const w : m_mdi->subWindowList(QMdiArea::StackingOrder)) {
        CDisplayWindow * const displayWindow = dynamic_cast<CDisplayWindow*>(w->widget());
        if (!displayWindow)
            continue;

        const QString windowKey = QString::number(windowsList.size());
        windowsList.append(windowKey);
        auto windowConf = conf.group(QStringLiteral("window/") + windowKey);
        displayWindow->storeProfileSettings(windowConf);
    }
    conf.setValue(QStringLiteral("windowsList"), windowsList);
}

void BibleTime::loadProfile(QAction * action) {
    BT_ASSERT(action);
    QVariant keyProperty = action->property("ProfileKey");
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    BT_ASSERT(keyProperty.type() == QVariant::String);
    #else
    BT_ASSERT(keyProperty.typeId() == QMetaType::QString);
    #endif
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
    using message::setQActionCheckedNoTrigger;

    // Disable updates while doing big changes:
    setUpdatesEnabled(false);

    // Close all open windows:
    m_mdi->closeAllSubWindows();

    // Reload main window settings:
    auto const sessionConf = btConfig().session();
    auto const mwConf = sessionConf.group(QStringLiteral("MainWindow"));
    restoreGeometry(mwConf.value<QByteArray>(QStringLiteral("geometry")));
    restoreState(mwConf.value<QByteArray>(QStringLiteral("state")));

    /*
     * restoreState includes visibility of child widgets, the manually added
     * qactions (so not including bookmark, bookshelf and mag) are not restored
     * though, so we restore their state here.
     */
    auto const guiConf = sessionConf.group(QStringLiteral("GUI"));
    setQActionCheckedNoTrigger(m_windowFullscreenAction, isFullScreen());
    setQActionCheckedNoTrigger(
                m_showTextAreaHeadersAction,
                guiConf.value<bool>(QStringLiteral("showTextWindowHeaders"),
                                    true));
    setQActionCheckedNoTrigger(
                m_showMainWindowToolbarAction,
                guiConf.value<bool>(QStringLiteral("showMainToolbar"), true));
    setQActionCheckedNoTrigger(
                m_showTextWindowNavigationAction,
                guiConf.value<bool>(QStringLiteral("showTextWindowNavigator"),
                                    true));
    setQActionCheckedNoTrigger(
                m_showTextWindowModuleChooserAction,
                guiConf.value<bool>(
                    QStringLiteral("showTextWindowModuleSelectorButtons"),
                    true));
    setQActionCheckedNoTrigger(
                m_showTextWindowToolButtonsAction,
                guiConf.value<bool>(QStringLiteral("showTextWindowToolButtons"),
                                    true));
    setQActionCheckedNoTrigger(
                m_toolbarsInEachWindow,
                guiConf.value<bool>(QStringLiteral("showToolbarsInEachWindow"),
                                    true));

    m_mdi->setMDIArrangementMode(
                static_cast<CMDIArea::MDIArrangementMode>(
                    mwConf.value<int>(QStringLiteral("MDIArrangementMode"),
                                      CMDIArea::ArrangementModeTile)));

    m_findWidget->setVisible(
                sessionConf.value<bool>(QStringLiteral("FindIsVisible"),
                                        false));

    QWidget * focusWindow = nullptr;
    struct WindowLoadStatus {
        QStringList failedModules;
        QList<CSwordModuleInfo *> okModules;
    };
    QMap<QString, WindowLoadStatus> failedWindows;
    for (auto const & w
         : sessionConf.value<QStringList>(QStringLiteral("windowsList")))
    {
        BT_ASSERT(!w.endsWith('/'));
        auto const windowConf =
                sessionConf.group(QStringLiteral("window/") + w);

        // Try to determine window modules:
        WindowLoadStatus wls;
        for (auto const & moduleName
             : windowConf.value<QStringList>(QStringLiteral("modules")))
        {
            if (auto * const m =
                        CSwordBackend::instance().findModuleByName(moduleName))
            {
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
        auto const key = windowConf.value<QString>(QStringLiteral("key"));
        if (auto * const window = createReadDisplayWindow(wls.okModules, key)) {
            window->applyProfileSettings(windowConf);
            if (windowConf.value<bool>(QStringLiteral("hasFocus"), false))
                focusWindow = window;
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
    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    BT_ASSERT(keyProperty.type() == QVariant::String);
    #else
    BT_ASSERT(keyProperty.typeId() == QMetaType::QString);
    #endif
    BT_ASSERT(btConfig().sessionNames().contains(keyProperty.toString()));

    /// \todo Ask for confirmation
    btConfig().deleteSession(keyProperty.toString());
    refreshProfileMenus();
}

void BibleTime::toggleFullscreen() {
    setWindowState(windowState() ^ Qt::WindowFullScreen);
    m_mdi->triggerWindowUpdate();
}

template <bool goingUp>
void BibleTime::autoScroll() {
    setDisplayFocus();

    static constexpr int const intervals[21] = {
        1, 2, 3, 5, 9, 15, 25, 43, 72, 120, 200,
        120, 72, 43, 25, 15, 9, 5, 3, 2, 1
    };

    static constexpr int const nudgeSpeed = goingUp ? 1 : -1;
    if (m_autoScrollSpeed == 0
        || ((m_autoScrollSpeed > 0) != goingUp // going in the opposite when
            && !m_autoScrollTimer.isActive())) // resuming from pause
    {
        m_autoScrollSpeed = nudgeSpeed; // start safe at slow speed
        m_autoScrollTimer.setInterval(intervals[m_autoScrollSpeed + 10]);
    } else if (m_autoScrollSpeed != nudgeSpeed * 10) { // Stay in [-10, 10]
        m_autoScrollSpeed += nudgeSpeed;
        if (m_autoScrollSpeed == 0) {
            m_autoScrollTimer.stop();
            m_autoScrollPauseAction->setEnabled(false);
            return;
        }
        m_autoScrollTimer.setInterval(intervals[m_autoScrollSpeed + 10]);
    }

    m_autoScrollTimer.start();
    m_autoScrollPauseAction->setEnabled(true);
}

template void BibleTime::autoScroll<true>();
template void BibleTime::autoScroll<false>();

void BibleTime::autoScrollPause() {
    if (!m_autoScrollSpeed)
        return;
    setDisplayFocus();
    if (m_autoScrollTimer.isActive()) {
        m_autoScrollTimer.stop();
    } else {
        m_autoScrollTimer.start();
    }
}

bool BibleTime::autoScrollAnyKey() {
    if (!m_autoScrollSpeed)
        return false;
    autoScrollStop();
    return true;
}

void BibleTime::autoScrollStop() {
    m_autoScrollTimer.stop();
    m_autoScrollSpeed = 0;
    m_autoScrollPauseAction->setEnabled(false);
}

void BibleTime::slotAutoScroll() {
    auto * display = getCurrentDisplay();
    if (display) {
        display->scroll(m_autoScrollSpeed > 0 ? -1 : 1);
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

void BibleTime::slotShowDebugWindow(bool show) {
    if (show) {
        BT_ASSERT(!m_debugWindow);
        m_debugWindow = new DebugWindow();
        BT_CONNECT(m_debugWindow, &QObject::destroyed, m_debugWidgetAction,
                   [action=m_debugWidgetAction] { action->setChecked(false); },
                   Qt::DirectConnection);
    } else {
        delete m_debugWindow;
    }
}
