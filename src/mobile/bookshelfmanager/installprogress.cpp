
#include "installprogress.h"

#include "mobile/btmmain.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include "mobile/ui/viewmanager.h"
#include <QQuickItem>
#include <QDebug>

namespace btm {

InstallProgress::InstallProgress(QObject* parent)
    : QObject(parent), progressObject_(0) {
}

void InstallProgress::openProgress(const QList<CSwordModuleInfo*>& modules) {
    if (progressObject_ == 0)
        findProgressObject();
    if (progressObject_ == 0)
        return;

    makeConnections();
    setProperties();
}

void InstallProgress::findProgressObject() {
    QtQuick2ApplicationViewer* viewer = getViewManager()->getViewer();
    QQuickItem * rootObject = 0;
    if (viewer != 0)
        rootObject = viewer->rootObject();
    if (rootObject != 0)
        progressObject_ = rootObject->findChild<QQuickItem*>("progress");
}

void InstallProgress::makeConnections() {
    bool ok = connect(progressObject_, SIGNAL(cancel()), this, SLOT(cancel()));
    Q_ASSERT(ok);
}

void InstallProgress::setProperties() {
    progressObject_->setProperty("visible", true);
    progressObject_->setProperty("minimumValue", 0.0);
    progressObject_->setProperty("maximumValue", 1.0);
    progressObject_->setProperty("value", 0.0);
}

void InstallProgress::cancel() {
    progressObject_->setProperty("visible", false);
}

}
