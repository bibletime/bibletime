
#include "viewmanager.h"

#include "qtquick2applicationviewer.h"

#include "btstyle.h"
#include "util/directory.h"
#include <cmath>
#include <QGuiApplication>
#include <QJsonValue>
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QUrl>
#include <QDebug>

namespace btm {

ViewManager::ViewManager()
    : viewer_(new QtQuick2ApplicationViewer()) {
    BtStyle style;
    style.setStyle(BtStyle::gnome);
    initialize_string_list_chooser_model();
    initialize_main_qml();
}

void ViewManager::initialize_string_list_chooser_model() {
    QQmlContext* ctx = viewer_->rootContext();
    QStringList list = QStringList();
    ctx->setContextProperty("gridChooserModel",list);
}

void ViewManager::initialize_main_qml() {
    viewer_->setMainQmlFile(util::directory::getQmlDir().filePath("main.qml"));
}

void ViewManager::show() {
    viewer_->showExpanded();
}

QtQuick2ApplicationViewer* ViewManager::getViewer() const {
    return viewer_;
}

} // end namespace
