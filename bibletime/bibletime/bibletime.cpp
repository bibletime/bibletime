/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//local includes
#include "config.h"
#include "bibletime.h"

//frontend includes
#include "frontend/cmdiarea.h"
#include "frontend/kstartuplogo.h"
#include "frontend/mainindex/cmainindex.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/displaywindow/creadwindow.h"
#include "frontend/displaywindow/cwritewindow.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/cbtconfig.h"

#include "util/ctoolclass.h"
#include "util/cpointers.h"

//backend includes
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbiblemoduleinfo.h"
#include "backend/cswordcommentarymoduleinfo.h"
#include "backend/cswordlexiconmoduleinfo.h"
#include "backend/cswordbookmoduleinfo.h"
#include "backend/cswordversekey.h"
#include "backend/cswordldkey.h"


//Qt includes
#include <qsplitter.h>

//KDE includes
#include <kdeversion.h>
#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaccel.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <krandomsequence.h>

using namespace Profile;

BibleTime::BibleTime()
: BibleTimeInterface("BibleTimeInterface"),
#if KDE_VERSION >= 0x030200
KMainWindow(KMainWindow::NoDCOPObject, 0,0, WType_TopLevel),
#else
KMainWindow(0,0, WType_TopLevel),
#endif
m_windowActionCollection(0),
m_initialized(false),
m_moduleList(0),
m_progress(0),
m_currentProfile(0),
m_mainSplitter(0),
m_leftPaneSplitter(0),
m_mdi(0),
m_profileMgr(),
m_backend(0),
m_mainIndex(0) {
	setObjId("BibleTimeInterface");
	setHelpMenuEnabled(false);

	initBackends();
	initView();
	initActions();

	createGUI("bibletimeui.rc");
	initMenubar();
	initConnections();

	readSettings();

	setPlainCaption("BibleTime " VERSION);

	// we don't save the geometry, it's stored in the startup profile
	setAutoSaveSettings(QString("MainWindow"), false);
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

	accel()->writeSettings(CBTConfig::getConfig());

	CBTConfig::set
		(CBTConfig::toolbar, m_viewToolbar_action->isChecked());
	CBTConfig::set
		(CBTConfig::mainIndex, m_viewMainIndex_action->isChecked());
	CBTConfig::set
		(CBTConfig::infoDisplay, m_viewInfoDisplay_action->isChecked());

	CBTConfig::set
		(CBTConfig::mainSplitterSizes, m_mainSplitter->sizes());
	CBTConfig::set
		(CBTConfig::leftPaneSplitterSizes, m_leftPaneSplitter->sizes());

	// set the default to false
	/* CBTConfig::set(CBTConfig::autoTileVertical, false);
	 CBTConfig::set(CBTConfig::autoTileHorizontal, false);
	 CBTConfig::set(CBTConfig::autoCascade, false);
	*/
	CBTConfig::set
		(CBTConfig::autoTileVertical, m_windowAutoTileVertical_action->isChecked());
	CBTConfig::set
		(CBTConfig::autoTileHorizontal, m_windowAutoTileHorizontal_action->isChecked());
	CBTConfig::set
		(CBTConfig::autoCascade, m_windowAutoCascade_action->isChecked());


	CProfile* p = m_profileMgr.startupProfile();
	if (p) {
		saveProfile(p);
	}
}

