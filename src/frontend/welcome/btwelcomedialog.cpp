/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btwelcomedialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../../util/btconnect.h"
#include "../../util/bticons.h"
#include "../../util/cresmgr.h"
#include "../bibletime.h"
#include "../messagedialog.h"


void BtWelcomeDialog::openWelcome() {
    BtWelcomeDialog dlg(BibleTime::instance());
    dlg.exec();
}

BtWelcomeDialog::BtWelcomeDialog(QWidget *parent, Qt::WindowFlags wflags)
    : QDialog(parent, wflags)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(CResMgr::mainMenu::help::tipOfTheDay::icon());
    resize(560, 300);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_iconLabel = new QLabel(this);
    m_iconLabel->setPixmap(BtIcons::instance().icon_bibletime.pixmap(48));
    mainLayout->addWidget(m_iconLabel, 0, Qt::AlignHCenter);

    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    mainLayout->addWidget(m_label);

    mainLayout->addStretch();

    m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);

    m_installButton = m_buttonBox->addButton(
                "", QDialogButtonBox::AcceptRole);

    m_laterButton = m_buttonBox->addButton(
                "", QDialogButtonBox::RejectRole);

    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);

    retranslateUi();

    BT_CONNECT(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    BT_CONNECT(m_buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));

}

void BtWelcomeDialog::slotAccept() {
    hide();
    accept();
    BibleTime::instance()->slotBookshelfWizard();
}

void BtWelcomeDialog::retranslateUi() {
    setWindowTitle(tr("Welcome to BibleTime"));

    QString msg("<p>");
    msg += tr("BibleTime is an easy to use but powerful Bible study tool.");
    msg += "</p><p>";
    msg += tr("Before you can use this application some works must be installed. ");
    msg += tr("Various works such as Bibles, books, commentaries, and lexicons are available from remote libraries. ");
    msg += "</p><p>";
    msg += tr("Choose the \"Install works\" button to download works. ");
    msg += tr("The menu \"Settings > Bookshelf Manager\" also installs works and can be used later.");
    m_label->setText(msg);

    m_laterButton->setText(tr("Install later"));

    m_installButton->setText( QString("      ") + tr("Install works...") + QString("      "));
    QFont font = m_installButton->font();
    font.setBold(true);
    m_installButton->setFont(font);
}

//void BtWelcomeDialog::linkClicked(const QUrl& url) {
//    QDesktopServices::openUrl(url);
//}
