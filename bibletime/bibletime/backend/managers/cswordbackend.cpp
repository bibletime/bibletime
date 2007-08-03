/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
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

#include "frontend/cbtconfig.h"

#include <dirent.h>

//Qt
#include <QDir>
#include <QFileInfo>

//KDE
#include <klocale.h>
#include <kstringhandler.h>

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
: sword::SWMgr(0, 0, false, new sword::EncodingFilterMgr( sword::ENC_UTF8 ), true) {
	m_displays.entry = 0;
	m_displays.chapter = 0;
	m_displays.book = 0;

	m_filters.gbf = 0;
	m_filters.thml = 0;
	m_filters.osis = 0;
	m_filters.plain = 0;

	filterInit();
}

CSwordBackend::CSwordBackend(const QString& path, const bool augmentHome)
: sword::SWMgr(!path.isEmpty() ? path.toLocal8Bit().constData() : 0, false, new sword::EncodingFilterMgr( sword::ENC_UTF8 ), false, augmentHome) // don't allow module renaming, because we load from a path
{
	//qDebug("CSwordBackend::CSwordBackend for %s, using %s", path.latin1(), configPath);
	m_displays.entry = 0;
	m_displays.chapter = 0;
	m_displays.book = 0;

	m_filters.gbf = 0;
	m_filters.thml = 0;
	m_filters.osis = 0;
	m_filters.plain = 0;

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

/** Initializes the Sword modules. */
const CSwordBackend::LoadError CSwordBackend::initModules() {
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
			newModule->module()->Disp(
				m_displays.chapter
				? m_displays.chapter
				: (m_displays.chapter = new CChapterDisplay)
			);
		}
		else if (!strcmp(curMod->Type(), "Commentaries")) {
			newModule = new CSwordCommentaryModuleInfo(curMod, this);
			newModule->module()->Disp(
				m_displays.entry
				? m_displays.entry
				: (m_displays.entry = new CEntryDisplay)
			);
		}
		else if (!strcmp(curMod->Type(), "Lexicons / Dictionaries")) {
			newModule = new CSwordLexiconModuleInfo(curMod, this);
			newModule->module()->Disp(
				m_displays.entry
				? m_displays.entry
				: (m_displays.entry = new CEntryDisplay)
			);
		}
		else if (!strcmp(curMod->Type(), "Generic Books")) {
			newModule = new CSwordBookModuleInfo(curMod, this);
			newModule->module()->Disp(
				m_displays.book
				? m_displays.book
				: (m_displays.book = new CBookDisplay)
			);
		}

		if (newModule) { 
			//append the new modules to our list, but only if it's supported
			//the constructor of CSwordModuleInfo prints a waring on stdout
			if (!newModule->hasVersion() || (newModule->minimumSwordVersion() <= sword::SWVersion::currentVersion)) {
				m_moduleList.append( newModule );	
			}
		}
	}

	ListCSwordModuleInfo::iterator end_it = m_moduleList.end();

	for (ListCSwordModuleInfo::iterator it = m_moduleList.begin() ; it != end_it; ++it) {
		m_moduleDescriptionMap.insert( (*it)->config(CSwordModuleInfo::Description), (*it)->name() );
	}

	//unlock modules if keys are present
	for (ListCSwordModuleInfo::iterator it = m_moduleList.begin() ; it != end_it; ++it) {
		if ( (*it)->isEncrypted() ) {
			const QString unlockKey = CBTConfig::getModuleEncryptionKey( (*it)->name() );

			if (!unlockKey.isNull()) {
				setCipherKey( (*it)->name().toUtf8().constData(), unlockKey.toUtf8().constData() );
			}
		}
	}

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
		if (!m_filters.gbf) {
			m_filters.gbf = new BT_GBFHTML();
		}
		module->AddRenderFilter(m_filters.gbf);
		noDriver = false;
	}
	else if (sourceformat == "PLAIN") {
		if (!m_filters.plain) {
			m_filters.plain = new BT_PLAINHTML();
		}
		module->AddRenderFilter(m_filters.plain);
		noDriver = false;
	}
	else if (sourceformat == "ThML") {
		if (!m_filters.thml) {
			m_filters.thml = new BT_ThMLHTML();
		}
		module->AddRenderFilter(m_filters.thml);
		noDriver = false;
	}
	else if (sourceformat == "OSIS") {
		if (!m_filters.osis) {
			m_filters.osis = new BT_OSISHTML();
		}

		module->AddRenderFilter(m_filters.osis);
		noDriver = false;
	}

	if (noDriver) { //no driver found
		if ( (moduleDriver == "RawCom") || (moduleDriver == "RawLD") ) {
			if (!m_filters.plain) {
				m_filters.plain = new BT_PLAINHTML();
			}
			module->AddRenderFilter(m_filters.plain);
			noDriver = false;
		}
	}
}

