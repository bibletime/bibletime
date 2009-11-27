/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/htmldialogs/btaboutdialog.h"

#include "util/directory.h"

// Sword includes:
#include <swversion.h>


// Forwards
static QString make_body(const QString& content);
static QString make_bold(const QString& content);
static QString make_br();
static QString make_center(const QString& content);
static QString make_head(const QString& content);
static QString make_html(const QString& content);
static QString make_file_icon(const QString& icon);
static QString make_link(const QString& link, const QString& text);
const char* bt_version();


// Implements the Help > About dialog box

BtAboutDialog::BtAboutDialog(QWidget *parent, Qt::WindowFlags wflags )
        : BtTabHtmlDialog (tr("About BibleTime"), 5, parent, wflags) {
    resize(550, 340);
    init_lic_tab();
    init_sword_tab();
    init_qt_tab();
    init_contributors_tab();
    init_bt_tab();
}

BtAboutDialog::~BtAboutDialog() {
}

void BtAboutDialog::init_bt_tab() {
    selectTab(0);
    setTabText("BibleTime" );
    QString content;
    content = make_file_icon("bibletime");
    content += "&nbsp;&nbsp;";
    content += make_bold("BibleTime " + QString(bt_version()));
    content = make_center(content) + make_br();
    content += tr("BibleTime is an easy to use but powerful Bible study tool.");
    content += make_br() + make_br();
    content += tr("We are looking for developers and translators.");
    content += " ";
    content += tr("If you would like to join our team, please send an email to info@bibletime.info.");
    content += make_br() + make_br();
    content += tr("(c)1999-2009, The BibleTime Team");
    content += make_br();
    content += make_link("http://www.bibletime.info", "http://www.bibletime.info");
    QString bibletime = make_html(make_head("") + make_body(content));
    setHtml(bibletime);
}

void BtAboutDialog::init_contributors_tab() {
    selectTab(1);
    setTabText(tr("Contributors"));
    QString content;
    content += make_bold(tr("The following people contributed to BibleTime:")) + make_br();
    // sorted alphabetically (last name)
    content += "<ul>";
    content += "<li>Thomas Abthorpe (" + tr("documentation and translation manager") + ")</li>";
    content += "<li>Joachim Ansorg (" + tr("project founder, developer") + ")</li>";
    content += "<li>David Blue (" + tr("designer") + ")</li>";
    content += "<li>Tim Brodie (" + tr("developer") + ")</li>";
    content += "<li>Timothy R. Butler (" + tr("designer") + ")</li>";
    content += "<li>Jim Campbell (" + tr("developer") + ")</li>";
    content += "<li>Lee Carpenter (" + tr("developer") + ")</li>";
    content += "<li>Jeremy Erickson (" + tr("packager") + ")</li>";
    content += "<li>Troy A. Griffitts (" + tr("creator of The Sword Project") + ")</li>";
    content += "<li>Martin Gruner (" + tr("project manager, developer") + ")</li>";
    content += "<li>Thomas Hagedorn (" + tr("domain sponsor") + ")</li>";
    content += "<li>Bob Harman (" + tr("howto") + ")</li>";
    content += "<li>Gary Holmlund (" + tr("developer") + ")</li>";
    content += "<li>Nikolay Igotti (" + tr("developer") + ")</li>";
    content += "<li>Eeli Kaikkonnen (" + tr("developer") + ")</li>";
    content += "<li>Chris Kujawa (" + tr("developer") + ")</li>";
    content += "<li>Mark Lybarger (" + tr("developer") + ")</li>";
    content += "<li>Luke Mauldin (" + tr("developer") + ")</li>";
    content += "<li>James Ots (" + tr("designer") + ")</li>";
    content += "<li>Fred Saalbach (" + tr("documentation") + ")</li>";
    content += "<li>Gary Sims (" + tr("developer") + ")</li>";
    content += "<li>Wolfgang Stradner (" + tr("tester, usability expert") + ")</li>";
    content += "<li>Thorsten Uhlmann (" + tr("developer") + ")</li>";
    content += "<li>David White (" + tr("developer") + ")</li>";
    content += "<li>Mark Zealey (" + tr("developer") + ")</li>";
    content += "</ul>";

    content += make_bold(tr("The following people translated BibleTime into their language:")) + make_br();
    // sorted alphabetically (last name)
    content += "<ul>";
    content += "<li>Horatiu Alexe</li>";
    content += "<li>Luis Barron</li>";
    content += "<li>Jan B&#x11B;lohoubek</li>";
    content += "<li>Chun-shek Chan</li>";
    content += "<li>Nouhoun Y. Diarra</li>";
    content += "<li>Rafael Fagundes</li>";
    content += "<li>Ilpo Kantonen</li>";
    content += "<li>Pavel Laukko</li>";
    content += "<li>Johan Lingen</li>";
    content += "<li>Piotr Markiewicz</li>";
    content += "<li>G&eacute;za Nov&aacute;k</li>";
    content += "<li>Gabriel P&eacute;rez</li>";
    content += "<li>Igor Plisco</li>";
    content += "<li>Zdenko Podobn&yacute;</li>";
    content += "<li>Jaak Ristioja</li>";
    content += "<li>Igor Rykhlin</li>";
    content += "<li>Vlad Savitsky</li>";
    content += "<li>Henrik Sonesson</li>";
    content += "<li>Johan van der Lingen</li>";
    content += "<li>Jean Van Schaftingen</li>";
    content += "<li>Roland Teschner</li>";
    content += "<li>Giovanni Tedaldi</li>";
    content += "<li>Dmitry Yurevich</li>";
    content += "<li>Esteban Zeller</li>";
    content += "</ul>";
    content += make_br();
    content += tr("Some names may be missing, please email bibletime-translations@lists.sourceforge.net if you notice errors or omissions.");

    QString contributors = make_html(make_head("") + make_body(content));
    setHtml(contributors);
}


