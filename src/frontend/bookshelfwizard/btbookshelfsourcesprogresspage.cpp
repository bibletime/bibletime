/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelfsourcesprogresspage.h"

#include <QApplication>
#include <QDate>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include "backend/btinstallbackend.h"
#include "backend/btsourcesthread.h"
#include "backend/config/btconfig.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "util/btconnect.h"


namespace {
QString const lastUpdate = "GUI/BookshelfWizard/lastUpdate";
}

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
    return WizardPage::sourcesPage; }

void BtBookshelfSourcesProgressPage::initializePage() {
    m_thread = new BtSourcesThread(this);
    BT_CONNECT(m_thread,      &BtSourcesThread::percentComplete,
               m_progressBar, &QProgressBar::setValue);
    BT_CONNECT(m_thread,   &BtSourcesThread::showMessage,
               m_msgLabel, &QLabel::setText);
    BT_CONNECT(m_thread, &BtSourcesThread::finished,
               this,     &BtBookshelfSourcesProgressPage::slotThreadFinished);
    m_installCompleted.store(false, std::memory_order_release);
    m_thread->start();
    m_stopButton->setEnabled(true);
    retranslateUi();
}

bool BtBookshelfSourcesProgressPage::isComplete() const
{ return m_installCompleted.load(std::memory_order_acquire); }

void BtBookshelfSourcesProgressPage::slotThreadFinished() {
    m_stopButton->setDisabled(true);
    if (m_thread->finishedSuccessfully())
        btConfig().setValue<QDate>(lastUpdate, QDate::currentDate());
    delete m_thread;
    m_installCompleted.store(true, std::memory_order_release);
    emit QWizardPage::completeChanged();
}

void BtBookshelfSourcesProgressPage::slotStopInstall() {
    m_stopButton->setDisabled(true);
    m_thread->stop();
}