/** This function deinitializes the modules and deletes them. */
const bool CSwordBackend::shutdownModules() {
	ListCSwordModuleInfo::iterator it = m_moduleList.begin();
	ListCSwordModuleInfo::iterator end = m_moduleList.end();

	while (it != end) {
		CSwordModuleInfo* current = (*it);
		it = m_moduleList.erase(it);
		delete current;
	}

	Q_ASSERT(m_moduleList.count() == 0);
	//BT  mods are deleted now, delete Sword mods, too.
	DeleteMods();

	return true;
}

/** Returns true if the given option is enabled. */
const bool CSwordBackend::isOptionEnabled( const CSwordModuleInfo::FilterTypes type) {
	return (getGlobalOption( optionName(type).toUtf8().constData() ) == "On");
}

/** Sets the given options enabled or disabled depending on the second parameter. */
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

void CSwordBackend::setDisplayOptions( const CSwordBackend::DisplayOptions ) {
	/*  if (m_displays.entry) {
	  m_displays.entry->setDisplayOptions(options); 
	 }
	  if (m_displays.chapter) {
	  m_displays.chapter->setDisplayOptions(options); 
	 }
	  if (m_displays.book) {
	  m_displays.book->setDisplayOptions(options);
	 }
	 */
}

/** This function searches for a module with the specified description */
CSwordModuleInfo* const CSwordBackend::findModuleByDescription(const QString& description) {
	CSwordModuleInfo* ret = 0;
	ListCSwordModuleInfo::iterator end_it = m_moduleList.end();

	for (ListCSwordModuleInfo::iterator it = m_moduleList.begin() ; it != end_it; ++it) {
		if ( (*it)->config(CSwordModuleInfo::Description) == description ) {
			ret = *it;
			break;
		}
	}

	return ret;
}

/** This function searches for a module with the specified description */
const QString CSwordBackend::findModuleNameByDescription(const QString& description) {
	if (m_moduleDescriptionMap.contains(description)) {
		return m_moduleDescriptionMap[description];
	}

	return QString::null;
}

/** This function searches for a module with the specified name */
CSwordModuleInfo* const CSwordBackend::findModuleByName(const QString& name) {
	CSwordModuleInfo* ret = 0;

	ListCSwordModuleInfo::iterator end_it = m_moduleList.end();

	for (ListCSwordModuleInfo::iterator it = m_moduleList.begin() ; it != end_it; ++it) {
		if ( (*it)->name() == name ) {
			ret = *it;
			break;
		}
	}

	return ret;
}

CSwordModuleInfo* const CSwordBackend::findSwordModuleByPointer(const sword::SWModule* const swmodule) {
	CSwordModuleInfo* ret = 0;
	ListCSwordModuleInfo::iterator end_it = m_moduleList.end();

	for (ListCSwordModuleInfo::iterator it = m_moduleList.begin() ; it != end_it; ++it) {
		if ( (*it)->module() == swmodule ) {
			ret = *it;
			break;
		}
	}
	
	return ret;
}

