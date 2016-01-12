/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfwizard/btbookshelfsourcesprogresspage.h"

#include "backend/btinstallbackend.h"
#include "backend/btsourcesthread.h"
#include "backend/config/btconfig.h"
#include "frontend/bookshelfwizard/btbookshelfwizardenums.h"
#include "frontend/bookshelfwizard/btbookshelfwizard.h"
#include "util/btconnect.h"

#include <QApplication>
#include <QDate>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
const QString lastUpdate = "GUI/BookshelfWizard/lastUpdate";
}

BtBookshelfSourcesProgressPage::BtBookshelfSourcesProgressPage(
        QWidget *parent)
    : BtBookshelfWizardPage(parent),
      m_installCompleted(false),
      m_msgLabel(nullptr),
      m_progressBar(nullptr),
      m_stopButton(nullptr),
      m_stopped(false),
      m_thread(nullptr),
      m_verticalLayout(nullptr) {

    setupUi();
    initConnections();
}

void BtBookshelfSourcesProgressPage::setupUi() {

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

void BtBookshelfSourcesProgressPage::initConnections() {
    BT_CONNECT(m_stopButton, SIGNAL(clicked()),
               this,            SLOT(slotStopInstall()));
}

void BtBookshelfSourcesProgressPage::retranslateUi() {
    m_stopButton->setText(tr("Stop"));

    setTitle(QApplication::translate(
                 "BookshelfWizard", "Updating Remote Libraries", 0));
    setSubTitle(QApplication::translate(
                    "BookshelfWizard",
                    "Updating information from remote libraries.", 0));
}

int BtBookshelfSourcesProgressPage::nextId() const {
    return WizardPage::sourcesPage;;
}

void BtBookshelfSourcesProgressPage::initializePage() {
    m_thread = new BtSourcesThread(this);
    BT_CONNECT(m_thread, SIGNAL(percentComplete(int)),
               this, SLOT(slotPercentComplete(int)));
    BT_CONNECT(m_thread, SIGNAL(showMessage(QString)),
               this, SLOT(slotShowMessage(QString)));
    BT_CONNECT(m_thread, SIGNAL(finished()),
               this, SLOT(slotThreadFinished()));
    m_installCompleted = false;
    m_thread->start();
    m_stopButton->setEnabled(true);
    retranslateUi();
}

bool BtBookshelfSourcesProgressPage::isComplete() const {
    return m_installCompleted;
}

void BtBookshelfSourcesProgressPage::slotPercentComplete(int percentComplete) {
    m_progressBar->setValue(percentComplete);
}

void BtBookshelfSourcesProgressPage::slotShowMessage(const QString& msg) {
    m_msgLabel->setText(msg);
}

void BtBookshelfSourcesProgressPage::slotThreadFinished() {
    btConfig().setValue<QDate>(lastUpdate,QDate::currentDate());
    m_installCompleted = true;
    emit QWizardPage::completeChanged();
}

void BtBookshelfSourcesProgressPage::slotStopInstall() {
    m_stopButton->setDisabled(true);
    m_thread->stop();
    m_stopped = true;
}
