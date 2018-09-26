/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/cmdiarea.h"

#include <QEvent>
#include <QMdiSubWindow>
#include <QMenu>
#include <QtGlobal>
#include <QTabBar>
#include <QTimer>
#include <QToolBar>
#include "frontend/btwebenginepage.h"
#include "frontend/btwebengineview.h"
#include <QWindowStateChangeEvent>
#include "bibletime.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/display/cdisplay.h"
#include "util/btconnect.h"


namespace {

inline CDisplayWindow * getDisplayWindow(const QMdiSubWindow * const mdiWindow) {
    return qobject_cast<CDisplayWindow *>(mdiWindow->widget());
}

inline BtWebEngineView * getWebViewFromDisplayWindow(const CDisplayWindow * const displayWindow) {
    if (!displayWindow)
        return nullptr;
    CDisplay * const display = displayWindow->displayWidget();
    if (!display)
        return nullptr;
    return qobject_cast<BtWebEngineView *>(display->view());
}

} // anonymous namespace


CMDIArea::CMDIArea(BibleTime * parent)
        : QMdiArea((BT_ASSERT(parent), parent))
        , m_mdiArrangementMode(ArrangementModeManual)
        , m_activeWindow(nullptr)
        , m_bibleTime(parent)
{
    // Set document-style tabs (for Mac):
    setDocumentMode(true);

    /*
      Activate windows based on the history of activation, e.g. when one has window A
      activated, and activates window B and then closes window B, then window A is activated.
    */
    setActivationOrder(QMdiArea::ActivationHistoryOrder);

    // Show scrollbars only when needed:
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    BT_CONNECT(this, SIGNAL(subWindowActivated(QMdiSubWindow *)),
               this, SLOT(slotSubWindowActivated(QMdiSubWindow *)));
}

void CMDIArea::fixSystemMenu(QMdiSubWindow* subWindow) {
    // Change Qt QMdiSubWindow Close action to have no shortcuts
    // This makes our closeWindow actions with Ctrl-W work correctly
    Q_FOREACH(QAction * const action, subWindow->systemMenu()->actions()) {
        if (action->text().contains("Close")) {
            action->setShortcuts(QList<QKeySequence>());
            break;
        }
    }
}

QMdiSubWindow * CMDIArea::addSubWindow(QWidget * widget,
                                       Qt::WindowFlags windowFlags)
{
    QMdiSubWindow * const subWindow = QMdiArea::addSubWindow(widget, windowFlags);
    subWindow->installEventFilter(this);
    fixSystemMenu(subWindow);

    // Manual arrangement mode
    enableWindowMinMaxFlags(true);
    if (m_mdiArrangementMode == ArrangementModeManual) {
        // Note that the window size/maximization may be changed later by a session restore.
        // If we already have an active window, make the new one simular to it
        if (activeSubWindow()) {
            if (activeSubWindow()->isMaximized()) {
                subWindow->showMaximized(); // Maximize the new window
            } else {
                // Make new window the same size as the active window and move it slightly.
                subWindow->resize(activeSubWindow()->size());
                QRect subWinGeom = activeSubWindow()->geometry();
                static const int MOVESIZE = 30;
                subWinGeom.translate(MOVESIZE, MOVESIZE);
                // If it goes off screen, move it almost to the top left
                if (!frameRect().contains(subWinGeom))
                    subWinGeom.moveTo(MOVESIZE, MOVESIZE);
                subWindow->setGeometry(subWinGeom);
            }
        } else {
            //set the window to be big enough
            subWindow->resize(400, 400);
        }
        subWindow->raise();
    } else {
        // Automatic arrangement modes
        enableWindowMinMaxFlags(false);
        triggerWindowUpdate();
    }
    return subWindow;
}

void CMDIArea::setMDIArrangementMode( const MDIArrangementMode newArrangementMode ) {
    m_mdiArrangementMode = newArrangementMode;
    switch (m_mdiArrangementMode) {
        case ArrangementModeManual:
            setViewMode(QMdiArea::SubWindowView);
            break;
        case ArrangementModeTile:
            setViewMode(QMdiArea::SubWindowView);
            tileSubWindows();
            break;
        case ArrangementModeTabbed:
            setViewMode(QMdiArea::TabbedView);
            for (auto win : subWindowList())
                win->showMaximized();
            break;
        default:
            setViewMode(QMdiArea::SubWindowView);
            triggerWindowUpdate();
            break;
    }
    Q_FOREACH(QTabBar * const tab, findChildren<QTabBar *>()) {
        QObject* parent = tab->parent();
        if (parent == this)
            tab->setTabsClosable(true);
    }
}

