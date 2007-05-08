/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cmdiarea.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordversekey.h"

#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/keychooser/ckeychooser.h"

#include "config.h"

//KDE includes
#include <kpopupmenu.h>
#include <kapplication.h>
#include <klocale.h>

//QT includes
#include <qobjectlist.h>
#include <qtimer.h>
#include <qevent.h>
#include <qtimer.h>
#if QT_VERSION < 0x030200
//We need this to close all windows with Qt < 3.2
#include <qwidgetlist.h>
#endif


CMDIArea::CMDIArea(QWidget *parent, const char *name )
: QWorkspace(parent, name),
m_guiOption(Nothing),
m_childEvent(false),
m_appCaption(QString::null) {
	initView();
	initConnections();
	readSettings();
}


/** Initializes the view of the MDI area */
void CMDIArea::initView() {
	setPaletteBackgroundColor( parentWidget()->paletteBackgroundColor() ); //work around a KDE bug (IMHO was in KDE 2.x)
}

/** Initilizes the connectiosn to SIGNALS */
void CMDIArea::initConnections() {
	connect(this, SIGNAL(windowActivated(QWidget*)),
			this, SLOT(slotClientActivated(QWidget*)));
}

/** Called whan a client window was activated */
void CMDIArea::slotClientActivated(QWidget* client) {
	if (!client || !isUpdatesEnabled()) {
		return;
	}

	CDisplayWindow* sp = dynamic_cast<CDisplayWindow*>(client);
	Q_ASSERT(sp);
	if (!sp || !sp->isReady()) {
		return;
	}

	QWidgetList windows = windowList();
	for ( QWidget* w = windows.first(); w; w = windows.next() ) {
		//Don't use!! It would disable accel enabling for the active window, see CDisplayWindow::windowActivated
		/*    if (w == client)
		    continue;
		*/

		CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(w);
		window->windowActivated( (window == sp) ? true : false);
	}

	emit sigSetToplevelCaption( ( m_appCaption = client->caption().stripWhiteSpace() ) );
}

/** Reimplementation. Used to make use of the fixedGUIOption part. */
void CMDIArea::childEvent( QChildEvent * e ) {
	QWorkspace::childEvent(e);

	if ( m_childEvent || !e) {
		return;
	}

	m_childEvent = true;

	if (!windowList().count()) {
		m_appCaption = QString::null;
		emit sigSetToplevelCaption( KApplication::kApplication()->makeStdCaption(m_appCaption) );
		emit sigLastPresenterClosed();
	}

	if ((e->inserted() || e->removed()) ) {
		if (e->inserted() && e->child() && e->child()->inherits("CDisplayWindow")) {
			e->child()->installEventFilter(this); //make sure we catch the events of the new window
		}
		else if (e->removed() && e->child() && e->child()->inherits("CDisplayWindow")) {
			e->child()->removeEventFilter(this); //make sure we catch the events of the new window
		}

		triggerWindowUpdate();
	}

	m_childEvent = false;
}

/** Reimplementation */
void CMDIArea::resizeEvent(QResizeEvent* e) {
	QWorkspace::resizeEvent(e);

	if (isUpdatesEnabled()) {
		triggerWindowUpdate();
	};
}

/**  */
void CMDIArea::saveSettings() {}

/**  */
void CMDIArea::readSettings() {}

/** Deletes all the presenters in the MDI area. */
void CMDIArea::deleteAll() {
#if QT_VERSION >= 0x030200
	closeAllWindows();
#else
	QWidgetListIt it(windowList());
	while (it.current() != 0){
		it.current()->close();
		++it;
	}
#endif
}

/** Enable / disable autoCascading */
void CMDIArea::setGUIOption( const MDIOption& newOption ) {
	//now do the initial action
	m_guiOption = newOption;

	triggerWindowUpdate();
}

