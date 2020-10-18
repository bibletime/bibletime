/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cswordbackend.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QString>
#include <QTextCodec>
#include "../../util/directory.h"
#include "../btglobal.h"
#include "../btinstallmgr.h"
#include "../config/btconfig.h"
#include "../drivers/cswordbiblemoduleinfo.h"
#include "../drivers/cswordbookmoduleinfo.h"
#include "../drivers/cswordcommentarymoduleinfo.h"
#include "../drivers/cswordlexiconmoduleinfo.h"

// Sword includes:
#include <encfiltmgr.h>
#include <filemgr.h>
#include <rtfhtml.h>
#include <swfiltermgr.h>
#include <swfilter.h>
#include <utilstr.h>


using namespace Rendering;

CSwordBackend * CSwordBackend::m_instance = nullptr;

CSwordBackend::CSwordBackend()
        : sword::SWMgr(nullptr, nullptr, false,
                       new sword::EncodingFilterMgr(sword::ENC_UTF8), true)
        , m_dataModel(this)
{}

CSwordBackend::CSwordBackend(const QString & path, const bool augmentHome)
        : sword::SWMgr(!path.isEmpty() ? path.toLocal8Bit().constData() : nullptr,
                       false, new sword::EncodingFilterMgr(sword::ENC_UTF8),
                       false, augmentHome)
{}

CSwordBackend::~CSwordBackend() {
    shutdownModules();
}

BtModuleList CSwordBackend::moduleList(CSwordModuleInfo::ModuleType type) const
{
    BtModuleList l;
    Q_FOREACH(CSwordModuleInfo * m, moduleList())
        if(m->type() == type)
            l.append(m);
    return l;
}

CSwordModuleInfo * CSwordBackend::findFirstAvailableModule(CSwordModuleInfo::ModuleType type) {

    Q_FOREACH(CSwordModuleInfo * m, moduleList())
        if(m->type() == type)
            return m;
    return nullptr;

}

void CSwordBackend::uninstallModules(BtConstModuleSet const & toBeDeleted) {
    if (toBeDeleted.empty())
        return;
    m_dataModel.removeModules(toBeDeleted);
    emit sigSwordSetupChanged(RemovedModules);

    BtInstallMgr installMgr;
    QMap<QString, sword::SWMgr *> mgrDict; // Maps config paths to SWMgr objects
    for (CSwordModuleInfo const * const mInfo : toBeDeleted) {
        // Find the install path for the sword manager:
        QString dataPath = mInfo->config(CSwordModuleInfo::DataPath);
        if (dataPath.left(2) == "./")
            dataPath = dataPath.mid(2);

        QString prefixPath =
            mInfo->config(CSwordModuleInfo::AbsoluteDataPath) + "/";
        if (prefixPath.contains(dataPath)) {
            // Remove module part to get the prefix path:
            prefixPath = prefixPath.remove(prefixPath.indexOf(dataPath),
                                           dataPath.length());
        } else { // This is an error, should not happen
            qWarning() << "Removing" << mInfo->name()
                       << "didn't succeed because the absolute path"
                       << prefixPath << "didn't contain the data path"
                       << dataPath;
            continue; // don't remove this, go to next of the for loop
        }

        // Create the sword manager and remove the module
        sword::SWMgr * mgr = mgrDict[prefixPath];
        if (!mgr) { // Create new mgr if it's not yet available
            mgrDict.insert(prefixPath,
                           new sword::SWMgr(prefixPath.toLocal8Bit()));
            mgr = mgrDict[prefixPath];
        }
        qDebug() << "Removing the module" << mInfo->name() << "...";
        installMgr.removeModule(mgr, mInfo->module().getName());
    }
    qDeleteAll(toBeDeleted);
    qDeleteAll(mgrDict);
    mgrDict.clear();
}

QList<CSwordModuleInfo *> CSwordBackend::getPointerList(const QStringList & names) const {
    QList<CSwordModuleInfo *> list;
    Q_FOREACH (const QString & name, names)
        if (CSwordModuleInfo * const mInfo = findModuleByName(name))
            list.append(mInfo);
    return list;
}

