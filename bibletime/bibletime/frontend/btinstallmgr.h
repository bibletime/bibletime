/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef BTINSTALLMGR_H
#define BTINSTALLMGR_H


//BibleTIme includes
#include "backend/cswordbackend.h"


//sword includes
#include <installmgr.h>
#include <ftptrans.h>

//Qt includes
#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>

namespace BookshelfManager {

	typedef QPtrList<sword::InstallSource> InstallSourceList;

	/**Our own reimplementation to provide status bar updates.
	  *@author The BibleTime team
	  */
class BTInstallMgr : public QObject, public sword::InstallMgr, public sword::StatusReporter {
		Q_OBJECT
public:
		class Tool {
public:
			class LocalConfig {
public:
				static const QString swordConfigFilename();
				static QStringList targetList();
				static void setTargetList( const QStringList& );
private:
				LocalConfig() {}
				;
			};

			class RemoteConfig {
public:
				static void initConfig();
				static const QString configPath();
				static const QString configFilename();

				static void resetRemoteSources();
				static void resetLocalSources();

				static QStringList sourceList( sword::InstallMgr* );
				static sword::InstallSource source( sword::InstallMgr*, const QString& name );
				static const bool isRemoteSource( sword::InstallSource* is );
				static void addSource( sword::InstallSource* );
				static void removeSource( sword::InstallMgr*, sword::InstallSource* );


private:
				RemoteConfig() {}
				;
			};

			static CSwordBackend* backend( sword::InstallSource* const );

private:
			Tool() {}
			;
		};

		BTInstallMgr();
		virtual ~BTInstallMgr();

protected:
		/* Reimplementations of method in StatusReporter */
		virtual void statusUpdate(double dltotal, double dlnow);
		virtual void preStatus(long totalBytes, long completedBytes, const char *message);

		virtual FTPTransport *createFTPTransport(const char *host, StatusReporter *statusReporter);

		long m_totalBytes;
		long m_completedBytes;

signals: // Signals
		void completed( const int, const int );
		/**
		* Emitted when a new file gets downloaded.
		*/
		void downloadStarted( const QString& );
	};

}

#endif
