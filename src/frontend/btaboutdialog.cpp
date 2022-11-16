/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btaboutdialog.h"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QIODevice>
#include <QLabel>
#include <QPalette>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTextStream>
#include <QtGlobal>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include <utility>
#include "../util/btconnect.h"
#include "../util/bticons.h"
#include "../util/directory.h"
#include "messagedialog.h"

// Sword includes:
#include <swversion.h>


#define MAKE_STYLE(t) "<style type=\"text/css\">"\
    "body{"\
        "background-color:" + (t)->palette().color(QPalette::Window).name() + ";"\
        "color:" + (t)->palette().color(QPalette::WindowText).name() +\
    "}"\
    "h3{font-weight:bold;text-align:center}"\
    "a{text-decoration:underline}"\
    "a:link{color:" + (t)->palette().color(QPalette::Link).name() + "}"\
    "a:visited{color:" + (t)->palette().color(QPalette::LinkVisited).name() + "}"\
    "</style>"
#define MAKE_HTML(t,x) "<html><head>" MAKE_STYLE(t) "</head><body>" + (x) + "</body></html>"
#define MAKE_LINK(c,u,t) "<a href=\"" u "\">"; (c) += (t); (c) += "</a>"
#define MAKE_LINK_STATIC(u,t) "<a href=\"" u "\">" t "</a>"
#define MAKE_CONTR(c,n,r) "<li>" n " (";\
    (c) += (r);\
    (c) += ")</li>"
#define MAKE_CONTR2(c,n,r,r2) "<li>" n " (";\
    (c) += (r);\
    (c) += ", ";\
    (c) += (r2);\
    (c) += ")</li>"

class BtAboutDialog::LicenseTab final : public QWidget {

public: // Methods:

    LicenseTab(QWidget * parent)
        : QWidget(parent)
        , m_label(new QLabel(this))
        , m_licenseBrowser(new QPlainTextEdit(this))
    {
        auto * const mainLayout = new QVBoxLayout(this);

        m_label->setWordWrap(true);
        mainLayout->addWidget(m_label);

        m_licenseBrowser->setReadOnly(true);
        { // Set monospace font:
            QFont licenseFont(QStringLiteral("BibleTime nonexistant font"));
            licenseFont.setStyleHint(QFont::Monospace);
            m_licenseBrowser->setFont(std::move(licenseFont));
        }
        mainLayout->addWidget(m_licenseBrowser);
    }

    void setLabelText(QString const & text) { m_label->setText(text); }

    void setLicense(QString const & license)
    { m_licenseBrowser->setPlainText(license); }

private: // Fields

    QLabel * const m_label;
    QPlainTextEdit * const m_licenseBrowser;

}; // LicenseTab

BtAboutDialog::BtAboutDialog(QWidget *parent, Qt::WindowFlags wflags)
        : QDialog(parent, wflags)
{
    setAttribute(Qt::WA_DeleteOnClose);
    resize(640, 380);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QWidget *top = new QWidget(this);
    QHBoxLayout *topLayout = new QHBoxLayout;
    m_iconLabel = new QLabel(this);
    m_iconLabel->setPixmap(BtIcons::instance().icon_bibletime.pixmap(48));
    topLayout->addWidget(m_iconLabel);
    m_versionLabel = new QLabel(this);
    QFont font = m_versionLabel->font();
    font.setPointSize(font.pointSize()+6);
    font.setBold(true);
    m_versionLabel->setFont(font);
    topLayout->addWidget(m_versionLabel);
    top->setLayout(topLayout);
    mainLayout->addWidget(top, 0, Qt::AlignCenter);

    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget);

    auto const addTab = [this]{
        auto * const tab = new QTextBrowser(this);
        tab->setOpenLinks(false);
        m_tabWidget->addTab(tab, "");
        BT_CONNECT(tab, &QTextBrowser::anchorClicked,
                   [](QUrl const & url) {
                       if (url.scheme() == "qt") {
                           qApp->aboutQt();
                       } else {
                           QDesktopServices::openUrl(url);
                       }
                   });
        return tab;
    };

    m_bibletimeTab = addTab();
    m_contributorsTab = addTab();
    m_swordTab = addTab();
    m_qtTab = addTab();

    {
        auto const & licensePath = util::directory::getLicensePath();
        QFile licenseFile(licensePath);
        if (licenseFile.open(QFile::ReadOnly)) {
            m_licenseTab = new LicenseTab(this);
            m_licenseTab->setLicense(QTextStream(&licenseFile).readAll());
            m_tabWidget->addTab(m_licenseTab, "");
            licenseFile.close();
        } else {
            qWarning() << "Failed to read license from" << licensePath;
        }
    }

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    mainLayout->addWidget(m_buttonBox);
    setLayout(mainLayout);

    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this, &BtAboutDialog::reject);

    retranslateUi();
}