void CMDIArea::closeTab(int i) {
   QMdiSubWindow *sub = subWindowList()[i];
   QWidget *win = sub->widget();
   win->close();
   setActiveSubWindow(sub);
   closeActiveSubWindow();
}

void CMDIArea::myTileVertical() {
    if (!updatesEnabled()) {
        return;
    }
    QList<QMdiSubWindow*> windows = usableWindowList();
    if (windows.isEmpty()) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);

    setUpdatesEnabled(false);

    QMdiSubWindow * const active = activeSubWindow();

    const int widthForEach = width() / windows.count();
    int x = 0;
    Q_FOREACH (QMdiSubWindow * const window, windows) {
        window->showNormal();

        const int preferredWidth = window->minimumWidth() + window->baseSize().width();
        const int actWidth = qMax(widthForEach, preferredWidth);

        window->setGeometry(x, 0, actWidth, height());
        x += actWidth;
    }

    if (active != nullptr) {
        active->setFocus();
    }

    setUpdatesEnabled(true);
    emitWindowCaptionChanged();
}

void CMDIArea::myTileHorizontal() {
    if (!updatesEnabled()) {
        return;
    }
    QList<QMdiSubWindow*> windows = usableWindowList();
    if (windows.isEmpty()) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);

    setUpdatesEnabled(false);
    QMdiSubWindow * const active = activeSubWindow();

    const int heightForEach = height() / windows.count();
    int y = 0;
    Q_FOREACH (QMdiSubWindow * const window, windows) {
        window->showNormal();

        const int preferredHeight = window->minimumHeight() + window->baseSize().height();
        const int actHeight = qMax(heightForEach, preferredHeight);

        window->setGeometry( 0, y, width(), actHeight );
        y += actHeight;
    }

    if (active != nullptr) {
        active->setFocus();
    }

    setUpdatesEnabled(true);
    emitWindowCaptionChanged();
}

// Tile the windows, tiling implemented by Qt
void CMDIArea::myTile() {
    if (!updatesEnabled() || usableWindowList().isEmpty()) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);
    tileSubWindows();
    emitWindowCaptionChanged();
}

void CMDIArea::myCascade() {
    if (!updatesEnabled()) {
        return;
    }
    QList<QMdiSubWindow*> windows = usableWindowList();
    if (windows.isEmpty()) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);

    if (windows.count() == 1) {
        windows.first()->showMaximized();
    }
    else {
        setUpdatesEnabled(false);

        QMdiSubWindow * const active = activeSubWindow();

        static const unsigned offsetX = 40;
        static const unsigned offsetY = 40;
        const unsigned int windowWidth =  width() - (windows.count() - 1) * offsetX;
        const unsigned int windowHeight = height() - (windows.count() - 1) * offsetY;
        unsigned int x = 0;
        unsigned int y = 0;

        Q_FOREACH (QMdiSubWindow * const window, windows) {
            if (window == active) { //leave out the active window which should be the top window
                continue;
            }
            window->showNormal();
            window->raise(); //make it the on-top-of-window-stack window to make sure they're in the right order
            window->setGeometry(x, y, windowWidth, windowHeight);
            x += offsetX;
            y += offsetY;
        }
        active->showNormal();
        active->setGeometry(x, y, windowWidth, windowHeight);
        active->raise();
        active->activateWindow();

        setUpdatesEnabled(true);
    }

    emitWindowCaptionChanged();
}

void CMDIArea::emitWindowCaptionChanged() {
    if (activeSubWindow()) {
        emit sigSetToplevelCaption(activeSubWindow()->windowTitle());
    }
    else {
        emit sigSetToplevelCaption(QString::null);
    }
}

QList<QMdiSubWindow*> CMDIArea::usableWindowList() const {
    //Take care: when new windows are added, they will not appear
    //in subWindowList() when their ChildAdded-Event is triggered
    QList<QMdiSubWindow*> ret;
    Q_FOREACH(QMdiSubWindow * const w, subWindowList())
        if (!w->isHidden())
            ret.append(w);
    return ret;
}

BtWebEngineView* CMDIArea::getActiveWebView()
{
    QMdiSubWindow* activeMdiWindow = activeSubWindow();
    CDisplayWindow* const activeWindow = getDisplayWindow(activeMdiWindow);
    BtWebEngineView* webView = getWebViewFromDisplayWindow(activeWindow);
    return webView;
}

