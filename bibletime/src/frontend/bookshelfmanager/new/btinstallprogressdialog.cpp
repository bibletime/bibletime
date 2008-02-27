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

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDialog>
#include <QHeaderView>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

#include <QDebug>


BtInstallProgressDialog::BtInstallProgressDialog(QTreeWidget* selectedModulesTreeWidget, QString destination)
	: QDialog()
{
	//create the dialog which shows the status and lets the user stop installation
	QTreeWidget* statusWidget = new QTreeWidget();
	statusWidget->setRootIsDecorated(false);
	statusWidget->setHeaderLabels(QStringList(tr("Work")) << tr("Progress") << QString::null);
	statusWidget->header()->setStretchLastSection(false);

	statusWidget->header()->setResizeMode(1, QHeaderView::Stretch);

	foreach (QTreeWidgetItem* sourceItem, selectedModulesTreeWidget->invisibleRootItem()->takeChildren()) {
		foreach (QTreeWidgetItem* moduleItem, sourceItem->takeChildren()) {
			if (moduleItem->checkState(0) == Qt::Checked) {
				qDebug("");
				// create a thread for this module
				BtInstallThread* thread = new BtInstallThread(moduleItem->text(0), sourceItem->text(0), destination);
				m_threads.append(thread);
				// progress widget/item
				QProgressBar* bar = new QProgressBar(statusWidget);
				bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
				QPushButton* stopButton = new QPushButton(tr("Stop"), statusWidget);
				stopButton->setFixedSize(stopButton->sizeHint());

				// the item
				QTreeWidgetItem* progressItem = new QTreeWidgetItem(statusWidget);

				progressItem->setSizeHint(2, stopButton->sizeHint());
				progressItem->setText(0, moduleItem->text(0));
				progressItem->setFlags(Qt::ItemIsEnabled);

				statusWidget->setItemWidget(progressItem, 1, bar);
				statusWidget->setItemWidget(progressItem, 2, stopButton);

				//connect the signals between the dialog, items and threads
				QObject::connect(stopButton, SIGNAL(clicked()), thread, SLOT(slotStopInstall()) );
				QObject::connect(thread, SIGNAL(installStopped(QString)), this, SLOT(oneItemStopped(QString)));
				//is this needed or is statusUpdated enough?
				QObject::connect(thread, SIGNAL(installCompleted(QString)), this, SLOT(oneItemCompleted(QString)));
				QObject::connect(thread, SIGNAL(statusUpdated(QString, int)), this, SLOT(slotStatusUpdated(QString, int)));
				
				qDebug("");
			}
		}
	}

	QPushButton* stopAllButton = new QPushButton(tr("Stop All"), this);
	
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(statusWidget);
	layout->addWidget(stopAllButton);

	connect(stopAllButton, SIGNAL(clicked()), SLOT(slotStopInstall()) );


	// start threads
	qDebug() << "start all threads...";
	foreach (QThread* t, m_threads) {
		t->start();
	}
}


BtInstallProgressDialog::~BtInstallProgressDialog()
{
}


void BtInstallProgressDialog::slotOneItemCompleted(QString module)
{
	qDebug("BtInstallProgressDialog::slotOneItemCompleted");
	// mark the item completed (change/remove it),
	// if all items are stopped/completed close the dialog
	if (false) {
		close();
	}
}

void BtInstallProgressDialog::slotOneItemStopped(QString module)
{
	qDebug("BtInstallProgressDialog::slotOneItemStopped");
	// mark the item completed (change/remove it),
	// if all items are stopped/completed close the dialog

	//find the item for this module name
	//remove the progress dialog
	// write "stopped"
	//remove the stop button

	if (false) {
		close();
	}
}

void BtInstallProgressDialog::slotStopInstall()
{
	qDebug("BtInstallProgressDialog::slotStopInstall");
}