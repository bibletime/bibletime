/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/


#ifndef INSTALLATIONMANAGERKIO_FTPTRANSPORT_H
#define INSTALLATIONMANAGERKIO_FTPTRANSPORT_H

//Bible
//Sword includes
#include <ftptrans.h>

//Qt includes
#include <QObject>
#include <QMap>
#include <QEventLoop>

//KDE includes
#include <kio/job.h>

//Std C++ includes
#include <vector>

namespace BookshelfManager {

class BTInstallMgr;

/**
* This is a reimplementation of Sword's FTP transport class which uses KDE's network functions.
* 
* @author Joachim Ansorg
*/
class KIO_FTPTransport : public QObject, public sword::FTPTransport {
	Q_OBJECT
public:
	KIO_FTPTransport(const char *host, sword::StatusReporter *statusReporter = 0);
	virtual ~KIO_FTPTransport();
	virtual char getURL(const char *destPath, const char *sourceURL);
	virtual std::vector<struct ftpparse> getDirList(const char *dirURL);

protected slots:
	void slotCopyResult(KIO::Job*);
	//  void slotCopyPercent(KIO::Job*, unsigned long);
	void slotTotalSize(KIO::Job *, KIO::filesize_t);
	void slotCopyProgress(KIO::Job *, KIO::filesize_t);
	void slotDirListingCanceled();

private:
	QMap<int, int> m_copyResults;
	QMap<int, std::vector< struct ftpparse > > m_dirListResults;
	bool m_listingCancelled;
	int m_totalSize; //size of currently downloaded file
	QEventLoop m_eventLoop;
};

}

#endif
