/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime.h"
#include "frontend/cmdiarea.h"

#include <QEvent>
#include <QMdiSubWindow>
#include <QTimer>
#include <QWindowStateChangeEvent>
#include <QMenu>


CMDIArea::CMDIArea(BibleTime *parent)
        : QMdiArea(parent), m_mdiArrangementMode(ArrangementModeManual) {
    Q_ASSERT(parent != 0);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(slotSubWindowActivated(QMdiSubWindow*)));
}

static const int moveSize = 30;

QMdiSubWindow* CMDIArea::addSubWindow(QWidget * widget, Qt::WindowFlags windowFlags) {
    QMdiSubWindow* subWindow = QMdiArea::addSubWindow(widget, windowFlags);
    subWindow->installEventFilter(this);

    // Change Qt QMdiSubWindow Close action to have no shortcut
    // This makes our closeWindow actions with Ctrl-W work correctly
    QList<QAction*> actions = subWindow->systemMenu()->actions();
    for (int i=0; i<actions.count(); i++) {
        QAction* action = actions.at(i);
        QString text = action->text();
        if (text.contains("Close")) {
            action->setShortcut(QKeySequence());
            break;
        }
    }

    // Manual arrangement mode
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
        triggerWindowUpdate();
    }
    return subWindow;
}

void CMDIArea::setMDIArrangementMode( const MDIArrangementMode newArrangementMode ) {
    m_mdiArrangementMode = newArrangementMode;
    triggerWindowUpdate();
}

void CMDIArea::myTileVertical() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }

    QList<QMdiSubWindow*> windows = usableWindowList();
    if (windows.count() == 1) {
        windows.at(0)->showMaximized();
    }
    else {
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
    }
    emitWindowCaptionChanged();
}

void CMDIArea::myTileHorizontal() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }

    QList<QMdiSubWindow*> windows = usableWindowList();

    if (windows.count() == 1) {
        windows.at(0)->showMaximized();
    }
    else {
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
    }
    emitWindowCaptionChanged();
}

void CMDIArea::myTile() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }
    tileSubWindows();
    emitWindowCaptionChanged();
}

void CMDIArea::myCascade() {
    if (!updatesEnabled() || !usableWindowList().count() ) {
        return;
    }

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
            window->raise(); //make it the on-top-of-window-stack window to make sure they're in the right order
            window->setGeometry(x, y, windowWidth, windowHeight);
            x += offsetX;
            y += offsetY;
        }
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
        emit sigSetToplevelCaption(QString());
    }
}

QList<QMdiSubWindow*> CMDIArea::usableWindowList() {
    //Take care: when new windows are added, they will not appear
    //in subWindowList() when their ChildAdded-Event is triggered
    QList<QMdiSubWindow*> ret;
    foreach(QMdiSubWindow* w, subWindowList()) {
        if (w->isMinimized() || w->isHidden()) { //not usable for us
            continue;
        }
        ret.append( w );
    }
    return ret;
}

void CMDIArea::slotSubWindowActivated(QMdiSubWindow* client) {
    if (!client || !updatesEnabled()) {
        return;
    }
    emit sigSetToplevelCaption( client->windowTitle().trimmed() );
}

void CMDIArea::resizeEvent(QResizeEvent* e) {
    /*
      Do not call QMdiArea::resizeEvent(e) unless we are in manual arrangement
      mode, since this would mess up our layout. The manual arrangement mode
      should be handled exactly as in QMdiArea.
    */
    if (m_mdiArrangementMode == ArrangementModeManual) {
        QMdiArea::resizeEvent(e);
    }
    else if (updatesEnabled()) {
        triggerWindowUpdate();
    }
}

//handle events of the client windows to update layout if necessary
bool CMDIArea::eventFilter(QObject *o, QEvent *e) {
    QMdiSubWindow *w(qobject_cast<QMdiSubWindow*>(o));

    // Let the event be handled by other filters:
    if (w == 0) return QMdiArea::eventFilter(o, e);

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
            case ArrangementModeCascade:
                QTimer::singleShot(0, this, SLOT(myCascade()));
                break;
            case ArrangementModeTile:
                QTimer::singleShot(0, this, SLOT(myTile()));
                break;
            default:
                break;
        }
    }
}
