/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/
#include "config.h"
#include "bibletime.h"
#include "bibletime.moc"

#include "frontend/cmdiarea.h"
#include "frontend/kstartuplogo.h"
#include "frontend/mainindex/cmainindex.h"
#include "frontend/mainindex/bookshelf/cbookshelfindex.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/cdisplaywindowfactory.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/displaywindow/cwritewindow.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/cbtconfig.h"

#include "util/ctoolclass.h"
#include "util/cpointers.h"
#include "util/directoryutil.h"

#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "backend/keys/cswordldkey.h"

//Qt includes
#include <QSplitter>
#include <QDebug>
#include <QAction>
#include <QApplication>

//KDE includes
#include <kcmdlineargs.h>


using namespace Profile;

BibleTime::BibleTime() : 
	QMainWindow(0),
	m_initialized(false),
	m_moduleList(0),
	m_currentProfile(0),
	m_mainSplitter(0),
	m_leftPaneSplitter(0),
	m_mdi(0),
	m_profileMgr(),
	m_backend(0),
	m_mainIndex(0)
{
	//setObjId("BibleTimeInterface"); //DCOP?
	initBackends();
	initView();
	initActions();
	initConnections();
	readSettings();
	setPlainCaption(QString());
	// we don't save the geometry, it's stored in the startup profile
// 	setAutoSaveSettings(QString("MainWindow"), false);
}

BibleTime::~BibleTime() {
	//  delete m_dcopInterface;
	// The backend is deleted by the BibleTimeApp instance
}

/** Saves the properties of BibleTime to the application wide configfile  */
void BibleTime::saveSettings() {
	if (m_mdi) {
		m_mdi->saveSettings();
	}

	//TODO: how to write settings?
	//accel()->writeSettings(CBTConfig::getConfig());

	CBTConfig::set(CBTConfig::toolbar, m_viewToolbar_action->isChecked());
	CBTConfig::set(CBTConfig::mainindexActiveTab, m_mainIndex->currentIndex());
	CBTConfig::set(CBTConfig::mainIndex, m_viewMainIndex_action->isChecked());
	CBTConfig::set(CBTConfig::infoDisplay, m_viewInfoDisplay_action->isChecked());
	CBTConfig::set(CBTConfig::mainSplitterSizes, m_mainSplitter->sizes());
	CBTConfig::set(CBTConfig::leftPaneSplitterSizes, m_leftPaneSplitter->sizes());

	// set the default to false
	/* CBTConfig::set(CBTConfig::autoTileVertical, false);
	 CBTConfig::set(CBTConfig::autoTileHorizontal, false);
	 CBTConfig::set(CBTConfig::autoCascade, false);
	*/
	CBTConfig::set(CBTConfig::autoTileVertical, m_windowAutoTileVertical_action->isChecked());
	CBTConfig::set(CBTConfig::autoTileHorizontal, m_windowAutoTileHorizontal_action->isChecked());
	CBTConfig::set(CBTConfig::autoCascade, m_windowAutoCascade_action->isChecked());

	CProfile* p = m_profileMgr.startupProfile();
	if (p) {
		saveProfile(p);
	}
}

/** Reads the settings from the configfile and sets the right properties. */
void BibleTime::readSettings() {
	qDebug("******************BibleTime::readSettings******************************");
	//  accel()->readSettings(CBTConfig::getConfig());
// 	CBTConfig::setupAccelSettings(CBTConfig::application, actionCollection());

	m_viewToolbar_action->setChecked( CBTConfig::get(CBTConfig::toolbar) );
	slotToggleToolbar();

	m_viewMainIndex_action->setChecked( CBTConfig::get(CBTConfig::mainIndex) );
	slotToggleMainIndex();

	m_mainIndex->setCurrentIndex(CBTConfig::get(CBTConfig::mainindexActiveTab));

	m_viewInfoDisplay_action->setChecked( CBTConfig::get(CBTConfig::infoDisplay) );
	slotToggleInfoDisplay();

	//we don't want to set wrong sizes
	if ( CBTConfig::get(CBTConfig::mainSplitterSizes).count() > 0 ) {
		m_mainSplitter->setSizes(CBTConfig::get(CBTConfig::mainSplitterSizes));
		m_leftPaneSplitter->setSizes(CBTConfig::get(CBTConfig::leftPaneSplitterSizes));
	}
	if ( CBTConfig::get(CBTConfig::autoTileVertical) ) {
			m_windowAutoTileVertical_action->setChecked( true );
			m_windowManualMode_action->setChecked(false);
			slotAutoTileVertical();
		}
	else if ( CBTConfig::get(CBTConfig::autoTileHorizontal) ) {
			m_windowAutoTileHorizontal_action->setChecked( true );
			m_windowManualMode_action->setChecked(false);
			slotAutoTileHorizontal();
		}
	else if ( CBTConfig::get(CBTConfig::autoCascade) ) {
			m_windowAutoCascade_action->setChecked(true);
			m_windowManualMode_action->setChecked(false);
			slotAutoCascade();
		}
	else {
		m_windowManualMode_action->setChecked(true);
		slotManualArrangementMode();
	}
}

