/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btconfig.h"

#include <QDebug>
#include <utility>
#include "../../util/btassert.h"
#include "../../util/directory.h" // DU::getUserBaseDir()
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/cswordbackend.h"

// Sword includes:
#include <versekey.h> // For search scope configuration


#define BTCONFIG_API_VERSION 1
namespace {
const QString BTCONFIG_API_VERSION_KEY = "btconfig_api_version";
}


/*
 * set the instance variable initially to 0, so it can be safely checked
 * whether the variable has been initialized yet.
 */
BtConfig * BtConfig::m_instance = nullptr;

BtConfig::StringMap BtConfig::m_defaultSearchScopes;


BtConfig::BtConfig(const QString & settingsFile)
    : BtConfigCore(settingsFile)
{
    BT_ASSERT(!m_instance && "BtConfig already initialized!");
    m_instance = this;

    if (m_defaultSearchScopes.isEmpty()) {
        m_defaultSearchScopes.insert(tr("Old testament"),          QString("Gen - Mal"));
        m_defaultSearchScopes.insert(tr("Moses/Pentateuch/Torah"), QString("Gen - Deut"));
        m_defaultSearchScopes.insert(tr("History"),                QString("Jos - Est"));
        m_defaultSearchScopes.insert(tr("Prophets"),               QString("Isa - Mal"));
        m_defaultSearchScopes.insert(tr("New testament"),          QString("Mat - Rev"));
        m_defaultSearchScopes.insert(tr("Gospels"),                QString("Mat - Joh"));
        m_defaultSearchScopes.insert(tr("Letters/Epistles"),       QString("Rom - Jude"));
        m_defaultSearchScopes.insert(tr("Paul's Epistles"),        QString("Rom - Phile"));
    }

#ifdef Q_OS_WIN
    const double minPointSize = 14.0;
    double pointSize = m_defaultFont.pointSizeF();
    if (pointSize < minPointSize)
        m_defaultFont.setPointSizeF(minPointSize);
#endif
}

BtConfig::InitState BtConfig::initBtConfig() {
    BT_ASSERT(!m_instance);

    const QString confFileName = util::directory::getUserBaseDir().absolutePath()
                                 + "/bibletimerc";
    bool confExisted = QFile::exists(confFileName);
    m_instance = new BtConfig(confFileName);
    if (!confExisted) {
        m_instance->setValue<int>(BTCONFIG_API_VERSION_KEY, BTCONFIG_API_VERSION);
        return INIT_OK;
    }

    int btConfigOldApi = m_instance->value<int>(BTCONFIG_API_VERSION_KEY, 0);
    if (btConfigOldApi == BTCONFIG_API_VERSION)
        return INIT_OK;
    return (btConfigOldApi < BTCONFIG_API_VERSION)
           ? INIT_NEED_UNIMPLEMENTED_FORWARD_MIGRATE
           : INIT_NEED_UNIMPLEMENTED_BACKWARD_MIGRATE;
}

void BtConfig::forceMigrate()
{ m_instance->setValue<int>(BTCONFIG_API_VERSION_KEY, BTCONFIG_API_VERSION); }

BtConfig& BtConfig::getInstance() {
    BT_ASSERT(m_instance && "BtConfig not yet initialized!");
    return *m_instance;
}

void BtConfig::destroyInstance() {
    delete m_instance;
    m_instance = nullptr;
}

void BtConfig::setModuleEncryptionKey(const QString & name,
                                      const QString & key)
{
    BT_ASSERT(!name.isEmpty());
    setValue("Module keys/" + name, key);
}

QString BtConfig::getModuleEncryptionKey(const QString & name) {
    BT_ASSERT(!name.isEmpty());
    return value<QString>("Module keys/" + name, QString());
}

BtConfig::ShortcutsMap BtConfig::getShortcuts(QString const & shortcutGroup) {
    beginGroup(shortcutGroup);
        ShortcutsMap allShortcuts;
        for (QString const & key : childKeys()) {
            QVariant variant = qVariantValue(key);

            QList<QKeySequence> shortcuts;

            if (variant.type() == QVariant::List) { // For BibleTime before 2.9
                for (QVariant const & shortcut : variant.toList())
                    shortcuts.append(shortcut.toString());
            } else if (variant.type() == QVariant::StringList
                       || variant.type() == QVariant::String)
            { // a StringList with one element is recognized as a QVariant::String
                for (QString const & shortcut : variant.toStringList())
                    shortcuts.append(shortcut);
            } else { // it's something we don't know, skip it
                continue;
            }

            allShortcuts.insert(key, shortcuts);
        }
    endGroup();
    return allShortcuts;
}

