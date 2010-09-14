/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cswordbackend.h"

#include "backend/rendering/centrydisplay.h"
#include "backend/rendering/cbookdisplay.h"
#include "backend/rendering/cchapterdisplay.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordbookmoduleinfo.h"
#include "backend/filters/bt_thmlhtml.h"
#include "backend/filters/bt_thmlplain.h"
#include "backend/filters/bt_osishtml.h"
#include "backend/filters/bt_gbfhtml.h"
#include "backend/filters/bt_plainhtml.h"
#include "backend/filters/osismorphsegmentation.h"

#include "backend/config/cbtconfig.h"

#include "util/directoryutil.h"

#include <dirent.h>

//Qt
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QDebug>

//Sword
#include <swdisp.h>
#include <swfiltermgr.h>
#include <encfiltmgr.h>
#include <rtfhtml.h>
#include <filemgr.h>
#include <utilstr.h>
#include <swfilter.h>

using namespace Filters;
using namespace Rendering;

CSwordBackend::CSwordBackend()
	: sword::SWMgr(0, 0, false, new sword::EncodingFilterMgr( sword::ENC_UTF8 ), true)
{
	m_filters.gbf = new BT_GBFHTML();
	m_filters.plain = new BT_PLAINHTML();
	m_filters.thml = new BT_ThMLHTML();
	m_filters.osis = new BT_OSISHTML();

	m_displays.entry = new CEntryDisplay();
	m_displays.chapter = new CChapterDisplay();
	m_displays.book = new CBookDisplay();

	filterInit();
}

CSwordBackend::CSwordBackend(const QString& path, const bool augmentHome)
	: sword::SWMgr(!path.isEmpty() ? path.toLocal8Bit().constData() : 0, false, new sword::EncodingFilterMgr( sword::ENC_UTF8 ), false, augmentHome) // don't allow module renaming, because we load from a path
{
	m_filters.gbf = new BT_GBFHTML();
	m_filters.plain = new BT_PLAINHTML();
	m_filters.thml = new BT_ThMLHTML();
	m_filters.osis = new BT_OSISHTML();

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

	delete m_displays.book;
	delete m_displays.chapter;
	delete m_displays.entry;
}