void BtAboutDialog::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event)
    QString version = "BibleTime " BT_VERSION;
    QFontMetrics fm(m_versionLabel->font());
    int w = width()  - m_iconLabel->width() - 80;
    QString shortVersion = fm.elidedText(version, Qt::ElideMiddle, w);
    m_versionLabel->setText(shortVersion);
}

void BtAboutDialog::retranslateUi() {
    setWindowTitle(tr("About BibleTime"));

    retranslateBtTab();
    retranslateContributorsTab();
    retranslateSwordTab();
    retranslateQtTab();
    retranslateLicenceTab();

    message::prepareDialogBox(m_buttonBox);
}

void BtAboutDialog::retranslateBtTab() {
    m_tabWidget->setTabText(0, tr("&BibleTime"));

    QString content("<p>");
    content += tr("BibleTime is an easy to use but powerful Bible study tool.");
    content += "</p><p>";
    content += tr("(c)1999-2021, The BibleTime Team");
    content += "</p><p>" MAKE_LINK_STATIC("https://bibletime.info", "bibletime.info")
               "</p>";
    m_bibletimeTab->setHtml(MAKE_HTML(m_bibletimeTab, content));
}

void BtAboutDialog::retranslateContributorsTab() {
    m_tabWidget->setTabText(1, tr("&Contributors"));

    const QString developer(tr("developer"));
    const QString designer(tr("designer"));
    const QString artist(tr("artist"));

    /****************************************************************************************
    ***               NB!!! Credits are sorted alphabetically by last name!               ***
    ****************************************************************************************/

    QString content("<p><b>");
    content += tr("The following people contributed to BibleTime:");
    content += "</b></p><ul>"
        MAKE_CONTR(content, "Thomas Abthorpe", tr("documentation and translation manager"))
        MAKE_CONTR2(content, "Joachim Ansorg", tr("project founder"), developer)
        MAKE_CONTR(content, "David Blue", designer)
        MAKE_CONTR(content, "Tim Brodie", developer)
        MAKE_CONTR(content, "Timothy R. Butler", designer)
        MAKE_CONTR(content, "Jim Campbell", developer)
        MAKE_CONTR(content, "Lee Carpenter", developer)
        MAKE_CONTR(content, "Jeremy Erickson", tr("packager"))
        MAKE_CONTR(content, "Troy A. Griffitts", tr("creator of The Sword Project"))
        MAKE_CONTR(content, "Martin Gruner", developer)
        MAKE_CONTR(content, "Thomas Hagedorn", tr("domain sponsor"))
        MAKE_CONTR(content, "Bob Harman", tr("howto"))
        MAKE_CONTR(content, "Gary Holmlund", developer)
        MAKE_CONTR(content, "Nikolay Igotti", developer)
        MAKE_CONTR(content, "Laurent Valentin Jospin", artist)
        MAKE_CONTR(content, "Eeli Kaikkonnen", developer)
        MAKE_CONTR(content, "Chris Kujawa", developer)
        MAKE_CONTR(content, "Mark Lybarger", developer)
        MAKE_CONTR(content, "Konstantin Maslyuk", developer)
        MAKE_CONTR(content, "Luke Mauldin", developer)
        MAKE_CONTR(content, "James Ots", designer)
        MAKE_CONTR(content, "Andrus Raag", artist)
        MAKE_CONTR2(content, "Jaak Ristioja", tr("project manager"), developer)
        MAKE_CONTR(content, "Fred Saalbach", tr("documentation"))
        MAKE_CONTR(content, "Erik Schanze", tr("documentation"))
        MAKE_CONTR(content, "Gary Sims", developer)
        MAKE_CONTR2(content, "Wolfgang Stradner", tr("tester"), tr("usability expert"))
        MAKE_CONTR(content, "Kang Sun", developer)
        MAKE_CONTR(content, "Thorsten Uhlmann", developer)
        MAKE_CONTR(content, "John Turpish", developer)
        MAKE_CONTR(content, "David White", developer)
        MAKE_CONTR(content, "Mark Zealey", developer)
        MAKE_CONTR(content, "Patrick Sebastian Zimmermann", developer)
    "</ul><p><b>";


    /****************************************************************************************
    ***               NB!!! Credits are sorted alphabetically by last name!               ***
    ****************************************************************************************/
    content += tr("The following people translated BibleTime into their language:");
    content += "</b></p><ul>"
                   "<li>Roy Alvear Aguirre</li>"
                   "<li>Horatiu Alexe</li>"
                   "<li>Andrew Alfy</li>"
                   "<li>Jan B&#x11B;lohoubek</li>"
                   "<li>Luis Barron</li>"
                   "<li>M&aacute;rio Castanheira</li>"
                   "<li>Chun-shek Chan</li>"
                   "<li>Ján Ďanovský</li>"
                   "<li>Nouhoun Y. Diarra</li>"
                   "<li>Rafael Fagundes</li>"
                   "<li>Eeli Kaikkonen</li>"
                   "<li>Ilpo Kantonen</li>"
                   "<li>Pavel Laukko</li>"
                   "<li>Johan van der Lingen</li>"
                   "<li>Piotr Markiewicz</li>"
                   "<li>Konstantin Maslyuk</li>"
                   "<li>G&eacute;za Nov&aacute;k</li>"
                   "<li>Gabriel P&eacute;rez</li>"
                   "<li>Igor Plisco</li>"
                   "<li>Zdenko Podobn&yacute;</li>"
                   "<li>Jaak Ristioja</li>"
                   "<li>Igor Rykhlin</li>"
                   "<li>Vlad Savitsky</li>"
                   "<li>Jean Van Schaftingen</li>"
                   "<li>Flavio Theodor de Lima Silva</li>"
                   "<li>Henrik Sonesson</li>"
                   "<li>Giovanni Tedaldi</li>"
                   "<li>Roland Teschner</li>"
                   "<li>Damian Wrzalski</li>"
                   "<li>Dmitry Yurevich</li>"
                   "<li>Esteban Zeller</li>"
                   "<li>Aaron Zhou</li>"
               "</ul><p>";
    content += tr("Some names may be missing, please file an issue at %1 if "
                  "you notice errors or omissions.").arg(MAKE_LINK_STATIC(
                          "https://github.com/bibletime/bibletime/issues",
                          "https://github.com/bibletime/bibletime/issues"));
    content += "</p>";

    m_contributorsTab->setHtml(MAKE_HTML(m_contributorsTab, content));
}


