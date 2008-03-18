/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cmdiarea.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/keychooser/ckeychooser.h"

//QT includes
#include <QObject>
#include <QTimer>
#include <QEvent>
#include <QMdiSubWindow>

CMDIArea::CMDIArea(QWidget *parent) : QMdiArea(parent),
	m_guiOption(Nothing),
	m_childEvent(false)
{
	initConnections();
	readSettings();
}

/** Initilizes the connectiosn to SIGNALS */
void CMDIArea::initConnections() {
	connect(this, SIGNAL(windowActivated(QWidget*)), this, SLOT(slotClientActivated(QWidget*)));
}

/** Called whan a client window was activated */
void CMDIArea::slotClientActivated(QWidget* client) {
	if (!client || !updatesEnabled()) {
		return;
	}

	CDisplayWindow* sp = dynamic_cast<CDisplayWindow*>(client);
	Q_ASSERT(sp);
	if (!sp || !sp->isReady()) {
		return;
	}

	//QWidgetList windows = windowList();
	QListIterator<QMdiSubWindow*> it(subWindowList());
	//for ( QWidget* w = windows.first(); w; w = windows.next() ) {
	while (it.hasNext()) {
		//Don't use!! It would disable accel enabling for the active window, see CDisplayWindow::windowActivated
		/*    if (w == client)
		    continue;
		*/

		CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(it.next());
		window->windowActivated( (window == sp) ? true : false);
	}

	emit sigSetToplevelCaption( client->windowTitle().trimmed() );
}

/** Reimplementation. Used to make use of the fixedGUIOption part. */
void CMDIArea::childEvent( QChildEvent * e ) {
	//qDebug("CMDIArea::childEvent");
	//qDebug() << "type" << int(e->type());

	if ( m_childEvent || !e) {
		return;
	}

	m_childEvent = true;

	if (!subWindowList().count()) {
		emit sigSetToplevelCaption(QString::null);
		emit sigLastPresenterClosed();
	}

	if ( (e->added() || e->removed() /*|| (int(e->type()) == 70 )*/) ) {
		if (e->added() && e->child() && e->child()->inherits("CDisplayWindow")) {
			//qDebug("added a displaywindow");
			e->child()->installEventFilter(this); //make sure we catch the events of the new window
		}
		else if (e->removed() && e->child() && e->child()->inherits("CDisplayWindow")) {
			//qDebug("removed a displaywindow");
			e->child()->removeEventFilter(this); //make sure we catch the events of the new window
		}

		triggerWindowUpdate();
	}
	QMdiArea::childEvent(e);
	m_childEvent = false;
}

/** Reimplementation */
void CMDIArea::resizeEvent(QResizeEvent* e) {
	QMdiArea::resizeEvent(e);

	if (updatesEnabled()) {
		triggerWindowUpdate();
	};
}

/**  */
void CMDIArea::saveSettings() {}

/**  */
void CMDIArea::readSettings() {}

/** Deletes all the presenters in the MDI area. */
void CMDIArea::deleteAll() {
	closeAllSubWindows();
}

/** Enable / disable autoCascading */
void CMDIArea::setGUIOption( const MDIOption& newOption ) {
	//now do the initial action
	m_guiOption = newOption;

	triggerWindowUpdate();
}