/** Creates a new presenter in the MDI area according to the type of the module. */
CDisplayWindow* BibleTime::createReadDisplayWindow(ListCSwordModuleInfo modules, const QString& key) {
	qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );
	qDebug("BibleTime::createReadDisplayWindow(ListCSwordModuleInfo modules, const QString& key)");
	CDisplayWindow* displayWindow = CDisplayWindowFactory::createReadInstance(modules, m_mdi);
	if ( displayWindow ) {
		displayWindow->init();
		if (m_mdi->windowList().count() == 0)
			displayWindow->showMaximized();
		else
			displayWindow->show();
		//   if (!key.isEmpty())
		displayWindow->lookup(key);
	}
	// We have to process pending events here, otherwise displayWindow is not fully painted
	qApp->processEvents();
	// Now all events, including mouse clicks for the displayWindow have been handled
	// and we can let the user click the same module again
	m_mainIndex->bookshelfIndex()->unfreezeModules(modules);
	qApp->restoreOverrideCursor();
	return displayWindow;
}


/** Creates a new presenter in the MDI area according to the type of the module. */
CDisplayWindow* BibleTime::createReadDisplayWindow(CSwordModuleInfo* module, const QString& key) {
	ListCSwordModuleInfo list;
	list.append(module);

	return createReadDisplayWindow(list, key);
}

CDisplayWindow* BibleTime::createWriteDisplayWindow(CSwordModuleInfo* module, const QString& key, const CDisplayWindow::WriteWindowType& type) {
	qApp->setOverrideCursor( QCursor(Qt::WaitCursor) );

	ListCSwordModuleInfo modules;
	modules.append(module);

	CDisplayWindow* displayWindow = CDisplayWindowFactory::createWriteInstance(modules, m_mdi, type);
	if ( displayWindow ) {
		displayWindow->init();
		if (m_mdi->windowList().count() == 0)
			displayWindow->showMaximized();
		else
			displayWindow->show();
		displayWindow->lookup(key);
	}

	qApp->restoreOverrideCursor();
	return displayWindow;
}

/** Refreshes all presenters.*/
void BibleTime::refreshDisplayWindows() {
	unsigned int index;
	for ( index = 0; index < m_mdi->windowList().count(); index++) {
		CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(m_mdi->windowList().at(index));
		if (window) {
			window->reload();
		}
	}
}

/** Called before quit. */
void BibleTime::slot_aboutToQuit() {
	saveSettings();
}

/** Called before a window is closed */
bool BibleTime::queryClose() {
	qDebug("BibleTime::queryClose");
	bool ret = true;

	for ( unsigned int index = 0; index < m_mdi->windowList().count(); ++index) {
		if (CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(m_mdi->windowList().at(index))) {
			ret = ret && window->queryClose();
		}
		qDebug() << "return value:" << ret;
	}
	qDebug() << "final return value:" << ret;
	return ret;
}

/** Restores the workspace if the flag for this is set in the config. */
void BibleTime::restoreWorkspace() {
	if (CProfile* p = m_profileMgr.startupProfile()) {
		loadProfile(p);
	}
}

/** Sets the plain caption of the main window */
void BibleTime::setPlainCaption( const QString& ) {
	QString suffix;
	if (!m_mdi->currentApplicationCaption().isEmpty())
		suffix = QString(" [").append(m_mdi->currentApplicationCaption()).append("]");
	QMainWindow::setWindowTitle( tr("BibleTime ").append(BT_VERSION) + suffix );
}

/** Processes the commandline options given to BibleTime. */
void BibleTime::processCommandline() {
	//TODO: port to Qt when we remove KApplication
	//QStringList args = qApp->QCoreApplication::arguments();

	KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
	if (!CBTConfig::get(CBTConfig::crashedTwoTimes) && !args->isSet("ignore-session")) { //restore workspace if it crashed only once
		restoreWorkspace();
	}

	//TODO: port to QT4
// 	//open every time a module if the command line option was set.
// 	if ( args.contains("--open-default-bible") && !CBTConfig::get(CBTConfig::crashedLastTime) && !CBTConfig::get(CBTConfig::crashedTwoTimes)) { //restore workspace if it crashed ony once
// 		QString bibleKey = args->getOption("open-default-bible");
// 		CSwordModuleInfo* bible = CBTConfig::get(CBTConfig::standardBible);
// 
// 		if (bibleKey == "<random>") {
// 			CSwordVerseKey vk(0);
// 			const int maxIndex = 32400;
// 
// 			KRandomSequence rs;
// 			int newIndex = rs.getLong(maxIndex);
// 			vk.setPosition(sword::TOP);
// 			vk.Index(newIndex);
// 			bibleKey = vk.key();
// 		}
// 		createReadDisplayWindow(bible, bibleKey);
// 		//    if (isVisible())
// 		m_mdi->myTileVertical();//we are sure only one window is open, which should be displayed fullscreen in the working area
// 	}
}

bool BibleTime::event(QEvent* e)
{
// /*	if (e->type() == QEvent::Polish) {
// 		qWarning("BibleTime::event type Polish");
// 		m_initialized = true;
// 	}*/
	if (e->type() == QEvent::Close) {	
	}
	return QMainWindow::event(e);
}

void BibleTime::closeEvent(QCloseEvent* e) {
	//qDebug("Now we will call qApp->quit");
	QMainWindow::closeEvent(e);
	e->ignore();
	qApp->quit();
}
