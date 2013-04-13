
#include "viewmanager.h"

#include "qtquick2applicationviewer.h"

#include "btstyle.h"
#include <cmath>
#include <QGuiApplication>
#include <QJsonValue>
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QUrl>
#include <QDebug>
#include "versechooser.h"

namespace btm {

static QString qmlFilePath(const QString& parentName, const QString& fileName) {
    QString filePath = QCoreApplication::applicationDirPath() + "/../share/";
    filePath += parentName + "/";
    filePath += fileName;
    return filePath;
}

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
    QString mainQml = qmlFilePath("qml", "main.qml");
    viewer_->setMainQmlFile(mainQml);
}

void ViewManager::show() {
    viewer_->showExpanded();
}

void ViewManager::showBibleVerseDialog(BtWindowInterface* bibleVerse) {
    VerseChooser* dlg = new VerseChooser(viewer_, bibleVerse);
    dlg->open();
}

QtQuick2ApplicationViewer* ViewManager::getViewer() const {
    return viewer_;
}

} // end namespace
