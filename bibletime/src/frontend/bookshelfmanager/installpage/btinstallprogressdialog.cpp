/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btinstallprogressdialog.h"
#include "btinstallprogressdialog.moc"

#include "btinstallthread.h"

#include "util/ctoolclass.h"
#include "util/cpointers.h"
#include "backend/managers/cswordbackend.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDialog>
#include <QHeaderView>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QCloseEvent>
#include <QMultiMap>

#include <QDebug>


BtInstallProgressDialog::BtInstallProgressDialog(QWidget* parent, QTreeWidget* selectedModulesTreeWidget, QString destination)
	: QDialog(parent)
{
	// we want this dialog to be deleted when user closes it or the downloads are completed
	setAttribute(Qt::WA_DeleteOnClose, true);

	//create the dialog which shows the status and lets the user stop installation
	m_statusWidget = new QTreeWidget();
	m_statusWidget->setRootIsDecorated(false);
	m_statusWidget->setHeaderLabels(QStringList(tr("Work")) << tr("Progress") << QString::null);
	m_statusWidget->header()->setStretchLastSection(false);
	m_statusWidget->header()->setResizeMode(1, QHeaderView::Stretch);
	m_statusWidget->header()->setMovable(false);
	//m_statusWidget->setColumnWidth(1, CToolClass::mWidth(m_statusWidget, 2));

	foreach (QTreeWidgetItem* sourceItem, selectedModulesTreeWidget->invisibleRootItem()->takeChildren()) {
		// create items and threads for modules under this source
		foreach (QTreeWidgetItem* moduleItem, sourceItem->takeChildren()) {
			if (moduleItem->checkState(0) == Qt::Checked) {
				// create a thread for this module
				BtInstallThread* thread = new BtInstallThread(this, moduleItem->text(0), sourceItem->text(0), destination);
				m_waitingThreads.insert(sourceItem->text(0), thread);
				m_threadsByModule.insert(moduleItem->text(0), thread);
				// progress widget/item
				QPushButton* stopButton = new QPushButton(tr("Stop"), m_statusWidget);
				stopButton->setFixedSize(stopButton->sizeHint());

				// the item
				QTreeWidgetItem* progressItem = new QTreeWidgetItem(m_statusWidget);
				m_statusWidget->setColumnWidth(2, stopButton->sizeHint().width());
				progressItem->setSizeHint(2, stopButton->sizeHint());
				progressItem->setText(0, moduleItem->text(0));
				progressItem->setFlags(Qt::ItemIsEnabled);
				m_statusWidget->setItemWidget(progressItem, 2, stopButton);
				progressItem->setText(1, tr("Waiting for turn..."));

				//connect the signals between the dialog, items and threads
				QObject::connect(stopButton, SIGNAL(clicked()), thread, SLOT(slotStopInstall()) );
				QObject::connect(thread, SIGNAL(installStopped(QString, QString)), this, SLOT(slotOneItemStopped(QString, QString)));
				//is this needed or is statusUpdated enough?
				QObject::connect(thread, SIGNAL(installCompleted(QString, QString, int)), this, SLOT(slotOneItemCompleted(QString, QString, int)));
				QObject::connect(thread, SIGNAL(statusUpdated(QString, int)), this, SLOT(slotStatusUpdated(QString, int)));
				QObject::connect(thread, SIGNAL(downloadStarted(QString)), this, SLOT(slotDownloadStarted(QString)));

				QObject::connect(thread, SIGNAL(preparingInstall(QString, QString)), this, SLOT(slotInstallStarted(QString, QString)));

			}
		}
	}

	m_statusWidget->setMinimumWidth(m_statusWidget->size().width());
	QPushButton* stopAllButton = new QPushButton(tr("Stop All"), this);
	
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_statusWidget);
	layout->addWidget(stopAllButton);

	connect(stopAllButton, SIGNAL(clicked()), SLOT(slotStopInstall()) );

	qApp->processEvents();

	startThreads();

}

void BtInstallProgressDialog::startThreads()
{
	// remove all the updated modules from the backend module list at once
	//foreach (QString mName, m_threadsByModule.keys()) {
	//}
	//ListCSwordModuleInfo CPointers::backend()->takeModulesFromList(m_threadsByModule.keys());
	qDebug() << "start threads...";
	//loop through the multimap of the waiting threads, start at most 3 threads for each source
	QMultiMap<QString, BtInstallThread*>::iterator threadIterator = m_waitingThreads.begin();
	while (threadIterator != m_waitingThreads.end()) {
		QString sourceName = threadIterator.key();
		qDebug() << sourceName;
		if (m_runningThreads.values(sourceName).count() < 3) {
			BtInstallThread* t = threadIterator.value();
			m_runningThreads.insert(sourceName, t);
			threadIterator = m_waitingThreads.erase(threadIterator);
			t->start();
		}
		else ++threadIterator;
	}
	qDebug("BtInstallProgressDialog::startThreads end");
}

