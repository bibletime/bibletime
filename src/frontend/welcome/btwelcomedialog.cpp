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

#include "btwelcomedialog.h"

#include <QDialogButtonBox>
#include <QFlags>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../../util/btconnect.h"
#include "../../util/bticons.h"
#include "../../util/cresmgr.h"


BtWelcomeDialog::BtWelcomeDialog(QWidget *parent, Qt::WindowFlags wflags)
    : QDialog(parent, wflags)
{
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(CResMgr::mainMenu::help::tipOfTheDay::icon());
    resize(560, 300);

    QVBoxLayout * mainLayout = new QVBoxLayout(this);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setPixmap(BtIcons::instance().icon_bibletime.pixmap(48));
    mainLayout->addWidget(m_iconLabel, 0, Qt::AlignHCenter);

    m_label = new QLabel(this);
    m_label->setWordWrap(true);
    mainLayout->addWidget(m_label);

    mainLayout->addStretch();

    m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);

    m_installButton = m_buttonBox->addButton(QString(),
                                             QDialogButtonBox::AcceptRole);

    m_laterButton = m_buttonBox->addButton(QString(),
                                           QDialogButtonBox::RejectRole);

    mainLayout->addWidget(m_buttonBox);

    setLayout(mainLayout);

    retranslateUi();

    BT_CONNECT(m_buttonBox, &QDialogButtonBox::rejected,
               this, &BtWelcomeDialog::reject);
    BT_CONNECT(m_buttonBox, &QDialogButtonBox::accepted,
               this, &BtWelcomeDialog::accept);
}

void BtWelcomeDialog::retranslateUi() {
    setWindowTitle(tr("Welcome to BibleTime"));

    m_label->setText(
                QStringLiteral("<p>%1</p><p>%2</p><p>%3</p>")
                    .arg(tr("BibleTime is an easy to use but powerful Bible "
                            "study tool."))
                    .arg(tr("Before you can use this application some works "
                            "must be installed. Various works such as Bibles, "
                            "books, commentaries, and lexicons are available "
                            "from remote libraries."))
                    .arg(tr("Choose the \"Install works\" button to download "
                            "works. The menu \"Settings > Bookshelf Manager\" "
                            "also installs works and can be used later.")));

    m_laterButton->setText(tr("Install later"));

    m_installButton->setText(
                QStringLiteral("      %1      ").arg(tr("Install works...")));

    QFont font(m_installButton->font());
    font.setBold(true);
    m_installButton->setFont(font);
}
