/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "btinstallmgr.h"
#include "kio_ftptransport.h"
#include "util/cpointers.h"

//Qt includes
#include <qfile.h>
#include <qfileinfo.h>

//KDE includes
#include <kapplication.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kprocess.h>

//Sword includes
#include <filemgr.h>
#include <swconfig.h>
#include <swbuf.h>
#include <map>
#include <utility>

//Stl includes
#include <functional>

using namespace sword;

namespace BookshelfManager {

	const QString BTInstallMgr::Tool::LocalConfig::swordConfigFilename() {
		//  Q_ASSERT( CPointers::backend()->sysconfig );
		return QString::fromLatin1("%1/.sword/sword.conf").arg(getenv("HOME"));
	}

	QStringList BTInstallMgr::Tool::RemoteConfig::sourceList( sword::InstallMgr* mgr ) {
		//  qWarning("BTInstallMgr::Tool::sourceList( sword::InstallMgr* mgr )");
		Q_ASSERT(mgr);
		Q_ASSERT(mgr->installConf);

		QStringList names;

		//add Sword remote sources
		for (InstallSourceMap::iterator it = mgr->sources.begin(); it != mgr->sources.end(); it++) {
			names << QString::fromLocal8Bit(it->second->caption);
		}

		// Add local directory sources
		SWConfig config(Tool::RemoteConfig::configFilename().latin1());
		sword::SectionMap::iterator sourcesSection = config.Sections.find("Sources");
		if (sourcesSection != config.Sections.end()) {
			sword::ConfigEntMap::iterator sourceBegin = sourcesSection->second.lower_bound("DIRSource");
			sword::ConfigEntMap::iterator sourceEnd = sourcesSection->second.upper_bound("DIRSource");

			while (sourceBegin != sourceEnd) {
				InstallSource is("DIR", sourceBegin->second.c_str());
				names << QString::fromLatin1(is.caption.c_str());

				sourceBegin++;
			}
		}

		return names;
	}

	QStringList BTInstallMgr::Tool::LocalConfig::targetList() {
		QStringList names = CPointers::backend()->swordDirList();
		return names;
	}

	void BTInstallMgr::Tool::LocalConfig::setTargetList( const QStringList& targets ) {
		//saves a new Sworc config using the provided target list
		QString filename = KGlobal::dirs()->saveLocation("data", "bibletime/") + "sword.conf"; //default is to assume the real location isn't writable
		bool directAccess = false;

		QFileInfo i(LocalConfig::swordConfigFilename());
		QFileInfo dirInfo(i.dirPath(true));

		if ( i.exists() && i.isWritable() ) { //we can write to the file ourself
			filename = LocalConfig::swordConfigFilename();
			directAccess = true;
		}
		else if ( !i.exists() && dirInfo.isWritable() ) { // if the file doesn't exist but th eparent is writable for us, create it
			filename = LocalConfig::swordConfigFilename();
			directAccess = true;
		}

		bool setDataPath = false;
		SWConfig conf(filename.local8Bit());
		conf.Sections.clear();

		for (QStringList::const_iterator it = targets.begin(); it != targets.end(); ++it) {
			QString t = *it;
			if (t.contains( QString("%1/.sword").arg(getenv("HOME")) )) {
				//we don't want HOME/.sword in the config
				continue;
			}
			else {
				conf["Install"].insert( std::make_pair(!setDataPath ? SWBuf("DataPath") : SWBuf("AugmentPath"), t.local8Bit()) );

				setDataPath = true;
			}
		}
		conf.Save();

		if (!directAccess) { //use kdesu to move the file to the right place
			KProcess *proc = new KProcess;
			*proc << "kdesu";
			*proc << QString::fromLatin1("-c") << QString("mv %1 %2").arg(filename).arg(LocalConfig::swordConfigFilename());
			proc->start(KProcess::Block);
		}
	}

	sword::InstallSource BTInstallMgr::Tool::RemoteConfig::source( sword::InstallMgr* mgr, const QString& name ) {
		Q_ASSERT(mgr);

		InstallSourceMap::iterator source = mgr->sources.find(name.latin1());
		if (source != mgr->sources.end()) {
			return *(source->second);
		}
		else { //not found in Sword, may be a local DIR source
			SWConfig config(Tool::RemoteConfig::configFilename().latin1());
			SectionMap::iterator sourcesSection = config.Sections.find("Sources");
			if (sourcesSection != config.Sections.end()) {
				ConfigEntMap::iterator sourceBegin =
					sourcesSection->second.lower_bound("DIRSource");
				ConfigEntMap::iterator sourceEnd =
					sourcesSection->second.upper_bound("DIRSource");

				while (sourceBegin != sourceEnd) {
					InstallSource is("DIR", sourceBegin->second.c_str());
					if (!strcmp(is.caption, name.latin1()) ) { //found local dir source
						return is;
					}

					sourceBegin++;//next source
				}
			}
		}

		InstallSource is("EMPTY");   //default return value
		is.caption = "unknown caption";
		is.source = "unknown source";
		is.directory = "unknown dir";

		return is;
	}

	const bool BTInstallMgr::Tool::RemoteConfig::isRemoteSource( sword::InstallSource* is ) {
		Q_ASSERT(is);
		if (is)
			return !strcmp(is->type, "FTP");
		else
			return false;
	}

