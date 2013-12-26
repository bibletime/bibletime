/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "installprogress.h"

#include "backend/btinstallbackend.h"
#include "backend/managers/cswordbackend.h"
#include "backend/btinstallthread.h"
#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include <QQuickItem>
#include <QDebug>

namespace btm {

InstallProgress::InstallProgress(QObject* parent)
    : QObject(parent), m_progressObject(0),
      m_thread(0), m_nextInstallIndex(0) {
}

void InstallProgress::openProgress(const QList<CSwordModuleInfo*>& modules) {
    if (modules.count() == 0)
        return;

    m_nextInstallIndex = 0;
    m_modules = modules;
    if (m_progressObject == 0)
        findProgressObject();
    if (m_progressObject == 0)
        return;

    QString destination = getSourcePath();
    if (destination.isEmpty())
        return;

    setProperties();

    m_thread = new BtInstallThread(m_modules, destination, this);
    // Connect the signals between the dialog, items and threads
    bool ok = false;
    ok = connect(m_thread, SIGNAL(preparingInstall(int)),
            this,     SLOT(slotInstallStarted(int)),
            Qt::QueuedConnection);
    Q_ASSERT(ok);
    ok = connect(m_thread, SIGNAL(downloadStarted(int)),
            this,     SLOT(slotDownloadStarted(int)),
            Qt::QueuedConnection);
    Q_ASSERT(ok);
    ok = connect(m_thread, SIGNAL(statusUpdated(int, int)),
            this,     SLOT(slotStatusUpdated(int, int)),
            Qt::QueuedConnection);
    Q_ASSERT(ok);
    ok = connect(m_thread, SIGNAL(installCompleted(int, bool)),
            this,     SLOT(slotOneItemCompleted(int, bool)),
            Qt::QueuedConnection);
    Q_ASSERT(ok);
    ok = connect(m_thread, SIGNAL(finished()),
            this,     SLOT(slotThreadFinished()),
            Qt::QueuedConnection);
    Q_ASSERT(ok);
    m_thread->start();
}

void InstallProgress::cancel() {
    m_progressObject->setProperty("visible", false);
}

void InstallProgress::close() {
    m_progressObject->setProperty("visible", false);
    CSwordBackend::instance()->reloadModules(CSwordBackend::AddedModules);
}

void InstallProgress::slotStopInstall() {
    m_thread->stopInstall();
}

QString InstallProgress::getModuleName(int moduleIndex) {
    Q_ASSERT(moduleIndex < m_modules.count());
    CSwordModuleInfo * module = m_modules.at(moduleIndex);
    return module->name();
}

void InstallProgress::slotInstallStarted(int moduleIndex) {
    Q_ASSERT(moduleIndex == m_nextInstallIndex);
    m_nextInstallIndex++;
    QString message = "Installing " + getModuleName(moduleIndex);
    m_progressObject->setProperty("text", message);
}

void InstallProgress::slotDownloadStarted(int moduleIndex) {
    m_progressObject->setProperty("value", 0);
    QString message = "Downloading " + getModuleName(moduleIndex);
    m_progressObject->setProperty("text", message);
}

void InstallProgress::slotStatusUpdated(int /*moduleIndex*/, int status) {
    m_progressObject->setProperty("value", status);
}

void InstallProgress::slotOneItemCompleted(int /* moduleIndex */, bool /* successful */) {
    // TODO show failed status
}

void InstallProgress::slotThreadFinished() {
    close();
}

void InstallProgress::findProgressObject() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QQuickItem * rootObject = 0;
    if (viewer != 0)
        rootObject = viewer->rootObject();
    if (rootObject != 0)
        m_progressObject = rootObject->findChild<QQuickItem*>("progress");
}

void InstallProgress::setProperties() {
    m_progressObject->setProperty("visible", true);
    m_progressObject->setProperty("minimumValue", 0.0);
    m_progressObject->setProperty("maximumValue", 100.0);
    m_progressObject->setProperty("value", 0.0);
}

QString InstallProgress::getSourcePath() {
    QStringList targets = BtInstallBackend::targetList();
    for (QStringList::iterator it = targets.begin(); it != targets.end(); ++it)  {
        // Add the path only if it's writable
        QString sourcePath = *it;
        if (sourcePath.isEmpty())
            continue;
        QDir dir(sourcePath);
        if (!dir.exists())
            continue;
        if (!dir.isReadable())
            continue;
        QFileInfo fi( dir.canonicalPath());
        if (!fi.isWritable())
            continue;
        return sourcePath;
    }
    return QString();
}

}
