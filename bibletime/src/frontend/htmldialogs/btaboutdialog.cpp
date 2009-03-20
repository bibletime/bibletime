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

#include "swversion.h"

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
	: BtTabHtmlDialog("About BibleTime", 4, parent, wflags)
{
	resize(550,340);
	init_lic_tab();
	init_sword_tab();
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

void BtAboutDialog::init_sword_tab()
{
	selectTab(1);
	setTabText("Sword" );

	QString version( sword::SWVersion::currentVersion.getText());
	QString content = make_br() + make_br();
	content += make_center(make_bold("Sword Version " + version));
	content += make_br();

	content += "BibleTime makes use of the SWORD Project. The SWORD Project is the CrossWire Bible Society's free Bible software project. Its purpose is to create cross-platform open-source tools-- covered by the GNU General Public License-- that allow programmers and Bible societies to write new Bible software more quickly and easily.";
	content += make_br() + make_br();
	content += "The SWORD Project" + make_br();
	content += make_link("http://www.crosswire.org/sword/index.jsp","www.crosswire.org/sword/index.jsp");

	setHtml(content);

}

void BtAboutDialog::init_qt_tab()
{
	selectTab(2);
	setTabText("Qt");
	QString content;
	content += make_br() + make_br();
	content += make_center(make_bold("Qt"));
	content += make_br();
	content += tr("This program uses Qt Open Source Edition version ");
	content += qVersion();
	content += make_br() + make_br();
	content += tr("Qt Open Source Edition is intended for the development of Open Source applications.");
	content += tr("Qt is a C++ toolkit for cross-platform application development.");
	content += make_br() + make_br();
	content += tr("Please see ");
	content += make_link("http://qtsoftware.com/company/model/","qtsoftware.com/company/model");
	content += tr(" for an overview of Qt licensing.");
	QString qt = make_html(make_head("") + make_body(content));
	setHtml(qt);
}

void BtAboutDialog::init_lic_tab()
{
	selectTab(3);
	setTabText(tr("License Agreement"));

	QByteArray text;
	text += tr("BibleTime is released under the GPL licence.");
	text += tr("You can download and use (but not distribute) the program for personal, private, public or commercial purposes without restrictions.");
	text += tr("You can give away or distribute the program if you also distribute the corresponding source code.");
	text += "<br><br>";
	text += tr("The complete legally binding license is below.");

	QFile licFile(util::filesystem::DirectoryUtil::getLicenseDir().path() + "/license.html");
	if (licFile.open(QFile::ReadOnly))
	{
		QByteArray html;
		while (!licFile.atEnd())
		{
			QByteArray line = licFile.readLine();
			html = html + line;
		}
		licFile.close();
		html.replace("TRANSLATED TEXT", text);
		setHtml(QString(html));
	}
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