void CSwordBackend::filterInit() {
	//HACK: replace Sword's OSISMorphSegmentation filter, seems to be buggy, ours works
	if (sword::SWOptionFilter* filter = optionFilters["OSISMorphSegmentation"])
	{
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

QList<CSwordModuleInfo*> CSwordBackend::takeModulesFromList(QStringList names)
{
	int numberOfRemoved = 0;
	QList<CSwordModuleInfo*> list;
	foreach(QString name, names) {
		CSwordModuleInfo* mInfo = findModuleByName(name);
		if (mInfo) {
			m_moduleList.removeAll(mInfo);
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
	m_moduleList.clear();

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
				m_moduleList.append( newModule );
			}
			else
			{
				delete newModule;
			}
		}
	}

	QList<CSwordModuleInfo*>::iterator end_it = m_moduleList.end();

	foreach (CSwordModuleInfo* mod, m_moduleList) {
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

	if (sourceformat == "GBF") {
		module->AddRenderFilter(m_filters.gbf);
		noDriver = false;
	}
	else if (sourceformat == "PLAIN") {
		module->AddRenderFilter(m_filters.plain);
		noDriver = false;
	}
	else if (sourceformat == "ThML") {
		module->AddRenderFilter(m_filters.thml);
		noDriver = false;
	}
	else if (sourceformat == "OSIS") {
		module->AddRenderFilter(m_filters.osis);
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
	QList<CSwordModuleInfo*>::iterator it = m_moduleList.begin();
	QList<CSwordModuleInfo*>::iterator end = m_moduleList.end();

	while (it != end) {
		CSwordModuleInfo* current = (*it);
		it = m_moduleList.erase(it);
		delete current;
	}

	Q_ASSERT(m_moduleList.count() == 0);
	//BT  mods are deleted now, delete Sword mods, too.
	DeleteMods();

	/* Cipher filters must be handled specially, because SWMgr creates them,
	 * stores them in cipherFilters and cleanupFilters and attaches them to locked
	 * modules. If these modules are removed, the filters need to be removed as well,
	 * so that they are re-created for the new module objects.
	 */
	sword::FilterMap::iterator cipher_it;
	for (cipher_it = cipherFilters.begin(); cipher_it != cipherFilters.end(); cipher_it++)
	{
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
			value = state ? "On": "Off";
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
	foreach(CSwordModuleInfo* mod, m_moduleList) {
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
	foreach(CSwordModuleInfo* mod, m_moduleList) {
		if (mod->name() == name) return mod;
	}
	return 0;
}

CSwordModuleInfo* CSwordBackend::findSwordModuleByPointer(const sword::SWModule* const swmodule) {
	foreach(CSwordModuleInfo* mod, m_moduleList) {
		if (mod->module() == swmodule ) return mod;
	}
	return 0;
}

CSwordModuleInfo* CSwordBackend::findModuleByPointer(const CSwordModuleInfo* const module) {
	foreach(CSwordModuleInfo* mod, m_moduleList) {
		if (mod  == module) return mod;
	}
	return 0;
}

/** Returns our local config object to store the cipher keys etc. locally for each user. The values of the config are merged with the global config. */
bool CSwordBackend::moduleConfig(const QString& module, sword::SWConfig& moduleConfig) {
	sword::SectionMap::iterator section;
	DIR *dir = opendir(configPath);

	struct dirent *ent;

	bool foundConfig = false;
	QString modFile;

	if (dir) {    // find and update .conf file
		rewinddir(dir);

		while ((ent = readdir(dir)) && !foundConfig) {
			if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
				modFile = QString(configPath);
					modFile.append("/");
					modFile.append( QString::fromLocal8Bit(ent->d_name) );

				moduleConfig = sword::SWConfig( modFile.toLocal8Bit().constData() );
				section = moduleConfig.Sections.find( module.toLocal8Bit().constData() );
				foundConfig = ( section != moduleConfig.Sections.end() );
			}
		}

		closedir(dir);
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

		QString myPath = util::filesystem::DirectoryUtil::getUserHomeDir().absolutePath();
		myPath.append("/.sword/mods.d");
		dir = opendir(myPath.toUtf8().constData());

		if (dir) {
			rewinddir(dir);

			while ((ent = readdir(dir)) && !foundConfig) {
				if ((strcmp(ent->d_name, ".")) && (strcmp(ent->d_name, ".."))) {
					modFile = myPath;
					modFile.append('/');
					modFile.append(ent->d_name);
					moduleConfig = sword::SWConfig( modFile.toLocal8Bit().constData() );
					section = moduleConfig.Sections.find( module.toLocal8Bit().constData() );
					foundConfig = ( section != moduleConfig.Sections.end() );
				}
			}

			closedir(dir);
		}
	}

	return foundConfig;
}

/** Returns the text used for the option given as parameter. */
const QString CSwordBackend::optionName( const CSwordModuleInfo::FilterTypes option ) {
	switch (option) {
		case CSwordModuleInfo::footnotes:			return QString("Footnotes");
		case CSwordModuleInfo::strongNumbers:		return QString("Strong's Numbers");
		case CSwordModuleInfo::headings:			return QString("Headings");
		case CSwordModuleInfo::morphTags:			return QString("Morphological Tags");
		case CSwordModuleInfo::lemmas:				return QString("Lemmas");
		case CSwordModuleInfo::hebrewPoints:		return QString("Hebrew Vowel Points");
		case CSwordModuleInfo::hebrewCantillation:	return QString("Hebrew Cantillation");
		case CSwordModuleInfo::greekAccents:		return QString("Greek Accents");
		case CSwordModuleInfo::redLetterWords:		return QString("Words of Christ in Red");
		case CSwordModuleInfo::textualVariants:		return QString("Textual Variants");
		case CSwordModuleInfo::scriptureReferences:	return QString("Cross-references");
		case CSwordModuleInfo::morphSegmentation:	return QString("Morph Segmentation");
	}
	return QString::null;
}

/** Returns the translated name of the option given as parameter. */
const QString CSwordBackend::translatedOptionName(const CSwordModuleInfo::FilterTypes option) {
	switch (option) {
		case CSwordModuleInfo::footnotes:					return QObject::tr("Footnotes");
		case CSwordModuleInfo::strongNumbers:				return QObject::tr("Strong's numbers");
		case CSwordModuleInfo::headings:					return QObject::tr("Headings");
		case CSwordModuleInfo::morphTags:					return QObject::tr("Morphological tags");
		case CSwordModuleInfo::lemmas:						return QObject::tr("Lemmas");
		case CSwordModuleInfo::hebrewPoints:				return QObject::tr("Hebrew vowel points");
		case CSwordModuleInfo::hebrewCantillation:			return QObject::tr("Hebrew cantillation marks");
		case CSwordModuleInfo::greekAccents:				return QObject::tr("Greek accents");
		case CSwordModuleInfo::redLetterWords:				return QObject::tr("Red letter words");
		case CSwordModuleInfo::textualVariants:				return QObject::tr("Textual variants");
		case CSwordModuleInfo::scriptureReferences:			return QObject::tr("Scripture cross-references");
		case CSwordModuleInfo::morphSegmentation:			return QObject::tr("Morph segmentation");
	}
	return QString::null;
}


const QString CSwordBackend::configOptionName( const CSwordModuleInfo::FilterTypes option ) {
	switch (option) {
		case CSwordModuleInfo::footnotes:				return QString("Footnotes");
		case CSwordModuleInfo::strongNumbers:			return QString("Strongs");
		case CSwordModuleInfo::headings:				return QString("Headings");
		case CSwordModuleInfo::morphTags:				return QString("Morph");
		case CSwordModuleInfo::lemmas:					return QString("Lemma");
		case CSwordModuleInfo::hebrewPoints:			return QString("HebrewPoints");
		case CSwordModuleInfo::hebrewCantillation:		return QString("Cantillation");
		case CSwordModuleInfo::greekAccents:			return QString("GreekAccents");
		case CSwordModuleInfo::redLetterWords:			return QString("RedLetterWords");
		case CSwordModuleInfo::textualVariants:			return QString("Variants");
		case CSwordModuleInfo::scriptureReferences:		return QString("Scripref");
		case CSwordModuleInfo::morphSegmentation:		return QString("MorphSegmentation");
	}
	return QString::null;
}

const QString CSwordBackend::booknameLanguage( const QString& language ) {
	if (!language.isEmpty()) {
		sword::LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName( language.toUtf8().constData() );

		//refresh the locale of all Bible and commentary modules!
		//use what sword returns, language may be different
		QString newLocaleName( sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName()  );

		foreach(CSwordModuleInfo* mod, m_moduleList) {
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
#ifdef SWORD_SYSCONF_CHANGED
		findConfig(&configType, &prefixPath, &configPath, &augPaths, &sysConfig);
#else
		findConfig(&configType, &prefixPath, &configPath, &augPaths, sysconfig);
#endif
		// now re-read module configuration files
		loadConfigDir(configPath);
	}
	else if (config) {
		config->Load();
	}

	initModules(reason);
}

const QStringList CSwordBackend::swordDirList() {
	QSet<QString> ret;
	const QString home = util::filesystem::DirectoryUtil::getUserHomeDir().absolutePath();

	//return a list of used Sword dirs. Useful for the installer
	QString configPath = QString("%1/.sword/sword.conf").arg(home);

	if (!QFile(configPath).exists()) {
		configPath = globalConfPath; //e.g. /etc/sword.conf, /usr/local/etc/sword.conf
	}

	QStringList configs = configPath.split(":");

	for (QStringList::const_iterator it = configs.begin(); it != configs.end(); ++it) {
		if (!QFileInfo(*it).exists()) {
			continue;
		}

		//get all DataPath and AugmentPath entries from the config file and add them to the list
		sword::SWConfig conf( (*it).toUtf8().constData() );
		ret << conf["Install"]["DataPath"].c_str();
		sword::ConfigEntMap group = conf["Install"];
		sword::ConfigEntMap::iterator start = group.equal_range("AugmentPath").first;
		sword::ConfigEntMap::iterator end = group.equal_range("AugmentPath").second;

		for (sword::ConfigEntMap::const_iterator it = start; it != end; ++it) {
			ret << QDir(it->second.c_str()).absolutePath(); //added augment path
		}
	}

	if (!home.isEmpty()) {
		// This is added to the set if not there already. Notice that
		// this prevents duplication only if the QDir::absolutePath() returns
		// string without the prepended "/".
		ret << home + "/.sword";
	}

	return ret.values();
}

void CSwordBackend::notifyChange(SetupChangedReason reason)
{
	emit sigSwordSetupChanged(reason);
}
