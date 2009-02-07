/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bthtmlfindtext.h"
#include "bthtmlreaddisplay.h"
#include "creaddisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/displaywindow/cdisplaywindow.h"
#include <QMdiSubWindow>

static BtHtmlFindText* dialog = 0;

void showBtHtmlFindText(CMDIArea* mdiArea)
{
	if (dialog == 0)
		dialog = new BtHtmlFindText(mdiArea);
	dialog->show();
}

BtHtmlFindText::BtHtmlFindText(CMDIArea* mdiArea, QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent,f), m_mdiArea(mdiArea)
{
	ui.setupUi(this);
	bool ok;
	ok = connect(ui.nextButton,SIGNAL(clicked()), this, SLOT(findNext()));
	Q_ASSERT(ok);
	ok = connect(ui.previousButton,SIGNAL(clicked()), this, SLOT(findPrevious()));
	Q_ASSERT(ok);
}

BtHtmlFindText::~BtHtmlFindText()
{
}

void BtHtmlFindText::findNext()
{
	QWebView* webView = getActiveWindowWebView();
	if (webView != 0)
	{
		QWebPage::FindFlags options = 0;
		if (ui.caseBox->checkState() == Qt::Checked)
			options |= QWebPage::FindCaseSensitively;
		QString searchText = ui.findTextComboBox->currentText();
		if (!searchText.isEmpty())
			webView->findText(searchText, options);
	}
}

void BtHtmlFindText::doHide()
{
	hide();
}

void BtHtmlFindText::findPrevious()
{
	QWebView* webView = getActiveWindowWebView();
	if (webView != 0)
	{
		QWebPage::FindFlags options = QWebPage::FindBackward;
		if (ui.caseBox->checkState() == Qt::Checked)
			options |= QWebPage::FindCaseSensitively;
		QString searchText = ui.findTextComboBox->currentText();
		if (!searchText.isEmpty())
			webView->findText(searchText,options);
	}
}


QWebView* BtHtmlFindText::getActiveWindowWebView()
{
	QMdiSubWindow* activeSubWindow = m_mdiArea->activeSubWindow();
	if (activeSubWindow == 0)
		return 0;

	QWidget* activeWindowWidget = activeSubWindow->widget();
	if (activeWindowWidget == 0)
		return 0;

	CDisplayWindow* cDisplayWindow = qobject_cast<CDisplayWindow*>(activeWindowWidget);
	if (cDisplayWindow == 0)
		return 0;

	CDisplay* cDisplay = cDisplayWindow->displayWidget();
	if (cDisplay == 0)
		return 0;

	QWidget* textView = cDisplay->view();
	if (textView == 0)
		return 0;

	QWebView* webView = qobject_cast<QWebView*>(textView);
	return webView;
}







