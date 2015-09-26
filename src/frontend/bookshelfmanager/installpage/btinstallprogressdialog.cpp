/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallprogressdialog.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QHeaderView>
#include <QMultiMap>
#include <QProgressBar>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include "backend/managers/cswordbackend.h"
#include "backend/btinstallthread.h"


BtInstallProgressDialog::BtInstallProgressDialog(const QList<CSwordModuleInfo *> & modules,
                                                 const QString & destination,
                                                 QWidget * parent,
                                                 Qt::WindowFlags flags)
        : QDialog(parent, flags)
        , m_nextInstallIndex(0)
{
    // we want this dialog to be deleted when user closes it or the downloads are completed
    setAttribute(Qt::WA_DeleteOnClose, true);

    setWindowTitle(tr("Install Progress"));

    m_statusWidget = new QTreeWidget();
    m_statusWidget->setRootIsDecorated(false);
    m_statusWidget->setHeaderLabels(QStringList(tr("Work")) << tr("Progress"));
    m_statusWidget->header()->setStretchLastSection(false);
#if QT_VERSION < 0x050000
    m_statusWidget->header()->setResizeMode(1, QHeaderView::Stretch);
    m_statusWidget->header()->setMovable(false);
#else
    m_statusWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_statusWidget->header()->setSectionsMovable(false);
#endif

    Q_FOREACH(const CSwordModuleInfo * module, modules) {
        QTreeWidgetItem * progressItem = new QTreeWidgetItem(m_statusWidget);
        progressItem->setText(0, module->name());
        progressItem->setIcon(0, module->moduleIcon());
        progressItem->setText(1, tr("Waiting for turn..."));
        progressItem->setFlags(Qt::ItemIsEnabled);
    }

    m_statusWidget->setMinimumWidth(m_statusWidget->size().width());
    m_stopAllButton = new QPushButton(tr("Stop All"), this);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(m_statusWidget);
    layout->addWidget(m_stopAllButton);

    connect(m_stopAllButton, SIGNAL(clicked()),
            this,            SLOT(slotStopInstall()));

    m_thread = new BtInstallThread(modules, destination, this);
    // Connect the signals between the dialog, items and threads
    connect(m_thread, SIGNAL(preparingInstall(int)),
            this,     SLOT(slotInstallStarted(int)),
            Qt::QueuedConnection);
    connect(m_thread, SIGNAL(downloadStarted(int)),
            this,     SLOT(slotDownloadStarted(int)),
            Qt::QueuedConnection);
    connect(m_thread, SIGNAL(statusUpdated(int, int)),
            this,     SLOT(slotStatusUpdated(int, int)),
            Qt::QueuedConnection);
    connect(m_thread, SIGNAL(installCompleted(int, bool)),
            this,     SLOT(slotOneItemCompleted(int, bool)),
            Qt::QueuedConnection);
    connect(m_thread, SIGNAL(finished()),
            this,     SLOT(slotThreadFinished()),
            Qt::QueuedConnection);
    #if QT_VERSION < 0x050000
    connect(m_thread, SIGNAL(terminated()),
            this,     SLOT(slotThreadFinished()),
            Qt::QueuedConnection);
    #endif
    m_thread->start();
}

BtInstallProgressDialog::~BtInstallProgressDialog() {
    m_thread->wait();
    delete m_thread;
}

void BtInstallProgressDialog::slotStopInstall() {
    m_stopAllButton->setDisabled(true);
    m_thread->stopInstall();
    for (int i = m_nextInstallIndex; i < m_statusWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem * const item = m_statusWidget->topLevelItem(i);
        item->setText(1, QString::null);
        item->setDisabled(true);
    }
}

void BtInstallProgressDialog::slotInstallStarted(int moduleIndex) {
    Q_ASSERT(moduleIndex == m_nextInstallIndex);
    m_nextInstallIndex++;
    QTreeWidgetItem * const item = m_statusWidget->topLevelItem(moduleIndex);
    item->setText(1, tr("Preparing install..."));
    m_statusWidget->scrollToItem(item);
}

void BtInstallProgressDialog::slotDownloadStarted(int moduleIndex) {
    QTreeWidgetItem * const item = m_statusWidget->topLevelItem(moduleIndex);
    item->setText(1, QString::null);
    getOrCreateProgressBar(item)->setValue(0);
}

void BtInstallProgressDialog::slotStatusUpdated(int moduleIndex, int status) {
    // find the progress bar for this module and update the value
    getOrCreateProgressBar(moduleIndex)->setValue(status);
}

void BtInstallProgressDialog::slotOneItemCompleted(int moduleIndex, bool successful) {
QTreeWidgetItem * const item = m_statusWidget->topLevelItem(moduleIndex);
    // update the list item
    m_statusWidget->setItemWidget(item, 1, NULL);
    item->setText(1, successful ? tr("Completed") : tr("Failed"));
    item->setDisabled(true);
}

void BtInstallProgressDialog::slotThreadFinished() {
    close();
}

void BtInstallProgressDialog::closeEvent(QCloseEvent * event) {
    if (event->spontaneous()) {
        event->ignore();
        slotStopInstall();
        return;
    }
    // other parts of the UI/engine must be updated
    CSwordBackend::instance()->reloadModules(CSwordBackend::AddedModules);
}

QProgressBar * BtInstallProgressDialog::getOrCreateProgressBar(int moduleIndex) {
    return getOrCreateProgressBar(m_statusWidget->topLevelItem(moduleIndex));
}

QProgressBar * BtInstallProgressDialog::getOrCreateProgressBar(QTreeWidgetItem * item) {
    QWidget * const itemWidget = m_statusWidget->itemWidget(item, 1);
    QProgressBar * progressBar = dynamic_cast<QProgressBar *>(itemWidget);
    if (!progressBar) {
        progressBar = new QProgressBar(m_statusWidget);
        progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_statusWidget->setItemWidget(item, 1, progressBar);
    }
    return progressBar;
}
