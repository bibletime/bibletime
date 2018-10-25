/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

// This class encapsulates most differences between QWebPage and QWebEnginePage
// Javascript issues are handled in BtHtmlReadDisplay

#include "btwebenginepage.h"
#include "btwebengineview.h"

#include "frontend/display/bthtmljsobject.h"
#include "util/directory.h"

#include <QDebug>
#include <QPainter>
#include <QPrinter>
#ifdef USEWEBENGINE
#include <QWebChannel>
#else
#include <QWebPage>
#endif

#ifdef USEWEBENGINE

BtWebEnginePage::BtWebEnginePage(QObject *parent)
    :QWebEnginePage(parent),
      m_webChannel(new QWebChannel(this)) {

    setWebChannel(m_webChannel);
}

bool BtWebEnginePage::acceptNavigationRequest(
        const QUrl& url, NavigationType type, bool isMainFrame) {
    if (type == QWebEnginePage::NavigationTypeLinkClicked) {
        emit linkClicked(url);
        return false;
    }
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

void BtWebEnginePage::addJavaScriptObject(const QString &name, QObject *object) {
    object->setObjectName(name);
    m_webChannel->registerObject(name, object);
}

void BtWebEnginePage::selectAll() {
    triggerAction(QWebEnginePage::SelectAll);
}

void BtWebEnginePage::print(QPrinter *printer) {
    QPainter painter;
    painter.begin(printer);
    BtWebEngineView *btWebEngineView = btView();
    btWebEngineView->render(&painter);
    painter.end();
}

void BtWebEnginePage::javaScriptConsoleMessage(
        JavaScriptConsoleMessageLevel /*level*/,
        const QString & message, int /*lineNumber*/,
        const QString & /*sourceID*/) {
    qWarning() << "javascript console :" << message;
}

#else

BtWebEnginePage::BtWebEnginePage(QObject *parent)
    :QWebPage(parent) {
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
}

void BtWebEnginePage::addJavaScriptObject(const QString &name, QObject *object) {
    object->setObjectName(name);
    mainFrame()->addToJavaScriptWindowObject(object->objectName(), object);
}

void BtWebEnginePage::selectAll() {
    triggerAction(QWebPage::SelectAll);
}

void BtWebEnginePage::print(QPrinter *printer) {
    mainFrame()->print(printer);
}

void BtWebEnginePage::setHtml(const QString& text) {
    // TODO - test
}

#endif

BtWebEngineView * BtWebEnginePage::btView() const {
    return qobject_cast<BtWebEngineView*>(view());
}
