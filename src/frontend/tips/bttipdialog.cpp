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

#include "bttipdialog.h"

#include <QCheckBox>
#include <QColor>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFlags>
#include <QFont>
#include <QHBoxLayout>
#include <QPalette>
#include <QPushButton>
#include <QSizePolicy>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
#include "../../backend/config/btconfig.h"
#include "../../util/btconnect.h"
#include "../../util/bticons.h"
#include "../../util/cresmgr.h"
#include "../messagedialog.h"


class QUrl;

namespace {

inline QString vertical_align(const QString &text) {
    return QStringLiteral(
                "<table height=\"100%\"><tr>"
                "<td style=\"vertical-align:middle\" height=\"100%\">%1</td>"
                "</tr></table>").arg(text);
}

inline QString make_style(QWidget *widget) {
    auto const & p = widget->palette();
    return QStringLiteral(
                "<style type=\"text/css\">"
                    "body{"
                        "background-color:%1;"
                        "color:%2"
                    "}"
                    "h3{font-weight:bold;text-align:center}"
                    "a{text-decoration:underline}"
                    "a:link{color:%3}"
                    "a:visited{color:%4}"
                "</style>")
            .arg(p.color(QPalette::Base).name(),
                 p.color(QPalette::Text).name(),
                 p.color(QPalette::Link).name(),
                 p.color(QPalette::LinkVisited).name());
}

inline QString make_html(QWidget *widget, const QString &text) {
    return QStringLiteral("<html><head>%1</head><body>%2</body></html>")
            .arg(make_style(widget), vertical_align(text));
}

auto const LastTipNumberKey = QStringLiteral("GUI/lastTipNumber");

} // anonymous namespace


BtTipDialog::BtTipDialog(QWidget *parent, Qt::WindowFlags wflags)
        : QDialog(parent, wflags)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Tip Of The Day"));
    setWindowIcon(CResMgr::mainMenu::help::tipOfTheDay::icon());
    resize(450, 240);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_tipView = new QTextBrowser(this);
    m_tipView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tipView->setOpenExternalLinks(true);
    m_tipView->setStyleSheet(
                QStringLiteral("QTextEdit{background-color:rgb(255,255,255)}"));
    QFont font = m_tipView->font();
    font.setPointSize(font.pointSize()+2);
    m_tipView->setFont(font);
    mainLayout->addWidget(m_tipView);

    QHBoxLayout* hLayout = new QHBoxLayout;

    m_showTipsCheckBox = new QCheckBox;
    m_showTipsCheckBox->setText(tr("Show tips at startup"));
    m_showTipsCheckBox->setChecked(
                btConfig().value<bool>(QStringLiteral("GUI/showTipAtStartup"),
                                       true));
    hLayout->addWidget(m_showTipsCheckBox);

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Close,
                                       Qt::Horizontal,
                                       this);
    message::prepareDialogBox(m_buttonBox);

    QPushButton *nextButton;
    nextButton = m_buttonBox->addButton(tr("Next Tip"),
                                        QDialogButtonBox::ActionRole);
    hLayout->addWidget(m_buttonBox);

    mainLayout->addLayout(hLayout);
    setLayout(mainLayout);

    BT_CONNECT(m_showTipsCheckBox, &QCheckBox::toggled,
               [](bool v) { btConfig().setValue("GUI/showTipAtStartup", v); });
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this,        &BtTipDialog::reject);
    BT_CONNECT(nextButton, &QPushButton::clicked,
               [this]{
                   m_tipNumber = (m_tipNumber + 1) % m_tips.size();
                   btConfig().setValue(LastTipNumberKey, m_tipNumber);
                   displayTip();
               });
    BT_CONNECT(m_tipView, &QTextBrowser::anchorClicked,
               [](QUrl const & url) { QDesktopServices::openUrl(url); });

    m_tipNumber = btConfig().value<int>(LastTipNumberKey, 0);
    retranslateUi();
}

void BtTipDialog::retranslateUi() {
    m_tips.clear();

    m_tips << tr("The currently active window can be auto scrolled up or down."
                 " Start scrolling by pressing Shift+Down. You can increase the"
                 " scrolling speed by pressing Shift+Down multiple times. To"
                 " pause scrolling press Space. To start scrolling again at the"
                 " previous speed press Space again. To slow down scrolling or scroll"
                 " the other direction press Shift+Up one or more times. Pressing"
                 " any other key or changing the active window will stop the scrolling.");

    m_tips << tr("To add multiple Bible works in parallel in your active Bible or commentary window"
        " select this icon and choose another Bible or commentary work.")
        + "<br><center>" + iconToHtml(CResMgr::modules::bible::icon_add()) + "</center>";

    m_tips << tr("To add multiple commentary works in parallel in your active commentary window"
        " select this icon and choose another commentary work.")
        + "<br><center>" + iconToHtml(CResMgr::modules::commentary::icon_add()) + "</center>";

    m_tips << tr("To learn more about the BibleTime project please go to our web site.")
        + "<br><center><a href=\"https://bibletime.info\">bibletime.info</a></center>";

    m_tips << tr("To synchronize a commentary window with the active Bible window, activate the"
        " commentary window and select this icon.") + "<br><center>"
        + iconToHtml(CResMgr::displaywindows::commentaryWindow::syncWindow::icon())
        + "</center><br>" + tr("Select the icon again to stop the synchronization.");

    m_tips << tr("To create a bookmark drag any verse reference from a Bible or commentary work"
        " into the Bookmarks window. An arrow will indicate the position that the bookmark will"
        " go when you release the cursor. Other works will have a reference in the upper left"
        " corner that can be used to create a bookmark.");

    m_tips << tr("To change a bookmark title or description, right click on the bookmark"
        " and select the Edit Bookmark menu. After finishing the edit the description can be"
        " seen by hovering over the bookmark.");

    m_tips << tr("To find more information about a work, go the the Bookshelf window, right"
        " click on the work, and select the About menu.");

    m_tips << tr("The Bookshelf, Bookmark, and Mag windows can be moved to new locations by"
        " dragging them from the title at the top of each window. They can be placed to the left,"
        " right, above, or below the works windows. They can be placed on top of each other and"
        " tabs will appear so each window can be selected. They can be resized by dragging the"
        " border between the window and another window.");

    m_tips << tr("You can search for Strong's numbers in a work. Start with a work that has Strong's"
        " numbers and hover over a word. Right click the word and use the Strong's Search"
        " menu. A search dialog will appear that allows you to see the use of the same"
        " Strong's number in other locations of the work.");

    m_tips << tr("You can save personal notes for specific verse references. You must install"
        " the Personal commentary. Open the Bookshelf Manager, choose Crosswire as the"
        " source, English as the language,  and look under Commentary. Once installed, open it"
        " like any other window, or in  parallel with a bible. Click a verse to edit it.");

    m_tips << tr("You can view Strong's number information in the MAG window by hovering over"
        " a word in a Bible work that has Strong's numbers. You should have the StrongsGreek"
        " and StrongsHebrew lexicons from Crosswire installed.");

    m_tips << tr("You can save your open windows in a session. Such a session can easily be restored"
        " later on. You can save as many sessions as you like. The session feature can be"
        " accessed under the Window menu entry.");

    displayTip();
}

void BtTipDialog::displayTip()
{ m_tipView->setHtml(make_html(this, m_tips[m_tipNumber])); }
