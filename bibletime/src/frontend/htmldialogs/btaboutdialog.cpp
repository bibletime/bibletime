/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/htmldialogs/btaboutdialog.h"

#include <QTextStream>
#include "util/directory.h"

// Sword includes:
#include <swversion.h>


#define BR "<br/>"
#define MAKE_CENTER(x) "<center>" + (x) + "</center>"
#define MAKE_CENTER_STATIC(x) "<center>" x "</center>"
#define MAKE_HTML(x) "<html><head></head><body>" + (x) + "</body></html>"
#define MAKE_BOLD(x) "<b>" + (x) + "</b>"
#define MAKE_BOLD_STATIC(x) "<b>" x "</b>"
#define MAKE_LINK(u,t) "<a href=\"" u "\">" + (t) + "</a>"
#define MAKE_LINK_STATIC(u,t) "<a href=\"" u "\">" t "</a>"
#define MAKE_ICON(i) "<img src=\"" + \
    QUrl::fromLocalFile(util::directory::getIconDir().path() + "/" i ".svg").toString() + \
    "\">"

BtAboutDialog::BtAboutDialog(QWidget *parent, Qt::WindowFlags wflags)
        : BtTabHtmlDialog(tr("About BibleTime"), 5, parent, wflags) {
    resize(550, 340);
    initLicenceTab();
    initSwordTab();
    initQtTab();
    initContributorsTab();
    initBtTab();
}

BtAboutDialog::~BtAboutDialog() {
    // Intentionally empty
}

void BtAboutDialog::initBtTab() {
    selectTab(0);
    setTabText("BibleTime" );

    QString content(MAKE_ICON("bibletime"));
    content += "&nbsp;&nbsp;" MAKE_BOLD_STATIC("BibleTime " BT_VERSION);
    content = MAKE_CENTER(content);
    content += BR;
    content += tr("BibleTime is an easy to use but powerful Bible study tool.");
    content += BR BR;
    content += tr("We are looking for developers and translators.");
    content += " ";
    content += tr("If you would like to join our team, please send an email to "
                  "info@bibletime.info.");
    content += BR BR;
    content += tr("(c)1999-2009, The BibleTime Team");
    content += BR MAKE_LINK_STATIC("http://www.bibletime.info", "http://www.bibletime.info");
    setHtml(MAKE_HTML(content));
}

