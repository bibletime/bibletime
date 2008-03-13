/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTINSTALLTHREAD_H
#define BTINSTALLTHREAD_H

#include <QThread>


/**
* Thread for installing a module.
*/
class BtInstallThread : public QThread
{
	Q_OBJECT
public:
    BtInstallThread(QString moduleName, QString sourceName, QString destinationName);

    ~BtInstallThread();

public slots:
	void slotStopInstall();
	void slotManagerStatusUpdated(int totalProgress, int fileProgress);

protected:
	virtual void run();

	QString m_module;
	QString m_destination;
	QString m_source;
	bool m_cancelled;

signals:
	void statusUpdated(QString module, int progressPercent);
	void installStopped(QString module);
	//is this needed?:
	void installCompleted(QString module, int errorStatus);
};

#endif
