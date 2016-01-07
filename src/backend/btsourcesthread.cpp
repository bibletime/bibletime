/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btsourcesthread.h"

#include <QString>
#include "btinstallbackend.h"
#include "backend/btinstallmgr.h"

BtSourcesThread::BtSourcesThread(QObject *parent)
    : QThread(parent),
      m_stop(false) {
}

void BtSourcesThread::run() {
    m_stop = false;
    updateRemoteLibraries();
}

void BtSourcesThread::stop() {
    m_stop = true;
}

void BtSourcesThread::updateRemoteLibraries() {
    emit percentComplete(0);
    emit showMessage(tr("Getting Library List"));
    updateRemoteLibrariesList();
    emit percentComplete(10);

    if (m_stop)
        return;

    updateRemoteLibraryWorks();
    emit finished();
}

void BtSourcesThread::updateRemoteLibrariesList() {
    BtInstallMgr iMgr;
    int ret = iMgr.refreshRemoteSourceConfiguration();
    if (ret ) {
        qWarning("InstallMgr: getting remote list returned an error.");
    }
}

void BtSourcesThread::updateRemoteLibraryWorks() {
    QStringList sourceNames = BtInstallBackend::sourceNameList();
    BtInstallMgr iMgr;
    int sourceCount = sourceNames.count();
    for (int i=0; i<sourceCount; ++i) {
        if (m_stop) {
            emit showMessage(tr("Updating stopped"));
            return;
        }
        QString sourceName = sourceNames.at(i);

        QString label = tr("Updating remote library") + " \"" + sourceName +"\"";
        emit showMessage(label);

        sword::InstallSource source = BtInstallBackend::source(sourceName);
        bool result = (iMgr.refreshRemoteSource(&source) == 0);
        if (result) {
            int percent = 10 + 90 * ( (i+1.0) / sourceCount );
            emit percentComplete(percent);
        } else {
            emit showMessage(tr("Error updating remote libraries."));
            return;
        }
    }
    emit percentComplete(100);
    emit showMessage(tr("Remote libraries have been updated."));
}
