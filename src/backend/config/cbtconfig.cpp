/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
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
#include "btglobal.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"
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
}

QString getKey(const bools ID) {
    switch (ID) {
        case firstSearchDialog:
            return "firstSearchDialog";
        case readOldBookmarks:
            return "readOldBookmarks";

        case showMainWindowToolbar:
            return "showMainWindowToolbar";
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
        case autoTabbed:
            return "autoTabbed";
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
        case showFormatToolbarButtons:
            return "showFormatToolbarButtons";
        case showTextWindowToolButtons:
            return "showTextWindowToolButtons";
        case showToolbarsInEachWindow:
            return "showToolbarsInEachWindow";
        case showTipAtStartup:
            return "showTipAtStartup";
        default:
            Q_ASSERT(false);
            return QString::null;
    }
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
        case tipNumber:
            return "tipNumber";
        default:
            Q_ASSERT(false);
            return QString::null;
    }
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
        default:
            Q_ASSERT(false);
            return QString::null;
    }
}

QString getKey(const stringMaps ID) {
    Q_ASSERT(ID == searchScopes);
    return "SearchScopes";
}

QString getKey(const CLanguageMgr::Language * const language) {
    Q_ASSERT(!language->name().isEmpty());
    return language->name();
}

} // anonymous namespace

QString IntListToString(const QList<int> &intList) {
    QStringList intStrings;
    Q_FOREACH (int i, intList) {
        intStrings << QString::number(i);
    }
    return intStrings.join(",");
}

QList<int> StringToIntList(const QString &intListString,
                           bool *ok,
                           QString::SplitBehavior splitBehavior)
{
    QList<int> intList;
    if (ok != 0) {
        *ok = true;

        if (!intListString.isEmpty()) {
            Q_FOREACH (const QString &intString, intListString.split(',', splitBehavior)) {
                int i = intString.trimmed().toInt(ok);
                if (!(*ok))
                    break;

                intList << i;
            }
        }
    } else {
        if (!intListString.isEmpty()) {
            Q_FOREACH (const QString &intString, intListString.split(',', splitBehavior)) {
                intList << intString.trimmed().toInt();
            }
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
        default:
            return QString::null;
    }
}

QString getDefault(const modules ID) {
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
        default:
            return QString::null;
    }
}

bool getDefault(const bools ID) {
    switch (ID) {
        case firstSearchDialog:
        case showMainWindowToolbar:
        case mainIndex:
        case infoDisplay:
        case autoTileVertical:
        case verseNumbers:
        case logo:
        case autoDeleteOrphanedIndices:
        case showTextWindowHeaders:
        case showTextWindowNavigator:
        case showTextWindowModuleSelectorButtons:
        case showTextWindowToolButtons:
        case showFormatToolbarButtons:
        case showToolbarsInEachWindow:
        case showTipAtStartup:
            return true;
        case readOldBookmarks:
        case autoTileHorizontal:
        case autoTile:
        case autoTabbed:
        case autoCascade:
        case lineBreaks:
        case crashedLastTime:
        case crashedTwoTimes:
        case bookshelfShowHidden:
        case allowNetworkConnection:
        default:
            return false;
    }
}

int getDefault(const ints ID) {
    switch (ID) {
        case bookshelfGrouping:
            return BTModuleTreeItem::CatLangMod;
        case searchType:
            return Search::BtSearchOptionsArea::AndType;
        case searchDialogHeight:
        case magDelay:
            return 400;
        case searchDialogWidth:
        case searchDialogX:
        case searchDialogY:
            return 200;
        case footnotes:
        case strongNumbers:
        case headings:
        case morphTags:
        case lemmas:
        case hebrewPoints:
        case hebrewCantillation:
        case greekAccents:
        case scriptureReferences:
        case morphSegmentation:
        case bookshelfPosX:
        case bookshelfPosY:
        case bookshelfHeight:
        case bookshelfWidth:
        case configDialogPosX:
        case configDialogPosY:
        case configDialogHeight:
        case configDialogWidth:
            return 1;
        case textualVariants:
        case bookshelfContentsX:
        case bookshelfContentsY:
        case mainindexActiveTab:
        case installPathIndex:
        case tipNumber:
        default:
            return 0;
    }
}

QList<int> getDefault(const intLists /*ID*/) {
     return QList<int>();
}

QStringList getDefault(const stringLists ID) {
    if (ID == searchTexts) {
        QStringList list;
        list.append(QString::null);
        return list;
    } else {
        return QStringList();
    }
}

StringMap getDefault(const stringMaps ID) {
    Q_ASSERT(ID == searchScopes);

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
            data.append(QString::fromUtf8(list.getElement(i)->getRangeText()));
            data.append("; ");
        }
        map[it.key()] = data;
    };

    return map;
}

QFont &getDefault(const CLanguageMgr::Language * const) {
    // Language specific lookup of the font name
    if (m_defaultFont)
        return *m_defaultFont;

    /// \todo make the font name and size a configuration option
    // int fontSize = QWebSettings::globalSettings()->fontSize(QWebSettings::DefaultFontSize);
    const int fontSize = 12;
    const QString fontName(QWebSettings::globalSettings()->fontFamily(QWebSettings::StandardFont));

    m_defaultFont = new QFont(fontName, fontSize); /// \todo there may be a mem leak here!
    return *m_defaultFont;
}

QString get(const strings ID) {
    QVariant v = getConfig()->value("strings/" + getKey(ID));
    if (v.isValid())
        return v.toString();

    return getDefault(ID);
}

CSwordModuleInfo *get(const modules ID) {
    QVariant v = getConfig()->value("modules/" + getKey(ID));
    CSwordBackend *b = CSwordBackend::instance();
    if (v.isValid())
        return b->findModuleByName(v.toString());

    return b->findModuleByName(getDefault(ID));
}

