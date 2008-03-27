/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cmdiarea.h"

//QT includes
#include <QTimer>
#include <QEvent>
#include <QMdiSubWindow>
#include <QDebug>

CMDIArea::CMDIArea(QWidget *parent) : QMdiArea(parent),
	m_mdiArrangementMode(ArrangementModeManual)
{
	connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(slotClientActivated(QMdiSubWindow*)));
}

/** Called whan a client window was activated */
void CMDIArea::slotClientActivated(QMdiSubWindow* client) {
	if (!client || !updatesEnabled()) {
		return;
	}
	emit sigSetToplevelCaption( client->windowTitle().trimmed() );
}

QMdiSubWindow* CMDIArea::addSubWindow(QWidget * widget, Qt::WindowFlags windowFlags)
{
	qDebug() << "CMDIArea::addSubWindow called";
	QMdiSubWindow* subWindow = QMdiArea::addSubWindow(widget, windowFlags);
	subWindow->installEventFilter(this);
	if (m_mdiArrangementMode == ArrangementModeManual)
	{
		subWindow->resize(400, 400); //set the window to be big enough
	}
	else
	{
		triggerWindowUpdate();
	}
	return subWindow;
}

/** Reimplementation */
void CMDIArea::resizeEvent(QResizeEvent* e) {
	//do not call QMdiArea::resizeEvent(e), this would mess up our layout
	if (updatesEnabled()) triggerWindowUpdate();
}

void CMDIArea::deleteAll() {
	closeAllSubWindows();
}

void CMDIArea::setMDIArrangementMode( const MDIArrangementMode newArrangementMode ) {
	m_mdiArrangementMode = newArrangementMode;
	triggerWindowUpdate();
}

const CMDIArea::MDIArrangementMode CMDIArea::getMDIArrangementMode(void) {
	return m_mdiArrangementMode;
}

void CMDIArea::myTileVertical() {
	if (!updatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QList<QMdiSubWindow*> windows = usableWindowList();
	if ((windows.count() == 1) && windows.at(0)) {
		if (windows.at(0)->isMaximized()){
			if (windows.at(0)->size() != this->size()) {
				windows.at(0)->resize(this->size());
			}
		}
		else
		{
			windows.at(0)->showMaximized();
		}
	}
	else {
		QMdiSubWindow* active = activeSubWindow();
		QMdiArea::tileSubWindows();
		if (active) active->setFocus();
	}
	emitWindowCaptionChanged();
}

void CMDIArea::myTileHorizontal() {
	if (!updatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QList<QMdiSubWindow*> windows = usableWindowList();
	if ((windows.count() == 1) && windows.at(0)) {
		if (windows.at(0)->isMaximized()){
			if (windows.at(0)->size() != this->size()) {
				windows.at(0)->resize(this->size());
			}
		}
		else
		{
			windows.at(0)->showMaximized();
		}
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
	emitWindowCaptionChanged();
}

void CMDIArea::myCascade() {
	if (!updatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QList<QMdiSubWindow*> windows = usableWindowList();
	if ( !windows.count() ) {
		return;
	}

	if ((windows.count() == 1) && windows.at(0)) {
		if (windows.at(0)->isMaximized()){
			if (windows.at(0)->size() != this->size()) {
				windows.at(0)->resize(this->size());
			}
		}
		else
		{
			windows.at(0)->showMaximized();
		}
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
	emitWindowCaptionChanged();
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
	QMdiSubWindow* w = dynamic_cast<QMdiSubWindow*>( o );
	//if (w) qDebug() << "CMDIArea::eventFilter called for CMdiSubWindow, e is type" << (int)(e->type());
 	if (w && (e->type() == QEvent::WindowStateChange) ) {
 		if ( (w->windowState() & Qt::WindowMinimized) || w->isHidden() ) { //window was minimized, trigger a tile/cascade update if necessary
			triggerWindowUpdate();
		}
	}
 	if (w && (e->type() == QEvent::Close))
 	{
 		triggerWindowUpdate();
 	}
	return false; //let the event be handled by other filters too
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
			default:
				break;
		}
	}
}
