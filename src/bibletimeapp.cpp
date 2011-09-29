/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletimeapp.h"

#include <QMessageBox>
#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "util/cresmgr.h"


BibleTimeApp::~BibleTimeApp() {
    // Prevent writing to the log file before the directory cache is init:
    if (!m_init) return;

    //we can set this safely now because we close now (hopyfully without crash)
    btConfig().setValue("state/crashedLastTime", false);
    btConfig().setValue("state/crashedTwoTimes", false);

    delete CDisplayTemplateMgr::instance();
    CLanguageMgr::destroyInstance();
    CSwordBackend::destroyInstance();
}

bool BibleTimeApp::initDisplayTemplateManager() {
    QString errorMessage;
    new CDisplayTemplateMgr(errorMessage);
    if (errorMessage.isNull()) return true;
    QMessageBox::critical(0, tr("Fatal error!"), errorMessage);
    return false;
}
