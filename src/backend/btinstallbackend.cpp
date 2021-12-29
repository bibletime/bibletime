/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btinstallbackend.h"

#include <map>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QList>
#include <type_traits>
#include <utility>
#include "../util/btassert.h"
#include "../util/directory.h"
#include "managers/cswordbackend.h"
#include "btinstallmgr.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <installmgr.h>
#include <multimapwdef.h>
#include <swconfig.h>
#include <swbuf.h>
#pragma GCC diagnostic pop


using namespace sword;

namespace BtInstallBackend {

/** Adds the source described by Source to the backend. */
bool addSource(sword::InstallSource& source) {
    SWConfig config(configFilename().toLatin1());
    if (isRemote(source)) {
        if (source.directory[ source.directory.length()-1 ] == '/') {
            source.directory--;
        }
    static_assert(std::is_same_v<decltype(source.type), SWBuf>);
    if (source.type == "FTP") {
            config["Sources"].insert( std::make_pair(SWBuf("FTPSource"), source.getConfEnt()) );
        }
        else if (source.type == "SFTP") {
            config["Sources"].insert( std::make_pair(SWBuf("SFTPSource"), source.getConfEnt()) );
        }
        else if (source.type == "HTTP") {
            config["Sources"].insert( std::make_pair(SWBuf("HTTPSource"), source.getConfEnt()) );
        }
        else if (source.type == "HTTPS") {
            config["Sources"].insert( std::make_pair(SWBuf("HTTPSSource"), source.getConfEnt()) );
        }
    }
    else if (source.type == "DIR") {
        config["Sources"].insert( std::make_pair(SWBuf("DIRSource"), source.getConfEnt()) );
    }
    config.save();
    return true;
}

/** Returns the Source struct. */
sword::InstallSource source(const QString &name) {
    BtInstallMgr mgr;
    auto const source = mgr.sources.find(name.toLatin1().data());
    if (source != mgr.sources.end()) {
        return *(source->second);
    }
    else { //not found in Sword, may be a local DIR source
        SWConfig config(configFilename().toLatin1());
        auto const sections = config.getSections();
        auto const sourcesSection = sections.find("Sources");
        if (sourcesSection != sections.end()) {
            auto sourceBegin =
                sourcesSection->second.lower_bound("DIRSource");
            auto const sourceEnd =
                sourcesSection->second.upper_bound("DIRSource");

            while (sourceBegin != sourceEnd) {
                InstallSource is("DIR", sourceBegin->second.c_str());
                if (is.caption == name) { // found local dir source
                    return is;
                }

                ++sourceBegin; //next source
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
bool deleteSource(const QString &name) {
    sword::InstallSource is = source(name );

    SWConfig config(configFilename().toLatin1());

    //this code can probably be shortened by using the stl remove_if functionality
    SWBuf sourceConfigEntry = is.getConfEnt();
    bool notFound = true;
    auto it(config["Sources"].begin());
    while (it != config["Sources"].end()) {
        //SWORD lib gave us a "nice" surprise: getConfEnt() adds uid, so old sources added by BT are not recognized here
        if (it->second == sourceConfigEntry) {
            config["Sources"].erase(it);
            notFound = false;
            break;
        }
        ++it;
    }
    if (notFound) {
        qDebug() << "source was not found, trying without uid";
        //try again without uid
        QString sce(sourceConfigEntry.c_str());
        QStringList l = sce.split('|');
        l.removeLast();
        sce = l.join("|").append("|");
        it = config["Sources"].begin();
        while (it != config["Sources"].end()) {
            if (it->second == sce) {
                config["Sources"].erase(it);
                break;
            }
            ++it;
        }
    }

    config.save();
    return true; /// \todo dummy
}

bool isRemote(const sword::InstallSource& source) {
    return source.type == "FTP"
           || source.type == "SFTP"
           || source.type == "HTTP"
           || source.type == "HTTPS";
}

QString configPath() {
    return util::directory::getUserHomeSwordDir().absolutePath().append("/InstallMgr");
}

QString configFilename() {
    return configPath().append("/InstallMgr.conf");
}

QStringList targetList() {
    QStringList names = CSwordBackend::instance()->swordDirList();
    return names;
}

bool setTargetList( const QStringList& targets ) {
    namespace DU = util::directory;

    //saves a new Sword config using the provided target list
    //QString filename = KGlobal::dirs()->saveLocation("data", "bibletime/") + "sword.conf"; //default is to assume the real location isn't writable
    //QString filename = util::DirectoryUtil::getUserBaseDir().canonicalPath().append("/.sword/sword.conf");
    //bool directAccess = false;
    QString filename = swordConfigFilename();
    {
        QFile f(filename);
        if (!f.exists()) {
            if (!f.open(QIODevice::ReadWrite)) {
                qWarning() << "The Sword config file can't be created!";
                return false;
            }
            f.close();
            qDebug() << "The Sword config file \"" << filename
                     << "\" had to be (re)created!";
        }
    }

    filename = util::directory::convertDirSeparators(filename);
    SWConfig conf(filename.toLocal8Bit());
    conf.getSections().clear();

#ifdef Q_OS_WIN
    // On Windows, add the sword directory to the config file.
    QString swordPath = DU::convertDirSeparators( DU::getApplicationSwordDir().absolutePath());
    conf["Install"].insert(
        std::make_pair( SWBuf("LocalePath"), swordPath.toLocal8Bit().data() )
    );
#endif

    bool setDataPath = false;
    for (auto const & target : targets) {
        QString t = DU::convertDirSeparators(target);
#ifdef Q_OS_WIN
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
    qDebug() << "Saving Sword configuration ...";
    conf.save();
    CSwordBackend::instance()->reloadModules(CSwordBackend::PathChanged);
    return true;
}

QStringList sourceNameList() {
    BtInstallMgr mgr;
    BT_ASSERT(mgr.installConf);

    QStringList names;

    //add Sword remote sources
    for (auto const & sourcePair : mgr.sources)
        names << QString::fromLocal8Bit(sourcePair.second->caption);

    // Add local directory sources
    SWConfig config(configFilename().toLatin1());
    auto const sourcesSection = config.getSections().find("Sources");
    if (sourcesSection != config.getSections().end()) {
        auto sourceBegin = sourcesSection->second.lower_bound("DIRSource");
        auto const sourceEnd = sourcesSection->second.upper_bound("DIRSource");

        while (sourceBegin != sourceEnd) {
            InstallSource is("DIR", sourceBegin->second.c_str());
            names << QString::fromLatin1(is.caption.c_str());

            ++sourceBegin;
        }
    }

    return names;
}


void initPassiveFtpMode() {
    SWConfig config(configFilename().toLatin1());
    config["General"]["PassiveFTP"] = "true";
    config.save();
}
QString swordConfigFilename() {
    namespace DU = util::directory;

    qDebug() << "Sword config:"
#ifdef Q_OS_WIN
             << DU::getUserHomeDir().absolutePath().append("/Sword/sword.conf");
    return DU::getUserHomeDir().absolutePath().append("/Sword/sword.conf");
//    return DU::getApplicationDir().absolutePath().append("/sword.conf");
#else
             << DU::getUserHomeDir().absolutePath().append("/.sword/sword.conf");
    return DU::getUserHomeDir().absolutePath().append("/.sword/sword.conf");
#endif
}

QDir swordDir() {
    namespace DU = util::directory;

#ifdef Q_OS_WIN
    return QDir(DU::getUserHomeDir().absolutePath().append("/Sword/"));
#else
    return QDir(DU::getUserHomeDir().absolutePath().append("/.sword/"));
#endif
}

std::unique_ptr<CSwordBackend> backend(sword::InstallSource const & is) {
    /// \anchor BackendNotSingleton
    auto ret(std::make_unique<CSwordBackend>(isRemote(is)
                                             ? is.localShadow.c_str()
                                             : is.directory.c_str(),
                                             false));
    ret->initModules(CSwordBackend::OtherChange);
    return ret;
}

} // namespace BtInstallBackend
