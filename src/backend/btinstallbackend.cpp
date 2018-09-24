/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btinstallbackend.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <swordxx/filemgr.h>
#include <swordxx/swconfig.h>
#include "../util/btassert.h"
#include "../util/directory.h"
#include "managers/cswordbackend.h"
#include "btinstallmgr.h"


namespace BtInstallBackend {

/** Adds the source described by Source to the backend. */
bool addSource(swordxx::InstallSource& source) {
    swordxx::SWConfig config(configFilename().toStdString());
    if (isRemote(source)) {
        if (!source.m_directory.empty() && source.m_directory.back() == '/') {
            source.m_directory.pop_back();
        }
        config["Sources"].emplace(source.m_type + "Source", source.getConfEnt());
    } else if (source.m_type == "DIR") {
        config["Sources"].emplace("DIRSource", source.getConfEnt());
    }
    config.save();
    return true;
}

/** Returns the Source struct. */
swordxx::InstallSource source(const QString &name) {
    BtInstallMgr mgr;
    auto const sourceIt(mgr.sources.find(name.toStdString()));
    if (sourceIt != mgr.sources.end()) {
        return *(sourceIt->second);
    } else { //not found in Sword, may be a local DIR source
        swordxx::SWConfig config(configFilename().toStdString());
        auto const sourcesSection(config.sections().find("Sources"));
        if (sourcesSection != config.sections().end()) {
            auto sourceBegin(sourcesSection->second.lower_bound("DIRSource"));
            auto const sourceEnd(sourcesSection->second.upper_bound("DIRSource"));

            while (sourceBegin != sourceEnd) {
                swordxx::InstallSource is("DIR", sourceBegin->second.c_str());
                if (is.m_caption == name.toStdString()) //found local dir source
                    return is;

                ++sourceBegin; //next source
            }
        }
    }

    swordxx::InstallSource is("EMPTY");   //default return value
    is.m_caption = "unknown caption";
    is.m_source = "unknown source";
    is.m_directory = "unknown dir";
    return is;
}

/** Deletes the source. */
bool deleteSource(const QString &name) {
    swordxx::InstallSource is = source(name );

    swordxx::SWConfig config(configFilename().toStdString());

    //this code can probably be shortened by using the stl remove_if functionality
    auto const sourceConfigEntry(is.getConfEnt());
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
        QString sce(QString::fromStdString(sourceConfigEntry));
        QStringList l = sce.split('|');
        l.removeLast();
        sce = l.join("|").append("|");
        it = config["Sources"].begin();
        while (it != config["Sources"].end()) {
            if (it->second == sce.toStdString()) {
                config["Sources"].erase(it);
                break;
            }
            ++it;
        }
    }

    config.save();
    return true; /// \todo dummy
}

/** Returns the moduleinfo list for the source. Delete the pointer after using. IS THIS POSSIBLE?*/
QList<CSwordModuleInfo*> moduleList(QString name) {
    /// \todo dummy
    Q_UNUSED(name);
    BT_ASSERT(false && "not implemented");
    return QList<CSwordModuleInfo*>();
}

bool isRemote(const swordxx::InstallSource& source) {
    return (source.m_type == "FTP")
            || (source.m_type == "FTPS")
            || (source.m_type == "HTTP")
            || (source.m_type == "HTTPS");
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
    swordxx::SWConfig conf(filename.toStdString());
    conf.sections().clear();

#ifdef Q_OS_WIN
    // On Windows, add the sword directory to the config file.
    QString swordPath = DU::convertDirSeparators( DU::getApplicationSwordDir().absolutePath());
    conf["Install"].insert(
        std::make_pair( SWBuf("LocalePath"), swordPath.toLocal8Bit().data() )
    );
#endif

    bool setDataPath = false;
    for (QStringList::const_iterator it = targets.begin(); it != targets.end(); ++it) {
        QString t = DU::convertDirSeparators(*it);
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
            conf["Install"].emplace(!setDataPath ? "DataPath": "AugmentPath",
                                    t.toStdString());
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
    for (auto const & sp : mgr.sources)
        names << QString::fromStdString(sp.second->m_caption);

    // Add local directory sources
    swordxx::SWConfig config(configFilename().toStdString());
    auto const sourcesSection(config.sections().find("Sources"));
    if (sourcesSection != config.sections().end()) {
        auto sourceBegin(sourcesSection->second.lower_bound("DIRSource"));
        auto const sourceEnd(sourcesSection->second.upper_bound("DIRSource"));

        while (sourceBegin != sourceEnd) {
            swordxx::InstallSource is("DIR", sourceBegin->second.c_str());
            names << QString::fromStdString(is.m_caption);

            ++sourceBegin;
        }
    }

    return names;
}


void initPassiveFtpMode() {
    swordxx::SWConfig config(configFilename().toStdString());
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

CSwordBackend * backend(const swordxx::InstallSource & is) {
    /// \anchor BackendNotSingleton
    CSwordBackend * const ret = new CSwordBackend((isRemote(is)
                                                   ? is.m_localShadow
                                                   : is.m_directory).c_str(),
                                                  false);
    ret->initModules(CSwordBackend::OtherChange);
    return ret;
}

} // namespace BtInstallBackend
