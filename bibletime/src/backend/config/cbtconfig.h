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

#include <QString>
#include <QFont>
#include <QList>
#include <QMap>
#include "backend/managers/cswordbackend.h"

class QSettings;
class BtActionCollection;

/**
 * This class is the interface to the config object of BibleTime
 * @author The BibleTime team
 */
class CBTConfig {
    friend class BibleTimeTest;

public:
	typedef QMap<QString, QString> StringMap;
    typedef std::pair<bool, QFont> FontSettingsPair;

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

		bookshelfShowHidden,

		allowNetworkConnection
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
		searchType,

		installPathIndex,

		bookshelfPosX,
		bookshelfPosY,
		bookshelfHeight,
		bookshelfWidth,

		configDialogPosX,
		configDialogPosY,
		configDialogHeight,
		configDialogWidth
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

public:
    static QString get( const strings );
    static CSwordModuleInfo* get( const modules );
    static bool get( const bools );
    static int get( const ints );
    static QList<int> get( const intLists );
    static QStringList get( const stringLists );
    static StringMap get( const stringMaps );

	static FontSettingsPair get( const CLanguageMgr::Language* const );

    static QString getDefault( const strings );
    static QString getDefault( const modules );
    static bool getDefault( const bools );
    static int getDefault( const ints );
    static QList<int> getDefault( const intLists );
    static QStringList getDefault( const stringLists );
    static StringMap getDefault( const stringMaps );
	static QFont& getDefault( const CLanguageMgr::Language* const );

    static void set( const strings,  const QString value );
    static void set( const modules, CSwordModuleInfo* const module );
    static void set( const modules, const QString& moduleName );
    static void set( const bools,   const bool value );
    static void set( const ints,   const int value );
    static void set( const intLists, const QList<int> value );
    static void set( const stringLists, const QStringList value);
    static void set( const stringMaps, const StringMap value);
	static void set( const CLanguageMgr::Language* const language, const FontSettingsPair& fontSettings );

	static CSwordBackend::FilterOptions getFilterOptionDefaults();
	static CSwordBackend::DisplayOptions getDisplayOptionDefaults();

    static void setupAccelSettings(const keys type, BtActionCollection* const actionCollection);
    static void saveAccelSettings(const keys type, BtActionCollection* const actionCollection);

	static QString getModuleEncryptionKey( const QString& name );
	static void setModuleEncryptionKey( const QString& name, const QString& key );

	/** Re-reads the config from disk */
	static void syncConfig();

	// Returns the config object
	static QSettings* getConfig();

private:
    typedef QMap<const CLanguageMgr::Language*, FontSettingsPair> FontCache;

    static QString getKey( const strings );
    static QString getKey( const modules );
    static QString getKey( const bools );
    static QString getKey( const ints );
    static QString getKey( const intLists );
    static QString getKey( const stringLists );
    static QString getKey( const stringMaps );
	static QString getKey( const CLanguageMgr::Language* const );

	static QString IntListToString( const QList<int> );
	static QList<int> StringToIntList( const QString );

private:
	//static caches
	static QFont* m_defaultFont;
    static FontCache fontConfigMap;
};

#endif
