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

#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/cbtconfig.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <boost/scoped_ptr.hpp>

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

bool showDebugMessages;

void myMessageOutput( QtMsgType type, const char *msg ) {
    //we use this messagehandler to switch debugging off in final releases
    switch (type) {
        case QtDebugMsg:
            if (showDebugMessages) { //only show messages if they are enabled!
                fprintf( stderr, "(BibleTime %s) Debug: %s\n", BT_VERSION, msg );
            }
            break;
        case QtWarningMsg:
            //if (showDebugMessages) //comment out for releases so users don't get our debug warnings
            fprintf( stderr, "(BibleTime %s) WARNING: %s\n", BT_VERSION, msg );
            break;
        case QtFatalMsg:
        case QtCriticalMsg:
            fprintf( stderr, "(BibleTime %s) _FATAL_: %s\nPlease report this bug! (http://www.bibletime.info/development_help.html)", BT_VERSION, msg );
            abort(); // dump core on purpose
    }
}

void registerMetaTypes() {
    qRegisterMetaTypeStreamOperators<BtBookshelfTreeModel::Grouping>("BtBookshelfTreeModel::Grouping");
}

int main(int argc, char* argv[]) {
    qInstallMsgHandler( myMessageOutput );

#ifdef BT_ENABLE_TESTING
    if (QString(argv[1]) == QString("--run-tests")) {
        BibleTimeTest testClass;
        return QTest::qExec(&testClass);
    }
#endif

    BibleTimeApp app(argc, argv); //for QApplication
    app.setApplicationName("bibletime");
    app.setApplicationVersion(BT_VERSION);

    registerMetaTypes();

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

    // This is the QT4 version, will only work if main App is QApplication
    // A binary option (on / off)
    showDebugMessages = QCoreApplication::arguments().contains("--debug");
    app.setProperty("--debug", showDebugMessages);

    //Migrate configuration data, if neccessary
    util::MigrationUtil::checkMigration();

//	setSignalHandler(signalHandler);

    BibleTime bibleTime;

    // a new BibleTime version was installed (maybe a completely new installation)
    if (CBTConfig::get(CBTConfig::bibletimeVersion) != BT_VERSION) {
        CBTConfig::set(CBTConfig::bibletimeVersion, BT_VERSION);
        bibleTime.saveConfigSettings();
    }

    // restore the workspace and process command line options
    //app.setMainWidget(bibletime_ptr); //no longer used in qt4 (QApplication)
    bibleTime.show();
    bibleTime.processCommandline(); //must be done after the bibletime window is visible

#ifndef NO_DBUS
    new BibleTimeDBusAdaptor(&bibleTime);
    // connect to D-Bus and register as an object:
    QDBusConnection::sessionBus().registerService("info.bibletime.BibleTime");
    QDBusConnection::sessionBus().registerObject("/BibleTime", &bibleTime);
#endif

    return app.exec();
}

