/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//own includes
#include "bibletimeapp.h"
#include "bibletime.h"
#include "bibletime_dbus_adaptor.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"
#include "util/migrationutil.h"

#include "backend/config/cbtconfig.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QTextCodec>
#ifndef NO_DBUS
#include <QDBusConnection>
#endif

#ifdef BT_ENABLE_TESTING
#include <QtTest/QtTest>
#include "tests/bibletime_test.h"
#endif

using namespace util::filesystem;

bool showDebugMessages = false;
BibleTime* bibletime_ptr = 0;

void myMessageOutput( QtMsgType type, const char *msg ) {
	//we use this messagehandler to switch debugging off in final releases
	switch (type) {
		case QtDebugMsg:
			if (showDebugMessages) { //only show messages if they are enabled!
				fprintf( stderr,"(BibleTime %s) Debug: %s\n", BT_VERSION, msg );
			}
			break;
		case QtWarningMsg:
			//if (showDebugMessages) //comment out for releases so users don't get our debug warnings
			fprintf( stderr,"(BibleTime %s) WARNING: %s\n", BT_VERSION, msg );
			break;
		case QtFatalMsg:
		case QtCriticalMsg:
			fprintf( stderr,"(BibleTime %s) _FATAL_: %s\nPlease report this bug! (http://www.bibletime.info/development_help.html)", BT_VERSION, msg );
			abort(); // dump core on purpose
	}
}

// TODO - redo signal handler
#if 0
extern "C" {
	static void setSignalHandler(void (*handler)(int));

	// Crash recovery signal handler
	static void signalHandler(int sigId) {
		setSignalHandler(SIG_DFL);
		fprintf(stderr, "*** BibleTime got signal %d (Exiting)\n", sigId);
		// try to cleanup all windows
		if (CBTConfig::get(CBTConfig::crashedLastTime)) {
			//crashed this time and the execution before this one, probably a bug which occurs every time
			CBTConfig::set(CBTConfig::crashedTwoTimes, true);
		}
		else {
			//try to restore next time.
			CBTConfig::set(CBTConfig::crashedLastTime, true);
		}
		if (bibletime_ptr) {
			bibletime_ptr->saveSettings();
			fprintf(stderr, "*** Saving seemed to be successful. If restoring does not work on next startup please use the option --ignore-session\n");
		}
		::exit(-1); //exit BibleTime
	}

	// Crash recovery signal handler
	static void crashHandler(int sigId) {
		setSignalHandler(SIG_DFL);
		fprintf(stderr, "*** BibleTime got signal %d (Crashing). Trying to save settings.\n", sigId);
		if (CBTConfig::get(CBTConfig::crashedLastTime)) {
			// crashed this time and the execution before this one,
			// probably a bug which occurs every time
			CBTConfig::set(CBTConfig::crashedTwoTimes, true);
		}
		else {
			//try to restore next time.
			CBTConfig::set(CBTConfig::crashedLastTime, true);
		}
		if (bibletime_ptr) {
			bibletime_ptr->saveSettings();
			fprintf(stderr, "*** Saving seemed to be successful. If restoring does not work on next startup please use the option --ignore-session\n");
		}
		// Return to DrKonqi.
	}

	static void setSignalHandler(void (*handler)(int)) {
		signal(SIGKILL, handler);
		signal(SIGTERM, handler);
		signal(SIGHUP,  handler);
		KCrash::setEmergencySaveFunction(crashHandler);
	}
}
#endif


int main(int argc, char* argv[]) {
	qInstallMsgHandler( myMessageOutput );

#ifdef BT_ENABLE_TESTING
	if (QString(argv[1]) == QString("--run-tests"))
	{
		BibleTimeTest testClass;
		return QTest::qExec(&testClass);
	}
#endif

#if 0
	//TODO: port to QT
 	static KCmdLineOptions options;
 	options.add("debug", ki18n("Enable debug messages"),0);
 	options.add("ignore-session", ki18n("Ignore the startup session that was saved when BibleTime was closed the last time."),0);
 	options.add("open-default-bible <key>", ki18n("Open the standard Bible with the given key. Use <random> to open at a random position."),0);
#endif


	BibleTimeApp app(argc, argv); //for QApplication
	app.setApplicationName("bibletime");
	app.setApplicationVersion(BT_VERSION);

	// This is needed for languagemgr language names to work, they use \uxxxx escape sequences in string literals
	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	//first install QT's own translations
	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);
	//then our own
	QTranslator BibleTimeTranslator;
	BibleTimeTranslator.load( QString("bibletime_ui_").append(QLocale::system().name()), DirectoryUtil::getLocaleDir().canonicalPath());
	app.installTranslator(&BibleTimeTranslator);

#if 0
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	// A binary option (on / off)
	if (args->isSet("debug")) {
		showDebugMessages = true;
		app.setProperty("--debug", true);
	}
	if (!args->getOptionList("test").isEmpty()) {
		QStringList testingFeatures = args->getOptionList("test");
		foreach (QString feature, testingFeatures) {
			app.setProperty(feature.toLatin1().data(), true);
		}
	}
#endif

	// This is the QT4 version, will only work if main App is QApplication
	// A binary option (on / off)
 	if (app.QCoreApplication::arguments().contains("--debug"))
	{
 		showDebugMessages = true;
 		app.setProperty("--debug", true);
 	}
 	else
	{
 		app.setProperty("--debug", false);
 	}

	//Migrate configuration data, if neccessary
	util::MigrationUtil::checkMigration();

//	setSignalHandler(signalHandler);

	bibletime_ptr = new BibleTime();

	// a new BibleTime version was installed (maybe a completely new installation)
	if (CBTConfig::get(CBTConfig::bibletimeVersion) != BT_VERSION) {

		CBTConfig::set(CBTConfig::bibletimeVersion, BT_VERSION);
		//TODO: unabled temporarily
		//bibletime_ptr->slotSettingsOptions();
		bibletime_ptr->slotSettingsOptions();
	}

	// restore the workspace and process command line options
	//app.setMainWidget(bibletime_ptr); //no longer used in qt4 (QApplication)
	bibletime_ptr->show();
	bibletime_ptr->processCommandline(); //must be done after the bibletime window is visible

#ifndef NO_DBUS
	new BibleTimeDBusAdaptor(bibletime_ptr);
    // connect to D-Bus and register as an object:
	QDBusConnection::sessionBus().registerObject("/BibleTime", bibletime_ptr);
#endif

	const int ret = app.exec();

	delete bibletime_ptr;
	return ret;
}

