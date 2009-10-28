/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
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
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/filters/bt_gbfhtml.h"
#include "backend/filters/bt_osishtml.h"
#include "backend/filters/bt_teihtml.h"
#include "backend/filters/bt_plainhtml.h"
#include "backend/filters/bt_thmlhtml.h"
#include "backend/filters/bt_thmlplain.h"
#include "backend/filters/osismorphsegmentation.h"
#include "backend/rendering/cbookdisplay.h"
#include "backend/rendering/cchapterdisplay.h"
#include "backend/rendering/centrydisplay.h"
#include "util/directory.h"

// Sword includes:
#include <encfiltmgr.h>
#include <filemgr.h>
#include <rtfhtml.h>
#include <swdisp.h>
#include <swfiltermgr.h>
#include <swfilter.h>
#include <utilstr.h>


using namespace Filters;
using namespace Rendering;

CSwordBackend::CSwordBackend()
        : sword::SWMgr(0, 0, false, new sword::EncodingFilterMgr( sword::ENC_UTF8 ), true),
          m_dataModel(this)
{
    m_filters.gbf = new BT_GBFHTML();
    m_filters.plain = new BT_PLAINHTML();
    m_filters.thml = new BT_ThMLHTML();
    m_filters.osis = new BT_OSISHTML();
    m_filters.tei = new BT_TEIHTML();

    m_displays.entry = new CEntryDisplay();
    m_displays.chapter = new CChapterDisplay();
    m_displays.book = new CBookDisplay();

    filterInit();
}

CSwordBackend::CSwordBackend(const QString& path, const bool augmentHome)
        : sword::SWMgr(!path.isEmpty() ? path.toLocal8Bit().constData() : 0, false, new sword::EncodingFilterMgr( sword::ENC_UTF8 ), false, augmentHome) { // don't allow module renaming, because we load from a path
    m_filters.gbf = new BT_GBFHTML();
    m_filters.plain = new BT_PLAINHTML();
    m_filters.thml = new BT_ThMLHTML();
    m_filters.osis = new BT_OSISHTML();
    m_filters.tei = new BT_TEIHTML();

    m_displays.entry = new CEntryDisplay();
    m_displays.chapter = new CChapterDisplay();
    m_displays.book = new CBookDisplay();

    filterInit();
}

CSwordBackend::~CSwordBackend() {
    shutdownModules();

    delete m_filters.gbf;
    delete m_filters.plain;
    delete m_filters.thml;
    delete m_filters.osis;
    delete m_filters.tei;

    delete m_displays.book;
    delete m_displays.chapter;
    delete m_displays.entry;
}

void CSwordBackend::filterInit() {
    //HACK: replace Sword's OSISMorphSegmentation filter, seems to be buggy, ours works
    if (sword::SWOptionFilter* filter = optionFilters["OSISMorphSegmentation"]) {
        cleanupFilters.remove(filter);
        optionFilters.erase("OSISMorphSegmentation");
        delete filter;
    }
    sword::SWOptionFilter* tmpFilter = new OSISMorphSegmentation();
    optionFilters.insert(sword::OptionFilterMap::value_type("OSISMorphSegmentation", tmpFilter));
    cleanupFilters.push_back(tmpFilter);

    //HACK: replace Sword's ThML strip filter with our own version
    //remove this hack as soon as Sword is fixed
    cleanupFilters.remove(thmlplain);
    delete thmlplain;
    thmlplain = new BT_ThMLPlain();
    cleanupFilters.push_back(thmlplain);
}

QList<CSwordModuleInfo*> CSwordBackend::takeModulesFromList(QStringList names) {
    int numberOfRemoved = 0;
    QList<CSwordModuleInfo*> list;
    foreach(QString name, names) {
        CSwordModuleInfo* mInfo = findModuleByName(name);
        if (mInfo) {
            m_dataModel.removeModule(mInfo);
            ++numberOfRemoved;
            list.append(mInfo);
        }
    }
    if (numberOfRemoved > 0)
        emit sigSwordSetupChanged(RemovedModules);
    return list;
}