CSwordModuleInfo* const CSwordBackend::findModuleByPointer(const CSwordModuleInfo* const module) {
	CSwordModuleInfo* ret = 0;

	ListCSwordModuleInfo::iterator end_it = m_moduleList.end();

	for (ListCSwordModuleInfo::iterator it = m_moduleList.begin() ; it != end_it; ++it) {
		if ( (*it)  == module ) {
			ret = *it;
			break;
		}
	}
	
	return ret;
}

/** Returns our local config object to store the cipher keys etc. locally for each user. The values of the config are merged with the global config. */
const bool CSwordBackend::moduleConfig(const QString& module, sword::SWConfig& moduleConfig) {
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

			for (entry = (*section).second.begin(); entry != (*section).second.end(); entry++) {
				moduleConfig.Sections[(*section).first].insert(sword::ConfigEntMap::value_type((*entry).first, (*entry).second));
			}
		}
	}

	if (!foundConfig && configType != 2) { //search in $HOME/.sword/
		QString myPath(getenv("HOME"));
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
		//   case CSwordModuleInfo::transliteration:
		//    return QString("Transliteration");
	}

	return QString::null;
}

/** Returns the translated name of the option given as parameter. */
const QString CSwordBackend::translatedOptionName(const CSwordModuleInfo::FilterTypes option) {
	switch (option) {

		case CSwordModuleInfo::footnotes:
		return i18n("Footnotes");

		case CSwordModuleInfo::strongNumbers:
		return i18n("Strong's numbers");

		case CSwordModuleInfo::headings:
		return i18n("Headings");

		case CSwordModuleInfo::morphTags:
		return i18n("Morphological tags");

		case CSwordModuleInfo::lemmas:
		return i18n("Lemmas");

		case CSwordModuleInfo::hebrewPoints:
		return i18n("Hebrew vowel points");

		case CSwordModuleInfo::hebrewCantillation:
		return i18n("Hebrew cantillation marks");

		case CSwordModuleInfo::greekAccents:
		return i18n("Greek accents");

		case CSwordModuleInfo::redLetterWords:
		return i18n("Red letter words");

		case CSwordModuleInfo::textualVariants:
		return i18n("Textual variants");

		case CSwordModuleInfo::scriptureReferences:
		return i18n("Scripture cross-references");

		case CSwordModuleInfo::morphSegmentation:
		return i18n("Morph segmentation");
		//   case CSwordModuleInfo::transliteration:
		//    return i18n("Transliteration between scripts");
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

		default:
		return QString::null;
	}

	return QString::null;
}

const QString CSwordBackend::booknameLanguage( const QString& language ) {
	if (!language.isEmpty()) {
		sword::LocaleMgr::getSystemLocaleMgr()->setDefaultLocaleName( language.toUtf8().constData() );

		//refresh the locale of all Bible and commentary modules!
		const ListCSwordModuleInfo::iterator end_it = m_moduleList.end();

		//use what sword returns, language may be different
		QString newLocaleName( sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName()  );

		for (ListCSwordModuleInfo::iterator it = m_moduleList.begin(); it != end_it; ++it) {
			if ( ((*it)->type() == CSwordModuleInfo::Bible) || ((*it)->type() == CSwordModuleInfo::Commentary) ) {
				//Create a new key, it will get the default bookname language
				((sword::VerseKey*)((*it)->module()->getKey()))->setLocale( newLocaleName.toUtf8().constData() );
			}
		}

	}

	return QString( sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName() );
}


/** Reload all Sword modules. */
void CSwordBackend::reloadModules() {
	shutdownModules();

	//delete Sword's config to make Sword reload it!

	if (myconfig) { // force reload on config object because we may have changed the paths
		delete myconfig;
		config = myconfig = 0;
		loadConfigDir(configPath);
	}
	else if (config) {
		config->Load();
	}

	initModules();
}

const QStringList CSwordBackend::swordDirList() {
	QStringList ret;
	const QString home = QString(getenv("HOME"));

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
			ret << it->second.c_str(); //added augment path
		}
	}

	if (!home.isEmpty()) {
		ret << home + "/.sword/";
	}

	return ret;
}

void CSwordBackend::filterInit() {
	//  qWarning("## INIT");

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
