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

#include "btwebengineview.h"
#include "btwebenginepage.h"

// This class encapsulates differences between QWebView and QWebEngineView

#ifdef USEWEBENGINE
BtWebEngineView::BtWebEngineView(QWidget *parent)
    : QWebEngineView(parent) {
}
#else
BtWebEngineView::BtWebEngineView(QWidget *parent)
    : QWebView(parent) {
}
#endif

BtWebEnginePage * BtWebEngineView::btPage() const {
    return qobject_cast<BtWebEnginePage*>(page());
}

void BtWebEngineView::findTextHighlight(const QString& text, bool caseSensitive) {
#ifdef USEWEBENGINE
    QWebEnginePage::FindFlags options;
    if (caseSensitive)
        options |= QWebEnginePage::FindCaseSensitively;
    QWebEngineView::findText("", options); // clear old highlight
    QWebEngineView::findText(text, options);
#else
    QWebPage::FindFlags options = QWebPage::HighlightAllOccurrences;
    if (caseSensitive)
        options |= QWebPage::FindCaseSensitively;
    QWebView::findText("", options); // clear old highlight
    QWebView::findText(text, options);

#endif
}

void BtWebEngineView::findText(const QString& text, bool caseSensitive, bool backward) {
#ifdef USEWEBENGINE
    QWebEnginePage::FindFlags options;
    if (backward)
        options |= QWebEnginePage::FindBackward;
    if (caseSensitive)
        options |= QWebEnginePage::FindCaseSensitively;
    QWebEngineView::findText(text, options);
#else
    QWebPage::FindFlags options = QWebPage::FindWrapsAroundDocument;
    if (backward)
        options |= QWebPage::FindBackward;
    if (caseSensitive)
        options |= QWebPage::FindCaseSensitively;
    QWebView::findText(text, options);
#endif
}