void BtConfig::setShortcuts(QString const & shortcutGroup,
                            ShortcutsMap const  & shortcuts)
{
    beginGroup(shortcutGroup);
        for (auto it = shortcuts.begin(); it != shortcuts.end(); ++it) {
            // Write beautiful string lists (since 2.9):
            /// \note saving QKeySequences directly doesn't appear to work!
            QStringList varList;
            for (QKeySequence const & shortcut : it.value())
                varList.append(shortcut.toString());

            if (!varList.empty())
                setValue(it.key(), varList);
        }
    endGroup();
}

FilterOptions BtConfig::getFilterOptions() {
    FilterOptions os;
    beginGroup("presentation");
    os.footnotes           = sessionValue<bool>("footnotes", true);
    os.strongNumbers       = sessionValue<bool>("strongNumbers", true);
    os.headings            = sessionValue<bool>("headings", true);
    os.morphTags           = sessionValue<bool>("morphTags", true);
    os.lemmas              = sessionValue<bool>("lemmas", true);
    os.redLetterWords      = sessionValue<bool>("redLetterWords", true);
    os.hebrewPoints        = sessionValue<bool>("hebrewPoints", true);
    os.hebrewCantillation  = sessionValue<bool>("hebrewCantillation", true);
    os.greekAccents        = sessionValue<bool>("greekAccents", true);
    os.textualVariants     = sessionValue<bool>("textualVariants", false);
    os.scriptureReferences = sessionValue<bool>("scriptureReferences", true);
    os.morphSegmentation   = sessionValue<bool>("morphSegmentation", true);
    endGroup();
    return os;
}

void BtConfig::setFilterOptions(const FilterOptions & os) {
    beginGroup("presentation");
    setSessionValue("footnotes", static_cast<bool>(os.footnotes));
    setSessionValue("strongNumbers", static_cast<bool>(os.strongNumbers));
    setSessionValue("headings", static_cast<bool>(os.headings));
    setSessionValue("morphTags", static_cast<bool>(os.morphTags));
    setSessionValue("lemmas", static_cast<bool>(os.lemmas));
    setSessionValue("redLetterWords", static_cast<bool>(os.redLetterWords));
    setSessionValue("hebrewPoints", static_cast<bool>(os.hebrewPoints));
    setSessionValue("hebrewCantillation", static_cast<bool>(os.hebrewCantillation));
    setSessionValue("greekAccents", static_cast<bool>(os.greekAccents));
    setSessionValue("textualVariants", static_cast<bool>(os.textualVariants));
    setSessionValue("scriptureReferences", static_cast<bool>(os.scriptureReferences));
    setSessionValue("morphSegmentation", static_cast<bool>(os.morphSegmentation));
    endGroup();
}

DisplayOptions BtConfig::getDisplayOptions() {
    DisplayOptions os;
    beginGroup("presentation");
    os.lineBreaks   = sessionValue<bool>("lineBreaks", false);
    os.verseNumbers = sessionValue<bool>("verseNumbers", true);
    endGroup();
    return os;
}

void BtConfig::setDisplayOptions(const DisplayOptions & os) {
    beginGroup("presentation");
    setSessionValue("lineBreaks", static_cast<bool>(os.lineBreaks));
    setSessionValue("verseNumbers", static_cast<bool>(os.verseNumbers));
    endGroup();
}

void BtConfig::setFontForLanguage(const CLanguageMgr::Language & language,
                                  const FontSettingsPair & fontSettings)
{
    auto fontAsString = fontSettings.second.toString();

    const QString & englishName = language.englishName();
    BT_ASSERT(!englishName.isEmpty());
    auto const & abbrev = language.abbrev();
    BT_ASSERT(!abbrev.isEmpty());

    QMutexLocker lock(&this->m_mutex);
    // write the language to the settings
    setValue("fonts/" + englishName, fontAsString);
    setValue("font standard settings/" + englishName, fontSettings.first);

    // (over-)write the language to the settings using abbreviation:
    setValue("fonts/" + abbrev, std::move(fontAsString));
    setValue("font standard settings/" + abbrev, fontSettings.first);

    // Remove language from the cache:
    m_fontCache.remove(&language);
}

