#include "installsourcesmanager.h"

#include "installsourcesthread.h"
#include "backend/btinstallbackend.h"
#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include <QDebug>
#include <QQuickItem>

namespace btm {

InstallSourcesManager::InstallSourcesManager(QObject* parent)
    : installSourcesThread_(0) {
    installSourcesThread_ = new InstallSourcesThread(this);
}

InstallSourcesManager::~InstallSourcesManager() {
    delete installSourcesThread_;
}

void InstallSourcesManager::findProgressObject() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QQuickItem * rootObject = 0;
    if (viewer != 0)
        rootObject = viewer->rootObject();
    if (rootObject != 0)
        progressObject_ = rootObject->findChild<QQuickItem*>("progress");
}

void InstallSourcesManager::refreshSources() {
    findProgressObject();
    Q_ASSERT(progressObject_ != 0);
    if (progressObject_ == 0)
        return;

    connect(progressObject_, SIGNAL(cancel()), this, SLOT(cancel()));

    progressObject_->setProperty("minimumValue", 0.0);
    progressObject_->setProperty("maximumValue", 100.0);
    progressObject_->setProperty("value", 0.0);
    progressObject_->setProperty("visible", true);
    progressObject_->setProperty("text", "Refreshing Source List");

    runThread();
}

void InstallSourcesManager::cancel() {
    progressObject_->setProperty("visible", false);
}

void InstallSourcesManager::runThread() {
    disconnect(installSourcesThread_);
    connect(installSourcesThread_, SIGNAL(percentComplete(int, const QString&)),
            this, SLOT(percentComplete(int, const QString&)));

    installSourcesThread_->start();
}

void InstallSourcesManager::percentComplete(int percent, const QString& title) {
    progressObject_->setProperty("value", percent);
    progressObject_->setProperty("text", title);
    if (percent == 100)
        progressObject_->setProperty("visible", false);
}
} // end namespace
