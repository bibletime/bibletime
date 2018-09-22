/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef BTWEBENGINEPAGE_H
#define BTWEBENGINEPAGE_H

#ifdef USEWEBENGINE
#include <QWebEnginePage>
#else
#include <QWebPage>
#include <QWebFrame>
#endif
#include <QUrl>
#include <QObject>

class QPrinter;
class QWidget;
class QWebChannel;
class BtWebEngineView;


#ifdef USEWEBENGINE
class BtWebEnginePage : public QWebEnginePage {
#else
class BtWebEnginePage : public QWebPage {
#endif

    Q_OBJECT

public:
    BtWebEnginePage(QObject *parent = 0);

    void addJavaScriptObject(const QString &name, QObject *object);
    BtWebEngineView * btView() const;
    void print(QPrinter* printer);
    void selectAll();

#ifdef USEWEBENGINE
signals:
    void linkClicked(const QUrl& url);

protected:
    virtual bool acceptNavigationRequest(
            const QUrl& url, NavigationType type, bool isMainFrame);

    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                  const QString & message, int lineNumber,
                                  const QString & sourceID);

private:

    QWebChannel *m_webChannel;

#else

    void setHtml(const QString& text);

#endif
};

#endif
