/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btinstallitem.h"
#include "btinstallitem.moc"

#include "btinstallthread.h"


#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QProgressBar>
#include <QPushButton>

BtInstallItem::BtInstallItem(QTreeWidget* statusWidget, QString sourceName, QString moduleName, QString destinationName)
	: QTreeWidgetItem(statusWidget)
{
	m_thread = new BtInstallThread(moduleName, sourceName, destinationName);
	//m_threadList.append(thread);

	QProgressBar* bar = new QProgressBar(statusWidget);
	bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QPushButton* stopButton = new QPushButton(QObject::tr("Stop"), statusWidget);
	stopButton->setFixedSize(stopButton->sizeHint());
	QTreeWidgetItem* progressItem = new QTreeWidgetItem(statusWidget);
	progressItem->setSizeHint(2, stopButton->sizeHint());
	progressItem->setText(0, moduleName);
	statusWidget->setItemWidget(progressItem, 1, bar);
	statusWidget->setItemWidget(progressItem, 2, stopButton);

	QObject::connect(stopButton, SIGNAL(clicked()), m_thread, SLOT(slotStopInstall()) );
}


BtInstallItem::~BtInstallItem()
{
}


