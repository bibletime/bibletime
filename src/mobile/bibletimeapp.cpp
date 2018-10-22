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

#include "bibletimeapp.h"
#include "btmmain.h"

#include <QDebug>
#include <QFile>
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "frontend/messagedialog.h"
#include "util/btassert.h"
#include "util/cresmgr.h"


BibleTimeApp::BibleTimeApp(int &argc, char **argv)
    : QGuiApplication(argc, argv)
    , m_init(false)
    , m_debugMode(false)
{
    setApplicationName("bibletime");
    setApplicationVersion(BT_VERSION);
    connect(this, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this, SLOT(applicationStateChanged(Qt::ApplicationState)));
}

BibleTimeApp::~BibleTimeApp() {
    // Prevent writing to the log file before the directory cache is init:
    if (!m_init || BtConfig::m_instance == nullptr)
        return;

    //we can set this safely now because we close now (hopyfully without crash)
    btConfig().setValue("state/crashedLastTime", false);
    btConfig().setValue("state/crashedTwoTimes", false);

    delete CDisplayTemplateMgr::instance();

    CLanguageMgr::destroyInstance();
    CSwordBackend::destroyInstance();

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
    CDisplayTemplateMgr::instance()->setMultiModuleHeadersVisible(false);
    if (errorMessage.isNull())
        return true;
    message::showCritical(nullptr, tr("Fatal error!"), errorMessage);
    return false;
}

void BibleTimeApp::applicationStateChanged(Qt::ApplicationState state) {
    if (state == Qt::ApplicationSuspended)
        saveSession();
}
