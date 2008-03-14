/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btinstallthread.h"
#include "btinstallthread.moc"

#include "bt_installmgr.h"
#include "backend.h"
#include "util/cpointers.h"
#include "backend/managers/cswordbackend.h"

#include <QApplication>
#include <QString>
#include <QThread>
#include <QDir>

#include <QDebug>

// sword
#include <filemgr.h>

BtInstallThread::BtInstallThread(QString moduleName, QString sourceName, QString destinationName)
	: QThread(),
	m_module(moduleName),
	m_destination(destinationName),
	m_source(sourceName),
	m_cancelled(false),
	m_installSource(backend::source(sourceName)),
	m_backendForSource(backend::backend(m_installSource))
{
}


BtInstallThread::~BtInstallThread()
{
	//delete m_iMgr;
}

void BtInstallThread::run()
{
	qDebug() << "BtInstallThread::run, mod:" << m_module << "src:" << m_source << "dest:" << m_destination;

	//Bt_InstallMgr iMgr;
	//m_iMgr = &iMgr;
	
	//make sure target/mods.d and target/modules exist
	QDir dir(m_destination);
	if (!dir.exists()) {
		dir.mkdir(m_destination);
		qDebug() << "made directory" << m_destination;
	}
	if (!dir.exists("modules")) {
		dir.mkdir("modules");
		qDebug() << "made directory" << m_destination << "/modules";
	}
	if (!dir.exists("mods.d")) {
		dir.mkdir("mods.d");
		qDebug() << "made directory" << m_destination << "/mods.d";
	}

	//m_iMgr = new Bt_InstallMgr();
	QObject::connect(&m_iMgr, SIGNAL(percentCompleted(int, int)), this, SLOT(slotManagerStatusUpdated(int, int)));
	QObject::connect(&m_iMgr, SIGNAL(downloadStarted()), this, SLOT(slotDownloadStarted()));
	//sword::InstallSource is = backend::source(m_source);
	//m_installSource = backend::source(m_source);
	//check whether it's an update. If yes, remove existing module first
	//TODO: silently removing without undo if the user cancels the update is WRONG!!!
	removeModule();

	// manager for the destination path
	sword::SWMgr lMgr( m_destination.toLatin1() );

	if (backend::isRemote(m_installSource)) {
		qDebug() << "calling install";
		int status = m_iMgr.installModule(&lMgr, 0, m_module.toLatin1(), &m_installSource);
		if (status != 0) {
			qWarning() << "Error with install: " << status << "module:" << m_module;
		}
		else {
			emit installCompleted(m_module, status);
		}
	}
	else { //local source
		m_iMgr.installModule(&lMgr, m_installSource.directory.c_str(), m_module.toLatin1());
	}
}

void BtInstallThread::slotStopInstall()
{
	this->terminate(); // It's dangerous to forcibly stop, but we will clean up the files
	qApp->processEvents();
	this->wait(); // wait for termination
	qApp->processEvents();
	// cleanup: remove the module, remove the temp files
	// Actually m_iMgr is unnecessary, it could be local in the run().
	if (true) {
		// remove the installed module, just to be sure because mgr may
		// have been terminated when copying files
		removeModule();
		qApp->processEvents();
		removeTempFiles();
		qApp->processEvents();
		//delete m_iMgr;
		//m_iMgr = 0; // dtor of this thread deletes iMgr
		emit installStopped(m_module);
		qApp->processEvents();
	}
}

void BtInstallThread::slotManagerStatusUpdated(int totalProgress, int fileProgress)
{
	qDebug("BtInstallThread::slotManagerStatusUpdated");
	emit statusUpdated(m_module, totalProgress);
}

void BtInstallThread::slotDownloadStarted()
{
	qDebug("BtInstallThread::slotDownloadStarted");
	emit downloadStarted(m_module);
}

void BtInstallThread::removeModule()
{
	CSwordModuleInfo* m = CPointers::backend()->findModuleByName(m_module);
	if (m) { //module found?
		QString prefixPath = m->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
		QString dataPath = m->config(CSwordModuleInfo::DataPath);
		if (dataPath.left(2) == "./") {
			dataPath = dataPath.mid(2);
		}

		if (prefixPath.contains(dataPath)) {
			prefixPath.remove( prefixPath.indexOf(dataPath), dataPath.length() );
		}
		else {
			prefixPath = QString::fromLatin1(CPointers::backend()->prefixPath);
		}

		sword::SWMgr mgr(prefixPath.toLatin1());
		//Bt_InstallMgr iMgr;
		m_iMgr.removeModule(&mgr, m->name().toLatin1());
	}
}

void BtInstallThread::removeTempFiles()
{
	qDebug("BtInstallThread::removeTempFiles");

	// (take the remote conf file for this module, take DataPath,
	// take the absolute path of the InstallMgr)

	//sword::InstallSource is = backend::source(m_source);
	if (backend::isRemote(m_installSource)) {
		// get the path for the module temp files
		CSwordModuleInfo* mInfo = m_backendForSource->findModuleByName(m_module);
		QString dataPath = mInfo->config(CSwordModuleInfo::AbsoluteDataPath);
		qDebug() << "Delete path:" << dataPath;
		// it's easier to use sword than qt
		sword::FileMgr::removeDir(dataPath.toLatin1().data());
	}
}