void BtAboutDialog::initContributorsTab() {
    selectTab(1);
    setTabText(tr("Contributors"));

    const QString developer(tr("developer"));
    const QString designer(tr("designer"));
    QString content;

    /****************************************************************************************
    ***               NB!!! Credits are sorted alphabetically by last name!               ***
    ****************************************************************************************/
    content += MAKE_BOLD(tr("The following people contributed to BibleTime:"));
    content += BR "<ul>";
    content += "<li>Thomas Abthorpe (" + tr("documentation and translation manager") +
               ")</li>";
    content += "<li>Joachim Ansorg (" + tr("project founder") + ", " + developer + ")</li>";
    content += "<li>David Blue (" + designer + ")</li>";
    content += "<li>Tim Brodie (" + developer + ")</li>";
    content += "<li>Timothy R. Butler (" + designer + ")</li>";
    content += "<li>Jim Campbell (" + developer + ")</li>";
    content += "<li>Lee Carpenter (" + developer + ")</li>";
    content += "<li>Jeremy Erickson (" + tr("packager") + ")</li>";
    content += "<li>Troy A. Griffitts (" + tr("creator of The Sword Project") + ")</li>";
    content += "<li>Martin Gruner (" + tr("project manager") + ", " + developer + ")</li>";
    content += "<li>Thomas Hagedorn (" + tr("domain sponsor") + ")</li>";
    content += "<li>Bob Harman (" + tr("howto") + ")</li>";
    content += "<li>Gary Holmlund (" + developer + ")</li>";
    content += "<li>Nikolay Igotti (" + developer + ")</li>";
    content += "<li>Eeli Kaikkonnen (" + developer + ")</li>";
    content += "<li>Chris Kujawa (" + developer + ")</li>";
    content += "<li>Mark Lybarger (" + developer + ")</li>";
    content += "<li>Luke Mauldin (" + developer + ")</li>";
    content += "<li>James Ots (" + designer + ")</li>";
    content += "<li>Andrus Raag (" + tr("artist") + ")</li>";
    content += "<li>Jaak Ristioja (" + developer + ")</li>";
    content += "<li>Fred Saalbach (" + tr("documentation") + ")</li>";
    content += "<li>Gary Sims (" + developer + ")</li>";
    content += "<li>Wolfgang Stradner (" + tr("tester") + ", " + tr("usability expert") +
               ")</li>";
    content += "<li>Kang Sun (" + developer + ")</li>";
    content += "<li>Thorsten Uhlmann (" + developer + ")</li>";
    content += "<li>David White (" + developer + ")</li>";
    content += "<li>Mark Zealey (" + developer + ")</li>";
    content += "</ul>";


    /****************************************************************************************
    ***               NB!!! Credits are sorted alphabetically by last name!               ***
    ****************************************************************************************/
    content += MAKE_BOLD(tr("The following people translated BibleTime into their "
                            "language:"));
    content += BR "<ul>"
                   "<li>Horatiu Alexe</li>"
                   "<li>Luis Barron</li>"
                   "<li>Jan B&#x11B;lohoubek</li>"
                   "<li>Chun-shek Chan</li>"
                   "<li>Nouhoun Y. Diarra</li>"
                   "<li>Rafael Fagundes</li>"
                   "<li>Ilpo Kantonen</li>"
                   "<li>Pavel Laukko</li>"
                   "<li>Piotr Markiewicz</li>"
                   "<li>G&eacute;za Nov&aacute;k</li>"
                   "<li>Gabriel P&eacute;rez</li>"
                   "<li>Igor Plisco</li>"
                   "<li>Zdenko Podobn&yacute;</li>"
                   "<li>Jaak Ristioja</li>"
                   "<li>Igor Rykhlin</li>"
                   "<li>Vlad Savitsky</li>"
                   "<li>Henrik Sonesson</li>"
                   "<li>Johan van der Lingen</li>"
                   "<li>Jean Van Schaftingen</li>"
                   "<li>Roland Teschner</li>"
                   "<li>Giovanni Tedaldi</li>"
                   "<li>Dmitry Yurevich</li>"
                   "<li>Esteban Zeller</li>"
               "</ul>" BR;
    content += tr("Some names may be missing, please email "
                  "bibletime-translations@lists.sourceforge.net if you notice errors or "
                  "omissions.");

    setHtml(MAKE_HTML(content));
}


void BtAboutDialog::initSwordTab() {
    selectTab(2);
    setTabText("Sword");

    QString version(sword::SWVersion::currentVersion.getText());
    QString content(BR BR);
    content += MAKE_CENTER(MAKE_BOLD(tr("SWORD library version %1").arg(version)));
    content += BR;
    content += tr("BibleTime makes use of the SWORD Project. The SWORD Project is the "
                  "CrossWire Bible Society's free Bible software project. Its purpose is to "
                  "create cross-platform open-source tools-- covered by the GNU General "
                  "Public License-- that allow programmers and Bible societies to write new "
                  "Bible software more quickly and easily.");
    content += BR BR;
    content += tr("The SWORD Project");
    content += BR MAKE_LINK_STATIC("http://www.crosswire.org/sword/index.jsp",
                                   "www.crosswire.org/sword/index.jsp");

    setHtml(content);
}

void BtAboutDialog::initQtTab() {
    selectTab(3);
    setTabText("Qt");

    QString content(BR BR MAKE_CENTER_STATIC(MAKE_BOLD_STATIC("Qt")) BR);
    content += tr("This program uses Qt version %1.").arg(qVersion());
    content += BR BR;
    content += tr("Qt is a cross-platform application and UI framework, created with C++ "
                  "language. It has been released under the LGPL license.");
    content += BR BR MAKE_LINK_STATIC("http://qt.nokia.com/", "http://qt.nokia.com/");
    setHtml(MAKE_HTML(content));
}

void BtAboutDialog::initLicenceTab() {
    QFile licFile(util::directory::getLicenseDir().path() + "/license.html");
    if (licFile.open(QFile::ReadOnly)) {
        selectTab(4);
        setTabText(tr("License"));

        QString text;
        text += tr("BibleTime is released under the GPL license.");
        text += " ";
        text += tr("You can download and use (but not distribute) the program for personal, "
                   "private, public or commercial purposes without restrictions.");
        text += " ";
        text += tr("You can give away or distribute the program if you also distribute the "
                   "corresponding source code.");
        text += BR BR;
        text += tr("The complete legally binding license is below.");

        setHtml(QTextStream(&licFile).readAll().replace("TRANSLATED TEXT", text));
        licFile.close();
    }
}
