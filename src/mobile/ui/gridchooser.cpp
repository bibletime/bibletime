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

#include "gridchooser.h"

#include <algorithm>
#include <cmath>
#include <QEventLoop>
#include <QQuickItem>
#include <QQmlContext>
#include <QDebug>
#include <QCoreApplication>
#include "qtquick2applicationviewer.h"
#include "util/btassert.h"
#include "util/btconnect.h"


namespace btm {

GridChooser::GridChooser(QtQuick2ApplicationViewer* viewer)
    : viewer_(viewer),
      gridChooserObject_(nullptr) {
    QQuickItem * rootObject = viewer_->rootObject();
    if (rootObject != nullptr)
        gridChooserObject_ = rootObject->findChild<QQuickItem*>("gridChooser");
}

GridChooser::~GridChooser() {
}

void GridChooser::open(const QStringList& stringList, const QString& highlight, const QString& title) {
    BT_ASSERT(gridChooserObject_);
    if (gridChooserObject_ == nullptr)
        return;

    gridChooserObject_->disconnect();
    BT_CONNECT(gridChooserObject_, SIGNAL(accepted(QString)),
               this,               SLOT(gridChooserAccepted(QString)));
    setProperties(stringList, highlight, title);
}

void GridChooser::setProperties(const QStringList& list, const QString& hightlight, const QString& title) {
    QQmlContext* ctx = viewer_->rootContext();
    ctx->setContextProperty("gridChooserModel",list);
    gridChooserObject_->setProperty("selected",hightlight);
    gridChooserObject_->setProperty("titleText",title);

    int maxLength = 0;
    for (int i = 0; i < list.count(); ++i) {
        QString text = list.at(i);
        maxLength = std::max(maxLength, text.length());
    }
    gridChooserObject_->setProperty("maxLength", maxLength);

    gridChooserObject_->setProperty("visible",true);
}

void GridChooser::gridChooserAccepted(QString value) {
    emit accepted(value);
}

void GridChooser::gridChooserCanceled() {
    emit canceled();
}

} // end namespace