/** Initializes the Sword modules. */
CSwordBackend::LoadError CSwordBackend::initModules(SetupChangedReason reason) {
    //  qWarning("globalSwordConfigPath is %s", globalConfPath);
    LoadError ret = NoError;

    shutdownModules(); //remove previous modules
    m_dataModel.clear();

    sword::ModMap::iterator end = Modules.end();
    ret = LoadError( Load() );

    for (sword::ModMap::iterator it = Modules.begin(); it != end; it++) {
        sword::SWModule* const curMod = (*it).second;
        CSwordModuleInfo* newModule = 0;

        if (!strcmp(curMod->Type(), "Biblical Texts")) {
            newModule = new CSwordBibleModuleInfo(curMod, this);
            newModule->module()->Disp(m_displays.chapter);
        }
        else if (!strcmp(curMod->Type(), "Commentaries")) {
            newModule = new CSwordCommentaryModuleInfo(curMod, this);
            newModule->module()->Disp(m_displays.entry);
        }
        else if (!strcmp(curMod->Type(), "Lexicons / Dictionaries")) {
            newModule = new CSwordLexiconModuleInfo(curMod, this);
            newModule->module()->Disp(m_displays.entry);
        }
        else if (!strcmp(curMod->Type(), "Generic Books")) {
            newModule = new CSwordBookModuleInfo(curMod, this);
            newModule->module()->Disp(m_displays.book);
        }

        if (newModule) {
            //Append the new modules to our list, but only if it's supported
            //The constructor of CSwordModuleInfo prints a warning on stdout
            if (!newModule->hasVersion() || (newModule->minimumSwordVersion() <= sword::SWVersion::currentVersion)) {
                m_dataModel.addModule(newModule);
            }
            else {
                delete newModule;
            }
        }
    }

    Q_FOREACH(CSwordModuleInfo* mod, m_dataModel.modules()) {
        m_moduleDescriptionMap.insert( mod->config(CSwordModuleInfo::Description), mod->name() );
        //unlock modules if keys are present
        if ( mod->isEncrypted() ) {
            const QString unlockKey = CBTConfig::getModuleEncryptionKey( mod->name() );
            if (!unlockKey.isNull()) {
                setCipherKey( mod->name().toUtf8().constData(), unlockKey.toUtf8().constData() );
            }
        }
    }

    emit sigSwordSetupChanged(reason);
    return ret;
}

void CSwordBackend::AddRenderFilters(sword::SWModule *module, sword::ConfigEntMap &section) {
    sword::SWBuf moduleDriver;
    sword::SWBuf sourceformat;
    sword::ConfigEntMap::iterator entry;
    bool noDriver = true;

    sourceformat = ((entry = section.find("SourceType")) != section.end()) ? (*entry).second : (sword::SWBuf) "";
    moduleDriver = ((entry = section.find("ModDrv")) != section.end()) ? (*entry).second : (sword::SWBuf) "";

    if (sourceformat == "OSIS") {
        module->AddRenderFilter(m_filters.osis);
        noDriver = false;
    }
    else if (sourceformat == "ThML") {
        module->AddRenderFilter(m_filters.thml);
        noDriver = false;
    }
    else if (sourceformat == "TEI") {
        module->AddRenderFilter(m_filters.tei);
        noDriver = false;
    }
    else if (sourceformat == "GBF") {
        module->AddRenderFilter(m_filters.gbf);
        noDriver = false;
    }
    else if (sourceformat == "PLAIN") {
        module->AddRenderFilter(m_filters.plain);
        noDriver = false;
    }

    if (noDriver) { //no driver found
        if ( (moduleDriver == "RawCom") || (moduleDriver == "RawLD") ) {
            module->AddRenderFilter(m_filters.plain);
            noDriver = false;
        }
    }
}

/** This function deinitializes the modules and deletes them. */
bool CSwordBackend::shutdownModules() {
    m_dataModel.clear(true);
    //BT  mods are deleted now, delete Sword mods, too.
    DeleteMods();

    /* Cipher filters must be handled specially, because SWMgr creates them,
     * stores them in cipherFilters and cleanupFilters and attaches them to locked
     * modules. If these modules are removed, the filters need to be removed as well,
     * so that they are re-created for the new module objects.
     */
    sword::FilterMap::iterator cipher_it;
    for (cipher_it = cipherFilters.begin(); cipher_it != cipherFilters.end(); cipher_it++) {
        //Delete the Filter and remove it from the cleanup list
        cleanupFilters.remove(cipher_it->second);
        delete cipher_it->second;
    }
    cipherFilters.clear();

    return true;
}

