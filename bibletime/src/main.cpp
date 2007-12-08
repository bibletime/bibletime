/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//standard includes
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

//own includes
#include "bibletimeapp.h"
#include "bibletime.h"
#include "config.h"

//util includes
#include "util/scoped_resource.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"
#include "util/migrationutil.h"

//frontend includes
#include "frontend/kstartuplogo.h"
#include "frontend/cbtconfig.h"

//KDE includes
#include <kcmdlineargs.h>
#include <kcrash.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kaboutdata.h>
#include <klocale.h>

using namespace util::filesystem;

bool showDebugMessages = false;
BibleTime* bibletime_ptr = 0;

void myMessageOutput( QtMsgType type, const char *msg ) {
	//we use this messagehandler to switch debugging off in final releases
	switch ( type ) {
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
			fprintf( stderr,"(BibleTime %s) _FATAL_: %s\nPlease contact info@bibletime.de and report this bug!", BT_VERSION, msg );
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
	
	//create about data for this application
	//static KCmdLineOptions options[] =
	//	{
	//		{"debug", I18N_NOOP("Enable debug messages"),0},
	//		{"ignore-session", I18N_NOOP("Ignore the startup session that was saved when BibleTime was closed the last time."),0},
	//		{"open-default-bible <key>", I18N_NOOP("Open the standard Bible with the given key. Use <random> to open at a random position."),0},
	//		//{"install-local <path>", I18N_NOOP("Open the SwordSetup dialog to install works from <path>"),0},
	//		{0,0,0}
	//	};
	static KCmdLineOptions options;
	options.add("debug", ki18n("Enable debug messages"),0);
	options.add("ignore-session", ki18n("Ignore the startup session that was saved when BibleTime was closed the last time."),0);
	options.add("open-default-bible <key>", ki18n("Open the standard Bible with the given key. Use <random> to open at a random position."),0);

	KAboutData aboutData(
		"bibletime",
		"bibletime",
		ki18n("BibleTime"),
		BT_VERSION,
		ki18n("Bible study tool for KDE"),
		KAboutData::License_GPL_V2,
		ki18n("(c)1999-2007, The BibleTime Team"),
		ki18n("BibleTime is an easy to use but powerful Bible study tool for KDE.\n\nWe are looking for developers and translators.\nIf you'd like to join our team, please send an email to info@bibletime.info."),
		"http://www.bibletime.info/",
		"info@bibletime.info"
	);

	/***********************************************
	*    Active developers (sorted by last name)   *
	************************************************/
	// Project coordination
	aboutData.addAuthor(ki18n("Joachim Ansorg"), ki18n("Project coordinator"), "nospam@joachim-ansorg.de", "http://www.joachim-ansorg.de/");
	// Second main developer
	aboutData.addAuthor(ki18n("Martin Gruner"), ki18n("Frontend, backend"), "mg.pub@gmx.net", "");
	// artwork
	aboutData.addAuthor(ki18n("James Ots"), ki18n("Crystal icons, crystal startlogo, webpage"), "me@jamesots.com", "http://www.jamesots.com");

	//inactiv

	/***********************************************
	*        Credits (sorted by last name)         *
	************************************************/
	// Update to the Romanian GUI translation
	aboutData.addCredit(ki18n("Horatiu Alexe"), ki18n("Romanian translation"), "");
	//highcolor icons, startlogo for BT <= 1.4
	aboutData.addCredit(ki18n("David Blue"), ki18n("High contrast template"), "davidslists@gmx.net");
	//helped out with the installation manager
	aboutData.addCredit(ki18n("Tim Brodie"),    ki18n("Installation manager"),"tbrodie@displayworksinc.com", "");
	//first set of icons and the first startup logos
	aboutData.addCredit(ki18n("Timothy R. Butler"), ki18n("Icons, startlogo"), "tbutler@uninetsolutions.com", "http://www.uninetsolutions.com");
	//GUI improvements
	aboutData.addCredit(ki18n("Jim Campbell"),   ki18n("GUI"), "jdc.email@gmail.com", ""); 
	//Indexed search, gui improvements
	aboutData.addCredit(ki18n("Lee Carpenter"),  ki18n("GUI, instant search"),"Lee Carpenter <elc@carpie.net>", "http://www.carpie.net");
	//Translations into Traditional and Simplified Chinese
	aboutData.addCredit(ki18n("Chun-shek Chan"),   ki18n("GUI translations into Traditional and Simplified Chinese"), "chunshek@gmail.com", "");
	//Binary packages of CLucene, BibleTime and Sword for Fedore, SUSE, Mandriva
	aboutData.addCredit(ki18n("Jeremy Erickson"),   ki18n("Binary packages"), "", "");
	//Started the Crosswire porject, also contributed some code
	aboutData.addCredit(ki18n("Troy A. Griffits"),   ki18n("Founder of the Sword project"), "scribe@crosswire.org", "http://www.crosswire.org/");  
	//Sponsored many years the www.bibletime.de domain!
	aboutData.addCredit(ki18n("Thomas Hagedorn"),   ki18n("Sponsored our internet domain for many years"), "tom@theta-consulting.de", "");
	//He provided us with the Bible Study HowTo
	aboutData.addCredit(ki18n("Bob Harman"),        ki18n("Bible Study HowTo"), "ncc@ncchampton.org", "");
	// and / or search for multiple words, other fixes and improvements
	aboutData.addCredit(ki18n("Nikolay Igotti"), ki18n("Search dialog enhancements"), "olonho@hotmail.com", "");
	// Language codes iso639-1, iso639-2 and SIL language codes
	aboutData.addCredit(ki18n("SIL International"), ki18n("Language codes and names"), "", "http://www.ethnologue.com/iso639");
	//Patch for the mag delay (2007-03-22)
	aboutData.addCredit(ki18n("Eeli Kaikkonen"),  ki18n("GUI improvements, KDE 4 port"),"eekaikko@mail.student.oulu.fi", "");
	//Update the the Finnish GUI translation
	aboutData.addCredit(ki18n("Ilpo Kantonen"),  ki18n("Finnish translation"),"", "");
	//Smaller frontend enhancements
	aboutData.addCredit(ki18n("Chris Kujawa"),  ki18n("Frontend"),"christopher.kujawa@verizon.net", "");
	//Update to the Czech GUI translation
	aboutData.addCredit(ki18n("Pavel Lauko"),  ki18n("Czech translation"),"", "");
	//Smaller searchdialog enhancements
	aboutData.addCredit(ki18n("Mark Lybarger"),  ki18n("Searchdialog"), "mlybarge@insight.rr.com","");
	//Polish GUI translation update
	aboutData.addCredit(ki18n("Piotr Markiewicz"),  ki18n("Polish translation"), "pmarki@o2.pl", "");
	//Smaller frontend enhancements
	aboutData.addCredit(ki18n("Luke Mauldin"),  ki18n("Frontend"), "lukeskyfly@txk.net", "");
	//translated parts of the russian website files
	aboutData.addCredit(ki18n("Igor Rykhlin"),   ki18n("Russian website translation"), "", "");
	// update to the Spanish website translation
	aboutData.addCredit(ki18n("Gabriel Pérez"),   ki18n("Spanish website translation updates"), "", "");
	//Update to the russian website translation
	aboutData.addCredit(ki18n("Igor Plisco"), ki18n("Update to the russian website translation"), "", "");
	// handbook documentation
	aboutData.addCredit(ki18n("Fred Saalbach"), ki18n("Documentation"), "saalbach@sybercom.net", "");
	// Original Russian website translator
	aboutData.addCredit(ki18n("Vlad Savitsky"), ki18n("Russian website translation"), "", "");
	// comitted search in default bible, opened modules, other smaller things
	aboutData.addCredit(ki18n("Gary Sims"), ki18n("Search dialog enhancements"), "gary@garysims.co.uk", "");
	// Very helpful testing
	aboutData.addCredit(ki18n("Wolfgang Stradner"), ki18n("Very helpful and detailed testing"), "wolfgang_stradner@wycliffe.org", "");
	//The first lead developer
	aboutData.addCredit(ki18n("Torsten Uhlmann"),   ki18n("The first lead developer"), "", "");
 	//Update of the dutch website translation
	aboutData.addCredit(ki18n("Johan van der Lingen"),   ki18n("Translation updates for the Dutch GUI, the website and the handbook"), "", "");
	//French handbook translation
	aboutData.addCredit(ki18n("Jean Van Schaftingen"), ki18n("French handbook translation"), "", "");
	//scoped_ptr and related classes
	aboutData.addCredit(ki18n("David White"),   ki18n("Helpful source code additions"), "", "http://www.wesnoth.org/");
	//Translated the GUI into Russian
	aboutData.addCredit(ki18n("Dmitry Yurevich"),   ki18n("Translated the GUI into Russian"), "", "");
	//The new keychooser for Bible modules
	aboutData.addCredit(ki18n("Mark Zealey"),   ki18n("New Bible key choosers"), "mspam@zealey.org", "");
	//Update to the Spanish GUI translation
 	aboutData.addCredit(ki18n("Esteban Zeller"),   ki18n("Spanish translation"), "", "");

	
	//special message so the translator get his credits in the about box, don't remove this!
	QString dummy = I18N_NOOP("_: NAME OF TRANSLATORS\nYour names"); //translator's name
	dummy = I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"); //translators eMail

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions( options );

	BibleTimeApp app;

	//For the transition time add our own locale dir as locale resource
	KGlobal::dirs()->addResourceDir("locale", DirectoryUtil::getLocaleDir().canonicalPath());
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	// A binary option (on / off)
	if (args->isSet("debug")) {
		showDebugMessages = true;
		app.setProperty("--debug", true);
	} 
	else {
		app.setProperty("--debug", false);
	}

	if (kapp->isSessionRestored()) {
		//TODO: how to restore session with pure Qt?
		//    qDebug("Restoring BibleTime");
		//RESTORE( BibleTime );
		//taken from 1.6.5 where the restoring multiple subwindows bug was fixed
		int n = 1;
		while (KMainWindow::canBeRestored(n)){
			if (KMainWindow::classNameOfToplevel(n) == QString("BibleTime")) {
				bibletime_ptr = new BibleTime;
				bibletime_ptr->restore(n);
			}
			n++;
   		}
		bibletime_ptr->restoreWorkspace();
	}
	else {
		//Migrate configuration data, if neccessary
		util::MigrationUtil::checkMigration();
		
		const bool showSplashScreen = CBTConfig::get(CBTConfig::logo);

		if (showSplashScreen) {
			KStartupLogo::createSplash();
			KStartupLogo::showSplash();
			KStartupLogo::setStatusMessage( i18n("Starting BibleTime") + QString("...") );
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
	}

	const int ret = app.exec();

	delete bibletime_ptr;
	return ret;
}

