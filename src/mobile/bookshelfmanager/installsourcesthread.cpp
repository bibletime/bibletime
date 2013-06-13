/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "installsourcesthread.h"

#include "backend/btinstallbackend.h"
#include "frontend/bookshelfmanager/btinstallmgr.h"
#include <QDebug>

namespace btm {


InstallSourcesThread::InstallSourcesThread(QObject *parent)
        : QThread(parent),
        done(false),
        m_cancelled(false) {
    m_iMgr = new BtInstallMgr();
}


InstallSourcesThread::~InstallSourcesThread() {
    delete m_iMgr;
}

void InstallSourcesThread::run() {
    refreshSourceList();
    QStringList sourceNames = BtInstallBackend::sourceNameList();
    refreshWorks(sourceNames);
}

void InstallSourcesThread::slotStopInstall() {

//    qDebug() << "*************************************\nBtInstallThread::slotStopInstall" << m_module << "\n********************************";
//    if (!done) {
//        done = true;
//        qDebug() << "*********************************\nBtInstallThread::slotStopInstall, installing" << m_module << "was cancelled\n**************************************";
//        m_iMgr->terminate();
//        //this->terminate(); // It's dangerous to forcibly stop, but we will clean up the files
//        //qApp->processEvents();
//        // wait to terminate for some secs. We rather let the execution go on and cleaning up to fail than the app to freeze
//        int notRun = this->wait(25000);
//        if (notRun) {
//            this->terminate();
//            this->wait(10000);
//            qDebug() << "installthread (" << m_module << ") terminated, delete m_iMgr";
//            delete m_iMgr; // this makes sure the ftp library will be cleaned up in the destroyer
//            m_iMgr = 0;
//        }
//    }
}

void InstallSourcesThread::slotManagerStatusUpdated(int /*totalProgress*/, int /*fileProgress*/) {
//    emit statusUpdated(m_module, totalProgress);
}

void InstallSourcesThread::slotDownloadStarted() {
//    emit downloadStarted(m_module);
}

void InstallSourcesThread::refreshSourceList() {
    int ret = m_iMgr->refreshRemoteSourceConfiguration();
    if ( !ret ) { //make sure the sources were updated sucessfully
        ;
    }
    else {
        qWarning("InstallMgr: getting remote list returned an error.");
    }
}

void InstallSourcesThread::refreshWorks(const QStringList& sourceNames) {
    int sourceCount = sourceNames.count();
    for (int i=0; i<sourceCount; ++i) {
        QString sourceName = sourceNames.at(i);
        int percent = 10 + 90 *((double)i/sourceCount);
        QString title = "Refreshing " + sourceName;
        emit percentComplete(percent, title);
        sword::InstallSource source = BtInstallBackend::source(sourceName);
        bool result = (m_iMgr->refreshRemoteSource(&source) == 0);
        if (result) {
            ;
        } else {
//        util::showWarning(this, tr("Warning"),
//                          tr("Failed to refresh source %1")
//                              .arg(QString(m_source.caption)));
        }
    }
    emit percentComplete(100, "Done");
}

}
