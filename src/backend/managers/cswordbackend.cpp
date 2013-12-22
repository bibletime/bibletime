/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/managers/cswordbackend.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QString>
#include <QTextCodec>
#include "backend/config/btconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/filters/btosismorphsegmentation.h"
#include "backend/filters/thmltoplain.h"
#include "btglobal.h"
#include "util/directory.h"

// Sword includes:
#include <encfiltmgr.h>
#include <filemgr.h>
#include <rtfhtml.h>
#include <swdisp.h>
#include <swfiltermgr.h>
#include <swfilter.h>
#include <utilstr.h>


using namespace Rendering;

CSwordBackend * CSwordBackend::m_instance = 0;

CSwordBackend::CSwordBackend()
        : sword::SWMgr(0, 0, false,
                       new sword::EncodingFilterMgr(sword::ENC_UTF8), true)
        , m_dataModel(this)
{
    filterInit();
}

CSwordBackend::CSwordBackend(const QString & path, const bool augmentHome)
        : sword::SWMgr(!path.isEmpty() ? path.toLocal8Bit().constData() : 0,
                       false, new sword::EncodingFilterMgr(sword::ENC_UTF8),
                       false, augmentHome)
{ // don't allow module renaming, because we load from a path
    filterInit();
}

CSwordBackend::~CSwordBackend() {
    shutdownModules();
}

void CSwordBackend::filterInit() {
    // HACK: replace Sword's OSISMorphSegmentation filter, seems to be buggy, ours works
    if (sword::SWOptionFilter * const filter = optionFilters["OSISMorphSegmentation"]) {
        cleanupFilters.remove(filter);
        optionFilters.erase("OSISMorphSegmentation");
        delete filter;
    }
    sword::SWOptionFilter * const tmpFilter = new Filters::BtOSISMorphSegmentation();
    optionFilters.insert(sword::OptionFilterMap::value_type("OSISMorphSegmentation", tmpFilter));
    cleanupFilters.push_back(tmpFilter);

    // HACK: replace Sword's ThML strip filter with our own version
    // Remove this hack as soon as Sword is fixed
    cleanupFilters.remove(thmlplain);
    delete thmlplain;
    thmlplain = new Filters::ThmlToPlain();
    cleanupFilters.push_back(thmlplain);
}

QList<CSwordModuleInfo *> CSwordBackend::takeModulesFromList(const QStringList & names) {
    QList<CSwordModuleInfo *> list;
    Q_FOREACH (const QString & name, names) {
        CSwordModuleInfo * const mInfo = findModuleByName(name);
        if (mInfo) {
            m_dataModel.removeModule(mInfo);
            list.append(mInfo);
        }
    }
    if (!list.isEmpty())
        emit sigSwordSetupChanged(RemovedModules);
    return list;
}

QList<CSwordModuleInfo *> CSwordBackend::getPointerList(const QStringList & names) const {
    QList<CSwordModuleInfo *> list;
    Q_FOREACH (const QString & name, names) {
        CSwordModuleInfo * const mInfo = findModuleByName(name);
        if (mInfo)
            list.append(mInfo);
    }
    return list;
}

QList<const CSwordModuleInfo *> CSwordBackend::getConstPointerList(
        const QStringList & names) const
{
    QList<const CSwordModuleInfo *> list;
    Q_FOREACH (const QString & name, names) {
        const CSwordModuleInfo * const mInfo = findModuleByName(name);
        if (mInfo)
            list.append(mInfo);
    }
    return list;
}

