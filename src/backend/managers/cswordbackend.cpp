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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QStringDecoder>
#endif
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

    setBooknameLanguage(btConfig().booknameLanguage());
    initModules();
    deleteOrphanedIndices();

    BT_ASSERT(!m_instance);
    m_instance = this;
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
    Q_EMIT sigSwordSetupChanged();

    BtInstallMgr installMgr;
    QMap<QString, sword::SWMgr *> mgrDict; // Maps config paths to SWMgr objects
    for (CSwordModuleInfo const * const mInfo : toBeDeleted) {
        // Find the install path for the sword manager:
        QString dataPath = mInfo->config(CSwordModuleInfo::DataPath);
        if (dataPath.left(2) == QStringLiteral("./"))
            dataPath = dataPath.mid(2);

        QString prefixPath =
            mInfo->config(CSwordModuleInfo::AbsoluteDataPath) + '/';
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

CSwordBackend::LoadError CSwordBackend::initModules() {
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
        } else if (modType == "Commentaries"sv) {
            newModule = std::make_unique<CSwordCommentaryModuleInfo>(*curMod,
                                                                     *this);
        } else if (modType == "Lexicons / Dictionaries"sv) {
            newModule = std::make_unique<CSwordLexiconModuleInfo>(*curMod,
                                                                  *this);
        } else if (modType == "Generic Books"sv) {
            newModule = std::make_unique<CSwordBookModuleInfo>(*curMod, *this);
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

    Q_EMIT sigSwordSetupChanged();
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

void CSwordBackend::setOption(CSwordModuleInfo::FilterOption const & option,
                              const int state)
{ m_manager.setGlobalOption(option.optionName, option.valueToString(state)); }

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

QString CSwordBackend::booknameLanguage() const
{ return BtLocaleMgr::defaultLocaleName(); }

void CSwordBackend::setBooknameLanguage(QString const & language) {
    BtLocaleMgr::setDefaultLocaleName(language);

    /* Use locale name returned by SWORD instead of using the given argument
       directly, as the language may be different. */
    auto const newLocaleName = BtLocaleMgr::defaultLocaleName().toUtf8();

    /* Refresh the locale of all Bible and commentary modules. Whereas the
       locale is only used by modules with verse keys (i.e. sword::VerseKey) and
       ignored on all other modules, we could just set the locale on those
       modules (bibles and commentaries) only. However, since the keys are used
       for all uncountable abominations, it feels safer to set the locale on all
       modules, lest an incorrect locale gets copied from a non-VerseKey module
       to modules with verse keys. This has the slight overhead of copying the
       locale name for each such module, but this is hopefully negligible due to
       this method not being called very often. */
    for (auto const * const mod : m_dataModel->moduleList())
        mod->swordModule().getKey()->setLocale(newLocaleName.constData());
}

void CSwordBackend::reloadModules() {
    shutdownModules();
    m_manager.reloadConfig();
    initModules();
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

// Return a list of used Sword dirs. Useful for the installer.
QStringList CSwordBackend::swordDirList() const {
    namespace DU = util::directory;

    // Get the set of sword directories that could contain modules:
    QSet<QString> swordDirSet;
    QList<QFileInfo> configs;

    auto const userHomeSwordDir = util::directory::getUserHomeSwordDir();
    if (auto conf = QFileInfo(userHomeSwordDir, QStringLiteral("sword.conf"));
        conf.exists())
    {
        // Use the private sword.conf file:
        configs << std::move(conf);
    } else {
        /*
          Did not find private sword.conf, will use shared sword.conf files to
          build the private one. Once the private sword.conf exist, the shared
          ones will not be searched again.
        */
        #ifdef Q_OS_WIN
        auto const swordPath = qEnvironmentVariable("SWORD_PATH");
        for (auto const & path : swordPath.split(QDir::listSeparator())) {
            QDir dir(path);

            /*
              On Windows, add the shared sword directory to the set so the new
              private sword.conf will have it. The user could decide to delete this
              shared path and it will not automatically come back.
            */
            swordDirSet << dir.absolutePath();

            // %ProgramData%\Sword\sword.conf
            if (auto conf = QFileInfo(dir, QStringLiteral("Sword/sword.conf"));
                conf.exists())
                configs << std::move(conf);
        }

        #else
        // /etc/sword.conf, /usr/local/etc/sword.conf
        for (auto const & path : QString(m_manager.globalConfPath).split(':'))
            if (auto conf = QFileInfo(path); conf.exists())
                configs << std::move(conf);
        #endif
    }

    // Search the sword.conf file(s) for sword directories that could contain modules
    #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    static constexpr auto const decoderFlags = QStringDecoder::Flag::Stateless;
    #endif
    for (auto const & fileInfo : configs) {
        /*
          Get all DataPath and AugmentPath entries from the config file and add
          them to the list:
        */
        sword::SWConfig conf(fileInfo.absoluteFilePath().toUtf8().constData());

        static auto const decodeConfEntry =
                [](sword::SWBuf const & buf) {
                    #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
                    return QString::fromUtf8(buf.c_str());
                    #else
                    QStringDecoder utf8Decoder("UTF-8", decoderFlags);
                    // Do not use auto here due to QTBUG-117705/QTBUG-117902:
                    QString result = utf8Decoder(buf.c_str());
                    if (!utf8Decoder.hasError())
                        return result;
                    QStringDecoder cp1252Decoder("Windows-1252", decoderFlags);
                    result = cp1252Decoder(buf.c_str());
                    if (!cp1252Decoder.hasError())
                        return result;
                    return QString::fromLatin1(buf.c_str());
                    #endif
                };

        swordDirSet << QDir(decodeConfEntry(conf["Install"]["DataPath"])).absolutePath();

        const sword::ConfigEntMap group(conf["Install"]);
        for (auto its = group.equal_range("AugmentPath");
             its.first != its.second;
             ++(its.first))
        {
            // Added augment path:
            swordDirSet << QDir(decodeConfEntry(its.first->second)).absolutePath();
        }
    }

    // Add the private sword path to the set if not there already:
    swordDirSet << userHomeSwordDir.absolutePath();

    QStringList swordDirs;
    for (auto dir: swordDirSet)
        swordDirs.append(dir);
    return swordDirs;
}

void CSwordBackend::deleteOrphanedIndices() {
    const QStringList entries = QDir(CSwordModuleInfo::getGlobalBaseIndexLocation()).entryList(QDir::Dirs);
    for (auto const & entry : entries) {
        if (entry == '.' || entry == QStringLiteral(".."))
            continue;
        if (CSwordModuleInfo * const module = findModuleByName(entry)) {
            if (!module->hasIndex()) { //index files found, but wrong version etc.
                qDebug() << "deleting outdated index for module" << entry;
                CSwordModuleInfo::deleteIndexForModule(entry);
            }
        } else { //no module exists
            if (btConfig().value<bool>(
                    QStringLiteral(
                        "settings/behaviour/autoDeleteOrphanedIndices"), true))
            {
                qDebug() << "deleting orphaned index in directory" << entry;
                CSwordModuleInfo::deleteIndexForModule(entry);
            }
        }
    }
}
