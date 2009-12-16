/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <cstdlib>
#include <cstdio>
#include <csignal>
#ifndef NO_DBUS
#include <QDBusConnection>
#endif
#include <QDebug>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>
#include <QVariant>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/cbtconfig.h"
#include "backend/managers/cswordbackend.h"
#include "bibletime.h"
#include "bibletime_dbus_adaptor.h"
#include "bibletimeapp.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/migrationutil.h"

#ifdef BT_ENABLE_TESTING
#include <QtTest/QtTest>
#include "tests/bibletime_test.h"
#endif


namespace {

bool showDebugMessages;

#ifdef Q_WS_WIN

FILE *out_fd = 0;
#define DEBUG_STREAM (out_fd != 0 ? out_fd :\
    out_fd = fopen(QDir::homePath().append("/BibleTime Debug.txt").toLocal8Bit().data(),"w"))
#define CLOSE_DEBUG_STREAM { if (out_fd != 0) fclose(out_fd); }

#else

#define DEBUG_STREAM (stderr)
#define CLOSE_DEBUG_STREAM

#endif

} // anonymous namespace


void myMessageOutput( QtMsgType type, const char *msg ) {
    //we use this messagehandler to switch debugging off in final releases
    FILE* outFd = 0;
    switch (type) {
        case QtDebugMsg:
            if (showDebugMessages) { //only show messages if they are enabled!
                outFd = DEBUG_STREAM;
                if (outFd != 0)
                    fprintf(outFd, "(BibleTime " BT_VERSION ") Debug: %s\n", msg);
            }
            break;
        case QtWarningMsg:
#ifndef QT_NO_DEBUG  // don't show in release builds so users don't get our debug warnings
            outFd = DEBUG_STREAM;
            if (outFd != 0)
                fprintf(outFd, "(BibleTime " BT_VERSION ") WARNING: %s\n", msg);
#endif
            break;
        case QtFatalMsg:
        case QtCriticalMsg:
            outFd = DEBUG_STREAM;
            if (outFd != 0)
                fprintf(outFd,
                        "(BibleTime " BT_VERSION ") _FATAL_: %s\nPlease report this bug! "
                            "(http://www.bibletime.info/development_help.html)",
                        msg);
            abort(); // dump core on purpose
    }
}

void registerMetaTypes() {
    qRegisterMetaType<CSwordBackend::FilterOptions>();
    qRegisterMetaType<CSwordBackend::DisplayOptions>();
    qRegisterMetaTypeStreamOperators<BtBookshelfTreeModel::Grouping>("BtBookshelfTreeModel::Grouping");
}

/// \todo Reimplement signal handler which handles consecutive crashes.

int main(int argc, char* argv[]) {
    namespace DU = util::directory;

    BibleTimeApp app(argc, argv); //for QApplication
    app.setApplicationName("bibletime");
    app.setApplicationVersion(BT_VERSION);

    showDebugMessages = QCoreApplication::arguments().contains("--debug");

#ifdef Q_WS_WIN
    // Use the default Qt message handler if --debug is not specified
    // This works with Visual Studio debugger Output Window
    if (showDebugMessages)
        qInstallMsgHandler( myMessageOutput );
#else
    qInstallMsgHandler( myMessageOutput );
#endif

#ifdef BT_ENABLE_TESTING
    if (QString(argv[1]) == QString("--run-tests")) {
        BibleTimeTest testClass;
        return QTest::qExec(&testClass);
    }
#endif

    /**
      \todo Reimplement "--ignore-session" and "--open-default-bible <key>"
            command line argument handling.
    */

#ifdef Q_WS_WIN

    // On Windows, add a path for Qt plugins to be loaded from
    app.addLibraryPath(app.applicationDirPath() + "/plugins");

    // Must set HOME var on Windows
    QString homeDir(getenv("APPDATA"));
    _putenv_s("HOME", qPrintable(homeDir));

#endif

    registerMetaTypes();

    if (!DU::initDirectoryCache()) {
        qFatal("Error initializing directory cache!");
        return EXIT_FAILURE;
    }

#ifdef Q_WS_WIN
    // change directory to the Sword or .sword directory in the $HOME dir so that
    // the sword.conf is found. It points to the sword/locales.d directory
    QString homeSwordDir = util::directory::getUserHomeDir().absolutePath();
    QDir dir;
    dir.setCurrent(homeSwordDir);
#endif

    // This is needed for languagemgr language names to work, they use \uxxxx escape sequences in string literals
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    //first install QT's own translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name());
    app.installTranslator(&qtTranslator);
    //then our own
    QTranslator BibleTimeTranslator;
    BibleTimeTranslator.load( QString("bibletime_ui_").append(QLocale::system().name()), DU::getLocaleDir().canonicalPath());
    app.installTranslator(&BibleTimeTranslator);

    app.setProperty("--debug", QVariant(showDebugMessages));

    //Migrate configuration data, if neccessary
    util::migration::checkMigration();

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

    int r = app.exec();
    CLOSE_DEBUG_STREAM;
    return r;
}

