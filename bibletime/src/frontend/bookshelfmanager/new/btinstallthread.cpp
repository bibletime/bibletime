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

#include <QString>
#include <QThread>
#include <QDir>

#include <QDebug>

BtInstallThread::BtInstallThread(QString moduleName, QString sourceName, QString destinationName)
	: QThread(),
	m_module(moduleName),
	m_destination(destinationName),
	m_source(sourceName),
	m_cancelled(false)
{
}


BtInstallThread::~BtInstallThread()
{
}

void BtInstallThread::run()
{
	qDebug("BtInstallThread::run");
	qDebug() << "Module:" << m_module;
	qDebug() << "source: "<<m_source;
	qDebug()<<"destination:"<<m_destination;

	Bt_InstallMgr iMgr;
	m_iMgr = &iMgr;
	qDebug() << "connect the status signal from iMgr to thread...";
	QObject::connect(&iMgr, SIGNAL(percentCompleted(int, int)), this, SLOT(slotManagerStatusUpdated(int, int)));
	QObject::connect(&iMgr, SIGNAL(downloadStarted()), this, SLOT(slotDownloadStarted()));
	sword::InstallSource is = backend::source(m_source);


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

	sword::SWMgr lMgr( m_destination.toLatin1() );

	//check whether it's an update. If yes, remove existing module first
	//TODO: silently removing without undo if the user cancels the update is WRONG!!!
	CSwordModuleInfo* m = CPointers::backend()->findModuleByName(m_module);
	if (m && !m_cancelled) { //module found?
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
		iMgr.removeModule(&mgr, m->name().toLatin1());
	}

	if (!m_cancelled && backend::isRemote(is)) {
		qDebug() << "calling install";
		int status = iMgr.installModule(&lMgr, 0, m_module.toLatin1(), &is);
		//this->setPriority(QThread::HighPriority);
		qWarning() << "INFO: return status of install manager: " << status;
		if (status != 0) {
			emit installStopped(m_module);
		}
		else {
			emit installCompleted(m_module, status);
		}
	}
	else if (!m_cancelled) { //local source
		iMgr.installModule(&lMgr, is.directory.c_str(), m_module.toLatin1());
	}
}

void BtInstallThread::slotStopInstall()
{
	//this->setPriority(QThread::IdlePriority);
	m_cancelled = true;
	if (m_iMgr) m_iMgr->terminate();
	//this->setPriority(QThread::NormalPriority);
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
