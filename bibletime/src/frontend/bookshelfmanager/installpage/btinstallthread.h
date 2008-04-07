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

#include "frontend/bookshelfmanager/bt_installmgr.h"

#include <QThread>

#include <boost/scoped_ptr.hpp>

class BtInstallProgressDialog;

/**
* Thread for installing a module.
*
* See the Qt thread documents. We have to be careful with signals and slots,
* with other things.

The main thread creates and owns the BtInstallThread object.
When the install thread (the run() method) has been started
the install thread object receives status update signals
from the installmgr object. This works because these signals are sent
from the running install thread but received in the thread object which
lives in the main thread (note the difference between a thread object and a
running thread). The slot of the thread object is executed in the main thread
event loop, not in the install thread.

The running thread sends update signals to the progress dialog.
(This works because the signals are queued, i.e. the dialog is running
in the main app event loop which queues the signals. ???)

When the user cancels installing the main thread sends signal to a slot
in the thread object. The slot is then run in the main thread, not
in the install thread (note again the difference between a thread object and a
running thread). That slot terminates the running install thread immediately.
That is not the cleanest way to do it, but the Sword library has no good
support for threads. Terminating a Sword InstallMgr takes time and leads to
slow response. Therefore we stop installing by force and clean up the
temporary files manually.

* Terminating a thread forcibly is "dangerous and discouraged" but we have to do it,
* I couldn't get cancelling work reliably otherwise. The Sword library is bad for threads.
* We use ftp connection and file resources; the connection can be ignored but the files
* have to be cleaned up after termination.
*/
class BtInstallThread : public QThread
{
	Q_OBJECT
public:
    BtInstallThread(QObject* parent, QString moduleName, QString sourceName, QString destinationName);

    ~BtInstallThread();

public slots:
	void slotStopInstall();
	void slotManagerStatusUpdated(int totalProgress, int fileProgress);
	void slotDownloadStarted();

public: // data member
	bool done;

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
	void installStopped(QString module, QString source);
	/** Emitted when installing is complete. */
	void installCompleted(QString module, QString source, int errorStatus);
	/** Emitted when the first file download has been started. */
	void downloadStarted(QString module);
	void preparingInstall(QString module, QString source);
};

#endif
