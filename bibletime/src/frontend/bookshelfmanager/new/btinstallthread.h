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

#include "bt_installmgr.h"

#include <QThread>

#include <boost/scoped_ptr.hpp>

//class Bt_InstallMgr;

/**
* Thread for installing a module.
*
* See the Qt thread documents. We have to be careful with signals and slots,
* with other things. This works because we don't send signals to the objects which live
* in this thread. This thread lives in the main app thread, therefore we can send signals
* to this from the main thread.
*
* Terminating a thread forcibly is "dangerous and discouraged" but we have to do it,
* I couldn't get cancelling work reliably otherwise. The Sword library is bad for threads.
* We use ftp connection and file resources; the connection can be ignored but the files
* have to be cleaned up after termination.
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
	void slotDownloadStarted();

protected:
	virtual void run();
	void removeModule();
	void removeTempFiles();

	QString m_module;
	QString m_destination;
	QString m_source;
	bool m_cancelled;
	//Bt_InstallMgr* m_iMgr;
	//sword::InstallSource m_installSource;
	Bt_InstallMgr m_iMgr;
	sword::InstallSource m_installSource;
	//TODO: it would be best to get the backend from the bookshelf manager install page
	// where it has already been created. Could fasten the progress dialog startup.
	boost::scoped_ptr<CSwordBackend> m_backendForSource;

signals:
	/** Emitted when the install progress status is updated. */
	void statusUpdated(QString module, int progressPercent);
	/** Emitted when installing has been stopped/cancelled. */
	void installStopped(QString module);
	/** Emitted when installing is complete. */
	void installCompleted(QString module, int errorStatus);
	/** Emitted when the first file download has been started. */
	void downloadStarted(QString module);
};

#endif
