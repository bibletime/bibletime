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
    style.setStyle(BtStyle::darkTheme);
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
