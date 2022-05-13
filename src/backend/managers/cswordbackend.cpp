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

#include "cswordbackend.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QString>
#include <QTextCodec>
#include <string_view>
#include "../../util/btconnect.h"
#include "../../util/directory.h"
#include "../btglobal.h"
#include "../btinstallmgr.h"
#include "../config/btconfig.h"
#include "../drivers/cswordbiblemoduleinfo.h"
#include "../drivers/cswordbookmoduleinfo.h"
#include "../drivers/cswordcommentarymoduleinfo.h"
#include "../drivers/cswordlexiconmoduleinfo.h"
#include "btlocalemgr.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <encfiltmgr.h>
#include <filemgr.h>
#include <rtfhtml.h>
#include <swfiltermgr.h>
#include <swfilter.h>
#include <swversion.h>
#include <utilstr.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


using namespace Rendering;

CSwordBackend * CSwordBackend::m_instance = nullptr;

CSwordBackend::CSwordBackend()
        : m_manager(nullptr, nullptr, false,
                    new sword::EncodingFilterMgr(sword::ENC_UTF8), true)
        , m_dataModel(BtBookshelfModel::newInstance())
{
    auto const clearCache =
            [this]() noexcept {
                std::atomic_store_explicit(
                            &m_availableLanguagesCache,
                            decltype(m_availableLanguagesCache)(),
                            std::memory_order_relaxed);
            };
    BT_CONNECT(m_dataModel.get(), &BtBookshelfModel::rowsAboutToBeInserted,
               clearCache);
    BT_CONNECT(m_dataModel.get(), &BtBookshelfModel::rowsAboutToBeRemoved,
               clearCache);
}

CSwordBackend::CSwordBackend(const QString & path, const bool augmentHome)
        : m_manager(!path.isEmpty() ? path.toLocal8Bit().constData() : nullptr,
                    false, new sword::EncodingFilterMgr(sword::ENC_UTF8),
                    false, augmentHome)
        , m_dataModel(BtBookshelfModel::newInstance())
{}

CSwordBackend::~CSwordBackend() {
    shutdownModules();
}

CSwordModuleInfo * CSwordBackend::findFirstAvailableModule(CSwordModuleInfo::ModuleType type) {

    for (CSwordModuleInfo * const m : moduleList())
        if(m->type() == type)
            return m;
    return nullptr;

}

std::shared_ptr<CSwordBackend::AvailableLanguagesCacheContainer const>
CSwordBackend::availableLanguages() noexcept
{
    auto oldCache = std::atomic_load_explicit(&m_availableLanguagesCache,
                                              std::memory_order_acquire);
    if (oldCache)
        return oldCache;

    auto const generateCache =
            [&model = *m_dataModel] {
                AvailableLanguagesCacheContainer newCache;
                for (auto const * const mod : model.moduleList()) {
                    newCache.emplace(mod->language());
                    if (auto lang2 = mod->glossaryTargetlanguage())
                        newCache.emplace(std::move(lang2));
                }

                return std::make_shared<AvailableLanguagesCacheContainer const>(
                            std::move(newCache)); // also makes container const
            };

    for (auto newCache = generateCache();; newCache = generateCache())
        if (std::atomic_compare_exchange_strong_explicit(
                &m_availableLanguagesCache,
                &oldCache,
                newCache,
                std::memory_order_acq_rel,
                std::memory_order_relaxed))
            return newCache;
}

void CSwordBackend::uninstallModules(BtConstModuleSet const & toBeDeleted) {
    if (toBeDeleted.empty())
        return;
    m_dataModel->removeModules(toBeDeleted);
    Q_EMIT sigSwordSetupChanged(RemovedModules);

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
        installMgr.removeModule(mgr, mInfo->swordModule().getName());
    }
    qDeleteAll(toBeDeleted);
    qDeleteAll(mgrDict);
    mgrDict.clear();
}

QList<CSwordModuleInfo *> CSwordBackend::getPointerList(const QStringList & names) const {
    QList<CSwordModuleInfo *> list;
    for (auto const & name : names)
        if (CSwordModuleInfo * const mInfo = findModuleByName(name))
            list.append(mInfo);
    return list;
}

BtConstModuleList CSwordBackend::getConstPointerList(const QStringList & names) const {
    BtConstModuleList list;
    for (auto const & name : names)
        if (CSwordModuleInfo const * const mInfo = findModuleByName(name))
            list.append(mInfo);
    return list;
}