void BtAboutDialog::retranslateSwordTab() {
    m_tabWidget->setTabText(2, tr("&SWORD"));

    QString version(sword::SWVersion::currentVersion.getText());
    QString content("<h3>");
    content += tr("SWORD library version %1").arg(version);
    content += "</h3><p>";
    content += tr("BibleTime makes use of the SWORD Project. The SWORD Project is the "
                  "CrossWire Bible Society's free Bible software project. Its purpose is to "
                  "create cross-platform open-source tools &mdash; covered by the GNU "
                  "General Public License &mdash; that allow programmers and Bible "
                  "societies to write new Bible software more quickly and easily.");
    content += "</p><p>";
    content += tr("The SWORD Project: ");
    content += MAKE_LINK_STATIC("http://www.crosswire.org/sword/",
                                "www.crosswire.org/sword") "</p>";

    m_swordTab->setHtml(MAKE_HTML(m_swordTab, content));
}

void BtAboutDialog::retranslateQtTab() {
    m_tabWidget->setTabText(3, tr("&Qt"));

    QString content("<h3>");
    content += tr("Qt toolkit version %1").arg(qVersion());
    content += "</h3><p>";
    content += tr("This program uses Qt version %1.").arg(qVersion());
    content += "</p><p>";
    content += tr("Qt is a cross-platform application and UI framework, created with C++ "
                  "language. It has been released under the LGPL license.");

    content += " " MAKE_LINK(content, "qt://about", tr("More info...")) "</p>";

    m_qtTab->setHtml(MAKE_HTML(m_qtTab, content));
}

void BtAboutDialog::retranslateLicenceTab() {
    if (!m_licenseTab)
        return;

    m_tabWidget->setTabText(m_tabWidget->indexOf(m_licenseTab), tr("&License"));

    m_licenseTab->setLabelText(
                QStringLiteral("<p>%1</p><p>%2</p>")
                .arg(tr("BibleTime is released under the GPL license. You "
                        "can download and use the program for personal, "
                        "private, public or commercial purposes without "
                        "restrictions, but can give away or distribute the "
                        "program only if you also distribute the "
                        "corresponding source code."),
                     tr("The complete legally binding license is below.")));
}
