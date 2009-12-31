/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
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
namespace CBTConfig {

typedef QMap<QString, QString> StringMap;
typedef std::pair<bool, QFont> FontSettingsPair;

enum strings {
    bibletimeVersion,
    language,
    displayStyle,
    bookshelfCurrentItem
};
enum modules {
    standardBible = 0, // Just to make sure, default is IMHO 0, so that's redundant here
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
    autoTile,
    autoCascade,

    lineBreaks,
    verseNumbers,

    logo,
    autoDeleteOrphanedIndices,
    crashedLastTime,
    crashedTwoTimes,

    bookshelfShowHidden,

    allowNetworkConnection,

    showTextWindowHeaders,
    showTextWindowNavigator,
    showTextWindowToolButtons,
    showTextWindowModuleSelectorButtons
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

QString    IntListToString(const QList<int> intList);
QList<int> StringToIntList(const QString    intListString);

QString     getDefault(const strings);
QString     getDefault(const modules);
bool        getDefault(const bools);
int         getDefault(const ints);
QList<int>  getDefault(const intLists);
QStringList getDefault(const stringLists);
StringMap   getDefault(const stringMaps);
QFont      &getDefault(const CLanguageMgr::Language * const);

QString           get(const strings);
CSwordModuleInfo *get(const modules);
bool              get(const bools);
int               get(const ints);
QList<int>        get(const intLists);
QStringList       get(const stringLists);
StringMap         get(const stringMaps);
FontSettingsPair  get(const CLanguageMgr::Language * const);

void set(const strings,     const QString            value);
void set(const modules,     CSwordModuleInfo * const module);
void set(const modules,     const QString&           moduleName);
void set(const bools,       const bool               value);
void set(const ints,        const int                value);
void set(const intLists,    const QList<int>         value);
void set(const stringLists, const QStringList        value);
void set(const stringMaps,  const StringMap          value);
void set(const CLanguageMgr::Language * const language,
         const FontSettingsPair &fontSettings);

CSwordBackend::FilterOptions  getFilterOptionDefaults();
CSwordBackend::DisplayOptions getDisplayOptionDefaults();

void setupAccelSettings(const keys type,
                        BtActionCollection * const actionCollection);
void saveAccelSettings (const keys type,
                        BtActionCollection * const actionCollection);

QString getModuleEncryptionKey(const QString &name);
void    setModuleEncryptionKey(const QString &name, const QString &key);

/**
  Re-reads the config from disk.
*/
void syncConfig();

/**
  Returns the config object.
*/
QSettings* getConfig();

} // namespace CBTConfig

#endif