CSwordBackend::LoadError CSwordBackend::initModules(const SetupChangedReason reason) {
    // qWarning("globalSwordConfigPath is %s", globalConfPath);

    shutdownModules(); // Remove previous modules
    m_dataModel->clear();

    const LoadError ret = static_cast<LoadError>(m_manager.load());

    for (auto const & modulePair : m_manager.getModules()) {
        sword::SWModule * const curMod = modulePair.second;
        BT_ASSERT(curMod);
        std::unique_ptr<CSwordModuleInfo> newModule;

        std::string_view const modType = curMod->getType();
        using namespace std::literals;
        if (modType == "Biblical Texts"sv) {
            newModule = std::make_unique<CSwordBibleModuleInfo>(*curMod, *this);
            newModule->setDisplay(&m_chapterDisplay);
        } else if (modType == "Commentaries"sv) {
            newModule = std::make_unique<CSwordCommentaryModuleInfo>(*curMod,
                                                                     *this);
            newModule->setDisplay(&m_entryDisplay);
        } else if (modType == "Lexicons / Dictionaries"sv) {
            newModule = std::make_unique<CSwordLexiconModuleInfo>(*curMod,
                                                                  *this);
            newModule->setDisplay(&m_entryDisplay);
        } else if (modType == "Generic Books"sv) {
            newModule = std::make_unique<CSwordBookModuleInfo>(*curMod, *this);
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
            if (newModule->isEncrypted()) {
                auto const unlockKey(
                        btConfig().getModuleEncryptionKey(newModule->name()));
                if (!unlockKey.isNull())
                    m_manager.setCipherKey(
                                newModule->name().toUtf8().constData(),
                                unlockKey.toUtf8().constData());
            }

            /// \todo Refactor data model to use shared_ptr to contain works
            m_dataModel->addModule(newModule.get());
            newModule.release();
        }
    }

    Q_EMIT sigSwordSetupChanged(reason);
    return ret;
}

void CSwordBackend::Private::addRenderFilters(sword::SWModule * module,
                                              sword::ConfigEntMap & section)
{
    auto entry(section.find("SourceType"));
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
    m_dataModel->clear(true);
    m_manager.shutdownModules();
}

void CSwordBackend::Private::shutdownModules() {
    //BT  mods are deleted now, delete Sword mods, too.
    deleteAllModules();

    /* Cipher filters must be handled specially, because SWMgr creates them,
     * stores them in cipherFilters and cleanupFilters and attaches them to locked
     * modules. If these modules are removed, the filters need to be removed as well,
     * so that they are re-created for the new module objects.
     */
    for (auto const & filterPair : cipherFilters) {
        //Delete the Filter and remove it from the cleanup list
        cleanupFilters.remove(filterPair.second);
        delete filterPair.second;
    }
    cipherFilters.clear();
}

void CSwordBackend::setOption(const CSwordModuleInfo::FilterTypes type,
                              const int state)
{
    if (type == CSwordModuleInfo::textualVariants) {
        switch (state) {
        case 0:
            m_manager.setGlobalOption(optionName(type).toUtf8().constData(),
                                      "Primary Reading");
            break;
        case 1:
            m_manager.setGlobalOption(optionName(type).toUtf8().constData(),
                                      "Secondary Reading");
            break;
        default:
            m_manager.setGlobalOption(optionName(type).toUtf8().constData(),
                                      "All Readings");
            break;
        }
    } else {
        m_manager.setGlobalOption(optionName(type).toUtf8().constData(),
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
    for (auto * const mod : m_dataModel->moduleList())
        if (mod->config(CSwordModuleInfo::Description) == description)
            return mod;
    return nullptr;
}

CSwordModuleInfo * CSwordBackend::findModuleByName(const QString & name) const {
    for (auto * const mod : m_dataModel->moduleList())
        if (mod->name().compare(name, Qt::CaseInsensitive) == 0)
            return mod;
    return nullptr;
}

CSwordModuleInfo * CSwordBackend::findSwordModuleByPointer(const sword::SWModule * const swmodule) const {
    for (auto * const mod : m_dataModel->moduleList())
        if (&mod->swordModule() == swmodule)
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

QString CSwordBackend::booknameLanguage() const
{ return BtLocaleMgr::defaultLocaleName(); }

void CSwordBackend::setBooknameLanguage(QString const & language) {
    BtLocaleMgr::setDefaultLocaleName(language);

    // Refresh the locale of all Bible and commentary modules!
    // Use what sword returns, language may be different.
    const QByteArray newLocaleName(BtLocaleMgr::defaultLocaleName().toUtf8());

    for (auto const * const mod : m_dataModel->moduleList()) {
        if (mod->type() == CSwordModuleInfo::Bible
            || mod->type() == CSwordModuleInfo::Commentary)
        {
            // Create a new key, it will get the default bookname language:
            using VK = sword::VerseKey;
            VK & vk = *static_cast<VK *>(mod->swordModule().getKey());
            vk.setLocale(newLocaleName.constData());
        }
    }
}

void CSwordBackend::reloadModules(const SetupChangedReason reason) {
    shutdownModules();
    m_manager.reloadConfig();
    initModules(reason);
}

void CSwordBackend::Private::reloadConfig() {
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
}

// Get one or more shared sword config (sword.conf) files
QStringList CSwordBackend::getSharedSwordConfigFiles() const {
#ifdef Q_OS_WIN
    //  %ProgramData%\Sword\sword.conf
    return QStringList(util::directory::convertDirSeparators(qgetenv("SWORD_PATH")) += "/Sword/sword.conf");
#else
    // /etc/sword.conf, /usr/local/etc/sword.conf
    return QString(m_manager.globalConfPath).split(":");
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
    for (auto const & filename : configs) {
        if (!QFileInfo(filename).exists())
            continue;

        /*
          Get all DataPath and AugmentPath entries from the config file and add
          them to the list:
        */
        sword::SWConfig conf(filename.toUtf8().constData());
        swordDirSet << QDir(QTextCodec::codecForLocale()->toUnicode(conf["Install"]["DataPath"].c_str())).absolutePath();

        const sword::ConfigEntMap group(conf["Install"]);
        for (auto its = group.equal_range("AugmentPath");
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
    for (auto const & entry : entries) {
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