void BtAboutDialog::init_sword_tab() {
    selectTab(2);
    setTabText("Sword" );

    QString version( sword::SWVersion::currentVersion.getText());
    QString content = make_br() + make_br();
    content += make_center(make_bold(tr("SWORD library version %1").arg(version)));
    content += make_br();

    content += tr("BibleTime makes use of the SWORD Project. The SWORD Project is the CrossWire Bible Society's free Bible software project. Its purpose is to create cross-platform open-source tools-- covered by the GNU General Public License-- that allow programmers and Bible societies to write new Bible software more quickly and easily.");
    content += make_br() + make_br();
    content += tr("The SWORD Project") + make_br();
    content += make_link("http://www.crosswire.org/sword/index.jsp", "www.crosswire.org/sword/index.jsp");

    setHtml(content);

}

void BtAboutDialog::init_qt_tab() {
    selectTab(3);
    setTabText("Qt");
    QString content;
    content += make_br() + make_br();
    content += make_center(make_bold("Qt"));
    content += make_br();
    content += tr("This program uses Qt version %1.").arg(qVersion());
    content += make_br() + make_br();
    content += tr("Qt is a cross-platform application and UI framework, created with C++ language. It has been released under the LGPL license.");
    content += make_br() + make_br();
    content += make_link("http://qt.nokia.com/", "http://qt.nokia.com/");
    //content += tr("Please see ");
    //content += make_link("http://qtsoftware.com/company/model/", "qtsoftware.com/company/model");
    //content += tr(" for an overview of Qt licensing.");
    QString qt = make_html(make_head("") + make_body(content));
    setHtml(qt);
}

void BtAboutDialog::init_lic_tab() {
    namespace DU = util::directory;

    selectTab(4);
    setTabText(tr("License"));

    QByteArray text;
    text += tr("BibleTime is released under the GPL license.");
    text += " ";
    text += tr("You can download and use (but not distribute) the program for personal, private, public or commercial purposes without restrictions.");
    text += " ";
    text += tr("You can give away or distribute the program if you also distribute the corresponding source code.");
    text += "<br/><br/>";
    //text += tr("It is allowed to distribute software under GPL for a small fee, but it must be accompanied with the complete source code, and the fact that it is freely available with no cost must not be hidden.");
    //text += "<br/><br/>";
    text += tr("The complete legally binding license is below.");

    QFile licFile(DU::getLicenseDir().path() + "/license.html");
    if (licFile.open(QFile::ReadOnly)) {
        QByteArray html;
        while (!licFile.atEnd()) {
            QByteArray line = licFile.readLine();
            html = html + line;
        }
        licFile.close();
        html.replace("TRANSLATED TEXT", text);
        setHtml(QString(html));
    }
}



// Helper functions

static QString make_center(const QString& content) {
    return "<center>" + content + "</center>";
}

static QString make_br() {
    return "<br/>";
}

static QString make_bold(const QString& content) {
    return "<b>" + content + "</b>";
}

static QString make_html(const QString& content) {
    return "<html>" + content + "</html>";
}

static QString make_head(const QString& content) {
    return "<head>" + content + "</head>";
}

static QString make_body(const QString& content) {
    return "<body>" + content + "</body>";
}

static QString make_link(const QString& link, const QString& text) {
    return "<a href=\"" + link + "\">" + text + "</a>";
}

static QString make_file_icon(const QString& icon) {
    namespace DU = util::directory;
    QUrl url = QUrl::fromLocalFile(  DU::getIconDir().path()  + "/" + icon + ".svg");
    QString html = "<img src=" + url.toString() + ">";
    return html;
}

