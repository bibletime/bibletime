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

#include "bibletimeapp.h"

#ifdef Q_OS_WIN
#include <array>
#endif
#include <memory>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QString>
#include <QtGlobal>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include "../backend/config/btconfig.h"
#include "../backend/managers/cswordbackend.h"
#include "../backend/managers/cdisplaytemplatemgr.h"
#include "../backend/managers/colormanager.h"
#include "../util/btassert.h"
#include "../util/bticons.h"
#include "messagedialog.h"


namespace {

std::unique_ptr<QFile> debugStream;

void myMessageOutput(QtMsgType type,
                     QMessageLogContext const &,
                     QString const & message)
{
    QByteArray msg = message.toLatin1();
    switch (type) {
    case QtDebugMsg:
        if (btApp->debugMode()) { // Only show messages if they are enabled!
            debugStream->write("(BibleTime " BT_VERSION ") Debug: ");
            debugStream->write(msg);
            debugStream->write("\n");
            debugStream->flush();
        }
        break;
    case QtInfoMsg:
        debugStream->write("(BibleTime " BT_VERSION ") INFO: ");
        debugStream->write(msg);
        debugStream->write("\n");
        debugStream->flush();
        break;
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

} // anonymous namespace

BibleTimeApp::BibleTimeApp(int &argc, char **argv)
    : QApplication(argc, argv)
    , m_init(false)
    , m_debugMode(qgetenv("BIBLETIME_DEBUG") == QByteArray("1"))
    , m_icons(nullptr)
{
    setApplicationName("bibletime");
    setApplicationVersion(BT_VERSION);

    // Support for retina displays
    this->setAttribute(Qt::AA_UseHighDpiPixmaps);

    #ifdef Q_OS_WIN
    // On Windows, add a path for Qt plugins to be loaded from:
    addLibraryPath(applicationDirPath() + "/plugins");

    // Must set HOME var on Windows:
    // getenv and qgetenv don't work right on Windows with unicode characters
    std::array<wchar_t, 4096u> homeDir;
    GetEnvironmentVariable(TEXT("APPDATA"), homeDir.data(), homeDir.size());
    SetEnvironmentVariable(TEXT("HOME"), homeDir.data());
    #endif

    // Setup message handler:
    #ifdef Q_OS_WIN
    // Use the default Qt message handler if --debug is not specified
    // This works with Visual Studio debugger Output Window
    if (showDebugMessages) {
        debugStream.reset(
                    new QFile(QDir::homePath().append("/BibleTime Debug.txt")));
        debugStream->open(QIODevice::WriteOnly | QIODevice::Text);
        qInstallMessageHandler(myMessageOutput);
    }
    #else
    debugStream.reset(new QFile);
    debugStream->open(stderr, QIODevice::WriteOnly | QIODevice::Text);
    qInstallMessageHandler(myMessageOutput);
    #endif
}

BibleTimeApp::~BibleTimeApp() {
    // Prevent writing to the log file before the directory cache is init:
    if (!m_init || BtConfig::m_instance == nullptr)
        return;

    //we can set this safely now because we close now (hopyfully without crash)
    btConfig().setValue("state/crashedLastTime", false);
    btConfig().setValue("state/crashedTwoTimes", false);

    delete CDisplayTemplateMgr::instance();
    CSwordBackend::destroyInstance();
    delete m_icons;

    BtConfig::destroyInstance();
}

bool BibleTimeApp::initBtConfig() {
    BT_ASSERT(m_init);

    BtConfig::InitState const r = BtConfig::initBtConfig();
    if (r == BtConfig::INIT_OK)
        return true;
    if (r == BtConfig::INIT_NEED_UNIMPLEMENTED_FORWARD_MIGRATE) {
        /// \todo Migrate from btConfigOldApi to BTCONFIG_API_VERSION
        qWarning() << "BibleTime configuration migration is not yet implemented!!!";
        if (message::showWarning(
                    nullptr,
                    tr("Warning!"),
                    tr("Migration to the new configuration system is not yet "
                       "implemented. Proceeding might result in <b>loss of data"
                       "</b>. Please backup your configuration files before "
                       "you continue!<br/><br/>Do you want to continue? Press "
                       "\"No\" to quit BibleTime immediately."),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No) == QMessageBox::No)
            return false;
    } else {
        BT_ASSERT(r == BtConfig::INIT_NEED_UNIMPLEMENTED_BACKWARD_MIGRATE);
        if (message::showWarning(
                    nullptr,
                    tr("Error loading configuration!"),
                    tr("Failed to load BibleTime's configuration, because it "
                       "appears that the configuration file corresponds to a "
                       "newer version of BibleTime. This is likely caused by "
                       "BibleTime being downgraded. Loading the new "
                       "configuration file may result in <b>loss of data</b>."
                       "<br/><br/>Do you still want to try to load the new "
                       "configuration file? Press \"No\" to quit BibleTime "
                       "immediately."),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No) == QMessageBox::No)
            return false;
    }
    BtConfig::forceMigrate();
    return true;
}

bool BibleTimeApp::initDisplayTemplateManager() {
    BT_ASSERT(m_init);

    QString errorMessage;
    new CDisplayTemplateMgr(errorMessage);
    if (errorMessage.isNull())
        return true;
    message::showCritical(nullptr, tr("Fatal error!"), errorMessage);
    return false;
}

void BibleTimeApp::initColorManager()
{ ColorManager::instance().loadColorMaps(); }

void BibleTimeApp::initIcons() { m_icons = new BtIcons(); }
