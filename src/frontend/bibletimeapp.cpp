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
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <QByteArray>
#include <QColor>
#include <QDebug>
#ifdef Q_OS_WIN
#include <QDir>
#endif
#include <QFile>
#include <QIODevice>
#include <QMessageBox>
#include <QPalette>
#include <QString>
#include <Qt>
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


class QMessageLogContext;

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
        std::abort();
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
    if (m_debugMode) {
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

void BibleTimeApp::initLightDarkPalette() {
    enum LightDarkMode {
        systemDefault = 0,
        light = 1,
        dark = 2
    };

    int lightDarkMode = btConfig().value<int>("GUI/lightDarkMode", 0);
    if (lightDarkMode == LightDarkMode::systemDefault)
        return;
    QPalette p;
    if (lightDarkMode == LightDarkMode::dark) {
        p.setColor(QPalette::WindowText,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::Button,QColor(0x31, 0x36, 0x3b));
        p.setColor(QPalette::Light,QColor(0x18, 0x1b, 0x1d));
        p.setColor(QPalette::Midlight,QColor(0x25, 0x29, 0x2c));
        p.setColor(QPalette::Dark,QColor(0x62, 0x6c, 0x76));
        p.setColor(QPalette::Mid,QColor(0x41, 0x48, 0x4e));
        p.setColor(QPalette::Text,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::BrightText,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::ButtonText,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::Base,QColor(0x1b, 0x1e, 0x20));
        p.setColor(QPalette::Window,QColor(0x2a, 0x2e, 0x32));
        p.setColor(QPalette::Shadow,QColor(0x76, 0x76, 0x76));
        p.setColor(QPalette::Highlight,QColor(0x3d, 0xae, 0xe9));
        p.setColor(QPalette::HighlightedText,QColor(0xfc, 0xfc, 0xfc));
        p.setColor(QPalette::Link,QColor(0x1d, 0x99, 0xf3));
        p.setColor(QPalette::LinkVisited,QColor(0x9b, 0x59, 0xb6));
    } else {
        p.setColor(QPalette::WindowText,QColor(0x23, 0x26, 0x29));
        p.setColor(QPalette::Button,QColor(0xf7, 0xf7, 0xf7));
        p.setColor(QPalette::Light,QColor(0x0, 0x0, 0x0));
        p.setColor(QPalette::Midlight,QColor(0x0, 0x0, 0x0));
        p.setColor(QPalette::Dark,QColor(0x7b, 0x7b, 0x7b));
        p.setColor(QPalette::Mid,QColor(0xa5, 0xa5, 0xa5));
        p.setColor(QPalette::Text,QColor(0x23, 0x26, 0x29));
        p.setColor(QPalette::BrightText,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::ButtonText,QColor(0x23, 0x26, 0x29));
        p.setColor(QPalette::Base,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::Window,QColor(0xef, 0xf0, 0xf1));
        p.setColor(QPalette::Shadow,QColor(0x76, 0x76, 0x76));
        p.setColor(QPalette::Highlight,QColor(0x3d, 0xae, 0xe9));
        p.setColor(QPalette::HighlightedText,QColor(0xff, 0xff, 0xff));
        p.setColor(QPalette::Link,QColor(0x29, 0x80, 0xb9));
        p.setColor(QPalette::LinkVisited,QColor(0x9b, 0x59, 0xb6));
    }
    setPalette(p);
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

void BibleTimeApp::initIcons() { m_icons = new BtIcons(); }
