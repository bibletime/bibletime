/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallthread.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QThread>
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/btinstallmgr.h"
#include "backend/btinstallbackend.h"

// Sword includes:
#include <filemgr.h>


BtInstallThread::BtInstallThread(const QString &moduleName, const QString &sourceName,
                                 const QString &destinationName, QObject *parent)
        : QThread(parent),
        done(false),
        m_module(moduleName),
        m_destination(destinationName),
        m_source(sourceName),
        m_cancelled(false) {
    m_iMgr = new BtInstallMgr();
}


BtInstallThread::~BtInstallThread() {
    delete m_iMgr;
}

void BtInstallThread::run() {
    qDebug() << "****************************************\nBtInstallThread::run, mod:" << m_module << "\n************************************";


    emit preparingInstall(m_module, m_source);
    //This is 0 before set here - remember when using the value when cancelling
    // the installation before this has been run
    m_installSource = QSharedPointer<sword::InstallSource>(new sword::InstallSource(BtInstallBackend::source(m_source)));
    m_backendForSource = QSharedPointer<CSwordBackend>(BtInstallBackend::backend(*m_installSource));

    //make sure target/mods.d and target/modules exist
    /// \todo move this to some common precondition
    QDir dir(m_destination);
    if (!dir.exists()) {
        dir.mkdir(m_destination);
        qDebug() << "made directory" << m_destination;
    }
    if (!dir.exists("modules")) {
        dir.mkdir("modules");
        qDebug() << "made directory" << m_destination << "/modules";
    }
    if (!dir.exists("mods.d")) {
        dir.mkdir("mods.d");
        qDebug() << "made directory" << m_destination << "/mods.d";
    }

    QObject::connect(m_iMgr, SIGNAL(percentCompleted(int, int)), this, SLOT(slotManagerStatusUpdated(int, int)), Qt::QueuedConnection);
    QObject::connect(m_iMgr, SIGNAL(downloadStarted()), this, SLOT(slotDownloadStarted()), Qt::QueuedConnection);

    //check whether it's an update. If yes, remove existing module first
    /// \todo silently removing without undo if the user cancels the update is WRONG!!!
    removeModule();

    // manager for the destination path
    sword::SWMgr lMgr( m_destination.toLatin1() );

    if (BtInstallBackend::isRemote(*m_installSource)) {
        qDebug() << "calling install";
        int status = m_iMgr->installModule(&lMgr, 0, m_module.toLatin1(), m_installSource.data());
        if (status != 0) {
            qWarning() << "Error with install: " << status << "module:" << m_module;
        }
        else {
            done = true;
            emit installCompleted(m_module, m_source, status);
        }
    }
    else { //local source
        emit statusUpdated(m_module, 0);
        int status = m_iMgr->installModule(&lMgr, m_installSource->directory.c_str(), m_module.toLatin1());
        if (status > 0) {
            qWarning() << "Error with install: " << status << "module:" << m_module;
        }
        else if (status == -1) {
            // it was terminated, do nothing
        }
        else {
            emit statusUpdated(m_module, 100);
            done = true;
            emit installCompleted(m_module, m_source, status);
        }
    }
}

void BtInstallThread::slotStopInstall() {
    qDebug() << "*************************************\nBtInstallThread::slotStopInstall" << m_module << "\n********************************";
    if (!done) {
        done = true;
        qDebug() << "*********************************\nBtInstallThread::slotStopInstall, installing" << m_module << "was cancelled\n**************************************";
        m_iMgr->terminate();
        //this->terminate(); // It's dangerous to forcibly stop, but we will clean up the files
        qDebug() << "BtInstallThread::slotStopInstall 2";
        //qApp->processEvents();
        // wait to terminate for some secs. We rather let the execution go on and cleaning up to fail than the app to freeze
        int notRun = this->wait(25000);
        if (notRun) {
            this->terminate();
            this->wait(10000);
            qDebug() << "installthread (" << m_module << ") terminated, delete m_iMgr";
            delete m_iMgr; // this makes sure the ftp library will be cleaned up in the destroyer
            m_iMgr = 0;
        }
        qDebug() << "BtInstallThread::slotStopInstall 3";
        // cleanup: remove the module, remove the temp files
        // if installation has already started
        if (m_installSource.data() != 0) {
            qDebug() << "BtInstallThread::slotStopInstall 4";
            // remove the installed module, just to be sure because mgr may
            // have been terminated when copying files
            removeModule();
            removeTempFiles();
            qDebug() << "BtInstallThread::slotStopInstall will emit installStopped...";
        }
        emit installStopped(m_module, m_source);
    }
}

void BtInstallThread::slotManagerStatusUpdated(int totalProgress, int /*fileProgress*/) {
    //qDebug() << "BtInstallThread::slotManagerStatusUpdated";
    emit statusUpdated(m_module, totalProgress);
}

void BtInstallThread::slotDownloadStarted() {
    qDebug() << "BtInstallThread::slotDownloadStarted";
    emit downloadStarted(m_module);
}

void BtInstallThread::removeModule() {
    qDebug() << "BtInstallThread::removeModule start";
    CSwordModuleInfo* m;
    m = CSwordBackend::instance()->findModuleByName(m_module);
    if (!m) {
        m = BtInstallBackend::backend(BtInstallBackend::source(m_destination.toLatin1()))->findModuleByName(m_module);
    }
    if (m) { //module found?
        qDebug() << "BtInstallThread::removeModule, module" << m_module << "found";
        QString prefixPath = m->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
        QString dataPath = m->config(CSwordModuleInfo::DataPath);
        if (dataPath.left(2) == "./") {
            dataPath = dataPath.mid(2);
        }

        if (prefixPath.contains(dataPath)) {
            prefixPath.remove( prefixPath.indexOf(dataPath), dataPath.length() );
        }
        else {
            prefixPath = QString::fromLatin1(CSwordBackend::instance()->prefixPath);
        }

        sword::SWMgr mgr(prefixPath.toLatin1());
        BtInstallMgr iMgr;
        iMgr.removeModule(&mgr, m->name().toLatin1());
    }
    else {
        qDebug() << "BtInstallThread::removeModule, module" << m_module << "not found";
    }
}

void BtInstallThread::removeTempFiles() {
    qDebug() << "BtInstallThread::removeTempFiles start";

    // (take the remote conf file for this module, take DataPath,
    // take the absolute path of the InstallMgr)

    //sword::InstallSource is = BtInstallBackend::source(m_source);
    if (BtInstallBackend::isRemote(*m_installSource)) {
        // get the path for the module temp files
        CSwordModuleInfo* mInfo = m_backendForSource->findModuleByName(m_module);
        QString dataPath = mInfo->config(CSwordModuleInfo::AbsoluteDataPath);
        qDebug() << "Delete path:" << dataPath;
        // it's easier to use sword than qt
        sword::FileMgr::removeDir(dataPath.toLatin1().data());
    }
}
