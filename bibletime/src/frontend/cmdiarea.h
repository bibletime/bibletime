/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CMDIAREA_H
#define CMDIAREA_H

//Qt includes
#include <QMdiArea>
#include <QList>

class CSwordModuleInfo;
class QEvent;
class QResizeEvent;
class QMdiSubWindow;

/** The MDI widget we use in BibleTime.
 * Enhances QMdiArea.
 */
class CMDIArea : public QMdiArea {
	Q_OBJECT
	Q_PROPERTY(MDIArrangementMode m_mdiArrangementMode READ getMDIArrangementMode WRITE setMDIArrangementMode)

public:
	/**
	* The options you can set for this widget.
	*/
	enum MDIArrangementMode {
		ArrangementModeTileVertical = 1,
		ArrangementModeTileHorizontal = 2,
		ArrangementModeCascade = 3,
		ArrangementModeManual = 4,
		MDIArrangementModeMIN = ArrangementModeTileVertical,
		MDIArrangementModeMAX = ArrangementModeManual
	};
	CMDIArea(QWidget *parent);
	/** Reimplementation
	*/
	virtual QMdiSubWindow* addSubWindow(QWidget* widget, Qt::WindowFlags windowFlags = 0);
	/**
	 */
	void setMDIArrangementMode( const MDIArrangementMode );
	/** */
	MDIArrangementMode getMDIArrangementMode(void) const;
	/**
	*/
	void emitWindowCaptionChanged();
	/**
	* Forces an update of the currently chosen window arrangement.
	*/
	void triggerWindowUpdate();
	/** Lists all subWindows which are not minimized or hidden
	 */
	QList<QMdiSubWindow*> usableWindowList();

public slots:
	/**
	* Called whan a client window was activated
	*/
	void slotClientActivated(QMdiSubWindow* client);
	/**
	* Deletes all the presenters in the MDI area.
	*/
	void deleteAll();
	/** Our own cascade version which, if only one window is left, shows this maximized.
	* Also necessary for autoCasacde feature
	*/
	void myCascade();
	/** Our own cascade version which, if only one window is left, shows this maximized.
	* Also necessary for autoTile feature
	*/
	void myTileVertical();
	/** Horizontal tile function
	* This function was taken from Qt's MDI example.
	*/
	void myTileHorizontal();
	/**
	 * Emits the signal to create a new display window in the MDI area.
	 */
	inline void emitCreateDisplayWindow(QList<CSwordModuleInfo*> modules, const QString keyName);

signals: // Signals
	/**
	* Emits a signal to set the acption of the toplevel widget.
	*/
	void sigSetToplevelCaption(const QString&);
	/**
	 */
	void createReadDisplayWindow(QList<CSwordModuleInfo*> modules, const QString& keyName);

private:
	/**
	* Reimplementation
	*/
	virtual void resizeEvent(QResizeEvent* e);
	/** 
	 * Used to handle Events of MDI windows
	 * */
	bool eventFilter( QObject *o, QEvent *e );
	/**
	 */
	MDIArrangementMode m_mdiArrangementMode;
};

/** Emits the signal to create a new display window in the MDI area. */
inline void CMDIArea::emitCreateDisplayWindow(QList<CSwordModuleInfo*> modules, const QString keyName) {
	emit createReadDisplayWindow(modules, keyName);
}


#endif
