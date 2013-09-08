
#include "gridchooser.h"

#include "qtquick2applicationviewer.h"

#include <algorithm>
#include <cmath>
#include <QEventLoop>
#include <QQuickItem>
#include <QQmlContext>
#include <QDebug>
#include <QCoreApplication>

namespace btm {

GridChooser::GridChooser(QtQuick2ApplicationViewer* viewer)
    : viewer_(viewer),
      gridChooserObject_(0) {
    QQuickItem * rootObject = viewer_->rootObject();
    if (rootObject != 0)
        gridChooserObject_ = rootObject->findChild<QQuickItem*>("gridChooser");
}

GridChooser::~GridChooser() {
}

void GridChooser::open(const QStringList& stringList, const QString& highlight, const QString& title) {
    Q_ASSERT(gridChooserObject_ != 0);
    if (gridChooserObject_ == 0)
        return;

    gridChooserObject_->disconnect();
    bool ok = connect(gridChooserObject_, SIGNAL(accepted(QString)),
                      this, SLOT(gridChooserAccepted(QString)));
    Q_ASSERT(ok);
    setProperties(stringList, highlight, title);
}

void GridChooser::setProperties(const QStringList& list, const QString& hightlight, const QString& title) {
    QQmlContext* ctx = viewer_->rootContext();
    ctx->setContextProperty("gridChooserModel",list);
    gridChooserObject_->setProperty("opacity",1);
    gridChooserObject_->setProperty("selected",hightlight);
    gridChooserObject_->setProperty("titleText",title);

    int maxLength = 0;
    for (int i = 0; i < list.count(); ++i) {
        QString text = list.at(i);
        maxLength = std::max(maxLength, text.length());
    }
    gridChooserObject_->setProperty("maxLength", maxLength);
}

void GridChooser::gridChooserAccepted(QString value) {
    emit accepted(value);
}

void GridChooser::gridChooserCanceled() {
    emit canceled();
}

} // end namespace
