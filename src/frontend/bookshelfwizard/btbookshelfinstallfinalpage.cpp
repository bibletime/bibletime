/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelfinstallfinalpage.h"

#include "backend/btinstallbackend.h"
#include "backend/btinstallthread.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "util/btconnect.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
const QString groupingOrderKey ("GUI/BookshelfWizard/InstallPage/grouping");
const QString installPathKey   ("GUI/BookshelfWizard/InstallPage/installPathIndex");
}

BtBookshelfInstallFinalPage::BtBookshelfInstallFinalPage(
        QWidget *parent)
    : QWizardPage(parent),
      m_msgLabel(nullptr),
      m_progressBar(nullptr),
      m_stopButton(nullptr),
      m_verticalLayout(nullptr),
      m_installFailed(false) {

    setupUi();
    initConnections();
    retranslateUi();
}

void BtBookshelfInstallFinalPage::setupUi() {

    m_verticalLayout = new QVBoxLayout(this);
    m_verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    QSpacerItem *verticalSpacer = new QSpacerItem(
                20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_verticalLayout->addItem(verticalSpacer);

    m_msgLabel = new QLabel(this);
    m_msgLabel->setAlignment(Qt::AlignCenter);
    m_msgLabel->setObjectName(QStringLiteral("msgLabel"));
    m_msgLabel->setWordWrap(true);
    m_verticalLayout->addWidget(m_msgLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("progressBar");
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_verticalLayout->addWidget(m_progressBar,Qt::AlignCenter);

    QHBoxLayout * horizontalLayout = new QHBoxLayout();
    m_stopButton = new QPushButton(this);
    horizontalLayout->addSpacerItem(
                new QSpacerItem(1, 1, QSizePolicy::Expanding));
    horizontalLayout->addWidget(m_stopButton);
    horizontalLayout->addSpacerItem(
                new QSpacerItem(1, 1, QSizePolicy::Expanding));
    m_verticalLayout->addLayout(horizontalLayout);

    QSpacerItem *verticalSpacer2 = new QSpacerItem(
                20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_verticalLayout->addItem(verticalSpacer2);
}

void BtBookshelfInstallFinalPage::initConnections() {
    BT_CONNECT(m_stopButton, SIGNAL(clicked()),
               this,            SLOT(slotStopInstall()));
}

void BtBookshelfInstallFinalPage::retranslateUi() {
    m_stopButton->setText(tr("Stop"));

}

int BtBookshelfInstallFinalPage::nextId() const {
    return -1;
}

void BtBookshelfInstallFinalPage::initializePage() {
    installWorks();
    m_installCompleted = false;
}

bool BtBookshelfInstallFinalPage::isComplete() const {
    return m_installCompleted;
}

BtBookshelfWizard *BtBookshelfInstallFinalPage::btWizard() {
    return qobject_cast<BtBookshelfWizard*>(wizard());
}

void BtBookshelfInstallFinalPage::installWorks() {

    m_modules = btWizard()->selectedWorks().toList();
    QString destination = btWizard()->installPath();
    m_thread = new BtInstallThread(m_modules, destination, this);

    BT_CONNECT(m_thread, SIGNAL(preparingInstall(int)),
               this,     SLOT(slotInstallStarted(int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(statusUpdated(int, int)),
               this,     SLOT(slotStatusUpdated(int, int)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(installCompleted(int, bool)),
               this,     SLOT(slotOneItemCompleted(int, bool)),
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, SIGNAL(finished()),
               this,     SLOT(slotThreadFinished()),
               Qt::QueuedConnection);

    m_progressBar->setValue(0);
    m_stopButton->setEnabled(true);
    m_installFailed = false;
    m_thread->start();
}

void BtBookshelfInstallFinalPage::slotStopInstall() {
    m_stopButton->setDisabled(true);
    m_thread->stopInstall();
    m_installFailed = true;
}

void BtBookshelfInstallFinalPage::slotInstallStarted(int moduleIndex) {

    QString name = m_modules.at(moduleIndex)->name();
    QString msg = QString(tr("Installing")) + " \"" + name + "\"";
    m_msgLabel->setText(msg);
    m_lastStatus = -1;
}

void BtBookshelfInstallFinalPage::slotStatusUpdated(int moduleIndex, int status) {
    // Skip initial high value sent by sword
    if (m_lastStatus == -1 && status > 80) {
        return;
    }

    if (m_lastStatus == status)
        return;
    m_lastStatus = status;

    int perModuleIncrement = 100 / m_modules.count();
    int progress = (moduleIndex * perModuleIncrement) + (status * perModuleIncrement / 100);
    m_progressBar->setValue(progress);
}

void BtBookshelfInstallFinalPage::slotOneItemCompleted(int moduleIndex, bool successful) {
    int progress = (moduleIndex+1)* (100 / m_modules.count());
    m_progressBar->setValue(progress);
    if (!successful)
        m_installFailed = true;
}

void BtBookshelfInstallFinalPage::slotThreadFinished() {
    m_progressBar->setValue(100);
    if (m_installFailed) {
        QString msg = "<b>";
        msg += QString(tr("Some of the selected works were not installed."));
        msg += "</b>";
        m_msgLabel->setText(tr("Some of the selected works were not installed."));
        m_msgLabel->setStyleSheet("QLabel { color : red }");
    }
    else {
        m_msgLabel->setText(tr("The selected works have been installed."));
    }

    CSwordBackend::instance()->reloadModules(CSwordBackend::AddedModules);

    m_installCompleted = true;
    emit QWizardPage::completeChanged();
}
