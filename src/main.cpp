/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <iostream>
#ifndef NO_DBUS
#include <QDBusConnection>
#endif
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/cbtconfig.h"
#include "bibletime.h"
#include "bibletime_dbus_adaptor.h"
#include "bibletimeapp.h"
#include "util/directory.h"
#include "util/migrationutil.h"


/// \todo Reimplement signal handler which handles consecutive crashes.

namespace {

/*******************************************************************************
  Printing command-line help.
*******************************************************************************/

/**
  Prints command-line help text for BibleTime when --help is used.
  \param[in] executable The executed file name (argv[0]).
*/
void printHelp(const QString &executable) {
    std::cout << qPrintable(executable) << std::endl << std::endl
        << "    --help, -h" << std::endl << "        "
        << qPrintable(QObject::tr("Show this help message and exit"))
        << std::endl << std::endl
        << "    --version, -V" << std::endl << "        "
        << qPrintable(QObject::tr("Output BibleTime version and exit"))
        << std::endl << std::endl
        << "    --ignore-session" << std::endl << "        "
        << qPrintable(QObject::tr("open a clean session"))
        << std::endl << std::endl
        << "    --open-default-bible <ref>" << std::endl << "        "
        << qPrintable(QObject::tr("Open the default Bible with the "
                                  "reference <ref>"))
        << std::endl << std::endl
        << qPrintable(QObject::tr("For command-line arguments parsed by the"
                                  " Qt toolkit, see %1.")
               .arg("http://doc.qt.nokia.com/latest/qapplication.html"))
        << ' ' << qPrintable(QObject::tr("All command-line arguments not "
                                         "recognized by BibleTime or Qt will "
                                         "be silently ignored."))
        << std::endl;
}

/*******************************************************************************
  Parsing command-line arguments
*******************************************************************************/

/**
  Parses all command-line arguments.
  \param[out] showDebugMessages Whether --debug was specified.
  \param[out] ignoreSession Whether --ignore-session was specified.
  \param[out] openBibleKey Will be set to --open-default-bible if specified.
  \retval -1 Parsing was successful, the application should exit with
             EXIT_SUCCESS.
  \retval 0 Parsing was successful.
  \retval 1 Parsing failed, the application should exit with EXIT_FAILURE.
*/
int parseCommandLine(bool &showDebugMessages, bool &ignoreSession,
                     QString &openBibleKey)
{
    QStringList args = BibleTimeApp::arguments();
    for (int i = 1; i < args.size(); i++) {
        const QString &arg = args.at(i);
        if (arg == "--help"
            || arg == "-h"
            || arg == "/?"
            || arg == "/h")
        {
            printHelp(args.at(0));
            return -1;
        } else if (arg == "--version"
                   || arg == "-V")
        {
            std::cout << "BibleTime " BT_VERSION << std::endl;
            return -1;
        } else if (arg == "--debug") {
            showDebugMessages = true;
        } else if (arg == "--ignore-session") {
            ignoreSession = true;
        } else if (arg == "--open-default-bible") {
            i++;
            if (i < args.size()) {
                openBibleKey = args.at(i);
            } else {
                std::cerr << qPrintable(QObject::tr(
                    "Error: %1 expects an argument. See --help for details.")
                    .arg("--open-default-bible")) << std::endl;
                return 1;
            }
        } else {
            std::cerr << qPrintable(QObject::tr(
                "Error: Invalid command-line argument: %1")
                .arg(arg)) << std::endl;
            return 1;
        }
    }
    return 0;
}

/*******************************************************************************
  Console messaging.
*******************************************************************************/

bool showDebugMessages = false;

#ifdef Q_WS_WIN

FILE *out_fd = 0;
#define DEBUG_STREAM (out_fd != 0 ? out_fd :\
    out_fd = fopen(QDir::homePath().append("/BibleTime Debug.txt").toLocal8Bit().data(),"w"))
#define CLOSE_DEBUG_STREAM { if (out_fd != 0) fclose(out_fd); }

#else

#define DEBUG_STREAM (stderr)
#define CLOSE_DEBUG_STREAM

#endif

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

/*******************************************************************************
  Handle Qt's meta type system.
*******************************************************************************/

void registerMetaTypes() {
    qRegisterMetaType<FilterOptions>();
    qRegisterMetaType<DisplayOptions>();
    qRegisterMetaTypeStreamOperators<BtBookshelfTreeModel::Grouping>("BtBookshelfTreeModel::Grouping");
}

} // anonymous namespace


/*******************************************************************************
  Program main entry point.
*******************************************************************************/

int main(int argc, char* argv[]) {
    namespace DU = util::directory;

    BibleTimeApp app(argc, argv); //for QApplication
    app.setApplicationName("bibletime");
    app.setApplicationVersion(BT_VERSION);

    // Parse command line arguments:
    bool ignoreSession = false;
    QString openBibleKey;
    int r = parseCommandLine(showDebugMessages, ignoreSession, openBibleKey);
    if (r != 0) {
        if (r < 0) return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }

#ifdef Q_WS_WIN
    // Use the default Qt message handler if --debug is not specified
    // This works with Visual Studio debugger Output Window
    if (showDebugMessages)
#endif
    qInstallMsgHandler(myMessageOutput);

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

    app.startInit();

#ifdef Q_WS_WIN
    // change directory to the Sword or .sword directory in the $HOME dir so that
    // the sword.conf is found. It points to the sword/locales.d directory
    QString homeSwordDir = util::directory::getUserHomeDir().absolutePath();
    QDir dir;
    dir.setCurrent(homeSwordDir);
#endif

#ifdef Q_WS_MAC
    // change to the user's sword dir containing the sword.conf config file, so that
    // Sword will correctly find it.
    QString homeSwordDir = util::directory::getUserHomeSwordDir().absolutePath();
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

//    setSignalHandler(signalHandler);

    BibleTime *mainWindow = new BibleTime();
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);

    // a new BibleTime version was installed (maybe a completely new installation)
    if (CBTConfig::get(CBTConfig::bibletimeVersion) != BT_VERSION) {
        CBTConfig::set(CBTConfig::bibletimeVersion, BT_VERSION);
        mainWindow->saveConfigSettings();
    }

    // restore the workspace and process command line options
    //app.setMainWidget(bibletime_ptr); //no longer used in qt4 (QApplication)
    mainWindow->show();

    // The following must be done after the bibletime window is visible:
    mainWindow->processCommandline(ignoreSession, openBibleKey);

#ifndef NO_DBUS
    new BibleTimeDBusAdaptor(mainWindow);
    // connect to D-Bus and register as an object:
    QDBusConnection::sessionBus().registerService("info.bibletime.BibleTime");
    QDBusConnection::sessionBus().registerObject("/BibleTime", mainWindow);
#endif

    r = app.exec();
    CLOSE_DEBUG_STREAM;
    return r;
}

