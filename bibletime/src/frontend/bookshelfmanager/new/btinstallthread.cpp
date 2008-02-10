/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btinstallthread.h"

BtInstallThread::BtInstallThread(QString moduleName, QString sourceName, QString destinationName)
	: QThread(),
	m_module(moduleName),
	m_destination(destinationName),
	m_source(sourceName)
{
}


BtInstallThread::~BtInstallThread()
{
}

void BtInstallThread::run()
{

}

void BtInstallThread::slotStopInstall()
{

}