BtConstModuleList CSwordBackend::getConstPointerList(const QStringList & names) const {
    BtConstModuleList list;
    Q_FOREACH (const QString & name, names)
        if (CSwordModuleInfo const * const mInfo = findModuleByName(name))
            list.append(mInfo);
    return list;
}

CSwordBackend::LoadError CSwordBackend::initModules(const SetupChangedReason reason) {
    // qWarning("globalSwordConfigPath is %s", globalConfPath);

    shutdownModules(); // Remove previous modules
    m_dataModel.clear();

    sword::ModMap::iterator end = Modules.end();
    const LoadError ret = static_cast<LoadError>(load());

    for (sword::ModMap::iterator it = Modules.begin(); it != end; ++it) {
        sword::SWModule * const curMod = it->second;
        BT_ASSERT(curMod);
        CSwordModuleInfo * newModule;

        const char * const modType = curMod->getType();
        if (!strcmp(modType, "Biblical Texts")) {
            newModule = new CSwordBibleModuleInfo(*curMod, *this);
            newModule->setDisplay(&m_chapterDisplay);
        } else if (!strcmp(modType, "Commentaries")) {
            newModule = new CSwordCommentaryModuleInfo(*curMod, *this);
            newModule->setDisplay(&m_entryDisplay);
        } else if (!strcmp(modType, "Lexicons / Dictionaries")) {
            newModule = new CSwordLexiconModuleInfo(*curMod, *this);
            newModule->setDisplay(&m_entryDisplay);
        } else if (!strcmp(modType, "Generic Books")) {
            newModule = new CSwordBookModuleInfo(*curMod, *this);
            newModule->setDisplay(&m_bookDisplay);
        } else {
            continue;
        }

        // Append the new modules to our list, but only if it's supported
        // The constructor of CSwordModuleInfo prints a warning on stdout
        if (!newModule->hasVersion()
            || (newModule->minimumSwordVersion() <= sword::SWVersion::currentVersion))
        {

            /* There is currently a deficiency in sword 1.8.1 in that backend->setCipherKey() does
             * not work correctly for modules from which data was already fetched. Therefore we have to
             * reload the modules. The cipher key must be set before any read occurs on the module.
             * Reading from the module can happen in subtle ways. The addModule below causes a read
             * to determine if the locked or unlocked icon is used by the model.
             */
            {
                if (newModule->isEncrypted()) {
                    const QString unlockKey = btConfig().getModuleEncryptionKey(newModule->name());
                    if (!unlockKey.isNull())
                        setCipherKey(newModule->name().toUtf8().constData(),
                                     unlockKey.toUtf8().constData());
                }
            }

                m_dataModel.addModule(newModule);
        } else {
            delete newModule;
        }
    }

    emit sigSwordSetupChanged(reason);
    return ret;
}

void CSwordBackend::addRenderFilters(sword::SWModule * module,
                                     sword::ConfigEntMap & section)
{
    sword::ConfigEntMap::const_iterator entry = section.find("SourceType");
    if (entry != section.end()) {
        if (entry->second == "OSIS") {
            module->addRenderFilter(&m_osisFilter);
            return;
        } else if (entry->second == "ThML") {
            module->addRenderFilter(&m_thmlFilter);
            return;
        } else if (entry->second == "TEI") {
            module->addRenderFilter(&m_teiFilter);
            return;
        } else if (entry->second == "GBF") {
            module->addRenderFilter(&m_gbfFilter);
            return;
        } else if (entry->second == "PLAIN") {
            module->addRenderFilter(&m_plainFilter);
            return;
        }
    }

    // No driver found
    entry = section.find("ModDrv");
    if (entry != section.end() && (entry->second == "RawCom" || entry->second == "RawLD"))
        module->addRenderFilter(&m_plainFilter);
}