BtInstallProgressDialog::~BtInstallProgressDialog()
{}


void BtInstallProgressDialog::slotOneItemCompleted(QString module, QString source, int status)
{
	QString message;
	//status comes from the sword installer. TODO: Additionally we should check that there are files really installed.
	if (status != 0) {
		message = tr("Failed");
	}
	else {
		message = tr("Completed");
	}
	oneItemStoppedOrCompleted(module, source, message);
}

void BtInstallProgressDialog::slotOneItemStopped(QString module, QString source)
{
	oneItemStoppedOrCompleted(module, source, tr("Cancelled"));
}

void BtInstallProgressDialog::oneItemStoppedOrCompleted(QString module, QString source, QString statusMessage)
{
	qDebug() << "BtInstallProgressDialog::oneItemStoppedOrCompleted" << module << source << statusMessage;
	// update the list item
	m_statusWidget->setItemWidget(getItem(module), 1, 0);
	getItem(module)->setText(1, statusMessage);
	m_statusWidget->itemWidget(getItem(module), 2)->setEnabled(false);
	getItem(module)->setDisabled(true);

	qDebug() << "remove from threads maps" << source << m_threadsByModule.value(module);
	m_runningThreads.remove(source, m_threadsByModule.value(module));
	m_waitingThreads.remove(source, m_threadsByModule.value(module));

	//start a waiting thread if there are any
	QList<BtInstallThread*> threadsForSource = m_waitingThreads.values(source);
	qDebug() << threadsForSource;
	if (!threadsForSource.isEmpty()) {
		qDebug() << "Threads are waiting for turn";
		BtInstallThread* thread = threadsForSource.at(0);
		m_waitingThreads.remove(source, thread);
		m_runningThreads.insert(source, thread);
		thread->start();
	}

	if (threadsDone()) {
		qDebug() << "close the dialog";
		close();
	}
}

void BtInstallProgressDialog::slotStopInstall()
{
	qDebug("BtInstallProgressDialog::slotStopInstall");

	// Clear the waiting threads map, stop all running threads.

	m_waitingThreads.clear();
	if (m_runningThreads.count() > 0) {
		foreach(BtInstallThread* thread, m_runningThreads) {
			thread->slotStopInstall();
		}
	} else {
		close();
	}
}

void BtInstallProgressDialog::slotStatusUpdated(QString module, int status)
{
	//qDebug("BtInstallProgressDialog::slotStatusUpdated");
	//qDebug() << "module:" << module << "status:" << status;
	// find the progress bar for this module and update the value
	QWidget* itemWidget = m_statusWidget->itemWidget(getItem(module) , 1);
	QProgressBar* bar = dynamic_cast<QProgressBar*>(itemWidget);
	if (bar) bar->setValue(status);
}

void BtInstallProgressDialog::slotInstallStarted(QString module, QString)
{
	getItem(module)->setText(1, tr("Preparing install..."));
}

void BtInstallProgressDialog::slotDownloadStarted(QString module)
{
	qDebug() << "BtInstallProgressDialog::slotDownloadStarted" << module;
	getItem(module)->setText(1, QString::null);
	//m_statusWidget->itemWidget(getItem(module), 1)->setVisible(true);

	QProgressBar* bar = new QProgressBar(m_statusWidget);
	bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	bar->setValue(0);
	m_statusWidget->setItemWidget(getItem(module), 1, bar);
}

QTreeWidgetItem* BtInstallProgressDialog::getItem(QString moduleName)
{
	//qDebug() << "BtInstallProgressDialog::getItem" << moduleName;
	return m_statusWidget->findItems(moduleName, Qt::MatchExactly).at(0);
}

void BtInstallProgressDialog::closeEvent(QCloseEvent* event)
{
	qDebug("BtInstallProgressDialog::closeEvent");
	
	if (event->spontaneous()) {
		event->ignore();
		return;
	}
	// other parts of the UI/engine must be updated
	CPointers::backend()->reloadModules(CSwordBackend::AddedModules);
}

bool BtInstallProgressDialog::threadsDone()
{
	return (m_waitingThreads.count() == 0 && m_runningThreads.count() == 0);
}