/**  */
void CMDIArea::myTileVertical() {
	if (!updatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QList<QMdiSubWindow*> windows = usableWindowList();

	if ((windows.count() == 1) && windows.at(0)) {
		//qDebug("should show maximized");
		windows.at(0)->showMaximized();
	}
	else {
		QMdiSubWindow* active = activeSubWindow();
		QMdiArea::tileSubWindows();
		if (active) active->setFocus();
	}
}

void CMDIArea::myTileHorizontal() {
	if (!updatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QList<QMdiSubWindow*> windows = usableWindowList();

	if ((windows.count() == 1) && windows.at(0)) {
		windows.at(0)->showMaximized();
	}
	else {
		QMdiSubWindow* active = activeSubWindow();
		if (active && active->isMaximized()) {
			active->showNormal();
		}

		//blockSignals(true);
		setUpdatesEnabled(false);
		int heightForEach = height() / windows.count();
		int y = 0;
		for ( int i = 0; i < int(windows.count()); ++i ) {
			QMdiSubWindow *window = windows.at(i);
			window->showNormal();

			const int preferredHeight = window->minimumHeight() + window->baseSize().height();
			const int actHeight = qMax(heightForEach, preferredHeight);

			window->setGeometry( 0, y, width(), actHeight );
			y += actHeight;
		}

		setUpdatesEnabled(true);
		active->setFocus();
		//blockSignals(false);
	}

}

/**  */
void CMDIArea::myCascade() {
	if (!updatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QList<QMdiSubWindow*> windows = usableWindowList();
	if ( !windows.count() ) {
		return;
	}

	if ((windows.count() == 1) && windows.at(0)) {
		windows.at(0)->showMaximized();
	}
	else {
		const int offsetX = 40;
		const int offsetY = 40;
		const int windowWidth =  width() - (windows.count()-1)*offsetX;
		const int windowHeight = height() - (windows.count()-1)*offsetY;

		int x = 0;
		int y = 0;

		QMdiSubWindow* const active = activeSubWindow();
		if (active && active->isMaximized()) {
			active->showNormal();
		}

		blockSignals(true);
		setUpdatesEnabled(false);

		for (int i(0); i < int(windows.count()); ++i) {
			QMdiSubWindow* window = windows.at(i);
			if (window == active) { //leave out the active window which should be the top window
				continue;
			}

			window->setUpdatesEnabled(false);

			window->raise(); //make it the on-top-of-window-stack window to make sure they're in the right order
			window->setGeometry(x, y, windowWidth, windowHeight);
			x += offsetX;
			y += offsetY;

			window->setUpdatesEnabled(true);
		}

		setUpdatesEnabled(true);

		active->setGeometry(x, y, windowWidth, windowHeight);
		active->raise();
		active->activateWindow();

		blockSignals(false);
	}
}

void CMDIArea::emitWindowCaptionChanged() {
	QString appCaption;
	if (activeSubWindow()) {
		appCaption = activeSubWindow()->windowTitle();
	}
	emit sigSetToplevelCaption(appCaption);
}

QList<QMdiSubWindow*> CMDIArea::usableWindowList() {
	QList<QMdiSubWindow*> ret;

	//QWidgetList windows = windowList();
	//for ( QWidget* w = windows.first(); w; w = windows.next() ) {
	QListIterator<QMdiSubWindow*> it(subWindowList());
	while (it.hasNext()) {	
		QMdiSubWindow* w = it.next();
		if (w->isMinimized() || w->isHidden()) { //not usable for us
			continue;
		}

		ret.append( w );
	}

	return ret;
}

bool CMDIArea::eventFilter( QObject *o, QEvent *e ) {
	Q_ASSERT(o);
	Q_ASSERT(e);
	
	CDisplayWindow* w = dynamic_cast<CDisplayWindow*>( o );
 	if ( w && (e->type() == QEvent::WindowStateChange) ) {
 		if ( (w->windowState() & Qt::WindowMinimized) || w->isHidden() ) { //window was minimized, trigger a tile/cascade update if necessary
			triggerWindowUpdate();
			//ret = false;
		}
	}

	return QMdiArea::eventFilter(o,e); // standard event processing
}


/*!
    \fn CMDIArea::triggerWindowUpdate()
 */
void CMDIArea::triggerWindowUpdate() {
	//qDebug("CMDIArea::triggerWindowUpdate");

	if (updatesEnabled() && usableWindowList().count() ) {
		switch (m_guiOption) {
			case autoTileVertical:
			QTimer::singleShot(0, this, SLOT(myTileVertical()));
			break;
			case autoTileHorizontal:
			QTimer::singleShot(0, this, SLOT(myTileHorizontal()));
			break;
			case autoCascade:
			QTimer::singleShot(0, this, SLOT(myCascade()));
			break;
			default:
			qDebug("CMDIArea::triggerWindowUpdate: no known m_guiType");
			break;
		}
	}
}
