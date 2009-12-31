/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/btinstallmgr.h"

#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/instbackend.h"
#include <QDebug>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

// Sword includes:
#include <installmgr.h>
#include <ftptrans.h>


using namespace sword;

BtInstallMgr::BtInstallMgr()
        : InstallMgr(instbackend::configPath().toLatin1(), this),
        m_firstCallOfPreStatus(true) { //use this class also as status reporter
    qDebug() << "BtInstallMgr::BtInstallMgr";
    this->setFTPPassive(true);
}

BtInstallMgr::~BtInstallMgr() {
    //doesn't really help because it only sets a flag
    terminate(); //make sure to close the connection
}

bool BtInstallMgr::isUserDisclaimerConfirmed() const {
    // \todo Check from config if it's been confirmed with "don't show this anymore" checked.
    // Create a dialog with the message, checkbox and Continue/Cancel, Cancel as default.
    return true;
}

void BtInstallMgr::statusUpdate(double dltotal, double dlnow) {
    //qDebug() << "BtInstallMgr::statusUpdate";
    if (dlnow > dltotal)
        dlnow = dltotal;

    int totalPercent = (int)((float)(dlnow + m_completedBytes) / (float)(m_totalBytes) * 100.0);

    if (totalPercent > 100) {
        totalPercent = 100;
    }
    else if (totalPercent < 0) {
        totalPercent = 0;
    }

    int filePercent  = (int)((float)(dlnow) / (float)(dltotal + 1) * 100.0);
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


void BtInstallMgr::preStatus(long totalBytes, long completedBytes, const char* message) {
    if (m_firstCallOfPreStatus) {
        m_firstCallOfPreStatus = false;
        emit downloadStarted();
    }
    qDebug() << "BtInstallMgr::preStatus:" << (int)totalBytes << "/" << (int)completedBytes << QString(message);
    m_completedBytes = completedBytes;
    m_totalBytes = (totalBytes > 0) ? totalBytes : 1; //avoid division by zero
}
