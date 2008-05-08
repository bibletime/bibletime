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
#include <QWindowStateChangeEvent>
#include <QMdiSubWindow>
#include <QDebug>

CMDIArea::CMDIArea(QWidget *parent) : QMdiArea(parent),
	m_mdiArrangementMode(ArrangementModeManual)
{
	setActivationOrder( CreationOrder ); //keep window order consistent
	connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(slotClientActivated(QMdiSubWindow*)));
}

QMdiSubWindow* CMDIArea::addSubWindow(QWidget * widget, Qt::WindowFlags windowFlags)
{
	QMdiSubWindow* subWindow = QMdiArea::addSubWindow(widget, windowFlags);
	subWindow->installEventFilter(this);
	
	//If we do have a maximized Window, set it to normal so that the new window can be seen
	if (activeSubWindow() && activeSubWindow()->isMaximized()){
		activeSubWindow()->showNormal();
	}
	
	if (m_mdiArrangementMode == ArrangementModeManual)
	{
		subWindow->resize(400, 400); //set the window to be big enough
		subWindow->raise();
	}
	else
	{
		triggerWindowUpdate();
	}
	return subWindow;
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
	if (activeSubWindow() && activeSubWindow()->isMaximized()){
		if (activeSubWindow()->size() != this->size()) {
			activeSubWindow()->resize(this->size());
		}
	}
	else if (windows.count() == 1) {
		windows.at(0)->showMaximized();
	}
	else {
		setUpdatesEnabled(false);
		QMdiSubWindow* active = activeSubWindow();
		QMdiArea::tileSubWindows();
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

	if (activeSubWindow() && activeSubWindow()->isMaximized()){
		if (activeSubWindow()->size() != this->size()) {
			activeSubWindow()->resize(this->size());
		}
	}
	else if (windows.count() == 1) {
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

	if (activeSubWindow() && activeSubWindow()->isMaximized()){
		if (activeSubWindow()->size() != this->size()) {
			activeSubWindow()->resize(this->size());
		}
	}
	else if (windows.count() == 1) {
		windows.at(0)->showMaximized();
	}
	else {
		setUpdatesEnabled(false);

		QMdiSubWindow* active = activeSubWindow();

		const unsigned int offsetX = 40;
		const unsigned int offsetY = 40;
		const unsigned int windowWidth =  width() - (windows.count()-1)*offsetX;
		const unsigned int windowHeight = height() - (windows.count()-1)*offsetY;
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

void CMDIArea::slotClientActivated(QMdiSubWindow* client) {
	if (!client || !updatesEnabled()) {
		return;
	}
	emit sigSetToplevelCaption( client->windowTitle().trimmed() );
}

//resize event of the MDI area itself, update layout if necessary
void CMDIArea::resizeEvent(QResizeEvent* /*e*/) {
	//do not call QMdiArea::resizeEvent(e), this would mess up our layout
	if (updatesEnabled()) triggerWindowUpdate();
}

//handle events of the client windows to update layout if necessary
bool CMDIArea::eventFilter(QObject *o, QEvent *e) {
	QMdiSubWindow* w = dynamic_cast<QMdiSubWindow*>(o);
	if (!w) return false; //let the event be handled by other filters too
	
 	if (e->type() == QEvent::WindowStateChange) {
 		Qt::WindowStates newState =  w->windowState();
 		Qt::WindowStates oldState = ((QWindowStateChangeEvent*)e)->oldState();
 		
 		//Do not handle window activation or deactivation here, it will produce wrong
 		//results because this event is handled too early. Let slotClientActivated() handle this.
 		
 		bool needsLayoutUpdate = false;
 		//Window was maximized or un-maximized
 		if ((newState ^ oldState) & Qt::WindowMaximized) needsLayoutUpdate = true;
 		//Window was minimized or de-minimized
 		if ((newState ^ oldState) & Qt::WindowMinimized) needsLayoutUpdate = true;
 		//update Layout?
		if (needsLayoutUpdate) triggerWindowUpdate();
 	}
 	if (e->type() == QEvent::Close) {
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