CSwordBackend::LoadError CSwordBackend::initModules(const SetupChangedReason reason) {
    // qWarning("globalSwordConfigPath is %s", globalConfPath);

    shutdownModules(); // Remove previous modules
    m_dataModel.clear();

    sword::ModMap::iterator end = Modules.end();
    const LoadError ret = static_cast<LoadError>(Load());

    for (sword::ModMap::iterator it = Modules.begin(); it != end; ++it) {
        sword::SWModule * const curMod = it->second;
        CSwordModuleInfo * newModule;

        const char * const modType = curMod->getType();
        if (!strcmp(modType, "Biblical Texts")) {
            newModule = new CSwordBibleModuleInfo(curMod, *this);
            newModule->module()->setDisplay(&m_chapterDisplay);
        } else if (!strcmp(modType, "Commentaries")) {
            newModule = new CSwordCommentaryModuleInfo(curMod, *this);
            newModule->module()->setDisplay(&m_entryDisplay);
        } else if (!strcmp(modType, "Lexicons / Dictionaries")) {
            newModule = new CSwordLexiconModuleInfo(curMod, *this);
            newModule->module()->setDisplay(&m_entryDisplay);
        } else if (!strcmp(modType, "Generic Books")) {
            newModule = new CSwordBookModuleInfo(curMod, *this);
            newModule->module()->setDisplay(&m_bookDisplay);
        } else {
            continue;
        }

        // Append the new modules to our list, but only if it's supported
        // The constructor of CSwordModuleInfo prints a warning on stdout
        if (!newModule->hasVersion()
            || (newModule->minimumSwordVersion() <= sword::SWVersion::currentVersion))
        {
            m_dataModel.addModule(newModule);
        } else {
            delete newModule;
        }
    }

    // Unlock modules if keys are present:
    Q_FOREACH(CSwordModuleInfo * mod, m_dataModel.moduleList()) {
        if (mod->isEncrypted()) {
            const QString unlockKey = btConfig().getModuleEncryptionKey(mod->name());
            if (!unlockKey.isNull())
                setCipherKey(mod->name().toUtf8().constData(),
                             unlockKey.toUtf8().constData());
        }
    }

    emit sigSwordSetupChanged(reason);
    return ret;
}

void CSwordBackend::AddRenderFilters(sword::SWModule * module,
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
    DeleteMods();

    /* Cipher filters must be handled specially, because SWMgr creates them,
     * stores them in cipherFilters and cleanupFilters and attaches them to locked
     * modules. If these modules are removed, the filters need to be removed as well,
     * so that they are re-created for the new module objects.
     */
    typedef sword::FilterMap::const_iterator FMCI;
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
    Q_FOREACH (CSwordModuleInfo * mod, m_dataModel.moduleList())
        if (mod->config(CSwordModuleInfo::Description) == description)
            return mod;
    return 0;
}

CSwordModuleInfo * CSwordBackend::findModuleByName(const QString & name) const {
    Q_FOREACH (CSwordModuleInfo * mod, m_dataModel.moduleList())
        if (mod->name() == name)
            return mod;
    return 0;
}

CSwordModuleInfo * CSwordBackend::findSwordModuleByPointer(const sword::SWModule * const swmodule) const {
    Q_FOREACH (CSwordModuleInfo * mod, m_dataModel.moduleList())
        if (mod->module() == swmodule)
            return mod;
    return 0;
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
    return QString::null;
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
    return QString::null;
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
    return QString::null;
}

const QString CSwordBackend::booknameLanguage(const QString & language) {
    if (!language.isEmpty()) {
        sword::LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName(language.toUtf8().constData());

        // Refresh the locale of all Bible and commentary modules!
        // Use what sword returns, language may be different.
        const QByteArray newLocaleName(QString(sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName()).toUtf8());

        Q_FOREACH (CSwordModuleInfo * mod, m_dataModel.moduleList()) {
            if (mod->type() == CSwordModuleInfo::Bible
                || mod->type() == CSwordModuleInfo::Commentary)
            {
                // Create a new key, it will get the default bookname language:
                typedef sword::VerseKey VK;
                VK & vk = *static_cast<VK *>(mod->module()->getKey());
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
        config = myconfig = 0;
        // we need to call findConfig to make sure that augPaths are reloaded
        findConfig(&configType, &prefixPath, &configPath, &augPaths, &sysConfig);
        // now re-read module configuration files
        loadConfigDir(configPath);
    } else if (config) {
        config->Load();
    }

    initModules(reason);
}

// Get one or more shared sword config (sword.conf) files
QStringList CSwordBackend::getSharedSwordConfigFiles() const {
#ifdef Q_OS_WIN
    //  %ALLUSERSPROFILE%\Sword\sword.conf
    return util::directory::convertDirSeparators(QString(getenv("SWORD_PATH"))) += "/Sword/sword.conf";
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
    typedef QStringList::const_iterator SLCI;
    typedef sword::ConfigEntMap::const_iterator CEMCI;

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
        swordDirSet << DU::convertDirSeparators(QString(getenv("SWORD_PATH")));
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
        typedef sword::ConfigEntMap::const_iterator CEMCI;
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

    return QStringList::fromSet(swordDirSet);
}

void CSwordBackend::deleteOrphanedIndices() {
    const QStringList entries = QDir(CSwordModuleInfo::getGlobalBaseIndexLocation()).entryList(QDir::Dirs);
    Q_FOREACH(const QString & entry, entries) {
        if (entry == "." || entry == "..")
            continue;
        CSwordModuleInfo * const module = findModuleByName(entry);
        if (module) { //mod exists
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