void CSwordBackend::shutdownModules() {
    m_dataModel.clear(true);
    //BT  mods are deleted now, delete Sword mods, too.
    deleteAllModules();

    /* Cipher filters must be handled specially, because SWMgr creates them,
     * stores them in cipherFilters and cleanupFilters and attaches them to locked
     * modules. If these modules are removed, the filters need to be removed as well,
     * so that they are re-created for the new module objects.
     */
    using FMCI = sword::FilterMap::const_iterator;
    for (FMCI it = cipherFilters.begin(); it != cipherFilters.end(); ++it) {
        //Delete the Filter and remove it from the cleanup list
        cleanupFilters.remove(it->second);
        delete it->second;
    }
    cipherFilters.clear();
}

void CSwordBackend::setOption(const CSwordModuleInfo::FilterTypes type,
                              const int state)
{
    if (type == CSwordModuleInfo::textualVariants) {
        switch (state) {
        case 0:
            setGlobalOption(optionName(type).toUtf8().constData(),
                            "Primary Reading");
            break;
        case 1:
            setGlobalOption(optionName(type).toUtf8().constData(),
                            "Secondary Reading");
            break;
        default:
            setGlobalOption(optionName(type).toUtf8().constData(),
                            "All Readings");
            break;
        }
    } else {
        setGlobalOption(optionName(type).toUtf8().constData(),
                        state ? "On" : "Off");
    }
}

void CSwordBackend::setFilterOptions(const FilterOptions & options) {
    setOption(CSwordModuleInfo::footnotes,           options.footnotes);
    setOption(CSwordModuleInfo::strongNumbers,       options.strongNumbers);
    setOption(CSwordModuleInfo::headings,            options.headings);
    setOption(CSwordModuleInfo::morphTags,           options.morphTags);
    setOption(CSwordModuleInfo::lemmas,              options.lemmas);
    setOption(CSwordModuleInfo::hebrewPoints,        options.hebrewPoints);
    setOption(CSwordModuleInfo::hebrewCantillation,  options.hebrewCantillation);
    setOption(CSwordModuleInfo::greekAccents,        options.greekAccents);
    setOption(CSwordModuleInfo::redLetterWords,      options.redLetterWords);
    setOption(CSwordModuleInfo::textualVariants,     options.textualVariants);
    setOption(CSwordModuleInfo::morphSegmentation,   options.morphSegmentation);
    // setOption(CSwordModuleInfo::transliteration,   options.transliteration);
    setOption(CSwordModuleInfo::scriptureReferences, options.scriptureReferences);
}

CSwordModuleInfo * CSwordBackend::findModuleByDescription(const QString & description) const {
    Q_FOREACH(CSwordModuleInfo * const mod, m_dataModel.moduleList())
        if (mod->config(CSwordModuleInfo::Description) == description)
            return mod;
    return nullptr;
}

CSwordModuleInfo * CSwordBackend::findModuleByName(const QString & name) const {
    Q_FOREACH(CSwordModuleInfo * const mod, m_dataModel.moduleList())
        if (mod->name().compare(name, Qt::CaseInsensitive) == 0)
            return mod;
    return nullptr;
}

CSwordModuleInfo * CSwordBackend::findSwordModuleByPointer(const sword::SWModule * const swmodule) const {
    Q_FOREACH(CSwordModuleInfo * const mod, m_dataModel.moduleList())
        if (&mod->module() == swmodule)
            return mod;
    return nullptr;
}

QString CSwordBackend::optionName(const CSwordModuleInfo::FilterTypes option) {
    switch (option) {
        case CSwordModuleInfo::footnotes:
            return "Footnotes";
        case CSwordModuleInfo::strongNumbers:
            return "Strong's Numbers";
        case CSwordModuleInfo::headings:
            return "Headings";
        case CSwordModuleInfo::morphTags:
            return "Morphological Tags";
        case CSwordModuleInfo::lemmas:
            return "Lemmas";
        case CSwordModuleInfo::hebrewPoints:
            return "Hebrew Vowel Points";
        case CSwordModuleInfo::hebrewCantillation:
            return "Hebrew Cantillation";
        case CSwordModuleInfo::greekAccents:
            return "Greek Accents";
        case CSwordModuleInfo::redLetterWords:
            return "Words of Christ in Red";
        case CSwordModuleInfo::textualVariants:
            return "Textual Variants";
        case CSwordModuleInfo::scriptureReferences:
            return "Cross-references";
        case CSwordModuleInfo::morphSegmentation:
            return "Morph Segmentation";
    }
    return QString();
}

