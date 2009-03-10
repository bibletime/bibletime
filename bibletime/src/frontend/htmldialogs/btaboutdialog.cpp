/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "config.h"
#include "btaboutdialog.h"
#include "util/directoryutil.h"

// Forwards
static QString make_body(const QString& content);
static QString make_bold(const QString& content);
static QString make_br();
static QString make_center(const QString& content);
static QString make_head(const QString& content);
static QString make_html(const QString& content);
static QString make_file_icon(const QString& icon);
static QString make_link(const QString& link, const QString& text);
static QString make_version();


// Implements the Help > About dialog box

BtAboutDialog::BtAboutDialog(QWidget *parent, Qt::WindowFlags wflags )
	: BtTabHtmlDialog("About BibleTime", 3, parent, wflags)
{
	resize(550,340);
	init_lic_tab();
	init_qt_tab();
	init_bt_tab();
}

BtAboutDialog::~BtAboutDialog()
{
}

void BtAboutDialog::init_bt_tab()
{
	selectTab(0);
	setTabText("BibleTime" );
	QString content;
	content = make_file_icon("bibletime");
	content += "&nbsp;&nbsp;";
	content += make_bold("BibleTime " + make_version());
	content = make_center(content) + make_br();
	content += tr("BibleTime is an easy to use but powerful Bible study tool.");
	content += make_br() + make_br();
	content += tr("We are looking for developers and translators.");
	content += tr("If you would like to join our team, please send an email to info@bibletime.info.");
	content += make_br() + make_br();
	content += tr("(c)1999-2009, The BibleTime Team");
	content += make_br();
	content += make_link("http://www.bibletime.info","http://www.bibletime.info");	
	QString bibletime = make_html(make_head("") + make_body(content));
	setHtml(bibletime);
}

void BtAboutDialog::init_qt_tab()
{
	selectTab(1);
	setTabText("Qt");
	QString content;
	content += make_br() + make_br();
	content += make_center(make_bold("Qt"));
	content += make_br();
	content += tr("This program uses Qt Open Source Edition version ");
	content += qVersion();
	content += make_br() + make_br();
	content += tr("Qt Open Source Edition is intended for the development of Open Source applications.");
	content += make_br() + make_br();
	content += tr("Please see ");
	content += make_link("http://qtsoftware.com/company/model/","qtsoftware.com/company/model");
	content += tr(" for an overview of Qt licensing.");
	content += make_br() + make_br();
	content += tr("Qt is a C++ toolkit for cross-platform application development.");
	QString qt = make_html(make_head("") + make_body(content));
	setHtml(qt);
}

void BtAboutDialog::init_lic_tab()
{
	selectTab(2);
	setTabText(tr("License Agreement"));
	QString lic = "file://" + util::filesystem::DirectoryUtil::getLicenseDir().path() + "/license.html";	
	QUrl url(lic);		
	setUrl(url);	
}


	
// Helper functions
		
static QString make_center(const QString& content)
{
	return "<center>" + content + "</center>";
}

static QString make_br()
{
	return "<br>";
}

static QString make_bold(const QString& content)
{
	return "<b>" + content + "</b>";
}

static QString make_html(const QString& content)
{
	return "<html>" + content + "</html>";
}

static QString make_head(const QString& content)
{
	return "<head>" + content + "</head>";
}

static QString make_body(const QString& content)
{
	return "<body>" + content + "</body>";
}

static QString make_link(const QString& link, const QString& text)
{
	return "<a href=\"" + link + "\">" + text +"</a>";
}

static QString make_version()
{
//	return "";
	return BT_VERSION;
}

static QString make_file_icon(const QString& icon)
{
	QString dir = "<img src=file://" + util::filesystem::DirectoryUtil::getIconDir().path();
	return dir + "/" + icon +".png >";
}