BtConfig::FontSettingsPair BtConfig::getFontForLanguage(
        const CLanguageMgr::Language & language)
{
    QMutexLocker lock(&this->m_mutex);
    // Check the cache first:
    FontCacheMap::const_iterator it(m_fontCache.find(&language));
    if (it != m_fontCache.end())
        return *it;

    // Retrieve the font from the settings
    FontSettingsPair fontSettings;

    const QString & englishName = language.englishName();
    BT_ASSERT(!englishName.isEmpty());
    auto const & abbrev = language.abbrev();
    BT_ASSERT(!abbrev.isEmpty());

    auto v = qVariantValue("font standard settings/" + abbrev);
    if (v.canConvert<bool>()) {
        fontSettings.first = v.value<bool>();
    } else {
        fontSettings.first =
                value<bool>("font standard settings/" + englishName, false);
    }

    QFont font;
    if (fontSettings.first) {
        v = qVariantValue("fonts/" + abbrev);
        auto fontName =
                v.canConvert<QString>()
                ? v.value<QString>()
                : value<QString>("fonts/" + englishName,
                                 getDefaultFont().toString());
        if (!font.fromString(std::move(fontName))) {
            /// \todo
        }
    } else {
        font = getDefaultFont();
    }
    fontSettings.second = font;

    // Cache the value:
    m_fontCache.insert(&language, fontSettings);

    return fontSettings;
}

BtConfig::StringMap BtConfig::getSearchScopesForCurrentLocale(const QStringList& scopeModules) {
    StringMap map = value<BtConfig::StringMap>("properties/searchScopes", m_defaultSearchScopes);


    // Convert map to current locale:
    for (StringMap::Iterator it = map.begin(); it != map.end(); it++) {
        QString &data = it.value();
        sword::ListKey list = parseVerseListWithModules(data, scopeModules);
        data.clear();
        for (int i = 0; i < list.getCount(); i++) {
            data.append(QString::fromUtf8(list.getElement(i)->getRangeText()));
            data.append("; ");
        }
    }
    return map;
}

void BtConfig::setSearchScopesWithCurrentLocale(const QStringList& scopeModules, StringMap searchScopes) {
    /**
     * We want to make sure that the search scopes are saved with english
     * key names so loading them will always work with each locale set.
     */
    BtConfig::StringMap::Iterator iter = searchScopes.begin();
    while (iter != searchScopes.end()) {
        QString &data = iter.value();
        bool parsingWorked = true;
        sword::ListKey list = parseVerseListWithModules(data, scopeModules);
        data.clear();
        for (int i = 0; i < list.getCount(); i++) {
            sword::VerseKey * verse(dynamic_cast<sword::VerseKey *>(list.getElement(i)));

            if (verse != nullptr) {
                verse->setLocale("en");
                data.append(QString::fromUtf8(verse->getRangeText()));
                data.append(";");
            } else {
                parsingWorked = false;
                break;
            }
        }

        if (parsingWorked)
            iter++;
        else
            iter = searchScopes.erase(iter);
    }
    setValue("properties/searchScopes", searchScopes);
}

sword::ListKey BtConfig::parseVerseListWithModules(const QString& data, const QStringList& scopeModules) {

    Q_FOREACH(QString moduleName, scopeModules) {
        auto module = CSwordBackend::instance()->findModuleByName(moduleName);
        if (module == nullptr)
            continue;
        sword::VerseKey vk = module->module().getKey();
        sword::ListKey list(vk.parseVerseList(data.toUtf8(), "Genesis 1:1", true));
        if (list.getCount() > 0)
            return list;
    }
    return sword::ListKey();
}

void BtConfig::deleteSearchScopesWithCurrentLocale() {
    remove("properties/searchScopes");
}

CSwordModuleInfo *BtConfig::getDefaultSwordModuleByType(const QString & moduleType) {
    const QString moduleName = value<QString>("settings/defaults/" + moduleType, QString());
    if (moduleName.isEmpty())
        return nullptr;

    return CSwordBackend::instance()->findModuleByName(moduleName);
}

void BtConfig::setDefaultSwordModuleByType(const QString &moduleType,
                                           const CSwordModuleInfo * const module)
{
    setValue("settings/defaults/" + moduleType,
             module != nullptr ? module->name() : QString());
}

/**
  \todo -CDisplayWindow gets a construct method that reads from config and constructs and
        returns the respective child window (check whether module is installed...)
        -CDisplayWindows get a new variable "id" or something, which is a unique identifier.
        The path in the configuration will use this id as name. (who gives out the IDs?)
        -values are updated as they are changed, just like the rest of bibletime
        -QMdiArea::subWindowActivated signal will trigger reading the window order and saving
        it to the config.
        Action Plan:
        1. get current code to work with old session system
        2. move complete code over to BtConfig
        3. remove CBTConfig
        4. implement BtConfig infrastructure for saving window configuration
         - function to add a window
         - function to remove a window
         - specify how to save ordering
        5. change CDisplayWindows to write all state changes to the configuration
        6. implement BtConfig::readSession and callers
        7. make session handling code work with QSetting paths instead of properties
        8. add gui for new session handling
        9. remove old gui for session handling
*/
