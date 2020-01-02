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

#include "btbookshelfsourcesprogresspage.h"

#include <QApplication>
#include <QDate>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include "../../backend/btinstallbackend.h"
#include "../../backend/btsourcesthread.h"
#include "../../backend/config/btconfig.h"
#include "../../util/btconnect.h"
#include "btbookshelfwizardenums.h"
#include "btbookshelfwizard.h"


BtBookshelfSourcesProgressPage::BtBookshelfSourcesProgressPage(QWidget * parent)
    : BtBookshelfWizardPage(parent)
{
    // Setup UI:
    QVBoxLayout * const vLayout = new QVBoxLayout(this);
    vLayout->setObjectName(QStringLiteral("verticalLayout"));

    vLayout->addItem(new QSpacerItem(20,
                                     40,
                                     QSizePolicy::Minimum,
                                     QSizePolicy::Expanding));

    m_msgLabel = new QLabel(this);
    m_msgLabel->setAlignment(Qt::AlignCenter);
    m_msgLabel->setObjectName(QStringLiteral("msgLabel"));
    m_msgLabel->setWordWrap(true);
    vLayout->addWidget(m_msgLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("progressBar");
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    vLayout->addWidget(m_progressBar, Qt::AlignCenter);

    QHBoxLayout * const horizontalLayout = new QHBoxLayout();
    m_stopButton = new QPushButton(this);
    horizontalLayout->addSpacerItem(
                new QSpacerItem(1, 1, QSizePolicy::Expanding));
    horizontalLayout->addWidget(m_stopButton);
    horizontalLayout->addSpacerItem(
                new QSpacerItem(1, 1, QSizePolicy::Expanding));
    vLayout->addLayout(horizontalLayout);

    vLayout->addItem(new QSpacerItem(20,
                                     40,
                                     QSizePolicy::Minimum,
                                     QSizePolicy::Expanding));

    // Initialize connections:
    BT_CONNECT(m_stopButton, &QPushButton::clicked,
               this,         &BtBookshelfSourcesProgressPage::slotStopInstall);
}

void BtBookshelfSourcesProgressPage::destroyThread() noexcept {
    if (m_thread) {
        m_thread->stop();
        while (!m_thread->wait()) /* join */;
        delete m_thread;
        m_thread = nullptr;
    }
}

void BtBookshelfSourcesProgressPage::retranslateUi() {
    m_stopButton->setText(tr("Stop"));

    setTitle(QApplication::translate(
                 "BookshelfWizard", "Updating Remote Libraries"));
    setSubTitle(QApplication::translate(
                    "BookshelfWizard",
                    "Updating information from remote libraries."));
}

int BtBookshelfSourcesProgressPage::nextId() const {
    if (btWizard().taskType() == WizardTaskType::updateWorks)
        return WizardPage::updateWorksPage;
    return WizardPage::sourcesPage;
}

void BtBookshelfSourcesProgressPage::initializePage() {
    destroyThread();

    m_installCompleted = false;
    m_thread = new BtSourcesThread(this);
    BT_CONNECT(m_thread,      &BtSourcesThread::percentComplete,
               m_progressBar, &QProgressBar::setValue,
               Qt::QueuedConnection);
    BT_CONNECT(m_thread,   &BtSourcesThread::showMessage,
               m_msgLabel, &QLabel::setText,
               Qt::QueuedConnection);
    BT_CONNECT(m_thread, &BtSourcesThread::finished,
               this,     &BtBookshelfSourcesProgressPage::slotThreadFinished,
               Qt::QueuedConnection);
    m_thread->start();
    m_stopButton->setEnabled(true);
    btWizard().downloadStarted();
    retranslateUi();
}

bool BtBookshelfSourcesProgressPage::isComplete() const
{ return m_installCompleted; }

void BtBookshelfSourcesProgressPage::slotThreadFinished() {
    m_stopButton->setDisabled(true);
    if (m_thread->finishedSuccessfully())
        BtBookshelfWizard::setAutoUpdateSources(false);
    m_installCompleted = true;
    emit QWizardPage::completeChanged();
    btWizard().downloadFinished();
}

void BtBookshelfSourcesProgressPage::slotStopInstall() {
    m_stopButton->setDisabled(true);
    m_thread->stop();
}
