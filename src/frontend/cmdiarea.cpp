/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"
#include "frontend/cmdiarea.h"
#include "frontend/displaywindow/btmodulechooserbar.h"

#include <QEvent>
#include <QMdiSubWindow>
#include <QTimer>
#include <QToolBar>
#include <QWindowStateChangeEvent>
#include <QMenu>


CMDIArea::CMDIArea(BibleTime *parent)
        : QMdiArea(parent), m_mdiArrangementMode(ArrangementModeManual), m_activeWindow(0), m_bibleTime(parent) {
    Q_ASSERT(parent != 0);

    #if QT_VERSION >= 0x040500
    // Set document-style tabs (for Mac):
    setDocumentMode(true);
    #endif

    /*
      Activate windows based on the history of activation, e.g. when one has window A
      activated, and activates window B and then closes window B, then window A is activated.
    */
    setActivationOrder(QMdiArea::ActivationHistoryOrder);

    // Show scrollbars only when needed:
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(slotSubWindowActivated(QMdiSubWindow*)));
}

static const int moveSize = 30;

void CMDIArea::fixSystemMenu(QMdiSubWindow* subWindow) {
    // Change Qt QMdiSubWindow Close action to have no shortcuts
    // This makes our closeWindow actions with Ctrl-W work correctly
    QList<QAction*> actions = subWindow->systemMenu()->actions();
    for (int i=0; i<actions.count(); i++) {
        QAction* action = actions.at(i);
        QString text = action->text();
        if (text.contains("Close")) {
            action->setShortcuts(QList<QKeySequence>());
            break;
        }
    }
}

QMdiSubWindow* CMDIArea::addSubWindow(QWidget * widget, Qt::WindowFlags windowFlags) {
    QMdiSubWindow* subWindow = QMdiArea::addSubWindow(widget, windowFlags);
    subWindow->installEventFilter(this);
    fixSystemMenu(subWindow);

    // Manual arrangement mode
    enableWindowMinMaxFlags(true);
    if (m_mdiArrangementMode == ArrangementModeManual) {
        // Note that the window size/maximization may be changed later by a session restore.
        // If we already have an active window, make the new one simular to it
        if (activeSubWindow()) {
            if (activeSubWindow()->isMaximized()) {
                // Maximize the new window
                subWindow->showMaximized();
            }
            else {
                // Make new window the same size as the active window and move it slightly.
                subWindow->resize(activeSubWindow()->size());
                QRect subWinGeom = activeSubWindow()->geometry();
                subWinGeom.translate(moveSize, moveSize);
                // If it goes off screen, move it almost to the top left
                if ( ! frameRect().contains(subWinGeom)) {
                    subWinGeom.moveTo(moveSize, moveSize);
                }
                subWindow->setGeometry(subWinGeom);
            }
        }
        else {
            //set the window to be big enough
            subWindow->resize(400, 400);
        }
        subWindow->raise();
    }
    else {
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
            break;
        default:
            setViewMode(QMdiArea::SubWindowView);
            triggerWindowUpdate();
            break;
    }
}

void CMDIArea::myTileVertical() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);

    QList<QMdiSubWindow*> windows = usableWindowList();
    setUpdatesEnabled(false);
    QMdiSubWindow* active = activeSubWindow();

    const int widthForEach = width() / windows.count();
    unsigned int x = 0;
    foreach (QMdiSubWindow *window, windows) {
        window->showNormal();

        const int preferredWidth = window->minimumWidth() + window->baseSize().width();
        const int actWidth = qMax(widthForEach, preferredWidth);

        window->setGeometry(x, 0, actWidth, height());
        x += actWidth;
    }

    if (active) active->setFocus();
    setUpdatesEnabled(true);
emitWindowCaptionChanged();
}

void CMDIArea::myTileHorizontal() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);

    QList<QMdiSubWindow*> windows = usableWindowList();
    setUpdatesEnabled(false);
    QMdiSubWindow* active = activeSubWindow();

    const int heightForEach = height() / windows.count();
    unsigned int y = 0;
    foreach (QMdiSubWindow *window, windows) {
        window->showNormal();

        const int preferredHeight = window->minimumHeight() + window->baseSize().height();
        const int actHeight = qMax(heightForEach, preferredHeight);

        window->setGeometry( 0, y, width(), actHeight );
        y += actHeight;
    }
    if (active) active->setFocus();
    setUpdatesEnabled(true);
    emitWindowCaptionChanged();
}

// Tile the windows, tiling implemented by Qt
void CMDIArea::myTile() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);
    tileSubWindows();
    emitWindowCaptionChanged();
}

void CMDIArea::myCascade() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }
    setViewMode(QMdiArea::SubWindowView);

    QList<QMdiSubWindow*> windows = usableWindowList();

    if (windows.count() == 1) {
        windows.at(0)->showMaximized();
    }
    else {
        setUpdatesEnabled(false);

        QMdiSubWindow* active = activeSubWindow();

        const unsigned int offsetX = 40;
        const unsigned int offsetY = 40;
        const unsigned int windowWidth =  width() - (windows.count() - 1) * offsetX;
        const unsigned int windowHeight = height() - (windows.count() - 1) * offsetY;
        unsigned int x = 0;
        unsigned int y = 0;

        foreach (QMdiSubWindow* window, windows) {
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

QList<QMdiSubWindow*> CMDIArea::usableWindowList() {
    //Take care: when new windows are added, they will not appear
    //in subWindowList() when their ChildAdded-Event is triggered
    QList<QMdiSubWindow*> ret;
    foreach(QMdiSubWindow* w, subWindowList()) {
        if (w->isHidden()) { //not usable for us
            continue;
        }
        ret.append( w );
    }
    return ret;
}

void CMDIArea::slotSubWindowActivated(QMdiSubWindow* client) {
    if (subWindowList().count() == 0)
        m_bibleTime->clearMdiToolBars();

    if (!client) {
        return;
    }
    emit sigSetToplevelCaption( client->windowTitle().trimmed() );

    // Notify child window it is active
    CDisplayWindow* activeWindow = qobject_cast<CDisplayWindow*>(client->widget());
    if (activeWindow != 0 && activeWindow != m_activeWindow) {
        m_activeWindow = activeWindow;
        activeWindow->windowActivated();
    }
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
    QMdiSubWindow *w(qobject_cast<QMdiSubWindow*>(o));

    // Let the event be handled by other filters:
    if (w == 0)
        return QMdiArea::eventFilter(o, e);

    switch (e->type()) {
        case QEvent::WindowStateChange: {
            Qt::WindowStates newState(w->windowState());
            Qt::WindowStates oldState(((QWindowStateChangeEvent*)e)->oldState());

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
    foreach(QMdiSubWindow* subWindow, subWindowList()) {
        Qt::WindowFlags flags = subWindow->windowFlags();
        if (enable) {
            flags |= Qt::WindowMinimizeButtonHint;
            flags |= Qt::WindowMaximizeButtonHint;
        }
        else {
            flags &= ~Qt::WindowMinimizeButtonHint;
            flags &= ~Qt::WindowMaximizeButtonHint;
        }
        subWindow->setWindowFlags(flags);
    }
}

