/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btinstallthread.h"

#include <memory>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QThread>
#include "btinstallbackend.h"
#include "managers/cswordbackend.h"

// Sword includes:
#include <filemgr.h>


namespace {

inline bool runMkdir(QDir & dir, const QString & dirName) {
    if (!dir.exists(dirName)) {
        if (!dir.mkpath(dirName)) {
            qDebug() << "failed to make directory" << dirName;
            return false;
        }
        qDebug() << "made directory" << dirName;
    }
    return true;
}

}

void BtInstallThread::run() {
    // Make sure target/mods.d and target/modules exist
    /// \todo move this to some common precondition
    QDir dir(m_destination);
    if (!runMkdir(dir, m_destination)
        || !runMkdir(dir, "modules")
        || !runMkdir(dir, "mods.d"))
    {
        return;
    }

    for (m_currentModuleIndex = 0;
         m_currentModuleIndex < m_modules.size();
         ++m_currentModuleIndex)
    {
        installModule();
        if (m_stopRequested.load(std::memory_order_relaxed))
            break;
    }
}

void BtInstallThread::installModule() {
    emit preparingInstall(m_currentModuleIndex);

    const CSwordModuleInfo * const module = m_modules.at(m_currentModuleIndex);

    QVariant vModuleName = module->property("installSourceName");
    QString moduleName = vModuleName.toString();
    sword::InstallSource installSource = BtInstallBackend::source(moduleName);
    std::unique_ptr<CSwordBackend> backendForSource(BtInstallBackend::backend(installSource));

    // Check whether it's an update. If yes, remove existing module first:
    /// \todo silently removing without undo if the user cancels the update is WRONG!!!
    if (!removeModule() && m_stopRequested.load(std::memory_order_relaxed))
        return;

    // manager for the destination path
    sword::SWMgr lMgr(m_destination.toLatin1());
    if (BtInstallBackend::isRemote(installSource)) {
        int status = m_iMgr.installModule(&lMgr,
                                          nullptr,
                                          module->name().toLatin1(),
                                          &installSource);
        if (status == 0) {
            emit statusUpdated(m_currentModuleIndex, 100);
        } else {
            qWarning() << "Error with install: " << status
                       << "module:" << module->name();
        }
        emit installCompleted(m_currentModuleIndex, status == 0);
    } else { // Local source
        int status = m_iMgr.installModule(&lMgr,
                                          installSource.directory.c_str(),
                                          module->name().toLatin1());
        if (status == 0) {
            emit statusUpdated(m_currentModuleIndex, 100);
        } else if (status != -1) {
            qWarning() << "Error with install: " << status
                       << "module:" << module->name();
        }
        emit installCompleted(m_currentModuleIndex, status == 0);
    }
}

void BtInstallThread::slotManagerStatusUpdated(int totalProgress, int /*fileProgress*/) {
    emit statusUpdated(m_currentModuleIndex, totalProgress);
}

void BtInstallThread::slotDownloadStarted() {
    emit downloadStarted(m_currentModuleIndex);
}

bool BtInstallThread::removeModule() {
    CSwordModuleInfo * const installedModule = m_modules.at(m_currentModuleIndex);
    CSwordModuleInfo * m = CSwordBackend::instance()->findModuleByName(installedModule->name());
    if (!m)
        m = BtInstallBackend::backend(BtInstallBackend::source(m_destination.toLatin1()))->findModuleByName(installedModule->name());

    if (!m)
        return false;

    qDebug() << "Removing module" << installedModule->name();
    QString prefixPath = m->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
    QString dataPath = m->config(CSwordModuleInfo::DataPath);
    if (dataPath.left(2) == "./")
        dataPath = dataPath.mid(2);

    if (prefixPath.contains(dataPath)) {
        prefixPath.remove(prefixPath.indexOf(dataPath), dataPath.length());
    } else {
        prefixPath = QString::fromLatin1(CSwordBackend::instance()->prefixPath);
    }

    sword::SWMgr mgr(prefixPath.toLatin1());
    BtInstallMgr().removeModule(&mgr, m->name().toLatin1());
    return true;
}
