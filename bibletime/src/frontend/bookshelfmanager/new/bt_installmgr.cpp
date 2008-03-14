/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

//BibleTime includes
#include "bt_installmgr.h"
#include "bt_installmgr.moc"

#include "frontend/bookshelfmanager/new/backend.h"

#include "backend/managers/cswordbackend.h"

//#include "util/cpointers.h"
//#include "util/directoryutil.h"

//Qt includes
//#include <QFile>
//#include <QFileInfo>
//#include <QDir>
#include <QDebug>
//#include <QApplication>
//Sword includes
//#include <filemgr.h>
//#include <swconfig.h>
//#include <swbuf.h>

//Stl includes
//#include <functional>
//#include <map>
//#include <utility>

using namespace sword;


Bt_InstallMgr::Bt_InstallMgr()
	: InstallMgr(backend::configPath().toLatin1(), this),
	m_firstCallOfPreStatus(true)
{ //use this class also as status reporter
	qDebug("Bt_InstallMgr::Bt_InstallMgr");
	this->setFTPPassive(true);
}

Bt_InstallMgr::~Bt_InstallMgr() {
	terminate(); //make sure to close the connection
}

void Bt_InstallMgr::statusUpdate(double dltotal, double dlnow)
{
	//qDebug("Bt_InstallMgr::statusUpdate");
	if (dlnow > dltotal)
		dlnow = dltotal;

	int totalPercent = (int)((float)(dlnow + m_completedBytes) / (float)(m_totalBytes) * 100.0);

	if (totalPercent > 100) {
		totalPercent = 100;
	}
	else if (totalPercent < 0) {
		totalPercent = 0;
	}

	int filePercent  = (int)((float)(dlnow) / (float)(dltotal+1) * 100.0);
	if (filePercent > 100) {
		filePercent = 100;
	}
	else if (filePercent < 0) {
		filePercent = 0;
	}
	//qApp->processEvents();
	//qDebug() << "status: total"<<totalPercent<<"file"<<filePercent;
	emit percentCompleted(totalPercent, filePercent);
}


void Bt_InstallMgr::preStatus(long totalBytes, long completedBytes, const char* message)
{
	if (m_firstCallOfPreStatus) {
		m_firstCallOfPreStatus = false;
		emit downloadStarted();
	}
	qDebug() << "Bt_InstallMgr::preStatus:" << (int)totalBytes << "/" << (int)completedBytes << QString(message);
	m_completedBytes = completedBytes;
	m_totalBytes = (totalBytes > 0) ? totalBytes : 1; //avoid division by zero
}
