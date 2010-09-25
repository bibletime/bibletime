/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bttipdialog.h"

#include "backend/config/cbtconfig.h"
#include "util/cresmgr.h"
#include "util/directory.h"

#include <QCheckBox>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWebView>


namespace {

inline QString vertical_align(const QString &text) {
    return "<table height=\"100%\"><tr><td style=\"vertical-align:middle\" "
            "height=\"100%\">" + text + "</td></tr></table>";
}

inline QString make_style(QWidget *widget) {
    const QPalette &p = widget->palette();
    return "<style type=\"text/css\">"
        "body{"
            "background-color:" + p.color(QPalette::Base).name() + ";"
            "color:" + p.color(QPalette::Text).name() + "}"
        "h3{font-weight:bold;text-align:center}"
        "a{text-decoration:underline}"
        "a:link{color:" + p.color(QPalette::Link).name() + "}"
        "a:visited{color:" + p.color(QPalette::LinkVisited).name() + "}"
    "</style>";
}

inline QString make_html(QWidget *widget, const QString &text) {
    return "<html><head>" + make_style(widget) + "</head><body>"
            + vertical_align(text) + "</body></html>";
}

inline QString make_icon(const QString &icon) {
    namespace DU = util::directory;
    QString fileName = DU::getIconDir().filePath(icon);
    QString iconUrl = QUrl::fromLocalFile(fileName).toString();
    return "<img src=\"" + iconUrl + "\" width=\"32\" />";
}

} // anonymous namespace


BtTipDialog::BtTipDialog(QWidget *parent, Qt::WindowFlags wflags)
        : QDialog(parent, wflags)
{
    namespace DU = util::directory;

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Tip Of The Day"));
    setWindowIcon(DU::getIcon(CResMgr::mainMenu::help::tipOfTheDay::icon));
    resize(450, 220);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_tipView = new QWebView;
    m_tipView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tipView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    mainLayout->addWidget(m_tipView);

    QHBoxLayout* hLayout = new QHBoxLayout;

    m_showTipsCheckBox = new QCheckBox;
    m_showTipsCheckBox->setText(tr("Show tips at startup"));
    bool showTips = CBTConfig::get(CBTConfig::showTipAtStartup);
    m_showTipsCheckBox->setChecked(showTips);
    hLayout->addWidget(m_showTipsCheckBox);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Close,
                                       Qt::Horizontal,
                                       this);
    QPushButton *nextButton;
    nextButton = m_buttonBox->addButton(tr("Next Tip"),
                                        QDialogButtonBox::ActionRole);
    hLayout->addWidget(m_buttonBox);

    mainLayout->addLayout(hLayout);
    setLayout(mainLayout);

    bool ok;
    ok = connect(m_showTipsCheckBox, SIGNAL(toggled(bool)),
                     this,               SLOT(startupBoxChanged(bool)));
    Q_ASSERT(ok);

    ok = connect(m_buttonBox, SIGNAL(rejected()),
                     this,        SLOT(reject()));
    Q_ASSERT(ok);

    ok = connect(nextButton, SIGNAL(clicked()),
                     this,       SLOT(nextTip()));
    Q_ASSERT(ok);

    ok = connect(m_tipView->page(), SIGNAL(linkClicked(const QUrl&)),
                     this,              SLOT(linkClicked(const QUrl&)));
    Q_ASSERT(ok);

    m_tipNumber = CBTConfig::get(CBTConfig::tipNumber);
    initTips();
    displayTip();
}

void BtTipDialog::initTips() {
    m_tips.clear();

    m_tips << tr("To add multiple Bible works in parallel in your active Bible window"
        " select this icon and choose another Bible work.")
        + "<br><center>" + make_icon(CResMgr::modules::bible::icon_add) + "</center>";

    m_tips << tr("To add multiple commentary works in parallel in your active commentary window"
        " select this icon and choose another commentary work.")
        + "<br><center>" + make_icon(CResMgr::modules::commentary::icon_add) + "</center>";

    m_tips << tr("To learn more about the BibleTime project please go to our web site.")
        + "<br><center><a href=\"http://www.bibletime.info\">www.bibletime.info</a></center>";

    m_tips << tr("To synchronize a commentary window with the active Bible window, activate the"
        " commentary window and select this icon.") + "<br><center>"
        + make_icon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon)
        + "</center><br>" + tr("Select the icon again to stop the synchronization.");

    m_tips << tr("To create a bookmark drag any verse reference from a Bible or commentary work"
        " into the Bookmarks window. An arrow will indicate the position that the bookmark will"
        " go when you release the cursor. Other works will have a reference in the upper left"
       " corner that can be used to create a bookmark.");

    m_tips << tr("To change a bookmark title or description, right click on the bookmark"
        " and select \"Edit Bookmark...\". After finishing the edit the description can be"
        " seen by hovering over the bookmark.");
}

void BtTipDialog::displayTip() {
    m_tipView->setHtml(make_html(this, m_tips[m_tipNumber]));
}

void BtTipDialog::startupBoxChanged(bool checked) {
    CBTConfig::set(CBTConfig::showTipAtStartup, checked);
}

void BtTipDialog::nextTip() {
    m_tipNumber++;
    if (m_tipNumber >= m_tips.count()) {
        m_tipNumber = 0;
    }
    CBTConfig::set(CBTConfig::tipNumber, m_tipNumber);
    displayTip();
}

void BtTipDialog::linkClicked(const QUrl& url) {
    QDesktopServices::openUrl(url);
}
