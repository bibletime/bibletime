/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bttabhtmldialog.h"  // See this file for more documentation of BtTabHtmlDialog
#include "util/dialogutil.h"
#include "util/directoryutil.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QWebView>
#include <QMenu>
#include <QTabWidget>
#include <QDesktopServices>
#include <QContextMenuEvent>

BtTabHtmlDialog::BtTabHtmlDialog
	(const QString& title, int tabs, QWidget *parent, Qt::WindowFlags wflags )
	: QDialog(parent, wflags), m_webView(0), m_tabWidget(0), m_tabs(tabs)
{
	//Set the flag to destroy when closed
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(title);
	resize(400, 300);

	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	if (tabs == 0)
	{
		m_webView = new BtWebView(this);
		init_connections(m_webView);
		vboxLayout->addWidget(m_webView);
		m_webView->setHtml("Hi");
	}
	else
	{	
		m_tabWidget = new QTabWidget(this);
		vboxLayout->addWidget(m_tabWidget);
		for (int i=0; i<tabs; i++)
		{
			QWebView* webView = new BtWebView(this);
			init_connections(webView);
			webView->setObjectName("View" + QString::number(i));
			webView->setHtml(" ");
			m_tabWidget->addTab(webView,"Tab" + QString::number(i));
			m_tabWidget->show();
		}
	}
	
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
	util::prepareDialogBox(buttonBox);
	vboxLayout->addWidget(buttonBox);

	bool ok;
	ok = connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	Q_ASSERT(ok);
	ok = connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	Q_ASSERT(ok);
}

BtTabHtmlDialog::~BtTabHtmlDialog()
{
}
							 
void BtTabHtmlDialog::init_connections(QWebView* webView)
{
	webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	bool ok = connect(webView, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));
	Q_ASSERT(ok);
}

void BtTabHtmlDialog::linkClicked(const QUrl url)
{
	QDesktopServices::openUrl(url);
}

void BtTabHtmlDialog::selectTab(int tab)
{
	Q_ASSERT(tab >= 0 && tab < m_tabWidget->count());
	m_tabWidget->setCurrentIndex(tab);	
}

QWebView* BtTabHtmlDialog::webView()
{
	QWebView* webview = 0;
	if (m_tabs == 0)
		webview = m_webView;
	else
	{
		QWidget* widget = m_tabWidget->currentWidget();
		QString name = widget->objectName();
		webview = qobject_cast<QWebView*>(widget);
	}
	Q_ASSERT(webview != 0);
	return webview;
}

void BtTabHtmlDialog::setHtml(const QString& html, const QUrl& baseUrl)
{
	QUrl url = baseUrl;
	if (url == QUrl())
	{
		QString dir = "file://" + util::filesystem::DirectoryUtil::getIconDir().path();	
		url.setUrl(dir);		
	}
	webView()->setHtml(html,url);
}

void BtTabHtmlDialog::setUrl(const QUrl& url)
{
	webView()->setUrl(url);	
}

void BtTabHtmlDialog::setTabText(const QString& tabName)
{
	Q_ASSERT(m_tabs != 0); // There are no tabs to name
	int index = m_tabWidget->currentIndex();
	m_tabWidget->setTabText(index,tabName);	
}

// ******************* BtWebView *******************

BtWebView::BtWebView(QWidget* parent)
	: QWebView(parent), m_popup(0)
{
	m_popup = new QMenu(this);
	QAction* copyAction = pageAction(QWebPage::Copy);
	m_popup->addAction(copyAction);
}

void BtWebView::contextMenuEvent(QContextMenuEvent* event)
{
	m_popup->exec(event->globalPos());
}