void CSwordBackend::setOption( const CSwordModuleInfo::FilterTypes type, const int state ) {
    sword::SWBuf value;

    switch (type) {

        case CSwordModuleInfo::textualVariants:

            if (state == 0) {
                value = "Primary Reading";
            }
            else if (state == 1) {
                value = "Secondary Reading";
            }
            else {
                value = "All Readings";
            }

            break;

        default:
            value = state ? "On" : "Off";
            break;
    };

    if (value.length())
        setGlobalOption(optionName(type).toUtf8().constData(), value.c_str());
}

void CSwordBackend::setFilterOptions( const CSwordBackend::FilterOptions options) {
    setOption( CSwordModuleInfo::footnotes,      options.footnotes );
    setOption( CSwordModuleInfo::strongNumbers,    options.strongNumbers );
    setOption( CSwordModuleInfo::headings,       options.headings );
    setOption( CSwordModuleInfo::morphTags,      options.morphTags );
    setOption( CSwordModuleInfo::lemmas,        options.lemmas );
    setOption( CSwordModuleInfo::hebrewPoints,     options.hebrewPoints );
    setOption( CSwordModuleInfo::hebrewCantillation,  options.hebrewCantillation );
    setOption( CSwordModuleInfo::greekAccents,     options.greekAccents );
    setOption( CSwordModuleInfo::redLetterWords,   options.redLetterWords );
    setOption( CSwordModuleInfo::textualVariants,   options.textualVariants );
    setOption( CSwordModuleInfo::morphSegmentation,  options.morphSegmentation );
    //  setOption( CSwordModuleInfo::transliteration,   options.transliteration );
    setOption( CSwordModuleInfo::scriptureReferences, options.scriptureReferences);
}

/** This function searches for a module with the specified description */
CSwordModuleInfo* CSwordBackend::findModuleByDescription(const QString& description) {
    Q_FOREACH (CSwordModuleInfo *mod, m_dataModel.modules()) {
        if (mod->config(CSwordModuleInfo::Description) == description) return mod;
    }
    return 0;
}

/** This function searches for a module with the specified description */
const QString CSwordBackend::findModuleNameByDescription(const QString& description) {
    if (m_moduleDescriptionMap.contains(description)) {
        return m_moduleDescriptionMap[description];
    }
    return QString::null;
}

/** This function searches for a module with the specified name */
CSwordModuleInfo* CSwordBackend::findModuleByName(const QString& name) {
    Q_FOREACH (CSwordModuleInfo *mod, m_dataModel.modules()) {
        if (mod->name() == name) return mod;
    }
    return 0;
}

CSwordModuleInfo* CSwordBackend::findSwordModuleByPointer(const sword::SWModule* const swmodule) {
    Q_FOREACH (CSwordModuleInfo *mod, m_dataModel.modules()) {
        if (mod->module() == swmodule ) return mod;
    }
    return 0;
}

CSwordModuleInfo* CSwordBackend::findModuleByPointer(const CSwordModuleInfo* const module) {
    Q_FOREACH (CSwordModuleInfo *mod, m_dataModel.modules()) {
        if (mod  == module) return mod;
    }
    return 0;
}

