/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "installprogress.h"

#include <QDebug>
#include <QQuickItem>
#include "backend/btinstallbackend.h"
#include "backend/managers/cswordbackend.h"
#include "backend/btinstallthread.h"
#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include "util/btassert.h"
#include "util/btconnect.h"


namespace btm {

InstallProgress::InstallProgress(QObject* parent)
    : QObject(parent), m_progressObject(nullptr),
      m_thread(nullptr), m_nextInstallIndex(0) {
}

void InstallProgress::openProgress(const QList<CSwordModuleInfo*>& modules) {
    if (modules.count() == 0)
        return;

    m_nextInstallIndex = 0;
    m_modules = modules;
    if (m_progressObject == nullptr)
        findProgressObject();
    if (m_progressObject == nullptr)
        return;

    QString destination = getSourcePath();
    if (destination.isEmpty())
        return;

    setProperties();

    m_thread = new BtInstallThread(m_modules, destination, this);
    // Connect the signals between the dialog, items and threads
    BT_CONNECT(m_thread, SIGNAL(preparingInstall(int)),
               this,     SLOT(slotInstallStarted(int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(downloadStarted(int)),
               this,     SLOT(slotDownloadStarted(int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(statusUpdated(int, int)),
               this,     SLOT(slotStatusUpdated(int, int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(installCompleted(int, bool)),
               this,     SLOT(slotOneItemCompleted(int, bool)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(finished()),
               this,     SLOT(slotThreadFinished()),
               Qt::QueuedConnection);
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
    BT_ASSERT(moduleIndex < m_modules.count());
    CSwordModuleInfo * module = m_modules.at(moduleIndex);
    return module->name();
}

void InstallProgress::slotInstallStarted(int moduleIndex) {
    BT_ASSERT(moduleIndex == m_nextInstallIndex);
    m_nextInstallIndex++;
    m_progressObject->setProperty(
                "text",
                tr("Installing %1").arg(getModuleName(moduleIndex)));
}

void InstallProgress::slotDownloadStarted(int moduleIndex) {
    m_progressObject->setProperty("value", 0);
    m_progressObject->setProperty(
                "text",
                tr("Downloading %1").arg(getModuleName(moduleIndex)));
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
    QQuickItem * rootObject = nullptr;
    if (viewer != nullptr)
        rootObject = viewer->rootObject();
    if (rootObject != nullptr)
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
