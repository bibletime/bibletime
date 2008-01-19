/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef NEWBTINSTALLMGR_H
#define NEWBTINSTALLMGR_H

//sword includes
#include <installmgr.h>
#include <ftptrans.h>

//Qt includes
#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>

class CSwordBackend;


typedef QList<sword::InstallSource*> InstallSourceList;

/**Our own reimplementation to provide status bar updates.
*@author The BibleTime team
*/
class Bt_InstallMgr : public QObject, public sword::InstallMgr, public sword::StatusReporter {
	Q_OBJECT
public:

	Bt_InstallMgr();
	virtual ~Bt_InstallMgr();

	void slotRefreshCanceled();

protected:
	/* Reimplementations of method in StatusReporter */
	virtual void statusUpdate(double dltotal, double dlnow);
	virtual void preStatus(long totalBytes, long completedBytes, const char *message);

	long m_totalBytes;
	long m_completedBytes;

signals:
	void completed( const int, const int );
};


#endif
