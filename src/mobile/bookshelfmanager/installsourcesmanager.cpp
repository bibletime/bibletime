#include "installsourcesmanager.h"

#include "installsources.h"
#include "backend/btinstallbackend.h"
#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include <QDebug>
#include <QQuickItem>
#include <QThread>

namespace btm {

InstallSourcesManager::InstallSourcesManager(QObject* parent)
    : m_worker(0) {
}

InstallSourcesManager::~InstallSourcesManager() {
}

void InstallSourcesManager::refreshSources() {
    findProgressObject();
    Q_ASSERT(progressObject_ != 0);
    if (progressObject_ == 0)
        return;
    progressObject_->disconnect(this);
    connect(progressObject_, SIGNAL(cancel()), this, SLOT(cancel()));

    progressObject_->setProperty("minimumValue", 0.0);
    progressObject_->setProperty("maximumValue", 100.0);
    progressObject_->setProperty("value", 0.0);
    progressObject_->setProperty("visible", true);
    progressObject_->setProperty("text", "Refreshing Source List");

    runThread();
}

void InstallSourcesManager::cancel() {
    m_worker->cancel();
    progressObject_->setProperty("visible", false);
}

void InstallSourcesManager::runThread() {
    QThread* thread = new QThread;
    m_worker = new InstallSources();
    m_worker->moveToThread(thread);
//    connect(m_worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(thread, SIGNAL(started()), m_worker, SLOT(process()));
    connect(m_worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(m_worker, SIGNAL(finished()), m_worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(m_worker, SIGNAL(percentComplete(int, const QString&)),
            this, SLOT(percentComplete(int, const QString&)));
    thread->start();
}

void InstallSourcesManager::percentComplete(int percent, const QString& title) {
    progressObject_->setProperty("value", percent);
    progressObject_->setProperty("text", title);
    if (percent == 100)
        progressObject_->setProperty("visible", false);
}

void InstallSourcesManager::findProgressObject() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QQuickItem * rootObject = 0;
    if (viewer != 0)
        rootObject = viewer->rootObject();
    if (rootObject != 0)
        progressObject_ = rootObject->findChild<QQuickItem*>("progress");
}

} // end namespace
