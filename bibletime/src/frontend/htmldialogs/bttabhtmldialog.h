/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTTABDIALOG_H
#define BTTABDIALOG_H

#include <QDialog>
#include <QString>
#include <QUrl>

class QTabWidget;
class QWebView;

// This class creates a dialog with zero or more tabs. For zero tabs it is 
// just a single QWebView inside the dialog. For 1 or more tabs, each tab
// contains a separate QWebView. Each QWebView can have either plain text or
// html text. The class will automatically delete itself when closed. 
// The class can either be directly called or subclassed. The dialog is not modal.

// Typical direct usage:
//
//                        Zero tabs
// BtTabHtmlDialog* dlg = new BtTabHtmlDialog("My Title", 0, parent);
// dlg->setHtml(htmlText);
// dlg->show();
//
//     or
//
//                        Two tabs
// BtTabHtmlDialog* dlg = new BtTabHtmlDialog("My Title", 2, parent);
// dlg->selectTab(0);
// dlg->setTabText(nameOfTab0);
// dlg->setHtml(htmlText0);
// dlg->selectTab(1);
// dlg->setTabText(nameOfTab1);
// dlg->setHtml(htmlText1);
// dlg->show();


class BtTabHtmlDialog : public QDialog  
{
	Q_OBJECT
public:
	BtTabHtmlDialog(const QString& title, int numberTabs, QWidget *parent=0, Qt::WindowFlags wflags = Qt::Dialog);
	~BtTabHtmlDialog();
	void selectTab(int tab);
	void setTabText(const QString& tabName);

// See QWebView::setHtml()
	void setHtml(const QString& html, const QUrl& baseUrl=QUrl());

// See QWebView::setUrl()
	void setUrl(const QUrl& url);
	
private slots:	
	void linkClicked(const QUrl url);

private:
	void init_connections(QWebView* webView);
	QWebView* webView();
	
	QWebView*   m_webView;
	QTabWidget* m_tabWidget;
	int m_tabs;
};

#endif
