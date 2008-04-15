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
#include "config.h"
#include "bibletime_dbus_adaptor.h"

#include "util/cresmgr.h"
#include "util/directoryutil.h"
#include "util/migrationutil.h"

#include "frontend/kstartuplogo.h"
#include "frontend/cbtconfig.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <boost/scoped_ptr.hpp>

#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QDBusConnection>

#ifdef BT_ENABLE_TESTING
#include <QtTest/QtTest>
#include "tests/bibletime_test.h"
#endif

#include <kcmdlineargs.h>
#include <kcrash.h>
#include <kaboutdata.h>

#include <klocalizedstring.h> //TODO: tmp, remove
#include <klocale.h>

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

int main(int argc, char* argv[]) {
	qInstallMsgHandler( myMessageOutput );

#ifdef BT_ENABLE_TESTING
	if (QString(argv[1]) == QString("--run-tests"))
	{
		BibleTimeTest testClass;
		return QTest::qExec(&testClass);
	}
#endif
	
	//TODO: port to QT
 	static KCmdLineOptions options;
 	options.add("debug", ki18n("Enable debug messages"),0);
 	options.add("ignore-session", ki18n("Ignore the startup session that was saved when BibleTime was closed the last time."),0);
 	options.add("open-default-bible <key>", ki18n("Open the standard Bible with the given key. Use <random> to open at a random position."),0);

// 	KAboutData aboutData(
// 		"bibletime",
// 		"bibletime",
// 		QObject::tr("BibleTime"),
// 		BT_VERSION,
// 		QObject::tr("Bible study tool for KDE"),
// 		KAboutData::License_GPL_V2,
// 		QObject::tr("(c)1999-2008, The BibleTime Team"),
// 		QObject::tr("BibleTime is an easy to use but powerful Bible study tool for KDE.\n\nWe are looking for developers and translators.\nIf you'd like to join our team, please send an email to info@bibletime.info."),
// 		"http://www.bibletime.info/",
// 		"info@bibletime.info"
// 	);
// 
// 	/***********************************************
// 	*    Active developers (sorted by last name)   *
// 	************************************************/
// 	// Project coordination
// 	aboutData.addAuthor(QObject::tr("Joachim Ansorg"), QObject::tr("Project coordinator"), "nospam@joachim-ansorg.de", "http://www.joachim-ansorg.de/");
// 	// Second main developer
// 	aboutData.addAuthor(QObject::tr("Martin Gruner"), QObject::tr("Frontend, backend"), "mg.pub@gmx.net", "");
// 	// artwork
// 	aboutData.addAuthor(QObject::tr("James Ots"), QObject::tr("Crystal icons, crystal startlogo, webpage"), "me@jamesots.com", "http://www.jamesots.com");
// 
// 	//inactiv
// 
// 	/***********************************************
// 	*        Credits (sorted by last name)         *
// 	************************************************/
// 	// Update to the Romanian GUI translation
// 	aboutData.addCredit(QObject::tr("Horatiu Alexe"), QObject::tr("Romanian translation"), "");
// 	//highcolor icons, startlogo for BT <= 1.4
// 	aboutData.addCredit(QObject::tr("David Blue"), QObject::tr("High contrast template"), "davidslists@gmx.net");
// 	//helped out with the installation manager
// 	aboutData.addCredit(QObject::tr("Tim Brodie"),    QObject::tr("Installation manager"),"tbrodie@displayworksinc.com", "");
// 	//first set of icons and the first startup logos
// 	aboutData.addCredit(QObject::tr("Timothy R. Butler"), QObject::tr("Icons, startlogo"), "tbutler@uninetsolutions.com", "http://www.uninetsolutions.com");
// 	//GUI improvements
// 	aboutData.addCredit(QObject::tr("Jim Campbell"),   QObject::tr("GUI"), "jdc.email@gmail.com", ""); 
// 	//Indexed search, gui improvements
// 	aboutData.addCredit(QObject::tr("Lee Carpenter"),  QObject::tr("GUI, instant search"),"Lee Carpenter <elc@carpie.net>", "http://www.carpie.net");
// 	//Translations into Traditional and Simplified Chinese
// 	aboutData.addCredit(QObject::tr("Chun-shek Chan"),   QObject::tr("GUI translations into Traditional and Simplified Chinese"), "chunshek@gmail.com", "");
// 	//Binary packages of CLucene, BibleTime and Sword for Fedore, SUSE, Mandriva
// 	aboutData.addCredit(QObject::tr("Jeremy Erickson"),   QObject::tr("Binary packages"), "", "");
// 	//Started the Crosswire porject, also contributed some code
// 	aboutData.addCredit(QObject::tr("Troy A. Griffits"),   QObject::tr("Founder of the Sword project"), "scribe@crosswire.org", "http://www.crosswire.org/");  
// 	//Sponsored many years the www.bibletime.de domain!
// 	aboutData.addCredit(QObject::tr("Thomas Hagedorn"),   QObject::tr("Sponsored our internet domain for many years"), "tom@theta-consulting.de", "");
// 	//He provided us with the Bible Study HowTo
// 	aboutData.addCredit(QObject::tr("Bob Harman"),        QObject::tr("Bible Study HowTo"), "ncc@ncchampton.org", "");
// 	// and / or search for multiple words, other fixes and improvements
// 	aboutData.addCredit(QObject::tr("Nikolay Igotti"), QObject::tr("Search dialog enhancements"), "olonho@hotmail.com", "");
// 	// Language codes iso639-1, iso639-2 and SIL language codes
// 	aboutData.addCredit(QObject::tr("SIL International"), QObject::tr("Language codes and names"), "", "http://www.ethnologue.com/iso639");
// 	//Patch for the mag delay (2007-03-22)
// 	aboutData.addCredit(QObject::tr("Eeli Kaikkonen"),  QObject::tr("GUI improvements, KDE 4 port"),"eekaikko@mail.student.oulu.fi", "");
// 	//Update the the Finnish GUI translation
// 	aboutData.addCredit(QObject::tr("Ilpo Kantonen"),  QObject::tr("Finnish translation"),"", "");
// 	//Smaller frontend enhancements
// 	aboutData.addCredit(QObject::tr("Chris Kujawa"),  QObject::tr("Frontend"),"christopher.kujawa@verizon.net", "");
// 	//Update to the Czech GUI translation
// 	aboutData.addCredit(QObject::tr("Pavel Lauko"),  QObject::tr("Czech translation"),"", "");
// 	//Smaller searchdialog enhancements
// 	aboutData.addCredit(QObject::tr("Mark Lybarger"),  QObject::tr("Searchdialog"), "mlybarge@insight.rr.com","");
// 	//Polish GUI translation update
// 	aboutData.addCredit(QObject::tr("Piotr Markiewicz"),  QObject::tr("Polish translation"), "pmarki@o2.pl", "");
// 	//Smaller frontend enhancements
// 	aboutData.addCredit(QObject::tr("Luke Mauldin"),  QObject::tr("Frontend"), "lukeskyfly@txk.net", "");
// 	//translated parts of the russian website files
// 	aboutData.addCredit(QObject::tr("Igor Rykhlin"),   QObject::tr("Russian website translation"), "", "");
// 	// update to the Spanish website translation
// 	aboutData.addCredit(QObject::tr("Gabriel Pérez"),   QObject::tr("Spanish website translation updates"), "", "");
// 	//Update to the russian website translation
// 	aboutData.addCredit(QObject::tr("Igor Plisco"), QObject::tr("Update to the russian website translation"), "", "");
// 	// handbook documentation
// 	aboutData.addCredit(QObject::tr("Fred Saalbach"), QObject::tr("Documentation"), "saalbach@sybercom.net", "");
// 	// Original Russian website translator
// 	aboutData.addCredit(QObject::tr("Vlad Savitsky"), QObject::tr("Russian website translation"), "", "");
// 	// comitted search in default bible, opened modules, other smaller things
// 	aboutData.addCredit(QObject::tr("Gary Sims"), QObject::tr("Search dialog enhancements"), "gary@garysims.co.uk", "");
// 	// Very helpful testing
// 	aboutData.addCredit(QObject::tr("Wolfgang Stradner"), QObject::tr("Very helpful and detailed testing"), "wolfgang_stradner@wycliffe.org", "");
// 	//The first lead developer
// 	aboutData.addCredit(QObject::tr("Torsten Uhlmann"),   QObject::tr("The first lead developer"), "", "");
//  	//Update of the dutch website translation
// 	aboutData.addCredit(QObject::tr("Johan van der Lingen"),   QObject::tr("Translation updates for the Dutch GUI, the website and the handbook"), "", "");
// 	//French handbook translation
// 	aboutData.addCredit(QObject::tr("Jean Van Schaftingen"), QObject::tr("French handbook translation"), "", "");
// 	//scoped_ptr and related classes
// 	aboutData.addCredit(QObject::tr("David White"),   QObject::tr("Helpful source code additions"), "", "http://www.wesnoth.org/");
// 	//Translated the GUI into Russian
// 	aboutData.addCredit(QObject::tr("Dmitry Yurevich"),   QObject::tr("Translated the GUI into Russian"), "", "");
// 	//The new keychooser for Bible modules
// 	aboutData.addCredit(QObject::tr("Mark Zealey"),   QObject::tr("New Bible key choosers"), "mspam@zealey.org", "");
// 	//Update to the Spanish GUI translation
//  	aboutData.addCredit(QObject::tr("Esteban Zeller"),   QObject::tr("Spanish translation"), "", "");
// 
// 	
// 	//special message so the translator get his credits in the about box, don't remove this!
// 	QString dummy = I18N_NOOP("_: NAME OF TRANSLATORS\nYour names"); //translator's name
// 	dummy = I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"); //translators eMail

	KLocalizedString dummy; //TODO: remove, only dummy code here
	KAboutData aboutData(
		"bibletime",
		"bibletime",
		dummy,
		BT_VERSION,
		dummy,
		KAboutData::License_GPL_V2,
		dummy,
		dummy,
		"http://www.bibletime.info/",
		"info@bibletime.info");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions( options );

	//BibleTimeApp app(argc, argv); //for QApplication
	BibleTimeApp app;
	
	//first install QT's own translations
	QTranslator qtTranslator;
	qtTranslator.load("qt_" + QLocale::system().name());
	app.installTranslator(&qtTranslator);
	//then our own
	QTranslator BibleTimeTranslator;
	BibleTimeTranslator.load( QLocale::system().name(), DirectoryUtil::getLocaleDir().canonicalPath());
	app.installTranslator(&BibleTimeTranslator);
	
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
	
	// This is the QT4 version, will only work if main App is QApplication
	// A binary option (on / off)
// 	if (app.QCoreApplication::arguments().contains("--debug")) {
// 		showDebugMessages = true;
// 		app.setProperty("--debug", true);
// 	} 
// 	else {
// 		app.setProperty("--debug", false);
// 	}

// 	if (kapp->isSessionRestored()) {
// 		//TODO: how to restore session with pure Qt?
// 		//    qDebug("Restoring BibleTime");
// 		//RESTORE( BibleTime );
// 		//taken from 1.6.5 where the restoring multiple subwindows bug was fixed
// 		int n = 1;
// 		while (KMainWindow::canBeRestored(n)){
// 			if (KMainWindow::classNameOfToplevel(n) == QString("BibleTime")) {
// 				bibletime_ptr = new BibleTime;
// 				bibletime_ptr->restore(n);
// 			}
// 			n++;
//    		}
// 		bibletime_ptr->restoreWorkspace();
// 	}
	
	//Migrate configuration data, if neccessary
	util::MigrationUtil::checkMigration();
	
	const bool showSplashScreen = CBTConfig::get(CBTConfig::logo);

	if (showSplashScreen) {
		KStartupLogo::createSplash();
		KStartupLogo::showSplash();
		KStartupLogo::setStatusMessage( QObject::tr("Starting BibleTime") + QString("...") );
	}

	setSignalHandler(signalHandler);


	bibletime_ptr = new BibleTime();

	// a new BibleTime version was installed (maybe a completely new installation)
	if (CBTConfig::get(CBTConfig::bibletimeVersion) != BT_VERSION) {
		KStartupLogo::hideSplash();

		CBTConfig::set(CBTConfig::bibletimeVersion, BT_VERSION);
		//TODO: unabled temporarily
		//bibletime_ptr->slotSettingsOptions();
		bibletime_ptr->slotSettingsOptions();
	}

	// restore the workspace and process command line options
	//app.setMainWidget(bibletime_ptr); //no longer used in qt4 (QApplication)
	bibletime_ptr->show();
	bibletime_ptr->processCommandline(); //must be done after the bibletime window is visible

	if (showSplashScreen) {
		KStartupLogo::hideSplash();
		KStartupLogo::deleteSplash();
	}
	
	new BibleTimeDBusAdaptor(bibletime_ptr);
	
    // connect to D-Bus and register as an object:
	QDBusConnection::sessionBus().registerObject("/BibleTime", bibletime_ptr);

	const int ret = app.exec();

	delete bibletime_ptr;
	return ret;
}