QString CSwordBackend::translatedOptionName(const CSwordModuleInfo::FilterTypes option) {
    switch (option) {
        case CSwordModuleInfo::footnotes:
            return QObject::tr("Footnotes");
        case CSwordModuleInfo::strongNumbers:
            return QObject::tr("Strong's numbers");
        case CSwordModuleInfo::headings:
            return QObject::tr("Headings");
        case CSwordModuleInfo::morphTags:
            return QObject::tr("Morphological tags");
        case CSwordModuleInfo::lemmas:
            return QObject::tr("Lemmas");
        case CSwordModuleInfo::hebrewPoints:
            return QObject::tr("Hebrew vowel points");
        case CSwordModuleInfo::hebrewCantillation:
            return QObject::tr("Hebrew cantillation marks");
        case CSwordModuleInfo::greekAccents:
            return QObject::tr("Greek accents");
        case CSwordModuleInfo::redLetterWords:
            return QObject::tr("Red letter words");
        case CSwordModuleInfo::textualVariants:
            return QObject::tr("Textual variants");
        case CSwordModuleInfo::scriptureReferences:
            return QObject::tr("Scripture cross-references");
        case CSwordModuleInfo::morphSegmentation:
            return QObject::tr("Morph segmentation");
    }
    return QString();
}


QString CSwordBackend::configOptionName(const CSwordModuleInfo::FilterTypes option) {
    switch (option) {
        case CSwordModuleInfo::footnotes:
            return "Footnotes";
        case CSwordModuleInfo::strongNumbers:
            return "Strongs";
        case CSwordModuleInfo::headings:
            return "Headings";
        case CSwordModuleInfo::morphTags:
            return "Morph";
        case CSwordModuleInfo::lemmas:
            return "Lemma";
        case CSwordModuleInfo::hebrewPoints:
            return "HebrewPoints";
        case CSwordModuleInfo::hebrewCantillation:
            return "Cantillation";
        case CSwordModuleInfo::greekAccents:
            return "GreekAccents";
        case CSwordModuleInfo::redLetterWords:
            return "RedLetterWords";
        case CSwordModuleInfo::textualVariants:
            return "Variants";
        case CSwordModuleInfo::scriptureReferences:
            return "Scripref";
        case CSwordModuleInfo::morphSegmentation:
            return "MorphSegmentation";
    }
    return QString();
}

const QString CSwordBackend::booknameLanguage(const QString & language) {
    if (!language.isEmpty()) {
        sword::LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(language.toUtf8().constData());

        // Refresh the locale of all Bible and commentary modules!
        // Use what sword returns, language may be different.
        const QByteArray newLocaleName(QString(sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName()).toUtf8());

        Q_FOREACH(CSwordModuleInfo const * const mod, m_dataModel.moduleList()) {
            if (mod->type() == CSwordModuleInfo::Bible
                || mod->type() == CSwordModuleInfo::Commentary)
            {
                // Create a new key, it will get the default bookname language:
                using VK = sword::VerseKey;
                VK & vk = *static_cast<VK *>(mod->module().getKey());
                vk.setLocale(newLocaleName.constData());
            }
        }

    }
    return sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName();
}

void CSwordBackend::reloadModules(const SetupChangedReason reason) {
    shutdownModules();

    //delete Sword's config to make Sword reload it!

    if (myconfig) { // force reload on config object because we may have changed the paths
        delete myconfig;
        config = myconfig = nullptr;
        // we need to call findConfig to make sure that augPaths are reloaded
        findConfig(&configType, &prefixPath, &configPath, &augPaths, &sysConfig);
        // now re-read module configuration files
        loadConfigDir(configPath);
    } else if (config) {
        config->load();
    }

    initModules(reason);
}

