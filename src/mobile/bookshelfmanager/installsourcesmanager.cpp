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

#include "installsourcesmanager.h"

#include <QDebug>
#include <QQuickItem>
#include <QThread>
#include "backend/btinstallbackend.h"
#include "mobile/bookshelfmanager/installsources.h"
#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include "util/btassert.h"
#include "util/btconnect.h"


namespace btm {

InstallSourcesManager::InstallSourcesManager(QObject* /* parent */)
    : m_worker(nullptr) {
}

InstallSourcesManager::~InstallSourcesManager() {
}

void InstallSourcesManager::refreshSources() {
    findProgressObject();
    BT_ASSERT(m_progressObject);
    if (m_progressObject == nullptr)
        return;
    m_progressObject->disconnect(this);
    BT_CONNECT(m_progressObject, SIGNAL(cancel()), this, SLOT(cancel()));

    m_progressObject->setProperty("minimumValue", 0.0);
    m_progressObject->setProperty("maximumValue", 100.0);
    m_progressObject->setProperty("value", 0.0);
    m_progressObject->setProperty("visible", true);
    m_progressObject->setProperty("text", tr("Refreshing Source List"));

    runThread();
}

void InstallSourcesManager::cancel() {
    m_worker->cancel();
    m_progressObject->setProperty("visible", false);
}

void InstallSourcesManager::runThread() {
    QThread* thread = new QThread;
    m_worker = new InstallSources();
    m_worker->moveToThread(thread);
//    BT_CONNECT(m_worker, SIGNAL(error(QString)),
//               this,     SLOT(errorString(QString)));
    BT_CONNECT(thread, SIGNAL(started()), m_worker, SLOT(process()));
    BT_CONNECT(m_worker, SIGNAL(finished()), thread, SLOT(quit()));
    BT_CONNECT(m_worker, SIGNAL(finished()), m_worker, SLOT(deleteLater()));
    BT_CONNECT(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    BT_CONNECT(m_worker, SIGNAL(percentComplete(int, QString const &)),
               this,     SLOT(percentComplete(int, QString const &)));
    thread->start();
}

void InstallSourcesManager::percentComplete(int percent, const QString& title) {
    m_progressObject->setProperty("value", percent);
    m_progressObject->setProperty("text", title);
    if (percent == 100) {
        m_progressObject->setProperty("visible", false);
        emit sourcesUpdated();
    }
}

void InstallSourcesManager::findProgressObject() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QQuickItem * rootObject = nullptr;
    if (viewer != nullptr)
        rootObject = viewer->rootObject();
    if (rootObject != nullptr)
        m_progressObject = rootObject->findChild<QQuickItem*>("progress");
}

} // end namespace
