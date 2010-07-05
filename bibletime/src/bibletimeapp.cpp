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
#include "backend/managers/cdisplaytemplatemgr.h"
#include "util/cresmgr.h"


BibleTimeApp::BibleTimeApp( int & argc, char ** argv ) : QApplication(argc, argv) {
//	initDCOP();
    CResMgr::init_tr();
}

BibleTimeApp::~BibleTimeApp() {

    //we can set this safely now because we close now (hopyfully without crash)
    CBTConfig::set(CBTConfig::crashedLastTime, false);
    CBTConfig::set(CBTConfig::crashedTwoTimes, false);

    CDisplayTemplateMgr::destroyInstance();
    CLanguageMgr::destroyInstance();
    CSwordBackend::destroyInstance();
}

/*
void BibleTimeApp::initDCOP() {
	const bool dcopOk = dcopClient()->attach();
	Q_ASSERT(dcopOk);
	if (dcopOk) {
		const Q3CString appId = dcopClient()->registerAs(kapp->name(), false);
		//   dcopClient()->setDefaultObject("BibleTimeInterface");
	}
}
*/
