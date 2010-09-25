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
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QWebView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDesktopServices>

BtTipDialog::BtTipDialog(QWidget *parent, Qt::WindowFlags wflags)
        : QDialog(parent, wflags), m_buttonBox(0), m_tipView(0), m_showTipsCheckBox(0)
{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("Tip Of The Day"));
    resize(450, 220);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_tipView = new QWebView;
    m_tipView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(m_tipView);

    QHBoxLayout* hLayout = new QHBoxLayout;

    m_showTipsCheckBox = new QCheckBox;
    m_showTipsCheckBox->setText(tr("Show tips at startup"));
    bool showTips = CBTConfig::get(CBTConfig::showTipAtStartup);
    m_showTipsCheckBox->setChecked(showTips);
    hLayout->addWidget(m_showTipsCheckBox);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, this);
    m_buttonBox->addButton(tr("Next Tip"), QDialogButtonBox::ActionRole);
    hLayout->addWidget(m_buttonBox);

    mainLayout->addLayout(hLayout);
    setLayout(mainLayout);

    bool ok;
    ok = connect(m_showTipsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(startupBoxChanged(int)));
    Q_ASSERT(ok);

    ok = connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    Q_ASSERT(ok);

    ok = connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(nextTip(QAbstractButton*)));
    Q_ASSERT(ok);

    m_tipView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    ok = connect(m_tipView->page(), SIGNAL(linkClicked(const QUrl&)), this, SLOT(linkClicked(const QUrl&)));
    Q_ASSERT(ok);

    m_tipNumber = CBTConfig::get(CBTConfig::tipNumber);
    create_tips();
    displayTip();
}

BtTipDialog::~BtTipDialog() {
    // Intentionally empty
}

void BtTipDialog::linkClicked(const QUrl& url) {
    QDesktopServices::openUrl(url);
}

void BtTipDialog::nextTip(QAbstractButton* button) {
    QString text = button->text();
    if (text == tr("Next Tip"))
    {
        m_tipNumber++;
        if (m_tipNumber >= m_tips.count())
            m_tipNumber = 0;
        CBTConfig::set(CBTConfig::tipNumber, m_tipNumber);
        displayTip();
    }
}

void BtTipDialog::startupBoxChanged(int value) {
    bool tipAtStartup = m_showTipsCheckBox->isChecked();
    CBTConfig::set(CBTConfig::showTipAtStartup, tipAtStartup);
}

static QString vertical_align(const QString& text)
{
    return "<table height=\"100%\"><tr><td style=\"vertical-align:middle\" height=\"100%\">" + text + "</td></tr></table>";
}

static QString make_style(QWidget* widget)
{
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

static QString make_html(QWidget* widget, const QString& text)
{
    QString vText = vertical_align(text);
    return "<html><head>" + make_style(widget) + "</head><body>" + vText + "</body></html>";
}

static QString make_icon(const QString& icon)
{
    namespace DU = util::directory;
    QString fileName = DU::getIconDir().filePath(icon);
    QUrl url = QUrl::fromLocalFile(fileName);
    QString fileUrl = url.toString();
    return "<img src=\"" + fileUrl + "\" width=\"32\" />";
}

void BtTipDialog::displayTip() {
    QString html = make_html(this, m_tips[m_tipNumber]);
    m_tipView->setHtml(html);
}

/********************************** Tips *************************************************/

void BtTipDialog::create_tips() {

    QString icon = make_icon(CResMgr::modules::bible::icon_add);
    m_tips << tr("To add multiple Bible works in parallel in your active Bible window"
        " select this icon and choose another Bible work.")
        + "<br><center>" + icon + "</center>";

    icon = make_icon(CResMgr::modules::commentary::icon_add);
    m_tips << tr("To add multiple commentary works in parallel in your active commentary window"
        " select this icon and choose another commentary work.")
        + "<br><center>" + icon + "</center>";

    m_tips << tr("To learn more about the BibleTime project please go to our web site.")
        + "<br><center><a href=\"http://www.bibletime.info\">www.bibletime.info</a></center>";

    icon = make_icon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon);
    m_tips << tr("To synchronize a commentary window with the active Bible window, activate the"
        " commentary window and select this icon.")
        + "<br><center>" + icon + "</center><br>"
        + tr("Select the icon again to stop the synchronization.");

    m_tips << tr("To create a bookmark drag any verse reference from a Bible or commentary work"
        " into the Bookmarks window. An arrow will indicate the position that the bookmark will"
        " go when you release the cursor. Other works will have a reference in the upper left"
       " corner that can be used to create a bookmark.");

    m_tips << tr("To change a bookmark title or description, right click on the bookmark"
        " and select \"Edit Bookmark...\". After finishing the edit the description can be"
        " seen by hovering over the bookmark.");
}