bool get(const bools ID) {
    QVariant v = getConfig()->value("bools/" + getKey(ID));
    if (v.isValid())
        return v.toBool();

    return getDefault(ID);
}

int get(const ints ID) {
    QVariant v = getConfig()->value("ints/" + getKey(ID));
    if (v.isValid())
        return v.toInt();

    return getDefault(ID);
}

QList<int> get(const intLists ID) {
    QVariant v = getConfig()->value("intlists/" + getKey(ID));
    if (v.isValid()) {
        bool ok;
        QList<int> r = StringToIntList(v.toString(), &ok);
        if (ok)
            return r;
    }
    return getDefault(ID);
}

QStringList get(const stringLists ID) {
    QVariant v = getConfig()->value("stringlists/" + getKey(ID));
    if (v.isValid())
        return v.toStringList();

    return getDefault(ID);
}

StringMap get(const stringMaps ID) {
    Q_ASSERT(ID == searchScopes);

    QSettings *s = getConfig();
    s->beginGroup(getKey(ID));
    const QStringList keys = s->childKeys();
    s->endGroup();

    if (keys.isEmpty())
        return getDefault(ID);

    /**
      Make sure we return the scopes in the chosen language. saved
      keys are in english.
    */
    StringMap map;
    sword::VerseKey vk;
    Q_FOREACH (const QString &key, keys) {
        Q_ASSERT(!key.isEmpty());
        QByteArray b = s->value(key).toString().toUtf8();
        sword::ListKey list(vk.ParseVerseList(b, "Genesis 1:1", true));
        QString data;
        for (int i = 0; i < list.Count(); i++) {
            data.append(QString::fromUtf8(list.getElement(i)->getRangeText()));
            data.append("; ");
        }
        map[key] = data; // Set the new data
    }
    return map;
}

FontSettingsPair get(const CLanguageMgr::Language * const language) {
    // Check the cache first:
    FontCacheMap::const_iterator it = m_fontCache.find(language);
    if (it != m_fontCache.end())
        return *it;

    QSettings *s = getConfig();
    FontSettingsPair fontSettings;
    fontSettings.first = s->value("font standard settings/" + getKey(language), false).toBool();

    if (fontSettings.first) {
        QVariant v = s->value("fonts/" + getKey(language));
        if (v.isValid()) {
            fontSettings.second.fromString(v.toString());
        } else {
            fontSettings.second = getDefault(language);
        }
    } else {
        fontSettings.second = getDefault(language);
    }

    // Cache the value:
    m_fontCache.insert(language, fontSettings);

    return fontSettings;
}

void set(const strings ID, const QString &value) {
    getConfig()->setValue("strings/" + getKey(ID), value);
}

void set(const modules ID, CSwordModuleInfo * const value) {
    getConfig()->setValue("modules/" + getKey(ID), value ? value->name() : QString::null);
}

void set(const modules ID, const QString &value) {
    CSwordModuleInfo *module(CSwordBackend::instance()->findModuleByName(value));
    if (module) {
        set(ID, module);
    }
}

void set(const bools ID, const bool value) {
    getConfig()->setValue("bools/" + getKey(ID), value);
}

void set(const ints ID, const int value) {
    getConfig()->setValue("ints/" + getKey(ID), value);
}

void set(const intLists ID, const QList<int> &value) {
    getConfig()->setValue("intlists/" + getKey(ID), IntListToString(value));
}

void set(const stringLists ID, const QStringList &value) {
    getConfig()->setValue("stringlists/" + getKey(ID), value);
}

void set(const stringMaps ID, const StringMap &value) {
    typedef StringMap::ConstIterator SMCI;
    using namespace sword;

    QSettings *s = getConfig();
    s->beginGroup(getKey(ID));

    // Clear all entries of this group to make sure old stuff gets removed:
    s->remove("");

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
                    VerseKey *range(dynamic_cast<VerseKey*>(list.getElement(i)));
                    if (range) {
                        range->setLocale("en");
                        data.append(QString::fromUtf8( range->getRangeText() ));
                        data.append(";");
                    }
                }
                s->setValue(it.key(), data);
            }
            break;
        }
        default: {
            for (SMCI it(value.begin()); it != value.end(); it++) {
                s->setValue(it.key(), it.value());
            }
            break;
        }
    }
    s->endGroup();
}

void set(const CLanguageMgr::Language * const language,
         const FontSettingsPair& value)
{
    QSettings *s = getConfig();
    QString lang = getKey(language);
    s->setValue("fonts/" + lang, value.second.toString());
    s->setValue("font standard settings/" + lang, value.first);

    // Remove language from the cache:
    m_fontCache.remove(language);
}

DisplayOptions getDisplayOptionDefaults() {
    DisplayOptions options;
    options.lineBreaks   = get(lineBreaks);
    options.verseNumbers = get(verseNumbers);
    return options;
}

FilterOptions getFilterOptionDefaults() {
    FilterOptions options;

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
                        BtActionCollection * const actionCollection)
{
    const char *groupName;
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

    actionCollection->setConfigGroup(groupName);
    actionCollection->readSettings();
}

void saveAccelSettings(const keys type,
                       BtActionCollection * const actionCollection)
{
    const char *groupName;
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
}


QString getModuleEncryptionKey(const QString &module) {
    Q_ASSERT(!module.isEmpty());
    static const QString nullString;

    QVariant v = getConfig()->value("Module keys/" + module);
    if (v.isValid())
        return v.toString();

    return nullString;
}

void setModuleEncryptionKey(const QString &module, const QString &key) {
    getConfig()->setValue("Module keys/" + module, key);
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
