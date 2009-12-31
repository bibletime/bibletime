/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/config/cbtconfig.h"

#include <QDebug>
#include <QLocale>
#include <QSettings>
#include <QVariant>
#include <QWebSettings>
#include "backend/btmoduletreeitem.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"
#include "util/cpointers.h"
#include "util/directory.h"

// Sword includes:
#include <versekey.h> // For range configuration


namespace CBTConfig {
namespace {

typedef QMap<const CLanguageMgr::Language*, FontSettingsPair> FontCacheMap;

QFont        *m_defaultFont = 0;
FontCacheMap  m_fontCache;

QString getKey(const strings ID) {
    switch (ID) {
        case bibletimeVersion:
            return "bibletimeVersion";
        case language:
            return "language";
        case displayStyle:
            return "displayStyle";
        case bookshelfCurrentItem:
            return "bookshelfCurrentItem";
    }
    Q_ASSERT(false);
    return QString::null;
}

QString getKey(const modules ID) {
    switch (ID) {
        case standardBible:
            return "standardBible";
        case standardCommentary:
            return "standardCommentary";
        case standardLexicon:
            return "standardLexicon";
        case standardDailyDevotional:
            return "standardDailyDevotional";
        case standardHebrewStrongsLexicon:
            return "standardHebrewLexicon";
        case standardGreekStrongsLexicon:
            return "standardGreekLexicon";
        case standardHebrewMorphLexicon:
            return "standardHebrewMorphLexicon";
        case standardGreekMorphLexicon:
            return "standardGreekMorphLexicon";
    }
    Q_ASSERT(false);
    return QString::null;
}

QString getKey(const bools ID) {
    switch (ID) {
        case firstSearchDialog:
            return "firstSearchDialog";
        case readOldBookmarks:
            return "readOldBookmarks";

        case toolbar:
            return "toolbar";
        case mainIndex:
            return "mainIndex";
        case infoDisplay:
            return "infoDisplay";

        case autoTileVertical:
            return "autoTileVertical";
        case autoTileHorizontal:
            return "autoTileHorizontal";
        case autoTile:
            return "autoTile";
        case autoCascade:
            return "autoCascade";

        case lineBreaks:
            return "lineBreaks";
        case verseNumbers:
            return "verseNumbers";

        case logo:
            return "logo";
        case autoDeleteOrphanedIndices:
            return "autoDeleteOrphanedIndices";
        case crashedLastTime:
            return "crashedLastTime";
        case crashedTwoTimes:
            return "crashedTwoTimes";

        case bookshelfShowHidden:
            return "bookshelfShowHidden";
        case allowNetworkConnection:
            return "allowNetworkConnection";
            
        case showTextWindowHeaders:
            return "showTextWindowHeaders";
        case showTextWindowNavigator:
            return "showTextWindowNavigator";
        case showTextWindowModuleSelectorButtons:
            return "showTextWindowModuleSelectorButtons";
        case showTextWindowToolButtons:
            return "showTextWindowToolButtons";
    }
    Q_ASSERT(false);
    return false;
}

QString getKey(const ints ID) {
    switch (ID) {
        case footnotes:
            return "footnotes";
        case strongNumbers:
            return "strongNumbers";
        case headings:
            return "headings";
        case morphTags:
            return "morphTags";
        case lemmas:
            return "lemmas";
        case hebrewPoints:
            return "hebrewPoints";
        case hebrewCantillation:
            return "hebrewCantillation";
        case greekAccents:
            return "greekAccents";
        case textualVariants:
            return "textualVariants";
        case scriptureReferences:
            return "scriptureReferences";
        case morphSegmentation:
            return "morphSegmentation";
        case bookshelfContentsX:
            return "bookshelfContentsX";
        case bookshelfContentsY:
            return "bookshelfContentsY";
        case magDelay:
            return "magDelay";
        case bookshelfGrouping:
            return "bookshelfGrouping";
        case mainindexActiveTab:
            return "mainindexActiveTab";
        case searchDialogWidth:
            return "searchDialogWidth";
        case searchDialogHeight:
            return "searchDialogHeight";
        case searchDialogX:
            return "searchDialogX";
        case searchDialogY:
            return "searchDialogY";
        case searchType:
            return "searchType";
        case installPathIndex:
            return "installPathIndex";
        case bookshelfPosX:
            return "bookshelfPosX";
        case bookshelfPosY:
            return "bookshelfPosY";
        case bookshelfHeight:
            return "bookshelfHeight";
        case bookshelfWidth:
            return "bookshelfWidth";
        case configDialogPosX:
            return "configDialogPosX";
        case configDialogPosY:
            return "configDialogPosY";
        case configDialogHeight:
            return "configDialogHeight";
        case configDialogWidth:
            return "configDialogWidth";
    }
    Q_ASSERT(false);
    return QString::null;
}

QString getKey(const intLists ID) {
    switch (ID) {
        case leftPaneSplitterSizes:
            return "leftPaneSplitterSizes";
        case mainSplitterSizes:
            return "mainSplitterSizes";
        case searchMainSplitterSizes:
            return "searchMainSplitterSizes";
        case searchResultSplitterSizes:
            return "searchResultSplitterSizes";
    }
    Q_ASSERT(false);
    return QString::null;
}

QString getKey(const stringLists ID) {
    switch (ID) {
        case searchCompletionTexts:
            return "searchCompletionTexts";
        case searchTexts:
            return "searchTexts";
        case searchModulesHistory:
            return "searchModulesHistory";
        case bookshelfOpenGroups:
            return "bookshelfOpenGroups";
        case hiddenModules:
            return "hiddenModules";
    }
    Q_ASSERT(false);
    return QString::null;
}

QString getKey(const stringMaps ID) {
    switch (ID) {
        case searchScopes:
            return "SearchScopes";
    };
    Q_ASSERT(false);
    return QString::null;
}

QString getKey(const CLanguageMgr::Language * const language) {
    Q_ASSERT(!language->name().isEmpty());
    return language->name();
}

} // anonymous namespace

QString IntListToString(const QList<int> intList) {
    QStringList intStrings;
    foreach(int i, intList) {
        intStrings << QString::number(i);
    }
    return intStrings.join(",");
}

QList<int> StringToIntList(const QString intListString) {
    QList<int> intList;
    if (!intListString.isEmpty() && intListString.contains(',')) {
        foreach(QString intString, intListString.split(',')) {
            intList << intString.trimmed().toInt();
        }
    }
    return intList;
}

QString getDefault(const strings ID) {
    switch (ID) {
        case bibletimeVersion:
            return "0.0"; // main() will realize this and set the value to VERSION
        case language:
            return QLocale::system().name();
        case displayStyle:
            return CDisplayTemplateMgr::defaultTemplate();
        case bookshelfCurrentItem:
            return QString();
    }
    return QString::null;
}

QString getDefault(const modules ID) {
    // CSwordBackend *b = CPointers::backend();
    switch (ID) {
        case standardBible:
            return "KJV";
        case standardCommentary:
            return "MHC";
        case standardLexicon:
            return "ISBE";
        case standardDailyDevotional:
            return ""; // No default

        case standardHebrewStrongsLexicon:
            return "StrongsHebrew";
        case standardGreekStrongsLexicon:
            return "StrongsGreek";
        case standardHebrewMorphLexicon:
            return "StrongsHebrew";
        case standardGreekMorphLexicon:
            return "StrongsGreek";
    }

    return QString::null;
}

bool getDefault(const bools ID) {
    switch (ID) {
        case firstSearchDialog:
            return true;
        case readOldBookmarks:
            return false;

        case toolbar:
            return true;
        case mainIndex:
            return true;
        case infoDisplay:
            return true;

        case autoTileVertical:
            return true;
        case autoTileHorizontal:
            return false;
        case autoTile:
            return false;
        case autoCascade:
            return false;

        case lineBreaks:
            return false;
        case verseNumbers:
            return true;

        case logo:
            return true;
        case autoDeleteOrphanedIndices:
            return true;
        case crashedLastTime:
            return false;
        case crashedTwoTimes:
            return false;
        case bookshelfShowHidden:
            return false;
        case allowNetworkConnection:
            return false;
            
        case showTextWindowHeaders:
            return true;
        case showTextWindowNavigator:
            return true;
        case showTextWindowModuleSelectorButtons:
            return true;
        case showTextWindowToolButtons:
            return true;
    }
    return false;
}

int getDefault(const ints ID) {
    switch (ID) {
        case footnotes:
            return int(true);
        case strongNumbers:
            return int(true);
        case headings:
            return int(true);
        case morphTags:
            return int(true);
        case lemmas:
            return int(true);
        case hebrewPoints:
            return int(true);
        case hebrewCantillation:
            return int(true);
        case greekAccents:
            return int(true);
        case textualVariants:
            return int(false);
        case scriptureReferences:
            return int(true);
        case morphSegmentation:
            return int(true);
        case bookshelfContentsX:
            return 0;
        case bookshelfContentsY:
            return 0;
        case magDelay:
            return 400;
        case bookshelfGrouping:
            return BTModuleTreeItem::CatLangMod;
        case searchDialogWidth:
            return 200;
        case searchDialogHeight:
            return 400;
        case searchDialogX:
            return 200;
        case searchDialogY:
            return 200;
        case searchType:
            return Search::BtSearchOptionsArea::AndType;
        case mainindexActiveTab:
            return 0;
        case installPathIndex:
            return 0;
        case bookshelfPosX:
            return 1;
        case bookshelfPosY:
            return 1;
        case bookshelfHeight:
            return 1;
        case bookshelfWidth:
            return 1;
        case configDialogPosX:
            return 1;
        case configDialogPosY:
            return 1;
        case configDialogHeight:
            return 1;
        case configDialogWidth:
            return 1;
    }
    return 0;
}

QList<int> getDefault(const intLists /*ID*/) {
    QList<int> result;
    /*switch ( ID ) {
        case leftPaneSplitterSizes: break;
        case mainSplitterSizes: break;
        case searchMainSplitterSizes: break;
        case searchResultSplitterSizes: break;*/
    return result;
}

QStringList getDefault(const stringLists ID) {
    switch (ID) {
        case searchTexts: {
            QStringList list;
            list.append(QString::null);
            return list;
        }
        case searchCompletionTexts:
            return QStringList();
        case bookshelfOpenGroups:
            return QStringList();
        case hiddenModules:
            return QStringList();
        case searchModulesHistory:
            return QStringList();
    }
    return QStringList();
}

StringMap getDefault(const stringMaps ID) {
    if (ID != searchScopes) return StringMap();

    StringMap map;
    map.insert(QObject::tr("Old testament"),          QString("Gen - Mal"));
    map.insert(QObject::tr("Moses/Pentateuch/Torah"), QString("Gen - Deut"));
    map.insert(QObject::tr("History"),                QString("Jos - Est"));
    map.insert(QObject::tr("Prophets"),               QString("Isa - Mal"));
    map.insert(QObject::tr("New testament"),          QString("Mat - Rev"));
    map.insert(QObject::tr("Gospels"),                QString("Mat - Joh"));
    map.insert(QObject::tr("Letters/Epistles"),       QString("Rom - Jude"));
    map.insert(QObject::tr("Paul's Epistles"),        QString("Rom - Phile"));

    // Make the list to the current bookname language!
    sword::VerseKey vk;
    vk.setLocale("en_US");

    for (StringMap::Iterator it(map.begin()); it != map.end(); it++) {
        sword::ListKey list(vk.ParseVerseList(it.value().toLocal8Bit(), "Genesis 1:1", true));
        QString data;
        for (int i(0); i < list.Count(); i++) {
            data.append(QString::fromUtf8(list.GetElement(i)->getRangeText()));
            data.append("; ");
        }
        map[it.key()] = data;
    };

    return map;
}

QFont &getDefault(const CLanguageMgr::Language * const) {
    // Language specific lookup of the font name
    if (m_defaultFont) return *m_defaultFont;

    /// \todo make the font name and size a configuration option
    // int fontSize = QWebSettings::globalSettings()->fontSize(QWebSettings::DefaultFontSize);
    int fontSize(12);
    QString fontName(QWebSettings::globalSettings()->fontFamily(QWebSettings::StandardFont));

    m_defaultFont = new QFont(fontName, fontSize); /// \todo there may be a mem leak here!
    return *m_defaultFont;
}

QString get(const strings ID) {
    getConfig()->beginGroup("strings");
    QString result(getConfig()->value(getKey(ID), getDefault(ID)).toString());
    getConfig()->endGroup();
    return result;
}

CSwordModuleInfo *get(const modules ID) {
    getConfig()->beginGroup("modules");
    CSwordModuleInfo *result(CPointers::backend()->findModuleByName(
                                 getConfig()->value(getKey(ID), getDefault(ID)).toString()
                             ));
    getConfig()->endGroup();
    return result;
}

bool get(const bools ID) {
    getConfig()->beginGroup("bools");
    bool result(getConfig()->value(getKey(ID), getDefault(ID)).toBool());
    getConfig()->endGroup();
    return result;
}

int get(const ints ID) {
    getConfig()->beginGroup("ints");
    int result(getConfig()->value(getKey(ID), getDefault(ID)).toInt());
    getConfig()->endGroup();
    return result;
}

QList<int> get(const intLists ID) {
    getConfig()->beginGroup("intlists");
    QList<int> result(StringToIntList(
                          getConfig()->value(getKey(ID), IntListToString(getDefault(ID)))
                          .toString()
                      ));
    getConfig()->endGroup();
    return result;
}

QStringList get(const stringLists ID) {
    getConfig()->beginGroup("stringlists");
    QStringList result(
        getConfig()->value(getKey(ID), getDefault(ID)).toStringList()
    );
    getConfig()->endGroup();
    return result;
}

StringMap get(const stringMaps ID) {
    StringMap map;

    getConfig()->beginGroup(getKey(ID));
    QStringList keys(getConfig()->childKeys());

    if (!keys.isEmpty()) {
        switch (ID) {
            case searchScopes: {
                /**
                  Make sure we return the scopes in the chosen language. saved
                  keys are in english.
                */
                sword::VerseKey vk;
                foreach (QString key, keys) {
                    Q_ASSERT(!key.isEmpty());
                    QByteArray b(getConfig()->value(key).toString().toUtf8());
                    sword::ListKey list(vk.ParseVerseList(b, "Genesis 1:1", true));
                    QString data;
                    for (int i(0); i < list.Count(); i++) {
                        data.append(QString::fromUtf8(list.GetElement(i)->getRangeText()));
                        data.append("; ");
                    }
                    map[key] = data; // Set the new data
                }
            }
            default:
                break;
        }
    }
    else {
        map = getDefault(ID);
    }
    getConfig()->endGroup();
    return map;
}

FontSettingsPair get(const CLanguageMgr::Language * const language) {
    // Check the cache first:
    FontCacheMap::const_iterator it(m_fontCache.find(language));
    if (it != m_fontCache.end()) return *it;

    FontSettingsPair settings;

    getConfig()->beginGroup("font standard settings");
    settings.first = getConfig()->value(getKey(language), false).toBool();
    getConfig()->endGroup();

    getConfig()->beginGroup("fonts");
    QFont font;
    if (settings.first) {
        font.fromString(getConfig()->value(getKey(language), getDefault(language)).toString());
    }
    else {
        font = getDefault(language);
    }
    settings.second = font;
    getConfig()->endGroup();

    // Cache the value:
    m_fontCache.insert(language, settings);

    return settings;
}

void set(const strings ID, const QString value) {
//     KConfigGroup cg = getConfig()->group("strings");
//     cg.writeEntry(getKey(ID), value);
    getConfig()->beginGroup("strings");
    getConfig()->setValue(getKey(ID), value);
    getConfig()->endGroup();
}

void set(const modules ID, CSwordModuleInfo * const value) {
//     KConfigGroup cg = getConfig()->group("modules");
//     cg.writeEntry(getKey(ID), value ? value->name() : QString::null);
    getConfig()->beginGroup("modules");
    getConfig()->setValue(getKey(ID), value ? value->name() : QString::null);
    getConfig()->endGroup();
}

void set(const modules ID, const QString& value) {
    CSwordModuleInfo *module(CPointers::backend()->findModuleByName(value));
    if (module) {
        set(ID, module);
    }
}

void set(const bools ID, const  bool value) {
    getConfig()->beginGroup("bools");
    getConfig()->setValue(getKey(ID), value);
    getConfig()->endGroup();
}

void set(const ints ID, const int value) {
    getConfig()->beginGroup("ints");
    getConfig()->setValue(getKey(ID), value);
    getConfig()->endGroup();
}

void set(const intLists ID, const QList<int> value) {
    getConfig()->beginGroup("intlists");
    getConfig()->setValue(getKey(ID), IntListToString(value));
    getConfig()->endGroup();
}

void set(const stringLists ID, const QStringList value) {
    getConfig()->beginGroup("stringlists");
    getConfig()->setValue(getKey(ID), value);
    getConfig()->endGroup();
}

void set(const stringMaps ID, const StringMap value) {
    typedef StringMap::ConstIterator SMCI;
    using namespace sword;

    getConfig()->beginGroup(getKey(ID));

    // Clear all entries of this group to make sure old stuff gets removed:
    getConfig()->remove("");

    switch (ID) {
        case searchScopes: {
            /**
              We want to make sure that the search scopes are saved with english
              key names so loading them will always work with each locale set.
            */
            VerseKey vk;
            for (SMCI it(value.begin()); it != value.end(); it++) {
                QString data;
                ListKey list(vk.ParseVerseList(it.value().toUtf8(), "Genesis 1:1", true));
                for (int i(0); i < list.Count(); i++) {
                    VerseKey *range(dynamic_cast<VerseKey*>(list.GetElement(i)));
                    if (range) {
                        range->setLocale("en");
                        data.append(QString::fromUtf8( range->getRangeText() ));
                        data.append(";");
                    }
                }
                getConfig()->setValue(it.key(), data);
            }
            break;
        }
        default: {
            for (SMCI it(value.begin()); it != value.end(); it++) {
                getConfig()->setValue(it.key(), it.value());
            }
            break;
        }
    }
    getConfig()->endGroup();
}

void set(const CLanguageMgr::Language * const language,
         const FontSettingsPair& value) {
    getConfig()->beginGroup("fonts");
    getConfig()->setValue(getKey(language), value.second.toString());
    getConfig()->endGroup();
    getConfig()->beginGroup("font standard settings");
    getConfig()->setValue(getKey(language), value.first);
    getConfig()->endGroup();

    // Remove language from the cache:
    m_fontCache.remove(language);
}

CSwordBackend::DisplayOptions getDisplayOptionDefaults() {
    CSwordBackend::DisplayOptions options;
    options.lineBreaks   = get(lineBreaks);
    options.verseNumbers = get(verseNumbers);
    return options;
}

CSwordBackend::FilterOptions getFilterOptionDefaults() {
    CSwordBackend::FilterOptions options;

    options.footnotes           = true; // Required for the info display
    options.strongNumbers       = true; // get(strongNumbers);
    options.headings            = get(headings);
    options.morphTags           = true; // Required for the info display
    options.lemmas              = true; // Required for the info display
    options.redLetterWords      = true;
    options.hebrewPoints        = get(hebrewPoints);
    options.hebrewCantillation  = get(hebrewCantillation);
    options.greekAccents        = get(greekAccents);
    options.textualVariants     = get(textualVariants);
    options.scriptureReferences = get(scriptureReferences);
    options.morphSegmentation   = get(morphSegmentation);

    return options;
}

void setupAccelSettings(const keys type,
                        BtActionCollection * const actionCollection) {
    qDebug() << "CBTConfig::setupAccelSettings begin";
    QString groupName;
    switch (type) {
        case allWindows:
            groupName = "Displaywindow shortcuts";
            break;
        case writeWindow:
            groupName = "Writewindow shortcuts";
            break;
        case readWindow:
            groupName = "Readwindow shortcuts";
            break;
        case bookWindow:
            groupName = "Book shortcuts";
            break;
        case bibleWindow:
            groupName =  "Bible shortcuts";
            break;
        case commentaryWindow:
            groupName = "Commentary shortcuts";
            break;
        case lexiconWindow:
            groupName = "Lexicon shortcuts";
            break;
        case application:
            groupName = "Application shortcuts";
            break;
    }
    qDebug() << groupName;
    Q_ASSERT(getConfig());

    actionCollection->setConfigGroup(groupName);
    actionCollection->readSettings();
    qDebug() << "CBTConfig::setupAccelSettings end";
}

void saveAccelSettings(const keys type,
                       BtActionCollection * const actionCollection) {
    qDebug() << "CBTConfig::saveAccelSettings begin";
    QString groupName;
    switch (type) {
        case allWindows:
            groupName = "Displaywindow shortcuts";
            break;
        case writeWindow:
            groupName = "Writewindow shortcuts";
            break;
        case readWindow:
            groupName = "Readwindow shortcuts";
            break;
        case bookWindow:
            groupName = "Book shortcuts";
            break;
        case bibleWindow:
            groupName = "Bible shortcuts";
            break;
        case commentaryWindow:
            groupName = "Commentary shortcuts";
            break;
        case lexiconWindow:
            groupName = "Lexicon shortcuts";
            break;
        case application:
            groupName = "Application shortcuts";
            break;
    }

    actionCollection->setConfigGroup(groupName);
    actionCollection->writeSettings();
    qDebug() << "CBTConfig::saveAccelSettings end";
}


QString getModuleEncryptionKey(const QString &module) {
    Q_ASSERT(!module.isEmpty());
    getConfig()->beginGroup("Module keys");
    QString result(getConfig()->value(module, QVariant(QString::null)).toString());
    getConfig()->endGroup();
    return result;
}

void setModuleEncryptionKey(const QString &module, const QString &key) {
    getConfig()->beginGroup("Module keys");
    getConfig()->setValue(module, key);
    getConfig()->endGroup();
}

QSettings *getConfig() {
    namespace DU = util::directory;
    static QSettings config(DU::getUserBaseDir().absolutePath() + "/bibletimerc", QSettings::IniFormat);
    return &config;
}

void syncConfig() {
    getConfig()->sync();
}

} // namespace CBTConfig
