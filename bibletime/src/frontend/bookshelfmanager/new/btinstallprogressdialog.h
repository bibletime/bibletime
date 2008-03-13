/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLPROGRESSDIALOG_H
#define BTINSTALLPROGRESSDIALOG_H

#include <QDialog>
#include <QString>

class QTreeWidget;
class QTreeWidgetItem;

/**

*/
class BtInstallProgressDialog : public QDialog
{
	Q_OBJECT
public:
	BtInstallProgressDialog(QTreeWidget* selectedModulesTreeWidget, QString destination);

	~BtInstallProgressDialog();

public slots:
	void slotOneItemCompleted(QString module);
	void slotOneItemStopped(QString module);
	void slotStopInstall();
	void slotStatusUpdated(QString module, int status);
	void slotDownloadStarted(QString module);

private:
	QList<QThread*> m_threads;
	QTreeWidget* m_statusWidget;

	QTreeWidgetItem* getItem(QString moduleName);
};

#endif
