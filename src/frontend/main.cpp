/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include <iostream>
#include <QDateTime>
#include <QLibraryInfo>
#include <QLocale>
#include <QQmlEngine>
#include <QTranslator>
#include "../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../backend/config/btconfig.h"
#include "../backend/managers/cswordbackend.h"
#include "../util/directory.h"
#include "bibletime.h"
#include "bibletimeapp.h"
#include "display/modelview/btqmlinterface.h"
#include "welcome/btwelcomedialog.h"

// Sword includes:
#include <swmgr.h>

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
  \param[out] ignoreSession Whether --ignore-session was specified.
  \param[out] openBibleKey Will be set to --open-default-bible if specified.
  \retval -1 Parsing was successful, the application should exit with
             EXIT_SUCCESS.
  \retval 0 Parsing was successful.
  \retval 1 Parsing failed, the application should exit with EXIT_FAILURE.
*/
int parseCommandLine(bool & showDebugMessages,
                     bool & ignoreSession,
                     QString & openBibleKey)
{
    QStringList args = BibleTimeApp::arguments();
    for (int i = 1; i < args.size(); i++) {
        const QString &arg = args.at(i);
        if (arg == QStringLiteral("--help")
                || arg == QStringLiteral("-h")
                || arg == QStringLiteral("/?")
                || arg == QStringLiteral("/h"))
        {
            printHelp(args.at(0));
            return -1;
        } else if (arg == QStringLiteral("--version")
                   || arg == QStringLiteral("-V")
                   || arg == QStringLiteral("/V"))
        {
            std::cout << "BibleTime " BT_VERSION << std::endl;
            return -1;
        } else if (arg == QStringLiteral("--debug")) {
            showDebugMessages = true;
        } else if (arg == QStringLiteral("--ignore-session")) {
            ignoreSession = true;
        } else if (arg == QStringLiteral("--open-default-bible")) {
            i++;
            if (i < args.size()) {
                openBibleKey = args.at(i);
            } else {
                std::cerr
                        << qPrintable(
                               QObject::tr("Error: %1 expects an argument. See "
                                           "--help for details.")
                               .arg(QStringLiteral("--open-default-bible")))
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
    qRegisterMetaType<alignmentMode>("alignmentMode");
    qRegisterMetaType<CSwordModuleSearch::SearchType>("SearchType");
    qRegisterMetaType<BtConfig::StringMap>("StringMap");
    qRegisterMetaType<QList<int> >("QList<int>");
    qRegisterMetaType<BtBookshelfTreeModel::Grouping>(
        "BtBookshelfTreeModel::Grouping");

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    qRegisterMetaTypeStreamOperators<BtBookshelfTreeModel::Grouping>(
        "BtBookshelfTreeModel::Grouping");
    qRegisterMetaTypeStreamOperators<alignmentMode>("alignmentMode");
    qRegisterMetaTypeStreamOperators<CSwordModuleSearch::SearchType>(
        "SearchType");
    qRegisterMetaTypeStreamOperators<BtConfig::StringMap>("StringMap");
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
#endif

    BtQmlInterface::typeId =
        qmlRegisterSingletonType<BtQmlInterface>(
            "BibleTime",
            1,
            0,
            "BtQmlInterface",
            [](QQmlEngine *, QJSEngine *) { return new BtQmlInterface(); });
}

} // anonymous namespace


/*******************************************************************************
  Program main entry point.
*******************************************************************************/

int main(int argc, char* argv[]) {
    namespace DU = util::directory;

    if (!sword::SWMgr::isICU) {
        qFatal("SWORD library is required to be built against ICU!");
        return EXIT_FAILURE;
    }

    registerMetaTypes();

    BibleTimeApp app(argc, argv); //for QApplication

    // Parse command line arguments:
    bool ignoreSession = false;
    QString openBibleKey;
    {
        bool showDebugMessages = false;
        if (int const r = parseCommandLine(showDebugMessages,
                                           ignoreSession,
                                           openBibleKey))
            return r < 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        app.setDebugMode(showDebugMessages);
    }

    if (!DU::initDirectoryCache()) {
        qFatal("Error initializing directory cache!");
        return EXIT_FAILURE;
    }

    app.startInit();
    if (!app.initBtConfig()) {
        return EXIT_FAILURE;
    }

    app.initLightDarkPalette();

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
    QLocale const defaultLocale;
    QTranslator qtTranslator;
    if (qtTranslator.load(defaultLocale,
                          QStringLiteral("qt_"),
                          QString(),
                          #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
                          QLibraryInfo::location(
                          #else
                          QLibraryInfo::path(
                          #endif
                              QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslator);
    //then our own
    QTranslator bibleTimeTranslator;
    if (bibleTimeTranslator.load(
                defaultLocale,
                QStringLiteral("bibletime_ui_"),
                QString(),
                DU::getLocaleDir().canonicalPath()))
        app.installTranslator(&bibleTimeTranslator);

    // Initialize display template manager:
    if (!app.initDisplayTemplateManager()) {
        qFatal("Error initializing display template manager!");
        return EXIT_FAILURE;
    }

    app.initIcons();

    auto * const mainWindow = new BibleTime(app);
    mainWindow->setAttribute(Qt::WA_DeleteOnClose);

    // a new BibleTime version was installed (maybe a completely new installation)
    if (btConfig().value<QString>(QStringLiteral("bibletimeVersion"),
                                  BT_VERSION) != BT_VERSION)
    {
        btConfig().setValue(QStringLiteral("bibletimeVersion"),
                            QString(BT_VERSION));
        mainWindow->saveConfigSettings();
    }

    auto const showWelcome = CSwordBackend::instance().moduleList().empty();
    if (showWelcome && BtWelcomeDialog().exec() == QDialog::Accepted)
        mainWindow->slotBookshelfWizard();

    mainWindow->show();

    // The following must be done after the bibletime window is visible:
    mainWindow->processCommandline(ignoreSession, openBibleKey);

    if (!showWelcome
        && btConfig().value<bool>(QStringLiteral("GUI/showTipAtStartup"), true))
        mainWindow->slotOpenTipDialog();

    return app.exec();
}

