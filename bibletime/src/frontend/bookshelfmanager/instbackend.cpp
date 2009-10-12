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
#include <QMessageBox>
#include <utility>
#include "backend/managers/cswordbackend.h"
#include "frontend/bookshelfmanager/btinstallmgr.h"
#include "util/cpointers.h"
#include "util/directoryutil.h"

// Sword includes:
#include <filemgr.h>
#include <swconfig.h>
#include <swbuf.h>


using namespace sword;

namespace instbackend {

/** Adds the source described by Source to the backend. */
bool addSource(sword::InstallSource& source) {
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
sword::InstallSource source(QString name) {
    qDebug("backend::source");
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
    return true; //TODO: dummy
}

/** Refreshes the remote source module list. */
bool refreshSource(QString /*name*/) {
    // not possible until manager and progressdialog work together
    return true; //TODO: dummy
}

/** Returns the moduleinfo list for the source. Delete the pointer after using. IS THIS POSSIBLE?*/
QList<CSwordModuleInfo*> moduleList(QString /*name*/) {
    QList<CSwordModuleInfo*> list; //TODO: dummy
    return list;
}

bool isRemote(const sword::InstallSource& source) {
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

QStringList targetList() {
    qDebug("backend::targetList");
    QStringList names = CPointers::backend()->swordDirList();
    return names;
}

bool setTargetList( const QStringList& targets ) {
    namespace DU = util::directoryutil;

    qDebug("backend::setTargetList");
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
        QMessageBox::warning(0, QObject::tr("Can't write file"), QObject::tr("The Sword config file can't be written!"));
        return false;
    }
    SWConfig conf(filename.toLocal8Bit());
    conf.Sections.clear();
    bool setDataPath = false;
    for (QStringList::const_iterator it = targets.begin(); it != targets.end(); ++it) {
        QString t = *it;
        if (t.contains(DU::getUserHomeDir().canonicalPath().append("/.sword"))) {
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
    qDebug("backend::sourceList");
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
    qDebug("backend::initPassiveFtpMode");
    SWConfig config(configFilename().toLatin1());
    config["General"]["PassiveFTP"] = "true";
    config.Save();
}
const QString swordConfigFilename() {
    namespace DU = util::directoryutil;

    qDebug("backend::swordConfigFilename");
    qDebug() << DU::getUserHomeDir().absolutePath().append("/.sword/sword.conf");
    return DU::getUserHomeDir().absolutePath().append("/.sword/sword.conf");
}

const QDir swordDir() {
    namespace DU = util::directoryutil;

    return QDir(DU::getUserHomeDir().absolutePath().append("/.sword/"));
}

CSwordBackend* backend( const sword::InstallSource& is) {
    qDebug("backend::backend");
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
