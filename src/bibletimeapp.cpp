/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletimeapp.h"

#include "backend/config/cbtconfig.h"
#include "util/cresmgr.h"


BibleTimeApp::~BibleTimeApp() {
    // Prevent writing to the log file before the directory cache is init:
    if (!m_init) return;

    //we can set this safely now because we close now (hopyfully without crash)
    CBTConfig::set(CBTConfig::crashedLastTime, false);
    CBTConfig::set(CBTConfig::crashedTwoTimes, false);

    deleteDisplayTemplateMgr();
    deleteLanguageMgr();
    deleteBackend();
}
