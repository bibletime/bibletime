/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/bookshelfmanager/instbackend.h"

#include <functional>
#include <map>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <utility>
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/btinstallmgr.h"
#include "util/cpointers.h"
#include "util/directory.h"
#include "util/dialogutil.h"

// Sword includes:
#include <filemgr.h>
#include <swconfig.h>
#include <swbuf.h>


using namespace sword;

namespace instbackend {

/** Adds the source described by Source to the backend. */
bool addSource(sword::InstallSource& source) {
    qDebug() << "backend::addSource";
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
sword::InstallSource source(QString name) {
    qDebug() << "backend::source";
    BtInstallMgr mgr;
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
bool deleteSource(QString name) {
    qDebug() << "backend::deleteSource";
    sword::InstallSource is = source(name );

    SWConfig config(configFilename().toLatin1());

    //this code can probably be shortened by using the stl remove_if functionality
    std::pair< ConfigEntMap::iterator, ConfigEntMap::iterator > range =
        isRemote(is)
        ? config["Sources"].equal_range("FTPSource")
        : config["Sources"].equal_range("DIRSource");

    ConfigEntMap::iterator it = range.first;
    SWBuf sourceConfigEntry = is.getConfEnt();
    bool notFound = true;
    while (it != range.second) {
        //SWORD lib gave us a "nice" surprise: getConfEnt() adds uid, so old sources added by BT are not recognized here
        if (it->second == sourceConfigEntry) {
            config["Sources"].erase(it);
            notFound = false;
            break;
        }
        ++it;
    }
    if (notFound) {
        qDebug() << "source was not found, try without uid";
        //try again without uid
        QString sce(sourceConfigEntry.c_str());
        QStringList l = sce.split('|');
        l.removeLast();
        sce = l.join("|").append("|");
        it = range.first;
        while (it != range.second) {
           qDebug() << it->second;
            if (it->second == sce) {
                config["Sources"].erase(it);
                break;
            }
            ++it;
        }
    }

    config.Save();
    return true; /// \todo dummy
}

/** Refreshes the remote source module list. */
bool refreshSource(QString /*name*/) {
    // not possible until manager and progressdialog work together
    return true; /// \todo dummy
}

/** Returns the moduleinfo list for the source. Delete the pointer after using. IS THIS POSSIBLE?*/
QList<CSwordModuleInfo*> moduleList(QString /*name*/) {
    QList<CSwordModuleInfo*> list; /// \todo dummy
    return list;
}

bool isRemote(const sword::InstallSource& source) {
    return !strcmp(source.type, "FTP");
}

const QString configPath() {
	QString confPath = util::directory::getUserHomeSwordDir().absolutePath();
	confPath.append("/InstallMgr");
    return confPath;
}

const QString configFilename() {
    return (configPath() + "/InstallMgr.conf");
}

QStringList targetList() {
    qDebug() << "backend::targetList";
    QStringList names = CPointers::backend()->swordDirList();
    return names;
}

bool setTargetList( const QStringList& targets ) {
    namespace DU = util::directory;

    qDebug() << "backend::setTargetList";
    //saves a new Sword config using the provided target list
    //QString filename = KGlobal::dirs()->saveLocation("data", "bibletime/") + "sword.conf"; //default is to assume the real location isn't writable
    //QString filename = util::DirectoryUtil::getUserBaseDir().canonicalPath().append("/.sword/sword.conf");
    //bool directAccess = false;
    QString filename = swordConfigFilename();
    QFileInfo i(filename);
    QFileInfo dirInfo(i.absolutePath());


    if ( !i.exists() && dirInfo.isWritable() ) {
        // if the file doesn't exist but the parent is writable, create it
        qWarning() << "The Sword config file does not exist, it has to be created";
        QFile f(filename);
        f.open(QIODevice::WriteOnly);
        f.close();
        i.refresh();
    }
    if ( i.exists() && i.isWritable() ) { //we can write to the file ourself
        qDebug() << "The Sword config file is writable";
    }
    else {
        // There is no way to save to the file
        qWarning() << "The Sword config file is not writable!";
        util::showWarning(0, QObject::tr("Can't write file"), QObject::tr("The Sword config file can't be written!"));
        return false;
    }

	filename = util::directory::convertDirSeparators(filename);
    SWConfig conf(filename.toLocal8Bit());
    conf.Sections.clear();

#ifdef Q_WS_WIN
	// On Windows, add the sword directory to the config file.
	QString swordPath = DU::convertDirSeparators( DU::getApplicationSwordDir().absolutePath());
	conf["Install"].insert( 
		std::make_pair( SWBuf("LocalePath"), swordPath.toLocal8Bit().data() )
	);
#endif

    bool setDataPath = false;
    for (QStringList::const_iterator it = targets.begin(); it != targets.end(); ++it) {
		QString t = DU::convertDirSeparators(*it);
#ifdef Q_WS_WIN
		if (t.contains(DU::convertDirSeparators(DU::getUserHomeDir().canonicalPath().append("\\Sword")))) {
#else
        if (t.contains(DU::getUserHomeDir().canonicalPath().append("/.sword"))) {
#endif
            //we don't want $HOME/.sword in the config
            continue;
        }
        else {
            qDebug() << "Add path to the conf file" << filename << ":" << t;
            conf["Install"].insert( std::make_pair(!setDataPath ? SWBuf("DataPath") : SWBuf("AugmentPath"), t.toLocal8Bit().data()) );
            setDataPath = true;
        }
    }
    qDebug() << "save the sword conf...";
    conf.Save();
    CPointers::backend()->reloadModules(CSwordBackend::PathChanged);
    return true;
}

QStringList sourceList() {
    qDebug() << "backend::sourceList";
    BtInstallMgr mgr;
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


void initPassiveFtpMode() {
    qDebug() << "backend::initPassiveFtpMode";
    SWConfig config(configFilename().toLatin1());
    config["General"]["PassiveFTP"] = "true";
    config.Save();
}
const QString swordConfigFilename() {
    namespace DU = util::directory;

    qDebug() << "backend::swordConfigFilename";
#ifdef Q_WS_WIN
    qDebug() << DU::getUserHomeDir().absolutePath().append("/Sword/sword.conf");
    return DU::getUserHomeDir().absolutePath().append("/Sword/sword.conf");
//    return DU::getApplicationDir().absolutePath().append("/sword.conf");
#else
    qDebug() << DU::getUserHomeDir().absolutePath().append("/.sword/sword.conf");
    return DU::getUserHomeDir().absolutePath().append("/.sword/sword.conf");
#endif
}

const QDir swordDir() {
    namespace DU = util::directory;

#ifdef Q_WS_WIN
    return QDir(DU::getUserHomeDir().absolutePath().append("/Sword/"));
#else
    return QDir(DU::getUserHomeDir().absolutePath().append("/.sword/"));
#endif
}

CSwordBackend* backend( const sword::InstallSource& is) {
    qDebug() << "backend::backend";
    CSwordBackend* ret = 0;
    if (isRemote(is)) {
        ret = new CSwordBackend( QString(is.localShadow.c_str()), false );
    }
    else {
        ret = new CSwordBackend( QString(is.directory.c_str()), false);
    }

    Q_ASSERT(ret);
    if (ret) {
        ret->initModules(CSwordBackend::OtherChange);
    }
    return ret;
}

}
