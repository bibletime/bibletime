/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2026 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookshelfinstallfinalpage.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QStringLiteral>
#include <Qt>
#include <QVBoxLayout>
#include <QWizardPage>
#include "../../backend/btinstallthread.h"
#include "../../backend/drivers/btmoduleset.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"
#include "../../util/btconnect.h"
#include "btbookshelfwizard.h"
#include "btbookshelfwizardenums.h"


namespace {
QString const groupingOrderKey = "GUI/BookshelfWizard/InstallPage/grouping";
QString const installPathKey =
        "GUI/BookshelfWizard/InstallPage/installPathIndex";
} // anonymous namespace

BtBookshelfInstallFinalPage::BtBookshelfInstallFinalPage(QWidget * parent)
    : BtBookshelfWizardPage(parent)
{
    // Setup UI:
    m_verticalLayout = new QVBoxLayout(this);

    m_verticalLayout->addItem(new QSpacerItem(20,
                                              40,
                                              QSizePolicy::Minimum,
                                              QSizePolicy::Expanding));

    m_msgLabel = new QLabel(this);
    m_msgLabel->setAlignment(Qt::AlignCenter);
    m_msgLabel->setWordWrap(true);
    m_verticalLayout->addWidget(m_msgLabel);

    m_msgLabel2 = new QLabel(this);
    m_msgLabel2->setAlignment(Qt::AlignCenter);
    m_msgLabel2->setWordWrap(true);
    m_verticalLayout->addWidget(m_msgLabel2);

    m_progressBar = new QProgressBar(this);
    m_verticalLayout->addWidget(m_progressBar,Qt::AlignCenter);

    QHBoxLayout * const horizontalLayout = new QHBoxLayout();
    m_stopButton = new QPushButton(this);
    horizontalLayout->addSpacerItem(
                new QSpacerItem(1, 1, QSizePolicy::Expanding));
    horizontalLayout->addWidget(m_stopButton);
    horizontalLayout->addSpacerItem(
                new QSpacerItem(1, 1, QSizePolicy::Expanding));
    m_verticalLayout->addLayout(horizontalLayout);

    m_verticalLayout->addItem(new QSpacerItem(20,
                                              40,
                                              QSizePolicy::Minimum,
                                              QSizePolicy::Expanding));

    // Initialize connections:
    BT_CONNECT(m_stopButton, &QPushButton::clicked,
               this,         &BtBookshelfInstallFinalPage::slotStopInstall);
}

void BtBookshelfInstallFinalPage::destroyThread() noexcept {
    if (m_thread) {
        m_thread->stopInstall();
        while (!m_thread->wait()) /* join */;
        delete m_thread;
        m_thread = nullptr;
    }
}

void BtBookshelfInstallFinalPage::retranslateUi() {
    m_stopButton->setText(tr("Stop"));

    if (btWizard().taskType() == WizardTaskType::updateWorks) {
        setTitle(QApplication::translate(
                     "BookshelfWizard", "Updating Works"));
        setSubTitle(QApplication::translate(
                        "BookshelfWizard",
                        "The selected works are being updated."));
    } else {
        setTitle(QApplication::translate(
                     "BookshelfWizard", "Installing Works"));
        setSubTitle(QApplication::translate(
                        "BookshelfWizard",
                        "The selected works are being installed."));
    }
}

int BtBookshelfInstallFinalPage::nextId() const { return -1; }

void BtBookshelfInstallFinalPage::initializePage() {
    destroyThread();
    retranslateUi();

    // Install works:
    auto & btWiz = btWizard();

    m_modules = btWiz.selectedWorks().values();
    BT_ASSERT(!m_modules.empty());

    m_thread = new BtInstallThread(m_modules, btWiz.installPath(), this);
    BT_CONNECT(m_thread, &BtInstallThread::preparingInstall,
               this,     &BtBookshelfInstallFinalPage::slotInstallStarted,
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, &BtInstallThread::statusUpdated,
               this,     &BtBookshelfInstallFinalPage::slotStatusUpdated,
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, &BtInstallThread::installCompleted,
               this,     &BtBookshelfInstallFinalPage::slotOneItemCompleted,
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, &BtInstallThread::finished,
               this,     &BtBookshelfInstallFinalPage::slotThreadFinished,
               Qt::QueuedConnection);

    m_progressBar->setValue(0);
    BT_ASSERT(m_modules.size() <= MAX_MODULES); // No int overflow
    m_progressBar->setMaximum(m_modules.size() * 100);

    m_stopButton->setEnabled(true);
    m_installFailed = false;
    m_installCompleted = false;
    m_thread->start();
    btWiz.downloadStarted();
}

bool BtBookshelfInstallFinalPage::isComplete() const
{ return m_installCompleted; }

void BtBookshelfInstallFinalPage::slotStopInstall() {
    m_stopButton->setDisabled(true);
    m_thread->stopInstall();
    m_installFailed = true;
}

void BtBookshelfInstallFinalPage::slotInstallStarted(int moduleIndex) {
    m_msgLabel->setText(
            tr("Installing \"%1\"").arg(m_modules.at(moduleIndex)->name()));
    m_msgLabel2->setText(m_modules.at(moduleIndex)->config(CSwordModuleInfo::Description));
    m_lastStatus = -1;
}

void BtBookshelfInstallFinalPage::slotStatusUpdated(int const moduleIndex,
                                                    int const status)
{
    BT_ASSERT(moduleIndex >= 0);
    BT_ASSERT(moduleIndex < m_modules.size());
    BT_ASSERT(status >= 0);
    BT_ASSERT(status <= 100);

    // Skip initial high value sent by Sword:
    if (m_lastStatus == -1 && status > 80)
        return;

    if (m_lastStatus == status)
        return;

    m_lastStatus = status;

    m_progressBar->setValue(moduleIndex * 100 + status);
}

void BtBookshelfInstallFinalPage::slotOneItemCompleted(int const moduleIndex,
                                                       bool const successful)
{
    BT_ASSERT(moduleIndex >= 0);
    BT_ASSERT(moduleIndex < m_modules.size());

    m_progressBar->setValue((moduleIndex + 1) * 100);
    if (!successful)
        m_installFailed = true;
}

void BtBookshelfInstallFinalPage::slotThreadFinished() {
    m_progressBar->setValue(m_progressBar->maximum());
    m_stopButton->setEnabled(false);
    if (m_installFailed) {
        m_msgLabel->setText(
                    tr("Some of the selected works were not installed."));
        m_msgLabel->setStyleSheet("QLabel{color:red}");
    } else {
        m_msgLabel->setText(tr("The selected works have been installed."));
    }
    m_msgLabel2->setText("");

    CSwordBackend::instance().reloadModules();

    m_installCompleted = true;
    Q_EMIT QWizardPage::completeChanged();
    btWizard().downloadFinished();
}
