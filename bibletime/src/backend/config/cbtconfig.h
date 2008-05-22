/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CBTCONFIG_H
#define CBTCONFIG_H

#include "backend/managers/cswordbackend.h"
#include "backend/drivers/cswordmoduleinfo.h"

//Qt includes
#include <QString>
#include <QFont>
#include <QList>
#include <QMap>

//Forward declarations
class QSettings;
class CLanguageMgr::Language;

/**
 * This class is the interface to the config object of BibleTime
 * @author The BibleTime team
 */
class CBTConfig {
public:
	typedef QMap<QString, QString> StringMap;

	enum strings {
		bibletimeVersion,
		language,
		displayStyle,
		bookshelfCurrentItem
	};
	enum modules {
		standardBible = 0, //just to make sure, default is IMHO 0, so that's redundant here
		standardCommentary,
		standardLexicon,
		standardDailyDevotional,
		standardHebrewStrongsLexicon,
		standardGreekStrongsLexicon,
		standardHebrewMorphLexicon,
		standardGreekMorphLexicon,
		lastModuleType = standardGreekMorphLexicon
	};
	enum bools {
		firstSearchDialog,
		readOldBookmarks,

		toolbar,
		mainIndex,
		infoDisplay,

		autoTileVertical,
		autoTileHorizontal,
		autoCascade,

		lineBreaks,
		verseNumbers,

		logo,
		autoDeleteOrphanedIndices,
		crashedLastTime,
		crashedTwoTimes,

		bookshelfShowHidden
	};
	enum ints {
		footnotes,
		strongNumbers,
		headings,
		morphTags,
		lemmas,
		hebrewPoints,
		hebrewCantillation,
		greekAccents,
		textualVariants,
		scriptureReferences,
		morphSegmentation,

		bookshelfContentsX,
		bookshelfContentsY,
		magDelay, /* The delay until a mouse move makes the content appear in the mag */
		bookshelfGrouping,
		mainindexActiveTab,
		
		searchDialogWidth,
		searchDialogHeight,
		searchDialogX,
		searchDialogY
	};
	enum intLists {
		leftPaneSplitterSizes,
		mainSplitterSizes,
		searchMainSplitterSizes,
		searchResultSplitterSizes
	};
	enum stringLists {
		searchCompletionTexts,
		searchTexts,
		searchModulesHistory,
		bookshelfOpenGroups,
		hiddenModules
	};
	enum keys {
		allWindows,
		readWindow,
		writeWindow,
		bookWindow,
		bibleWindow,
		commentaryWindow,
		lexiconWindow,
		application
	};
	enum stringMaps {
		searchScopes
	};
	typedef std::pair<bool, QFont> FontSettingsPair;
	typedef QMap<const CLanguageMgr::Language*, CBTConfig::FontSettingsPair> FontCache;

	static const QString get( const CBTConfig::strings );
	static CSwordModuleInfo* const get( const CBTConfig::modules );
	static const bool get( const CBTConfig::bools );
	static const int get( const CBTConfig::ints );
	static const QList<int> get( const CBTConfig::intLists );
	static const QStringList get( const CBTConfig::stringLists );
	static const CBTConfig::StringMap get( const CBTConfig::stringMaps );

	static const FontSettingsPair get( const CLanguageMgr::Language* const );

	static const QString getDefault( const CBTConfig::strings );
	static const QString getDefault( const CBTConfig::modules );
	static const bool getDefault( const CBTConfig::bools );
	static const int getDefault( const CBTConfig::ints );
	static const QList<int> getDefault( const CBTConfig::intLists );
	static const QStringList getDefault( const CBTConfig::stringLists );
	static const CBTConfig::StringMap getDefault( const CBTConfig::stringMaps );
	static const QFont& getDefault( const CLanguageMgr::Language* const );

	static void set( const CBTConfig::strings,  const QString value );
	static void set( const CBTConfig::modules, CSwordModuleInfo* const module );
	static void set( const CBTConfig::modules, const QString& moduleName );
	static void set( const CBTConfig::bools,   const bool value );
	static void set( const CBTConfig::ints,   const int value );
	static void set( const CBTConfig::intLists, const QList<int> value );
	static void set( const CBTConfig::stringLists, const QStringList value);
	static void set( const CBTConfig::stringMaps, const CBTConfig::StringMap value);
	static void set( const CLanguageMgr::Language* const language, const FontSettingsPair& fontSettings );

	static const CSwordBackend::FilterOptions getFilterOptionDefaults();
	static const CSwordBackend::DisplayOptions getDisplayOptionDefaults();

//	static void setupAccelSettings(const CBTConfig::keys type, KActionCollection* const actionCollection);
//	static void saveAccelSettings(const CBTConfig::keys type, KActionCollection* const actionCollection);

	static const QString getModuleEncryptionKey( const QString& name );
	static void setModuleEncryptionKey( const QString& name, const QString& key );
	
	/** Re-reads the config from disk */
	static void syncConfig();

private:
	friend class BibleTimeTest;
	/** The config object.
	* @return A config object which is used currently, may be the global config or the session config
	*/
	static QSettings* const getConfig();

	static const QString getKey( const CBTConfig::strings );
	static const QString getKey( const CBTConfig::modules );
	static const QString getKey( const CBTConfig::bools );
	static const QString getKey( const CBTConfig::ints );
	static const QString getKey( const CBTConfig::intLists );
	static const QString getKey( const CBTConfig::stringLists );
	static const QString getKey( const CBTConfig::stringMaps );
	static const QString getKey( const CLanguageMgr::Language* const );
	
	static const QString IntListToString( const QList<int> );
	static const QList<int> StringToIntList( const QString );

	//static caches
	static QFont* m_defaultFont;

	static FontCache fontConfigMap;
};


#endif
