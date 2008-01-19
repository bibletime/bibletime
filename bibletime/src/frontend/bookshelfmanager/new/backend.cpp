#include "backend.h"

#include "frontend/bookshelfmanager/new/bt_installmgr.h"
#include "backend/managers/cswordbackend.h"

#include "util/cpointers.h"
#include "util/directoryutil.h"

//Qt includes
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

//Sword includes
#include <filemgr.h>
#include <swconfig.h>
#include <swbuf.h>

//Stl includes
#include <functional>
#include <map>
#include <utility>

using namespace sword;

namespace backend {

/** Adds the source described by Source to the backend. */
bool addSource(sword::InstallSource& source)
{
	qDebug("backend::addSource");
	SWConfig config(configFilename().toLatin1());
	if (!strcmp(source.type, "FTP")) {
		//make sure the path doesn't have a trailing slash, sword doesn't like it
		if (source.directory[ source.directory.length()-1 ] == '/') {
			source.directory--; //make one char shorter
		}

		config["Sources"].insert( std::make_pair(SWBuf("FTPSource"), source.getConfEnt()) );
	}
	else if (!strcmp(source.type, "DIR")) {
		config["Sources"].insert( std::make_pair(SWBuf("DIRSource"), source.getConfEnt()) );
	}
	config.Save();
	return true;
}

/** Returns the Source struct. */
sword::InstallSource source(QString name)
{
	qDebug("backend::source");
	Bt_InstallMgr mgr;
	InstallSourceMap::iterator source = mgr.sources.find(name.toLatin1().data());
	if (source != mgr.sources.end()) {
		return *(source->second);
	}
	else { //not found in Sword, may be a local DIR source
		SWConfig config(configFilename().toLatin1());
		SectionMap::iterator sourcesSection = config.Sections.find("Sources");
		if (sourcesSection != config.Sections.end()) {
			ConfigEntMap::iterator sourceBegin =
				sourcesSection->second.lower_bound("DIRSource");
			ConfigEntMap::iterator sourceEnd =
				sourcesSection->second.upper_bound("DIRSource");

			while (sourceBegin != sourceEnd) {
				InstallSource is("DIR", sourceBegin->second.c_str());
				if (!strcmp(is.caption, name.toLatin1()) ) { //found local dir source
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

/** Deletes the source. */
bool deleteSource(QString name)
{
	qDebug("backend::deleteSource");
	sword::InstallSource is = source(name );

	SWConfig config(configFilename().toLatin1());

	//this code can probably be shortened by using the stl remove_if functionality
	std::pair< ConfigEntMap::iterator, ConfigEntMap::iterator > range =
		isRemote(is)
		? config["Sources"].equal_range("FTPSource")
		: config["Sources"].equal_range("DIRSource");

	ConfigEntMap::iterator it = range.first;
	while (it != range.second) {
		if (it->second == is.getConfEnt()) {
			//    qWarning("found the source!");
			config["Sources"].erase(it);
			break;
		}
		++it;
	}

	config.Save();
}

/** Refreshes the remote source module list. */
bool refreshSource(QString name)
{
	// not possible until manager and progressdialog work together
}

/** Returns the moduleinfo list for the source. Delete the pointer after using. IS THIS POSSIBLE?*/
ListCSwordModuleInfo moduleList(QString name)
{

}

bool isRemote(const sword::InstallSource& source)
{
	return !strcmp(source.type, "FTP");
}

const QString configPath() {
	const char *envhomedir  = getenv("HOME");
	QString confPath = QString(envhomedir ? envhomedir : ".");
	confPath.append("/.sword/InstallMgr");

	return confPath;
}

const QString configFilename() {
	return (configPath() + "/InstallMgr.conf");
}

QStringList targetList()
{
	qDebug("backend::targetList");
	QStringList names = CPointers::backend()->swordDirList();
	return names;
}

void setTargetList( const QStringList& targets )
{
	qDebug("backend::setTargetList");
	//saves a new Sworc config using the provided target list
	//QString filename = KGlobal::dirs()->saveLocation("data", "bibletime/") + "sword.conf"; //default is to assume the real location isn't writable
	QString filename = util::filesystem::DirectoryUtil::getUserBaseDir().canonicalPath().append("/.sword/sword.conf");
	bool directAccess = false;

	QFileInfo i(swordConfigFilename());
	QFileInfo dirInfo(i.absolutePath());

	if ( i.exists() && i.isWritable() ) { //we can write to the file ourself
		filename = swordConfigFilename();
		directAccess = true;
	}
	else if ( !i.exists() && dirInfo.isWritable() ) { // if the file doesn't exist but th eparent is writable for us, create it
		filename = swordConfigFilename();
		directAccess = true;
	}

	bool setDataPath = false;
	SWConfig conf(filename.toLocal8Bit());
	conf.Sections.clear();

	for (QStringList::const_iterator it = targets.begin(); it != targets.end(); ++it) {
		QString t = *it;
		if (t.contains( util::filesystem::DirectoryUtil::getUserBaseDir().canonicalPath().append("/.sword") )) {
			//we don't want $HOME/.sword in the config
			continue;
		}
		else {
			conf["Install"].insert( std::make_pair(!setDataPath ? SWBuf("DataPath") : SWBuf("AugmentPath"), t.toLocal8Bit().data()) );
			setDataPath = true;
		}
	}
	conf.Save();

// 	TODO: check to see how this can be reactivated (and how about Qt-only?)
// 	if (!directAccess) { //use kdesu to move the file to the right place
// 		KProcess *proc = new KProcess;
// 		*proc << "kdesu";
// 		*proc << QString::fromLatin1("-c") << QString("mv %1 %2").arg(filename).arg(LocalConfig::swordConfigFilename());
// 		proc->start();
// 		proc->waitForFinished();
// 	}

}

QStringList sourceList()
{
	qDebug("backend::sourceList");
	Bt_InstallMgr mgr;
	Q_ASSERT(mgr.installConf);

	QStringList names;

	//add Sword remote sources
	for (InstallSourceMap::iterator it = mgr.sources.begin(); it != mgr.sources.end(); it++) {
		names << QString::fromLocal8Bit(it->second->caption);
	}

	// Add local directory sources
	SWConfig config(configFilename().toLatin1());
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


void initPassiveFtpMode()
{
	qDebug("backend::initPassiveFtpMode");
	SWConfig config(configFilename().toLatin1());
	config["General"]["PassiveFTP"] = "true";
	config.Save();
}
const QString swordConfigFilename()
{
	qDebug("backend::swordConfigFilename");
	return util::filesystem::DirectoryUtil::getUserBaseDir().canonicalPath().append("/.sword/sword.conf");
}

CSwordBackend* backend( const sword::InstallSource& is)
{
	qDebug("backend::backend");
	CSwordBackend* ret = 0;
	if (isRemote(is)) {
		ret = new CSwordBackend( QString(is.localShadow.c_str()), false );
	}
	else {
		ret = new CSwordBackend( QString(is.directory.c_str()) );
	}

	Q_ASSERT(ret);
	if (ret) {
		ret->initModules();
	}
	return ret;
}


};