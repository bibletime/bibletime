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

#ifndef BTWEBENGINEVIEW_H
#define BTWEBENGINEVIEW_H

#include "frontend/btwebenginepage.h"

#ifdef USEWEBENGINE
#include <QWebEngineView>
#else
#include <QWebView>
#endif

class BtWebEnginePage;

#ifdef USEWEBENGINE
class BtWebEngineView : public QWebEngineView {
#else
class BtWebEngineView : public QWebView {
#endif

    Q_OBJECT

public:
    BtWebEngineView(QWidget *parent = 0);

    BtWebEnginePage * btPage() const;

    void findTextHighlight(const QString& text, bool caseSensitive);
    void findText(const QString& text, bool caseSensitive, bool backwards);
};

#endif



