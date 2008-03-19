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
#include <QDebug>

CMDIArea::CMDIArea(QWidget *parent) : QMdiArea(parent),
	m_guiOption(Nothing)
{
	connect(this, SIGNAL(windowActivated(QWidget*)), this, SLOT(slotClientActivated(QWidget*)));
}

/** Called whan a client window was activated */
void CMDIArea::slotClientActivated(QWidget* client) {
	if (!client || !updatesEnabled()) {
		return;
	}

	CDisplayWindow* sp = dynamic_cast<CDisplayWindow*>(client);
	Q_ASSERT(sp);
	if (!sp || !sp->isReady()) return;

	foreach(QMdiSubWindow* w, subWindowList()){
		CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(w);
		window->windowActivated( (window == sp) ? true : false);
	}

	emit sigSetToplevelCaption( client->windowTitle().trimmed() );
}

/** Reimplementation. Used to make use of the fixedGUIOption part. */
void CMDIArea::childEvent( QChildEvent * e ) {
	qDebug() << "CMDIArea::childEvent type" << int(e->type());
	if (!e) return;

	if (!subWindowList().count()) {
		emit sigSetToplevelCaption(QString::null);
	}

	if ( (e->added() || e->removed()) ) {
		if (e->added() && e->child() && e->child()->inherits("CDisplayWindow")) {
			e->child()->installEventFilter(this); //make sure we catch the events of the new window
		}
		else if (e->removed() && e->child() && e->child()->inherits("CDisplayWindow")) {
			e->child()->removeEventFilter(this); //make sure we catch the events of the new window
		}
		triggerWindowUpdate();
	}
	QMdiArea::childEvent(e);
}

/** Reimplementation */
void CMDIArea::resizeEvent(QResizeEvent* e) {
	QMdiArea::resizeEvent(e);
	if (updatesEnabled()) triggerWindowUpdate();
}

/** Deletes all the presenters in the MDI area. */
void CMDIArea::deleteAll() {
	closeAllSubWindows();
}

/** Enable / disable autoCascading */
void CMDIArea::setGUIOption( const MDIOption& newOption ) {
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
		if (active && active->isMaximized()) active->showNormal();

		setUpdatesEnabled(false);
		const int heightForEach = height() / windows.count();
		int y = 0;
		for ( int i = 0; i < int(windows.count()); ++i ) {
			QMdiSubWindow *window = windows.at(i);
			window->showNormal();

			const int preferredHeight = window->minimumHeight() + window->baseSize().height();
			const int actHeight = qMax(heightForEach, preferredHeight);

			window->setGeometry( 0, y, width(), actHeight );
			y += actHeight;
		}
		active->setFocus();
		setUpdatesEnabled(true);
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
		if (active && active->isMaximized()) active->showNormal();

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
		active->setGeometry(x, y, windowWidth, windowHeight);
		active->raise();
		active->activateWindow();

		setUpdatesEnabled(true);
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
	//Take care: when new windows are added, they will not appear 
	//in subWindowList() when their ChildAdded-Event is triggered
	QList<QMdiSubWindow*> ret;
	foreach(QMdiSubWindow* w, subWindowList())
	{
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
		}
	}
	return QMdiArea::eventFilter(o,e); // standard event processing
}

void CMDIArea::triggerWindowUpdate() {
	if (updatesEnabled()) {
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
