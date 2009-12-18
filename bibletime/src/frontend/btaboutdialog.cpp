/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/btaboutdialog.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTabWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWebView>
#include "util/dialogutil.h"
#include "util/directory.h"

// Sword includes:
#include <swversion.h>


#define BR "<br/>"
#define MAKE_CENTER(x) "<center>" + (x) + "</center>"
#define MAKE_CENTER_STATIC(x) "<center>" x "</center>"
#define MAKE_STYLE(t) "<style type=\"text/css\">"\
    "body{"\
        "background-color:" + (t)->palette().color(QPalette::Window).name() + ";"\
        "color:" + (t)->palette().color(QPalette::WindowText).name() +\
    "}"\
    "a{text-decoration:underline}"\
    "a:link{color:" + (t)->palette().color(QPalette::Link).name() + "}"\
    "a:visited{color:" + (t)->palette().color(QPalette::LinkVisited).name() + "}"\
    "</style>"
#define MAKE_HTML(t,x) "<html><head>" MAKE_STYLE(t) "</head><body>" + (x) + "</body></html>"
#define MAKE_BOLD(x) "<b>" + (x) + "</b>"
#define MAKE_BOLD_STATIC(x) "<b>" x "</b>"
#define MAKE_LINK(u,t) "<a href=\"" u "\">" + (t) + "</a>"
#define MAKE_LINK_STATIC(u,t) "<a href=\"" u "\">" t "</a>"

BtAboutDialog::BtAboutDialog(QWidget *parent, Qt::WindowFlags wflags)
        : QDialog(parent, wflags)
{
    setAttribute(Qt::WA_DeleteOnClose);
    resize(550, 340);

    m_layout = new QVBoxLayout(this);

    QWidget *top = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout;
    QLabel *iconLabel = new QLabel(this);
    iconLabel->setPixmap(QIcon(util::directory::getIconDir().path() + "/bibletime.svg").pixmap(48));
    topLayout->addWidget(iconLabel);
    topLayout->addWidget(new QLabel("<h1><b>BibleTime " BT_VERSION "</b></h1>"), 1);
    top->setLayout(topLayout);
    m_layout->addWidget(top, 0, Qt::AlignCenter);

    m_tabWidget = new QTabWidget(this);
    m_layout->addWidget(m_tabWidget);

    initTab(m_bibletimeTab);
    initTab(m_contributorsTab);
    initTab(m_swordTab);
    initTab(m_qtTab);
    initTab(m_licenceTab);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    m_layout->addWidget(m_buttonBox);
    setLayout(m_layout);

    connect(m_buttonBox, SIGNAL(accepted()),
            this,        SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()),
            this,        SLOT(reject()));

    retranslateUi();
}

BtAboutDialog::~BtAboutDialog() {
    // Intentionally empty
}

void BtAboutDialog::initTab(QWebView *&tab) {
    tab = new QWebView(this);
    m_tabWidget->addTab(tab, "");
    tab->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(tab, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));
}

void BtAboutDialog::retranslateUi() {
    setWindowTitle(tr("About BibleTime"));

    retranslateBtTab();
    retranslateContributorsTab();
    retranslateSwordTab();
    retranslateQtTab();
    retranslateLicenceTab();

    util::prepareDialogBox(m_buttonBox);
}

void BtAboutDialog::retranslateBtTab() {
    m_tabWidget->setTabText(0, tr("&BibleTime"));

    QString content(tr("BibleTime is an easy to use but powerful Bible study tool."));
    content += BR BR;
    content += tr("We are looking for developers and translators.");
    content += " ";
    content += tr("If you would like to join our team, please send an email to "
                  "info@bibletime.info.");
    content += BR BR;
    content += tr("(c)1999-2009, The BibleTime Team");
    content += BR MAKE_LINK_STATIC("http://www.bibletime.info", "http://www.bibletime.info");
    m_bibletimeTab->setHtml(MAKE_HTML(m_bibletimeTab, content));
}

void BtAboutDialog::retranslateContributorsTab() {
    m_tabWidget->setTabText(1, tr("&Contributors"));

    const QString developer(tr("developer"));
    const QString designer(tr("designer"));

    /****************************************************************************************
    ***               NB!!! Credits are sorted alphabetically by last name!               ***
    ****************************************************************************************/
    QString content(MAKE_BOLD(tr("The following people contributed to BibleTime:")));
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

    m_contributorsTab->setHtml(MAKE_HTML(m_contributorsTab, content));
}


void BtAboutDialog::retranslateSwordTab() {
    m_tabWidget->setTabText(2, tr("&Sword"));

    QString version(sword::SWVersion::currentVersion.getText());
    QString content(MAKE_CENTER(MAKE_BOLD(tr("SWORD library version %1").arg(version))));
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

    m_swordTab->setHtml(MAKE_HTML(m_swordTab, content));
}

void BtAboutDialog::retranslateQtTab() {
    m_tabWidget->setTabText(3, tr("&Qt"));

    QString content(MAKE_CENTER_STATIC(MAKE_BOLD_STATIC("Qt")) BR);
    content += tr("This program uses Qt version %1.").arg(qVersion());
    content += BR BR;
    content += tr("Qt is a cross-platform application and UI framework, created with C++ "
                  "language. It has been released under the LGPL license.");
    content += " ";
    content += MAKE_LINK("about:qt", tr("More info..."));
    m_qtTab->setHtml(MAKE_HTML(m_qtTab, content));
}

void BtAboutDialog::retranslateLicenceTab() {
    m_tabWidget->setTabText(4, tr("&License"));

    QFile licFile(util::directory::getLicenseDir().path() + "/license.html");
    if (licFile.open(QFile::ReadOnly)) {

        QString text("<p>");
        text += tr("BibleTime is released under the GPL license.");
        text += " ";
        text += tr("You can download and use (but not distribute) the program for personal, "
                   "private, public or commercial purposes without restrictions.");
        text += " ";
        text += tr("You can give away or distribute the program if you also distribute the "
                   "corresponding source code.");
        text += BR BR;
        text += tr("The complete legally binding license is below.");
        text += "<hr/></p>";

        QString content(QTextStream(&licFile).readAll().replace("<!-- TR TEXT -->", text));
        content.replace("<!-- HEADER -->", MAKE_STYLE(m_licenceTab), Qt::CaseInsensitive);
        m_licenceTab->setHtml(content);
        licFile.close();
    }
}

void BtAboutDialog::linkClicked(const QUrl &url) {
    if (url.scheme() == "about") {
        if (url.path() == "qt") {
            qApp->aboutQt();
        }
    } else {
        QDesktopServices::openUrl(url);
    }
}
