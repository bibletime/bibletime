/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "bibletimeapp.h"

#include <QDebug>
#include <QFile>
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "frontend/messagedialog.h"
#include "util/cresmgr.h"
#include "util/directory.h"


BibleTimeApp::BibleTimeApp(int &argc, char **argv)
    : QGuiApplication(argc, argv)
    , m_init(false) {
    setApplicationName("bibletime");
    setApplicationVersion(BT_VERSION);
}

BibleTimeApp::~BibleTimeApp() {
    // Prevent writing to the log file before the directory cache is init:
    if (!m_init || BtConfig::m_instance == 0)
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
    Q_ASSERT(m_init);

    return BtConfig::initBtConfig();
}

bool BibleTimeApp::initDisplayTemplateManager() {
    Q_ASSERT(m_init);

    QString errorMessage;
    new CDisplayTemplateMgr(errorMessage);
    if (errorMessage.isNull())
        return true;
    message::showCritical(0, tr("Fatal error!"), errorMessage);
    return false;
}