/** Returns our local config object to store the cipher keys etc. locally for each user. The values of the config are merged with the global config. */
bool CSwordBackend::moduleConfig(const QString& module, sword::SWConfig& moduleConfig) {

    sword::SectionMap::iterator section;
    QDir dir(QString::fromUtf8(configPath));
    bool foundConfig = false;

    QFileInfoList list = dir.entryInfoList();
    if (dir.isReadable()) {
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);

            moduleConfig = sword::SWConfig( fileInfo.absoluteFilePath().toLocal8Bit().constData() );
            section = moduleConfig.Sections.find( module.toLocal8Bit().constData() );
            foundConfig = ( section != moduleConfig.Sections.end() );
        }
    }
    else { //try to read mods.conf
        moduleConfig = sword::SWConfig("");//global config
        section = config->Sections.find( module.toLocal8Bit().constData() );
        foundConfig = ( section != config->Sections.end() );

        sword::ConfigEntMap::iterator entry;

        if (foundConfig) { //copy module section

            for (entry = section->second.begin(); entry != section->second.end(); entry++) {
                moduleConfig.Sections[section->first].insert(sword::ConfigEntMap::value_type(entry->first, entry->second));
            }
        }
    }

    if (!foundConfig && configType != 2) { //search in $HOME/.sword/

        QString myPath = util::directory::getUserHomeSwordModsDir().absolutePath();
        dir.setPath(myPath);

        QFileInfoList list = dir.entryInfoList();
        if (dir.isReadable()) {
            for (int i = 0; i < list.size(); ++i) {
                QFileInfo fileInfo = list.at(i);
                moduleConfig = sword::SWConfig( fileInfo.absoluteFilePath().toLocal8Bit().constData() );
                section = moduleConfig.Sections.find( module.toLocal8Bit().constData() );
                foundConfig = ( section != moduleConfig.Sections.end() );
            }
        }
    }

    return foundConfig;
}

/** Returns the text used for the option given as parameter. */
const QString CSwordBackend::optionName( const CSwordModuleInfo::FilterTypes option ) {
    switch (option) {
        case CSwordModuleInfo::footnotes:
            return QString("Footnotes");
        case CSwordModuleInfo::strongNumbers:
            return QString("Strong's Numbers");
        case CSwordModuleInfo::headings:
            return QString("Headings");
        case CSwordModuleInfo::morphTags:
            return QString("Morphological Tags");
        case CSwordModuleInfo::lemmas:
            return QString("Lemmas");
        case CSwordModuleInfo::hebrewPoints:
            return QString("Hebrew Vowel Points");
        case CSwordModuleInfo::hebrewCantillation:
            return QString("Hebrew Cantillation");
        case CSwordModuleInfo::greekAccents:
            return QString("Greek Accents");
        case CSwordModuleInfo::redLetterWords:
            return QString("Words of Christ in Red");
        case CSwordModuleInfo::textualVariants:
            return QString("Textual Variants");
        case CSwordModuleInfo::scriptureReferences:
            return QString("Cross-references");
        case CSwordModuleInfo::morphSegmentation:
            return QString("Morph Segmentation");
    }
    return QString::null;
}

