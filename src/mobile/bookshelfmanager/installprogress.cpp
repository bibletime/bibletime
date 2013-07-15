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
    : QObject(parent), m_progressObject(0) {
}

void InstallProgress::openProgress(const QList<CSwordModuleInfo*>& modules) {
    if (modules.count() == 0)
        return;

    if (m_progressObject == 0)
        findProgressObject();
    if (m_progressObject == 0)
        return;

    QString destination = getSourcePath();
    if (destination.isEmpty())
        return;

    setProperties();

    Q_FOREACH(const CSwordModuleInfo *module, modules) {
        const QString sourceName(module->property("installSourceName").toString());
        // create a thread for this module
        BtInstallThread* thread = new BtInstallThread(module->name(), sourceName, destination);
        m_waitingThreads.insert(sourceName, thread);
        m_threadsByModule.insert(module->name(), thread);

        QObject::connect(thread, SIGNAL(installStopped(QString, QString)),
                         this, SLOT(slotOneItemStopped(QString, QString)), Qt::QueuedConnection);
        QObject::connect(thread, SIGNAL(installCompleted(QString, QString, int)),
                         this, SLOT(slotOneItemCompleted(QString, QString, int)), Qt::QueuedConnection);
        QObject::connect(thread, SIGNAL(statusUpdated(QString, int)),
                         this, SLOT(slotStatusUpdated(QString, int)), Qt::QueuedConnection);
        QObject::connect(thread, SIGNAL(downloadStarted(QString)),
                         this, SLOT(slotDownloadStarted(QString)), Qt::QueuedConnection);
        QObject::connect(thread, SIGNAL(preparingInstall(QString, QString)),
                         this, SLOT(slotInstallStarted(QString, QString)), Qt::QueuedConnection);
    }

    connect(m_progressObject, SIGNAL(cancel()), this, SLOT(slotStopInstall()));
    startThreads();
}

void InstallProgress::cancel() {
    m_progressObject->setProperty("visible", false);
}

void InstallProgress::close() {
    m_progressObject->setProperty("visible", false);
    CSwordBackend::instance()->reloadModules(CSwordBackend::AddedModules);
}

void InstallProgress::slotOneItemCompleted(QString module, QString source, int status) {
    QString message;
    //status comes from the sword installer.
    if (status != 0) {
        message = tr("Failed");
    }
    else {
        message = tr("Completed");
    }
    oneItemStoppedOrCompleted(module, source, message);
}

void InstallProgress::slotOneItemStopped(QString module, QString source) {
    oneItemStoppedOrCompleted(module, source, tr("Cancelled"));
}

// TODO show failed status
void InstallProgress::oneItemStoppedOrCompleted(QString module, QString source, QString statusMessage) {
    qDebug() << "\n**********************************\nBtInstallProgressDialog::oneItemStoppedOrCompleted" << module << statusMessage << "\n******************************************";
    qDebug() << "remove from threads maps" << source << m_threadsByModule.value(module);
    m_runningThreads.remove(source, m_threadsByModule.value(module));
    m_waitingThreads.remove(source, m_threadsByModule.value(module));

    //non-concurrent
    QMultiMap<QString, BtInstallThread*>::iterator threadIterator = m_waitingThreads.end();
    if (m_runningThreads.isEmpty() && threadIterator != m_waitingThreads.begin()) {
        --threadIterator; // the last item
        QString sourceName = threadIterator.key();
        BtInstallThread* t = threadIterator.value();
        m_runningThreads.insert(sourceName, t);
        threadIterator = m_waitingThreads.erase(threadIterator);
        t->start();
    }

    if (threadsDone()) {
        qDebug() << "close the dialog";
        close();
    }
}

void InstallProgress::slotStopInstall() {
    qDebug() << "BtInstallProgressDialog::slotStopInstall";

    // Clear the waiting threads map, stop all running threads.
    m_waitingThreads.clear();
    if (m_runningThreads.count() > 0) {
        foreach(BtInstallThread* thread, m_runningThreads) {
            thread->slotStopInstall();
        }
    }
    else {
        close();
    }
}

void InstallProgress::slotStatusUpdated(QString module, int status) {
    m_progressObject->setProperty("value", status);
}

void InstallProgress::slotInstallStarted(QString module, QString) {
}

void InstallProgress::slotDownloadStarted(QString module) {
    QString message = "Installing " + module;
    m_progressObject->setProperty("text", message);
    m_progressObject->setProperty("value", 0);
}

void InstallProgress::startThreads() {
    QMultiMap<QString, BtInstallThread*>::iterator threadIterator = m_waitingThreads.end();
    if (threadIterator != m_waitingThreads.begin()) {
        // go to the last item which is actually the first in the visible list
        // because the iterator is reversed compared to insert order
        --threadIterator;
        QString sourceName = threadIterator.key();
        BtInstallThread* t = threadIterator.value();
        m_runningThreads.insert(sourceName, t);
        threadIterator = m_waitingThreads.erase(threadIterator);
        t->start();
    }
}

bool InstallProgress::threadsDone() {
    return (m_waitingThreads.isEmpty() && m_runningThreads.isEmpty());
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
