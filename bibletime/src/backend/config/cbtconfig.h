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

//Qt includes
#include <QString>
#include <QFont>
#include <QList>
#include <QMap>

//Forward declarations
class QSettings;
//class CLanguageMgr::Language;

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
		searchDialogY,
		searchType
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

	static QString get( const CBTConfig::strings );
	static CSwordModuleInfo* get( const CBTConfig::modules );
	static bool get( const CBTConfig::bools );
	static int get( const CBTConfig::ints );
	static QList<int> get( const CBTConfig::intLists );
	static QStringList get( const CBTConfig::stringLists );
	static CBTConfig::StringMap get( const CBTConfig::stringMaps );

	static FontSettingsPair get( const CLanguageMgr::Language* const );

	static QString getDefault( const CBTConfig::strings );
	static QString getDefault( const CBTConfig::modules );
	static bool getDefault( const CBTConfig::bools );
	static int getDefault( const CBTConfig::ints );
	static QList<int> getDefault( const CBTConfig::intLists );
	static QStringList getDefault( const CBTConfig::stringLists );
	static CBTConfig::StringMap getDefault( const CBTConfig::stringMaps );
	static QFont& getDefault( const CLanguageMgr::Language* const );

	static void set( const CBTConfig::strings,  const QString value );
	static void set( const CBTConfig::modules, CSwordModuleInfo* const module );
	static void set( const CBTConfig::modules, const QString& moduleName );
	static void set( const CBTConfig::bools,   const bool value );
	static void set( const CBTConfig::ints,   const int value );
	static void set( const CBTConfig::intLists, const QList<int> value );
	static void set( const CBTConfig::stringLists, const QStringList value);
	static void set( const CBTConfig::stringMaps, const CBTConfig::StringMap value);
	static void set( const CLanguageMgr::Language* const language, const FontSettingsPair& fontSettings );

	static CSwordBackend::FilterOptions getFilterOptionDefaults();
	static CSwordBackend::DisplayOptions getDisplayOptionDefaults();

//	static void setupAccelSettings(const CBTConfig::keys type, KActionCollection* const actionCollection);
//	static void saveAccelSettings(const CBTConfig::keys type, KActionCollection* const actionCollection);

	static QString getModuleEncryptionKey( const QString& name );
	static void setModuleEncryptionKey( const QString& name, const QString& key );

	/** Re-reads the config from disk */
	static void syncConfig();

private:
	friend class BibleTimeTest;
	/** The config object.
	* @return A config object which is used currently, may be the global config or the session config
	*/
	static QSettings* getConfig();

	static QString getKey( const CBTConfig::strings );
	static QString getKey( const CBTConfig::modules );
	static QString getKey( const CBTConfig::bools );
	static QString getKey( const CBTConfig::ints );
	static QString getKey( const CBTConfig::intLists );
	static QString getKey( const CBTConfig::stringLists );
	static QString getKey( const CBTConfig::stringMaps );
	static QString getKey( const CLanguageMgr::Language* const );

	static QString IntListToString( const QList<int> );
	static QList<int> StringToIntList( const QString );

	//static caches
	static QFont* m_defaultFont;

	typedef QMap<const CLanguageMgr::Language*, CBTConfig::FontSettingsPair> FontCache;
	static QMap<const CLanguageMgr::Language*, CBTConfig::FontSettingsPair> fontConfigMap;
};


#endif