// Get one or more shared sword config (sword.conf) files
QStringList CSwordBackend::getSharedSwordConfigFiles() const {
#ifdef Q_OS_WIN
    //  %ProgramData%\Sword\sword.conf
    return QStringList(util::directory::convertDirSeparators(qgetenv("SWORD_PATH")) += "/Sword/sword.conf");
#else
    // /etc/sword.conf, /usr/local/etc/sword.conf
    return QString(globalConfPath).split(":");
#endif
}

// Get the private sword directory
QString CSwordBackend::getPrivateSwordConfigPath() const {
    return util::directory::getUserHomeSwordDir().absolutePath();
}

QString CSwordBackend::getPrivateSwordConfigFile() const {
    return util::directory::convertDirSeparators(getPrivateSwordConfigPath() += "/sword.conf");
}

// Return a list of used Sword dirs. Useful for the installer.
QStringList CSwordBackend::swordDirList() const {
    namespace DU = util::directory;
    using SLCI = QStringList::const_iterator;

    // Get the set of sword directories that could contain modules:
    QSet<QString> swordDirSet;
    QStringList configs;

    if (QFile(getPrivateSwordConfigFile()).exists()) {
        // Use the private sword.conf file:
        configs << getPrivateSwordConfigFile();
    } else {
        /*
          Did not find private sword.conf, will use shared sword.conf files to
          build the private one. Once the private sword.conf exist, the shared
          ones will not be searched again.
        */
        configs = getSharedSwordConfigFiles();

#ifdef Q_OS_WIN
        /*
          On Windows, add the shared sword directory to the set so the new
          private sword.conf will have it. The user could decide to delete this
          shared path and it will not automatically come back.
        */
        swordDirSet << DU::convertDirSeparators(qgetenv("SWORD_PATH"));
#endif
    }

    // Search the sword.conf file(s) for sword directories that could contain modules
    for (SLCI it = configs.begin(); it != configs.end(); ++it) {
        if (!QFileInfo(*it).exists())
            continue;

        /*
          Get all DataPath and AugmentPath entries from the config file and add
          them to the list:
        */
        sword::SWConfig conf(it->toUtf8().constData());
        swordDirSet << QDir(QTextCodec::codecForLocale()->toUnicode(conf["Install"]["DataPath"].c_str())).absolutePath();

        const sword::ConfigEntMap group(conf["Install"]);
        using CEMCI = sword::ConfigEntMap::const_iterator ;
        for (std::pair<CEMCI, CEMCI> its = group.equal_range("AugmentPath");
             its.first != its.second;
             ++(its.first))
        {
            // Added augment path:
            swordDirSet << QDir(QTextCodec::codecForLocale()->toUnicode(its.first->second.c_str())).absolutePath();
        }
    }

    // Add the private sword path to the set if not there already:
    swordDirSet << getPrivateSwordConfigPath();

    QStringList swordDirs;
    for (auto dir: swordDirSet)
        swordDirs.append(dir);
    return swordDirs;
}

void CSwordBackend::deleteOrphanedIndices() {
    const QStringList entries = QDir(CSwordModuleInfo::getGlobalBaseIndexLocation()).entryList(QDir::Dirs);
    Q_FOREACH(const QString & entry, entries) {
        if (entry == "." || entry == "..")
            continue;
        if (CSwordModuleInfo * const module = findModuleByName(entry)) {
            if (!module->hasIndex()) { //index files found, but wrong version etc.
                qDebug() << "deleting outdated index for module" << entry;
                CSwordModuleInfo::deleteIndexForModule(entry);
            }
        } else { //no module exists
            if (btConfig().value<bool>("settings/behaviour/autoDeleteOrphanedIndices", true)) {
                qDebug() << "deleting orphaned index in directory" << entry;
                CSwordModuleInfo::deleteIndexForModule(entry);
            }
        }
    }
}
