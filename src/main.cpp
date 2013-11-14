/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include <cstdlib>
#include <iostream>
#include <QDateTime>
#ifndef NO_DBUS
#include <QDBusConnection>
#endif
#include <QFile>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/btconfig.h"
#include "bibletime.h"
#include "bibletime_dbus_adaptor.h"
#include "bibletimeapp.h"
#include "frontend/searchdialog/btsearchoptionsarea.h" 
#include "util/directory.h"


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
        << qPrintable(QObject::tr("Open a clean session"))
        << std::endl << std::endl
        << "    --open-default-bible <ref>" << std::endl << "        "
        << qPrintable(QObject::tr("Open the default Bible with the "
                                  "reference <ref>"))
        << std::endl << std::endl
        << qPrintable(QObject::tr("For command-line arguments parsed by the"
                                  " Qt toolkit, see %1.")
               .arg("http://doc.qt.nokia.com/latest/qapplication.html"))
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
                        "Error: %1 expects an argument.")
                    .arg("--open-default-bible")) << ' '
                        << qPrintable(QObject::tr("See --help for details."))
                        << std::endl;
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

#if QT_VERSION >= 0x040600
QScopedPointer<QFile> debugStream;
#else
struct DebugStreamPtr {
    QFile * m_f;
    inline DebugStreamPtr() : m_f(0) {}
    inline ~DebugStreamPtr() { delete m_f; }
    inline void reset(QFile * f) { m_f = f; }
    inline QFile * operator->() const {
        Q_ASSERT(m_f);
        return m_f;
    }
} debugStream;
#endif
bool showDebugMessages = false;

void myMessageOutput(
        QtMsgType type,
#if QT_VERSION >= 0x050000
        const QMessageLogContext&,
        const QString& message ) {
    QByteArray msg = message.toLatin1();
#else
        const char *msg ) {
#endif
    // We use this messagehandler to switch debugging off in final releases
    switch (type) {
        case QtDebugMsg:
            if (showDebugMessages) { // Only show messages if they are enabled!
                debugStream->write("(BibleTime " BT_VERSION ") Debug: ");
                debugStream->write(msg);
                debugStream->write("\n");
                debugStream->flush();
            }
            break;
        case QtWarningMsg:
#ifndef QT_NO_DEBUG  // don't show in release builds so users don't get our debug warnings
            debugStream->write("(BibleTime " BT_VERSION ") WARNING: ");
            debugStream->write(msg);
            debugStream->write("\n");
            debugStream->flush();
#endif
            break;
        case QtCriticalMsg:
            debugStream->write("(BibleTime " BT_VERSION ") CRITICAL: ");
            debugStream->write(msg);
            debugStream->write("\nPlease report this bug at "
                               "http://bugs.bibletime.info/");
            debugStream->flush();
            break;
        case QtFatalMsg:
            debugStream->write("(BibleTime " BT_VERSION ") FATAL: ");
            debugStream->write(msg);
            debugStream->write("\nPlease report this bug at "
                               "http://bugs.bibletime.info/");

            // Dump core on purpose (see qInstallMsgHandler documentation):
            debugStream->close();
            abort();
    }
}

/*******************************************************************************
  Handle Qt's meta type system.
*******************************************************************************/

void registerMetaTypes() {
    qRegisterMetaType<FilterOptions>("FilterOptions");
    qRegisterMetaType<DisplayOptions>("DisplayOptions");
    qRegisterMetaTypeStreamOperators<BtBookshelfTreeModel::Grouping>("BtBookshelfTreeModel::Grouping");

    qRegisterMetaType<BTModuleTreeItem::Grouping>("Grouping");
    qRegisterMetaTypeStreamOperators<BTModuleTreeItem::Grouping>("Grouping");

    qRegisterMetaType<alignmentMode>("alignmentMode");
    qRegisterMetaTypeStreamOperators<alignmentMode>("alignmentMode");

    qRegisterMetaType<Search::BtSearchOptionsArea::SearchType>("SearchType");
    qRegisterMetaTypeStreamOperators<Search::BtSearchOptionsArea::SearchType>("SearchType");

    qRegisterMetaType<BtConfig::StringMap>("StringMap");
    qRegisterMetaTypeStreamOperators<BtConfig::StringMap>("StringMap");

    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
}

} // anonymous namespace


/*******************************************************************************
  Program main entry point.
*******************************************************************************/

int main(int argc, char* argv[]) {
    namespace DU = util::directory;

    BibleTimeApp app(argc, argv); //for QApplication

    // Parse command line arguments:
    bool ignoreSession = false;
    QString openBibleKey;
    int r = parseCommandLine(showDebugMessages, ignoreSession, openBibleKey);
    if (r != 0) {
        if (r < 0) return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }

    // Initialize random number generator:
    srand(qHash(QDateTime::currentDateTime().toString(Qt::ISODate)));

    // Setup debugging:
#ifdef Q_OS_WIN
    // Use the default Qt message handler if --debug is not specified
    // This works with Visual Studio debugger Output Window
    if (showDebugMessages) {
        debugStream.reset(new QFile(QDir::homePath().append("/BibleTime Debug.txt")));
        debugStream->open(QIODevice::WriteOnly | QIODevice::Text);
#if QT_VERSION >= 0x050000
        qInstallMessageHandler(myMessageOutput);
#else
        qInstallMsgHandler(myMessageOutput);
#endif
    }
#else
    debugStream.reset(new QFile);
    debugStream->open(stderr, QIODevice::WriteOnly | QIODevice::Text);
#if QT_VERSION >= 0x050000
        qInstallMessageHandler(myMessageOutput);
#else
        qInstallMsgHandler(myMessageOutput);
#endif
#endif

#ifdef Q_OS_WIN

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
    if (!app.initBtConfig()) {
        return EXIT_FAILURE;
    }

#ifdef Q_OS_WIN
    // change directory to the Sword or .sword directory in the $HOME dir so that
    // the sword.conf is found. It points to the sword/locales.d directory
    QString homeSwordDir = util::directory::getUserHomeDir().absolutePath();
    QDir dir;
    dir.setCurrent(homeSwordDir);
#endif

#ifdef Q_OS_MAC
    // change to the user's sword dir containing the sword.conf config file, so that
    // Sword will correctly find it.
    QString homeSwordDir = util::directory::getUserHomeSwordDir().absolutePath();
    QDir dir;
    dir.setCurrent(homeSwordDir);
#endif

    //first install QT's own translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name());
    app.installTranslator(&qtTranslator);
    //then our own
    QTranslator BibleTimeTranslator;
    BibleTimeTranslator.load( QString("bibletime_ui_").append(QLocale::system().name()), DU::getLocaleDir().canonicalPath());
    app.installTranslator(&BibleTimeTranslator);

    app.setProperty("--debug", QVariant(showDebugMessages));

    // Initialize display template manager:
    if (!app.initDisplayTemplateManager()) {
        qFatal("Error initializing display template manager!");
        return EXIT_FAILURE;
    }

    BibleTime *mainWindow = new BibleTime();
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);

    // a new BibleTime version was installed (maybe a completely new installation)
    if (btConfig().value<QString>("bibletimeVersion", BT_VERSION) != BT_VERSION) {
        btConfig().setValue("bibletimeVersion", QString(BT_VERSION));
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

    if (btConfig().value<bool>("GUI/showTipAtStartup", true))
        mainWindow->slotOpenTipDialog();

    r = app.exec();
    return r;
}