/** Reads the settings from the configfile and sets the right properties. */
void BibleTime::readSettings() {
	//  accel()->readSettings(CBTConfig::getConfig());
	CBTConfig::setupAccelSettings(CBTConfig::application, actionCollection());

	m_viewToolbar_action->setChecked( CBTConfig::get
										  (CBTConfig::toolbar) );
	slotToggleToolbar();

	m_viewMainIndex_action->setChecked( CBTConfig::get
											(CBTConfig::mainIndex) );
	slotToggleMainIndex();

	m_viewInfoDisplay_action->setChecked( CBTConfig::get
											  (CBTConfig::infoDisplay) );
	slotToggleInfoDisplay();

	m_mainSplitter->setSizes(
		CBTConfig::get
			(CBTConfig::mainSplitterSizes)
		);
	m_leftPaneSplitter->setSizes(
		CBTConfig::get
			(CBTConfig::leftPaneSplitterSizes)
		);

	if ( CBTConfig::get
				(CBTConfig::autoTileVertical) ) {
			m_windowAutoTileVertical_action->setChecked( true );
			m_windowManualMode_action->setChecked(false);
			slotAutoTileVertical();
		}
	else if ( CBTConfig::get
				  (CBTConfig::autoTileHorizontal) ) {
			m_windowAutoTileHorizontal_action->setChecked( true );
			m_windowManualMode_action->setChecked(false);
			slotAutoTileHorizontal();
		}
	else if ( CBTConfig::get
				  (CBTConfig::autoCascade) ) {
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
	kapp->setOverrideCursor( waitCursor );

	CDisplayWindow* displayWindow = CDisplayWindow::createReadInstance(modules, m_mdi);
	if ( displayWindow ) {
		displayWindow->init();
		displayWindow->show();
		//   if (!key.isEmpty())
		displayWindow->lookup(key);
	}

	kapp->restoreOverrideCursor();
	return displayWindow;
}


/** Creates a new presenter in the MDI area according to the type of the module. */
CDisplayWindow* BibleTime::createReadDisplayWindow(CSwordModuleInfo* module, const QString& key) {
	ListCSwordModuleInfo list;
	list.append(module);

	return createReadDisplayWindow(list, key);
}

CDisplayWindow* BibleTime::createWriteDisplayWindow(CSwordModuleInfo* module, const QString& key, const CDisplayWindow::WriteWindowType& type) {
	kapp->setOverrideCursor( waitCursor );

	ListCSwordModuleInfo modules;
	modules.append(module);

	CDisplayWindow* displayWindow = CDisplayWindow::createWriteInstance(modules, m_mdi, type);
	if ( displayWindow ) {
		displayWindow->init();
		displayWindow->show();
		displayWindow->lookup(key);
	}

	kapp->restoreOverrideCursor();
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
bool BibleTime::queryExit() {
	bool ret = false;
	if (m_initialized) {
		saveSettings();
		ret = true;
	}

	return ret;
}

/** Called before a window is closed */
bool BibleTime::queryClose() {
	bool ret = true;

	for ( unsigned int index = 0; index < m_mdi->windowList().count(); ++index) {
		if (CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(m_mdi->windowList().at(index))
		   ) {
			ret = ret && window->queryClose();
		}
	}

	return ret;
}

/** Reimplementation used for sessions management. */
void BibleTime::saveProperties(KConfig* /*myConfig*/) {}

/** Reimplementation used for session management. */
void BibleTime::readProperties(KConfig* /*myConfig*/) {}

/** Restores the workspace if the flag for this is set in the config. */
void BibleTime::restoreWorkspace() {
	if (CProfile* p = m_profileMgr.startupProfile()) {
		loadProfile(p);
	}
}

/** Sets the caption of the mainwindow */
void BibleTime::setCaption( const QString& ) {
	KMainWindow::setPlainCaption( KApplication::kApplication()->makeStdCaption( m_mdi->currentApplicationCaption() ) );
}

/** Sets the plain caption of the main window */
void BibleTime::setPlainCaption( const QString& ) {
	KMainWindow::setPlainCaption( KApplication::kApplication()->makeStdCaption( m_mdi->currentApplicationCaption() ) );
}

/** Processes the commandline options given to BibleTime. */
void BibleTime::processCommandline() {
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	//   if (CBTConfig::get(CBTConfig::crashedLastTime)) {
	if (!CBTConfig::get
				(CBTConfig::crashedTwoTimes) && !args->isSet("ignore-session")) { //restore workspace if it crashed only once
			restoreWorkspace();
		}
	//   }

	//open every time a module if the command line option was set.
	if ( args->isSet("open-default-bible") && !CBTConfig::get
				(CBTConfig::crashedLastTime) && !CBTConfig::get
					(CBTConfig::crashedTwoTimes)) { //restore workspace if it crashed ony once
				QString bibleKey = args->getOption("open-default-bible");
				CSwordModuleInfo* bible = CBTConfig::get
											  (CBTConfig::standardBible);

				if (bibleKey == "<random>") {
					CSwordVerseKey vk(0);
					const int maxIndex = 32400;

					KRandomSequence rs;
					int newIndex = rs.getLong(maxIndex);
					vk.setPosition(sword::TOP);
					vk.Index(newIndex);
					bibleKey = vk.key();
				}
				createReadDisplayWindow(bible, bibleKey);
				//    if (isVisible())
				m_mdi->myTileVertical();//we are sure only one window is open, which should be displayed fullscreen in the working area
			}
}

void BibleTime::polish() {
	m_initialized = true;

	KMainWindow::polish();
	applyMainWindowSettings(CBTConfig::getConfig(), QString::fromLatin1("MainWindow"));
}