void CMDIArea::slotSubWindowActivated(QMdiSubWindow* client) {
    if (subWindowList().isEmpty())
        m_bibleTime->clearMdiToolBars();

    if (client == nullptr) {
        return;
    }
    emit sigSetToplevelCaption( client->windowTitle().trimmed() );

    // Notify child window it is active
    CDisplayWindow* const activeWindow = getDisplayWindow(client);
    if (activeWindow != nullptr && activeWindow != m_activeWindow) {
        m_activeWindow = activeWindow;
        activeWindow->windowActivated();
    }
}

void CMDIArea::findNextTextInActiveWindow(QString const & text, bool cs)
{ findTextInActiveWindow(text, cs, false); }

void CMDIArea::findPreviousTextInActiveWindow(QString const & text, bool cs)
{ findTextInActiveWindow(text, cs, true); }

void CMDIArea::highlightTextInActiveWindow(const QString& text, bool caseSensitive) {
    CDisplayWindow* const displayWindow = getDisplayWindow(activeSubWindow());
    displayWindow->displayWidget()->highlightText(text, caseSensitive);
}

void CMDIArea::findTextInActiveWindow(QString const & text,
                                      bool caseSensitive,
                                      bool backward)
{
    CDisplayWindow* const displayWindow = getDisplayWindow(activeSubWindow());
    displayWindow->displayWidget()->findText(text, caseSensitive, backward);
}

void CMDIArea::resizeEvent(QResizeEvent* e) {
    /*
      Do not call QMdiArea::resizeEvent(e) if we are in manual arrangement
      mode, since this would mess up our layout. Also, don't call it for the
      automatic arrangement modes that we implement. Call it only for those
      modes implemented by Qt
    */
    if (m_mdiArrangementMode == ArrangementModeTabbed) {
        QMdiArea::resizeEvent(e);
    }
    else if (updatesEnabled()) {
        // Handle resize for automatic modes that we implement
        triggerWindowUpdate();
    }
}

//handle events of the client windows to update layout if necessary
bool CMDIArea::eventFilter(QObject *o, QEvent *e) {
    const QMdiSubWindow * const w = qobject_cast<QMdiSubWindow*>(o);

    // Let the event be handled by other filters:
    if (w == nullptr)
        return QMdiArea::eventFilter(o, e);

    switch (e->type()) {
        case QEvent::WindowStateChange: {
            Qt::WindowStates const newState(w->windowState());
            Qt::WindowStates const oldState(
                    static_cast<QWindowStateChangeEvent *>(e)->oldState());

            /*
              Do not handle window activation or deactivation here, it will
              produce wrong results because this event is handled too early. Let
              slotSubWindowActivated() handle this.
            */

            // Check if subwindow was maximized or un-maximized:
            if ((newState ^ oldState) & Qt::WindowMaximized) {
                triggerWindowUpdate();
                break;
            }

            // Check if subwindow was minimized or de-minimized:
            if ((newState ^ oldState) & Qt::WindowMinimized) {
                triggerWindowUpdate();
            }
            break;
        }
        case QEvent::Close:
            triggerWindowUpdate();
            break;
        case QEvent::WindowTitleChange:
            if (o == activeSubWindow()) {
                emit sigSetToplevelCaption(w->windowTitle());
            }
            return QMdiArea::eventFilter(o, e);
            break;
        default:
            break;
    }

    return false; // Don't filter the event out
}

void CMDIArea::triggerWindowUpdate() {
    if (updatesEnabled()) {
        switch (m_mdiArrangementMode) {
            case ArrangementModeTileVertical:
                QTimer::singleShot(0, this, SLOT(myTileVertical()));
                break;
            case ArrangementModeTileHorizontal:
                QTimer::singleShot(0, this, SLOT(myTileHorizontal()));
                break;
            case ArrangementModeTile:
                QTimer::singleShot(0, this, SLOT(myTile()));
                break;
            case ArrangementModeCascade:
                QTimer::singleShot(0, this, SLOT(myCascade()));
                break;
            default:
                break;
        }
    }
}

void CMDIArea::enableWindowMinMaxFlags(bool enable)
{
    Q_FOREACH (QMdiSubWindow * const subWindow, subWindowList()) {
        Qt::WindowFlags flags = subWindow->windowFlags();
        if (enable) {
            flags |= (Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
        }
        else {
            flags &= ~(Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
        }
        subWindow->setWindowFlags(flags);
    }
}

