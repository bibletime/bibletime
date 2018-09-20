/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include <cstdlib>
#include <iostream>
#include <memory>
#include <QDateTime>
#include <QFile>
#include <QLibraryInfo>
#include <QLocale>
#include <QQmlEngine>
#include <QTextCodec>
#include <QTranslator>
#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/bookshelfmodel/btbookshelftreemodel.h"
#include "backend/config/btconfig.h"
#include "bibletime.h"
#include "bibletimeapp.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"
#include "frontend/display/modelview/btqmlinterface.h"
#include "frontend/welcome/btwelcomedialog.h"
#include "util/directory.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

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
  Console messaging.
*******************************************************************************/

std::unique_ptr<QFile> debugStream;
bool showDebugMessages = false;

void myMessageOutput(
        QtMsgType type,
        const QMessageLogContext&,
        const QString& message ) {
    QByteArray msg = message.toLatin1();
    switch (type) {
    case QtDebugMsg:
        if (showDebugMessages) { // Only show messages if they are enabled!
            debugStream->write("(BibleTime " BT_VERSION ") Debug: ");
            debugStream->write(msg);
            debugStream->write("\n");
            debugStream->flush();
        }
        break;
#if QT_VERSION >= 0x050500
    case QtInfoMsg:
        debugStream->write("(BibleTime " BT_VERSION ") INFO: ");
        debugStream->write(msg);
        debugStream->write("\n");
        debugStream->flush();
        break;
#endif
    case QtWarningMsg:
        debugStream->write("(BibleTime " BT_VERSION ") WARNING: ");
        debugStream->write(msg);
        debugStream->write("\n");
        debugStream->flush();
        break;
    case QtCriticalMsg:
        debugStream->write("(BibleTime " BT_VERSION ") CRITICAL: ");
        debugStream->write(msg);
        debugStream->write("\nPlease report this bug at "
                           "https://github.com/bibletime/bibletime/issues"
                           "\n");
        debugStream->flush();
        break;
    case QtFatalMsg:
        debugStream->write("(BibleTime " BT_VERSION ") FATAL: ");
        debugStream->write(msg);
        debugStream->write("\nPlease report this bug at "
                           "https://github.com/bibletime/bibletime/issues"
                           "\n");

        // Dump core on purpose (see qInstallMsgHandler documentation):
        debugStream->close();
        abort();
    }
}

inline void installMessageHandler() {
    qInstallMessageHandler(myMessageOutput);
}

/*******************************************************************************
  Parsing command-line arguments
*******************************************************************************/

/**
  Parses all command-line arguments.
  \param[out] ignoreSession Whether --ignore-session was specified.
  \param[out] openBibleKey Will be set to --open-default-bible if specified.
  \retval -1 Parsing was successful, the application should exit with
             EXIT_SUCCESS.
  \retval 0 Parsing was successful.
  \retval 1 Parsing failed, the application should exit with EXIT_FAILURE.
*/
int parseCommandLine(bool & ignoreSession, QString & openBibleKey) {
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
                   || arg == "-V"
                   || arg == "/V")
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

    qRegisterMetaType<CSwordModuleSearch::SearchType>("SearchType");
    qRegisterMetaTypeStreamOperators<CSwordModuleSearch::SearchType>("SearchType");

    qRegisterMetaType<BtConfig::StringMap>("StringMap");
    qRegisterMetaTypeStreamOperators<BtConfig::StringMap>("StringMap");

    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");

    // for Qml interface
    qmlRegisterType<BtQmlInterface>("BibleTime", 1, 0, "BtQmlInterface");
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
    if (int const r = parseCommandLine(ignoreSession, openBibleKey))
        return r < 0 ? EXIT_SUCCESS : EXIT_FAILURE;

    // Initialize random number generator:
    srand(qHash(QDateTime::currentDateTime().toString(Qt::ISODate)));

    // Setup debugging:
#ifdef Q_OS_WIN
    // Use the default Qt message handler if --debug is not specified
    // This works with Visual Studio debugger Output Window
    if (showDebugMessages) {
        debugStream.reset(new QFile(QDir::homePath().append("/BibleTime Debug.txt")));
        debugStream->open(QIODevice::WriteOnly | QIODevice::Text);
        installMessageHandler();
    }
#else
    debugStream.reset(new QFile);
    debugStream->open(stderr, QIODevice::WriteOnly | QIODevice::Text);
    installMessageHandler();
#endif

#ifdef Q_OS_WIN

    // On Windows, add a path for Qt plugins to be loaded from
    app.addLibraryPath(app.applicationDirPath() + "/plugins");

    // Must set HOME var on Windows
    // getenv and qgetenv don't work right on Windows with unicode characters
#define BUFSIZE 4096
    wchar_t homeDir[BUFSIZE];
    GetEnvironmentVariable(TEXT("APPDATA"), homeDir, BUFSIZE);
    SetEnvironmentVariable(TEXT("HOME"), homeDir);
#endif

    registerMetaTypes();

    if (!DU::initDirectoryCache()) {
        qFatal("Error initializing directory cache!");
        return EXIT_FAILURE;
    }

    app.startInit(showDebugMessages);
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
    if (qtTranslator.load("qt_" + QLocale::system().name(),
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslator);
    //then our own
    QTranslator BibleTimeTranslator;
    if (BibleTimeTranslator.load(
                QString("bibletime_ui_").append(QLocale::system().name()),
                DU::getLocaleDir().canonicalPath()))
        app.installTranslator(&BibleTimeTranslator);

    // Initialize display template manager:
    if (!app.initDisplayTemplateManager()) {
        qFatal("Error initializing display template manager!");
        return EXIT_FAILURE;
    }

    app.initIcons();

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

    BtBookshelfModel *bookshelfModel = CSwordBackend::instance()->model();
    if (bookshelfModel->moduleList().empty())
        BtWelcomeDialog::openWelcome();
    else if (btConfig().value<bool>("GUI/showTipAtStartup", true))
        mainWindow->slotOpenTipDialog();

    return app.exec();
}