/**  */
void CMDIArea::myTileVertical() {
	if (!isUpdatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QPtrList<QWidget> windows = usableWindowList();

	if ((windows.count() == 1) && windows.at(0)) {
		m_appCaption = windows.at(0)->caption();
		windows.at(0)->showMaximized();
	}
	else {
		QWidget* active = activeWindow();
		QWorkspace::tile();
		active->setFocus();
	}
}

void CMDIArea::myTileHorizontal() {
	if (!isUpdatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QPtrList<QWidget> windows = usableWindowList();

	if ((windows.count() == 1) && windows.at(0)) {
		m_appCaption = windows.at(0)->caption();
		windows.at(0)/*->parentWidget()*/->showMaximized();
	}
	else {

		QWidget* active = activeWindow();
		if (active->isMaximized()) {
			active->showNormal();
		}

		blockSignals(true);
		setUpdatesEnabled(false);
		int heightForEach = height() / windows.count();
		int y = 0;
		for ( int i = 0; i < int(windows.count()); ++i ) {
			QWidget *window = windows.at(i);
			window->parentWidget()->showNormal();
			qApp->sendPostedEvents( 0, QEvent::ShowNormal );

			const int preferredHeight = window->minimumHeight() + window->parentWidget()->baseSize().height();
			const int actHeight = QMAX(heightForEach, preferredHeight);

			window->parentWidget()->setGeometry( 0, y, width(), actHeight );
			y += actHeight;
		}

		setUpdatesEnabled(true);
		active->setFocus();
		blockSignals(false);
	}

}

/**  */
void CMDIArea::myCascade() {
	if (!isUpdatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QPtrList<QWidget> windows = usableWindowList();
	if ( !windows.count() ) {
		return;
	}

	if ((windows.count() == 1) && windows.at(0)) {
		m_appCaption = windows.at(0)->caption();
		windows.at(0)->parentWidget()->showMaximized();
	}
	else {
		const int offsetX = 40;
		const int offsetY = 40;
		const int windowWidth =  width() - (windows.count()-1)*offsetX;
		const int windowHeight = height() - (windows.count()-1)*offsetY;

		int x = 0;
		int y = 0;

		QWidget* const active = activeWindow();
		if (active->isMaximized()) {
			active->showNormal();
		}

		blockSignals(true);
		setUpdatesEnabled(false);

		for (int i(0); i < int(windows.count()); ++i) {
			QWidget* window = windows.at(i);
			if (window == active) { //leave out the active window which should be the top window
				continue;
			}

			window->setUpdatesEnabled(false);

			window->parentWidget()->raise(); //make it the on-top-of-window-stack window to make sure they're in the right order
			window->parentWidget()->setGeometry(x, y, windowWidth, windowHeight);
			x += offsetX;
			y += offsetY;

			window->setUpdatesEnabled(true);
		}

		setUpdatesEnabled(true);

		active->parentWidget()->setGeometry(x, y, windowWidth, windowHeight);
		active->parentWidget()->raise();
		active->setActiveWindow();

		blockSignals(false);
	}
}

/*!
    \fn CMDIArea::emitWindowCaptionChanged()
 */
void CMDIArea::emitWindowCaptionChanged() {
	if (activeWindow()) {
		m_appCaption = activeWindow()->caption();
	}

	emit sigSetToplevelCaption(currentApplicationCaption());
}


/*!
    \fn CMDIArea::usableWindowsCount()
 */
QPtrList<QWidget> CMDIArea::usableWindowList() {
	QPtrList<QWidget> ret;

	QWidgetList windows = windowList();
	for ( QWidget* w = windows.first(); w; w = windows.next() ) {
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

	QWidget* w = dynamic_cast<QWidget*>( o );
	bool ret = QWorkspace::eventFilter(o,e);

#if QT_VERSION >= 0x030300
 	if ( w && (e->type() == QEvent::WindowStateChange) ) {
 		if (o->inherits("CDisplayWindow") && ((w->windowState() & Qt::WindowMinimized) || w->isHidden())) { //window was minimized, trigger a tile/cascade update if necessary
			triggerWindowUpdate();
			ret = false;
		}
		else if (!o->inherits("CDisplayWindow")){
			qDebug("CMDIArea: bad mdi child classname: %s", o->className());
			o->dumpObjectInfo();
			o->dumpObjectTree();
		}
	}
#else
	if (w && o->inherits("CDisplayWindow")){
		if ((e->type() == QEvent::ShowMinimized) ||
			(e->type() == QEvent::Hide)){
			triggerWindowUpdate();
			ret = false;
		}
	}
	else if (!o->inherits("CDisplayWindow")){
		qDebug("bad mdi child classname: %s", o->className());
		o->dumpObjectInfo();
		o->dumpObjectTree();
	}
#endif
	return ret; // standard event processing
}


/*!
    \fn CMDIArea::triggerWindowUpdate()
 */
void CMDIArea::triggerWindowUpdate() {
	qDebug("CMDIArea::triggerWindowUpfdate");

	if (isUpdatesEnabled() && usableWindowList().count() ) {
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
