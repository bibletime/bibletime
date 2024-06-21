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

#include "cmdiarea.h"

#include <QAction>
#include <QEvent>
#include <QFlags>
#include <QKeySequence>
#include <QMdiSubWindow>
#include <QMenu>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QStyle>
#include <QTimer>
#include <QWidget>
#include <QWindowStateChangeEvent>
#include "../util/btassert.h"
#include "../util/btconnect.h"
#include "bibletime.h"
#include "displaywindow/cdisplaywindow.h"
#include "display/btmodelviewreaddisplay.h"


namespace {

inline CDisplayWindow * getDisplayWindow(const QMdiSubWindow * const mdiWindow) {
    return qobject_cast<CDisplayWindow *>(mdiWindow->widget());
}

} // anonymous namespace


CMDIArea::CMDIArea(BibleTime * parent)
        : QMdiArea((BT_ASSERT(parent), parent))
        , m_mdiArrangementMode(ArrangementModeManual)
        , m_activeWindow(nullptr)
        , m_bibleTime(parent)
{
    setFocusPolicy(Qt::ClickFocus);

    setDocumentMode(true); // Document-style tabs for Mac
    setTabsClosable(true);
    setTabsMovable(true);

    /*
      Activate windows based on the history of activation, e.g. when one has window A
      activated, and activates window B and then closes window B, then window A is activated.
    */
    setActivationOrder(QMdiArea::ActivationHistoryOrder);

    // Show scrollbars only when needed:
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    BT_CONNECT(this, &QMdiArea::subWindowActivated,
               [this](QMdiSubWindow* client) {
                   if (subWindowList().isEmpty())
                       m_bibleTime->clearMdiToolBars();

                   if (client == nullptr) {
                       return;
                   }
                   Q_EMIT sigSetToplevelCaption( client->windowTitle().trimmed() );

                   // Notify child window it is active
                   CDisplayWindow* const activeWindow = getDisplayWindow(client);
                   if (activeWindow != nullptr && activeWindow != m_activeWindow) {
                       m_activeWindow = activeWindow;
                       activeWindow->windowActivated();
                   }
               });
}

QMdiSubWindow * CMDIArea::addDisplayWindow(CDisplayWindow * const displayWindow)
{
    QMdiSubWindow * const subWindow = addSubWindow(displayWindow);
    subWindow->setWindowIcon(displayWindow->windowIcon());
    BT_CONNECT(displayWindow, &CDisplayWindow::windowIconChanged,
               subWindow,     &QMdiSubWindow::setWindowIcon);

    // Change Qt QMdiSubWindow Close action to have no shortcuts
    // This makes our closeWindow actions with Ctrl-W work correctly
    for (auto * const action : subWindow->systemMenu()->actions()) {
        if (action->text().contains(QStringLiteral("Close"))) {
            action->setShortcuts(QList<QKeySequence>());
            break;
        }
    }

    // Manual arrangement mode
    if (m_mdiArrangementMode == ArrangementModeManual) {
        enableWindowMinMaxFlags(true);

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
            myTile();
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
    for (auto * const window : windows) {
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
    for (auto * const window : windows) {
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

// Tile the windows
void CMDIArea::myTile() {
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

    const QRect domain = contentsRect();
    const int n = windows.size();
    const int ncols = qMax(qCeil(qSqrt(qreal(n))), 1);
    const int nrows = qMax((n % ncols) ? (n / ncols + 1) : (n / ncols), 1);
    const int nspecial = (n % ncols) ? (ncols - n % ncols) : 0;
    const int dx = domain.width()  / ncols;
    const int dy = domain.height() / nrows;

    int i = 0;
    for (int row = 0; row < nrows; ++row) {
        const int y1 = int(row * (dy + 1));
        for (int col = 0; col < ncols; ++col) {
            if (row == 1 && col < nspecial)
                continue;
            const int x1 = int(col * (dx + 1));
            int x2 = int(x1 + dx);
            int y2 = int(y1 + dy);
            if (row == 0 && col < nspecial) {
                y2 *= 2;
                if (nrows != 2)
                    y2 += 1;
                else
                    y2 = domain.bottom();
            }
            if (col == ncols - 1 && x2 != domain.right())
                x2 = domain.right();
            if (row == nrows - 1 && y2 != domain.bottom())
                y2 = domain.bottom();
            QWidget *widget = windows.at(i++);
            QRect newGeometry = QRect(QPoint(x1, y1), QPoint(x2, y2));
            widget->setGeometry(QStyle::visualRect(widget->layoutDirection(), domain, newGeometry));
        }
    }

    if (active != nullptr) {
        active->setFocus();
    }

    setUpdatesEnabled(true);
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

        for (auto * const window : windows) {
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
        Q_EMIT sigSetToplevelCaption(activeSubWindow()->windowTitle());
    }
    else {
        Q_EMIT sigSetToplevelCaption(QString());
    }
}

QList<QMdiSubWindow*> CMDIArea::usableWindowList() const {
    //Take care: when new windows are added, they will not appear
    //in subWindowList() when their ChildAdded-Event is triggered
    QList<QMdiSubWindow*> ret;
    for (auto * const w : subWindowList())
        if (!w->isHidden())
            ret.append(w);
    return ret;
}

void CMDIArea::findNextTextInActiveWindow() { findTextInActiveWindow(false); }

void CMDIArea::findPreviousTextInActiveWindow() { findTextInActiveWindow(true);}

void CMDIArea::highlightTextInActiveWindow(const QString& text, bool caseSensitive) {
    CDisplayWindow* const displayWindow = getDisplayWindow(activeSubWindow());
    displayWindow->displayWidget()->highlightText(text, caseSensitive);
}

void CMDIArea::findTextInActiveWindow(bool const backward)
{ getDisplayWindow(activeSubWindow())->displayWidget()->findText(backward); }

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
              lambda connected to subWindowActivated() handle this.
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
                Q_EMIT sigSetToplevelCaption(w->windowTitle());
            }
            break;
        default:
            break;
    }

    return QMdiArea::eventFilter(o, e);
}

void CMDIArea::triggerWindowUpdate() {
    if (updatesEnabled()) {
        switch (m_mdiArrangementMode) {
            case ArrangementModeTileVertical:
                QTimer::singleShot(0, this, &CMDIArea::myTileVertical);
                break;
            case ArrangementModeTileHorizontal:
                QTimer::singleShot(0, this, &CMDIArea::myTileHorizontal);
                break;
            case ArrangementModeTile:
                QTimer::singleShot(0, this, &CMDIArea::myTile);
                break;
            case ArrangementModeCascade:
                QTimer::singleShot(0, this, &CMDIArea::myCascade);
                break;
            default:
                break;
        }
    }
}

void CMDIArea::enableWindowMinMaxFlags(bool enable)
{
    for (auto * const subWindow : subWindowList()) {
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