/** Returns the translated name of the option given as parameter. */
const QString CSwordBackend::translatedOptionName(const CSwordModuleInfo::FilterTypes option) {
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


const QString CSwordBackend::configOptionName( const CSwordModuleInfo::FilterTypes option ) {
    switch (option) {
        case CSwordModuleInfo::footnotes:
            return QString("Footnotes");
        case CSwordModuleInfo::strongNumbers:
            return QString("Strongs");
        case CSwordModuleInfo::headings:
            return QString("Headings");
        case CSwordModuleInfo::morphTags:
            return QString("Morph");
        case CSwordModuleInfo::lemmas:
            return QString("Lemma");
        case CSwordModuleInfo::hebrewPoints:
            return QString("HebrewPoints");
        case CSwordModuleInfo::hebrewCantillation:
            return QString("Cantillation");
        case CSwordModuleInfo::greekAccents:
            return QString("GreekAccents");
        case CSwordModuleInfo::redLetterWords:
            return QString("RedLetterWords");
        case CSwordModuleInfo::textualVariants:
            return QString("Variants");
        case CSwordModuleInfo::scriptureReferences:
            return QString("Scripref");
        case CSwordModuleInfo::morphSegmentation:
            return QString("MorphSegmentation");
    }
    return QString::null;
}

const QString CSwordBackend::booknameLanguage( const QString& language ) {
    if (!language.isEmpty()) {
        sword::LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName( language.toUtf8().constData() );

        //refresh the locale of all Bible and commentary modules!
        //use what sword returns, language may be different
        QString newLocaleName( sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName()  );

        Q_FOREACH (CSwordModuleInfo *mod, m_dataModel.modules()) {
            if ( (mod->type() == CSwordModuleInfo::Bible) || (mod->type() == CSwordModuleInfo::Commentary) ) {
                //Create a new key, it will get the default bookname language
                ((sword::VerseKey*)(mod->module()->getKey()))->setLocale( newLocaleName.toUtf8().constData() );
            }
        }

    }
    return QString( sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName() );
}


/** Reload all Sword modules. */
void CSwordBackend::reloadModules(SetupChangedReason reason) {
    shutdownModules();

    //delete Sword's config to make Sword reload it!

    if (myconfig) { // force reload on config object because we may have changed the paths
        delete myconfig;
        config = myconfig = 0;
        // we need to call findConfig to make sure that augPaths are reloaded
        findConfig(&configType, &prefixPath, &configPath, &augPaths, &sysConfig);
        // now re-read module configuration files
        loadConfigDir(configPath);
    }
    else if (config) {
        config->Load();
    }

    initModules(reason);
}

// Get one or more shared sword config (sword.conf) files
QStringList CSwordBackend::getSharedSwordConfigFiles()
{
	QStringList configPath;
#ifdef Q_WS_WIN
	//  %ALLUSERSPROFILE%\Sword\sword.conf
	QString tmp = util::directory::getSharedSwordDir().filePath("sword.conf");
	QString globalPath = util::directory::convertDirSeparators(QString(getenv("SWORD_PATH")));
	configPath << globalPath.append("/Sword/sword.conf");
#else
	// /etc/sword.conf, /usr/local/etc/sword.conf
	QString globalPaths(globalConfPath); // from libsword
	configPath = globalPaths.split(":");
#endif
	return configPath;
}

// Get the private sword directory
QString CSwordBackend::getPrivateSwordConfigPath()
{
    const QString homeSwordPath = util::directory::getUserHomeSwordDir().absolutePath();
	return homeSwordPath;
}

QString CSwordBackend::getPrivateSwordConfigFile()
{
	QString file = getPrivateSwordConfigPath() + "/sword.conf";
	return util::directory::convertDirSeparators(file);
}

    
//return a list of used Sword dirs. Useful for the installer
const QStringList CSwordBackend::swordDirList() {
    namespace DU = util::directory;

	// Get the set of sword directories that could contain modules
    QSet<QString> swordDirSet;

	QStringList configs;
    if ( QFile( getPrivateSwordConfigFile() ).exists() ) {
		// Use the private sword.conf file
		configs << getPrivateSwordConfigFile();
	} else {
		// Did not find private sword.conf, will use shared sword.conf files 
		// to build the private one. Once the private sword.conf exist, the
		// shared ones will not be searched again.
        configs = getSharedSwordConfigFiles();

		// On Windows, add the shared sword directory to the set so the new
		// private sword.conf will have it. The user could decide to delete
		// this shared path and it will not automatically come back.
#ifdef Q_WS_WIN
		swordDirSet << DU::convertDirSeparators(QString(getenv("SWORD_PATH")));
#endif
    }

	// Search the sword.conf file(s) for sword directories that could contain modules
    for (QStringList::const_iterator it = configs.begin(); it != configs.end(); ++it) {
		QString path = *it;
        if (!QFileInfo(*it).exists()) {
            continue;
        }

        //get all DataPath and AugmentPath entries from the config file and add them to the list
        sword::SWConfig conf( (*it).toUtf8().constData() );
        swordDirSet << QDir((QTextCodec::codecForLocale()->toUnicode(conf["Install"]["DataPath"].c_str()))).absolutePath();
        sword::ConfigEntMap group = conf["Install"];
        sword::ConfigEntMap::iterator start = group.equal_range("AugmentPath").first;
        sword::ConfigEntMap::iterator end = group.equal_range("AugmentPath").second;

        for (sword::ConfigEntMap::const_iterator it = start; it != end; ++it) {
            QDir((QTextCodec::codecForLocale()->toUnicode(it->second.c_str()))).absolutePath();
            swordDirSet << QDir((QTextCodec::codecForLocale()->toUnicode(it->second.c_str()))).absolutePath(); //added augment path
        }
    }

	// Add the private sword path to the set if not there already. 
		swordDirSet << getPrivateSwordConfigPath();

    return swordDirSet.values();
}

void CSwordBackend::notifyChange(SetupChangedReason reason) {
    emit sigSwordSetupChanged(reason);
}