	void BTInstallMgr::Tool::RemoteConfig::addSource( sword::InstallSource* is ) {
		if (!is) {
			return;
		}

		SWConfig config(Tool::RemoteConfig::configFilename().latin1());
		if (!strcmp(is->type, "FTP")) {
			//make sure the path doesn't have a trailing slash, sword doesn't like it
			if (is->directory[ is->directory.length()-1 ] == '/') {
				is->directory--; //make one char shorter
			}

			config["Sources"].insert( std::make_pair(SWBuf("FTPSource"), is->getConfEnt()) );
		}
		else if (!strcmp(is->type, "DIR")) {
			config["Sources"].insert( std::make_pair(SWBuf("DIRSource"), is->getConfEnt()) );
		}
		config.Save();
	}

	void BTInstallMgr::Tool::RemoteConfig::initConfig() {
		SWConfig config(Tool::RemoteConfig::configFilename().latin1());
		config["General"]["PassiveFTP"] = "true";
		config.Save();
	}

	const QString BTInstallMgr::Tool::RemoteConfig::configPath() {
		const char *envhomedir  = getenv("HOME");
		QString confPath = QString(envhomedir ? envhomedir : ".");
		confPath.append("/.sword/InstallMgr");

		return confPath;
	}

	const QString BTInstallMgr::Tool::RemoteConfig::configFilename() {
		return (configPath() + "/InstallMgr.conf");
	}

	void BTInstallMgr::Tool::RemoteConfig::removeSource( sword::InstallMgr* mgr, sword::InstallSource* is) {
		Q_ASSERT(mgr);
		Q_ASSERT(is);

		SWConfig config(Tool::RemoteConfig::configFilename().latin1());

		//this code can probably be shortened by using the stl remove_if functionality
		std::pair< ConfigEntMap::iterator, ConfigEntMap::iterator > range =
			isRemoteSource(is)
			? config["Sources"].equal_range("FTPSource")
			: config["Sources"].equal_range("DIRSource");

		ConfigEntMap::iterator it = range.first;
		while (it != range.second) {
			if (it->second == is->getConfEnt()) {
				//    qWarning("found the source!");
				config["Sources"].erase(it);
				break;
			}

			++it;
		}

		config.Save();
	}

	void BTInstallMgr::Tool::RemoteConfig::resetRemoteSources() {
		SWConfig config(Tool::RemoteConfig::configFilename().latin1());
		config["Sources"].erase( //remove all FTP sources
			config["Sources"].lower_bound("FTPSource"),
			config["Sources"].upper_bound("FTPSource")
		);
		config.Save();
	}

	void BTInstallMgr::Tool::RemoteConfig::resetLocalSources() {
		SWConfig config(Tool::RemoteConfig::configFilename().latin1());
		config["Sources"].erase( //remove all FTP sources
			config["Sources"].lower_bound("DIRSource"),
			config["Sources"].upper_bound("DIRSource")
		);
		config.Save();
	}

	CSwordBackend* BTInstallMgr::Tool::backend( sword::InstallSource* const is) {
		Q_ASSERT(is);
		if (!is) {
			return 0;
		}

		CSwordBackend* ret = 0;
		if (RemoteConfig::isRemoteSource(is)) {
			//     qWarning("## remote backend for %s", is->localShadow.c_str());
			ret = new CSwordBackend( QString(is->localShadow.c_str()), false );
		}
		else {
			//    qWarning("## local  backend for %s", QString(is->directory.c_str()).latin1());
			ret = new CSwordBackend( QString(is->directory.c_str()) );
		}

		Q_ASSERT(ret);
		if (ret) {
			ret->initModules();
		}

		return ret;
	}

BTInstallMgr::BTInstallMgr() : InstallMgr(Tool::RemoteConfig::configPath().latin1(), this) { //use this class also as status reporter
	}

	BTInstallMgr::~BTInstallMgr() {
		terminate(); //make sure to close the connection
	}

	void BTInstallMgr::statusUpdate(double dltotal, double dlnow) {

		if (dlnow > dltotal)
			dlnow = dltotal;

		int totalPercent = (int)((float)(dlnow + m_completedBytes) / (float)(m_totalBytes) * 100.0);

		if (totalPercent > 100) {
			totalPercent = 100;
		}
		else if (totalPercent < 0) {
			totalPercent = 0;
		}

		int filePercent  = (int)((float)(dlnow) / (float)(dltotal+1) * 100.0);
		if (filePercent > 100) {
			filePercent = 100;
		}
		else if (filePercent < 0) {
			filePercent = 0;
		}

		emit completed(totalPercent, filePercent);
	}

	void BTInstallMgr::preStatus(long totalBytes, long completedBytes, const char* /*message*/) {
		qWarning("pre Status: %i / %i", (int)totalBytes, (int)completedBytes);
		emit downloadStarted( "unknown filename" );

		m_completedBytes = completedBytes;
		m_totalBytes = (totalBytes > 0) ? totalBytes : 1; //avoid division by zero
	}

	FTPTransport *BTInstallMgr::createFTPTransport(const char *host, StatusReporter *statusReporter) {
		return new KIO_FTPTransport(host, statusReporter);
	}


}

