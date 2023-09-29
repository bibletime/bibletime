/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btconfig.h"

#include <cstddef>
#include <limits>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QKeySequence>
#include <QLocale>
#include <QSettings>
#include <QVariant>
#include <memory>
#include <type_traits>
#include <utility>
#include "../../util/btassert.h"
#include "../../util/directory.h"
#include "../btglobal.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../language.h"
#include "../managers/cswordbackend.h"


// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swkey.h>
#include <swmodule.h>
#include <listkey.h>
#include <versekey.h> // For search scope configuration
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


#define BTCONFIG_API_VERSION 1
namespace {
auto const BTCONFIG_API_VERSION_KEY = QStringLiteral("btconfig_api_version");
auto const GROUP_SESSIONS = QStringLiteral("sessions");
auto const GROUP_SESSIONS_PREFIX = QStringLiteral("sessions/");
auto const KEY_CURRENT_SESSION = QStringLiteral("sessions/currentSession");
auto const KEY_SESSION_NAME = QStringLiteral("sessions/%1/name");
} // anonymous namespace

/*
 * set the instance variable initially to 0, so it can be safely checked
 * whether the variable has been initialized yet.
 */
BtConfig * BtConfig::m_instance = nullptr;

BtConfig::StringMap BtConfig::m_defaultSearchScopes;


BtConfig::BtConfig(const QString & settingsFile)
    : BtConfigCore(
          std::make_shared<QSettings>(settingsFile, QSettings::IniFormat))
{
    BT_ASSERT(!m_instance && "BtConfig already initialized!");
    m_instance = this;

    if (m_defaultSearchScopes.isEmpty()) {
        m_defaultSearchScopes.insert(QT_TR_NOOP("Old testament"),
                                     QStringLiteral("Gen - Mal"));
        m_defaultSearchScopes.insert(QT_TR_NOOP("Moses/Pentateuch/Torah"),
                                     QStringLiteral("Gen - Deut"));
        m_defaultSearchScopes.insert(QT_TR_NOOP("History"),
                                     QStringLiteral("Jos - Est"));
        m_defaultSearchScopes.insert(QT_TR_NOOP("Prophets"),
                                     QStringLiteral("Isa - Mal"));
        m_defaultSearchScopes.insert(QT_TR_NOOP("New testament"),
                                     QStringLiteral("Mat - Rev"));
        m_defaultSearchScopes.insert(QT_TR_NOOP("Gospels"),
                                     QStringLiteral("Mat - Joh"));
        m_defaultSearchScopes.insert(QT_TR_NOOP("Letters/Epistles"),
                                     QStringLiteral("Rom - Jude"));
        m_defaultSearchScopes.insert(QT_TR_NOOP("Paul's Epistles"),
                                     QStringLiteral("Rom - Phile"));
    }

#ifdef Q_OS_WIN
    const double minPointSize = 14.0;
    double pointSize = m_defaultFont.pointSizeF();
    if (pointSize < minPointSize)
        m_defaultFont.setPointSizeF(minPointSize);
#endif

    // Read all session keys and names:
    auto sessionsConf = group(GROUP_SESSIONS);
    for (auto && sessionKey : sessionsConf.childGroups()) {
        // Skip empty//keys just in case:
        if (sessionKey.isEmpty())
            continue;

        auto sessionName =
                sessionsConf.value<QString>(
                    sessionKey + QStringLiteral("/name"));
        if (!sessionName.isEmpty())
            m_sessionNames.insert(std::move(sessionKey),
                                  std::move(sessionName));
    }

    // Get current session key:
    m_currentSessionKey = value<QString>(KEY_CURRENT_SESSION);

    /*
      If no session with the current session key exists, default to the first
      session found. If no sessions were found, create a default session.
    */
    if (m_currentSessionKey.isEmpty()
        || !m_sessionNames.contains(m_currentSessionKey))
    {
        if (m_sessionNames.isEmpty()) {
            m_currentSessionKey = QString::number(0, 36);
            setValue(KEY_CURRENT_SESSION, m_currentSessionKey);
            setValue(KEY_SESSION_NAME.arg(m_currentSessionKey),
                     tr("Default Session"));
        } else {
            m_currentSessionKey = m_sessionNames.keys().first();
        }
    }
}

BtConfig::InitState BtConfig::initBtConfig() {
    BT_ASSERT(!m_instance);

    const QString confFileName = util::directory::getUserBaseDir().absolutePath()
                                 + QStringLiteral("/bibletimerc");
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

void BtConfig::setCurrentSession(QString const & key) {
    BT_ASSERT(!key.isEmpty());
    BT_ASSERT(m_sessionNames.contains(key));
    m_currentSessionKey = key;

    setValue(KEY_CURRENT_SESSION, key);
}

QString BtConfig::addSession(QString const & name) {
    BT_ASSERT(!name.isEmpty());

    // Generate a new session key:
    QString key = QString::number(0u, 36);
    if (m_sessionNames.contains(key)) {
        QString keyPrefix;
        std::size_t i = 1u;
        for (;;) {
            key = QString::number(i, 36);
            if (!m_sessionNames.contains(keyPrefix + key))
                break;
            if (i == std::numeric_limits<std::size_t>::max()) {
                i = 0u;
                keyPrefix.append('_');
            } else {
                i++;
            }
        }
    }
    BT_ASSERT(!m_sessionNames.contains(key));
    m_sessionNames.insert(key, name);

    setValue(KEY_SESSION_NAME.arg(key), name);
    return key;
}


void BtConfig::deleteSession(QString const & key) {
    BT_ASSERT(m_sessionNames.contains(key));
    BT_ASSERT(key != m_currentSessionKey);
    m_sessionNames.remove(key);

    remove(GROUP_SESSIONS_PREFIX + key);
}

BtConfigCore BtConfig::session() const
{ return group(GROUP_SESSIONS_PREFIX + m_currentSessionKey); }

void BtConfig::destroyInstance() {
    delete m_instance;
    m_instance = nullptr;
}

void BtConfig::setModuleEncryptionKey(const QString & name,
                                      const QString & key)
{
    BT_ASSERT(!name.isEmpty());
    setValue(QStringLiteral("Module keys/") + name, key);
}

QString BtConfig::getModuleEncryptionKey(const QString & name) {
    BT_ASSERT(!name.isEmpty());
    return value<QString>(QStringLiteral("Module keys/") + name, QString());
}

BtConfig::ShortcutsMap BtConfig::getShortcuts(QString const & shortcutGroup) {
    ShortcutsMap allShortcuts;
    auto shortcutsConf = group(shortcutGroup);
    for (QString const & key : shortcutsConf.childKeys()) {
        auto const variant = shortcutsConf.qVariantValue(key);

        QList<QKeySequence> shortcuts;
        #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        if (variant.type() == QVariant::List) { // For BibleTime before 2.9
        #else
        auto const typeId = variant.typeId();
        if (typeId == QMetaType::QVariantList) { // For BibleTime before 2.9
        #endif
            for (QVariant const & shortcut : variant.toList())
                shortcuts.append(shortcut.toString());
        #if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        } else if (variant.type() == QVariant::StringList
                   || variant.type() == QVariant::String)
        #else
        } else if (typeId == QMetaType::QStringList
                   || typeId == QMetaType::QString)
        #endif
        { // a StringList with one element is recognized as a QVariant::String
            for (QString const & shortcut : variant.toStringList())
                shortcuts.append(shortcut);
        } else { // it's something we don't know, skip it
            continue;
        }

        allShortcuts.insert(key, shortcuts);
    }
    return allShortcuts;
}

void BtConfig::setShortcuts(QString const & shortcutGroup,
                            ShortcutsMap const  & shortcuts)
{
    auto shortcutsConf = group(shortcutGroup);
    for (auto it = shortcuts.begin(); it != shortcuts.end(); ++it) {
        // Write beautiful string lists (since 2.9):
        /// \note saving QKeySequences directly doesn't appear to work!
        QStringList varList;
        for (QKeySequence const & shortcut : it.value())
            varList.append(shortcut.toString());

        if (!varList.empty())
            shortcutsConf.setValue(it.key(), varList);
    }
}

FilterOptions BtConfig::loadFilterOptionsFromGroup(BtConfigCore const & group) {
    FilterOptions os;
    auto const subConf = group.group(QStringLiteral("presentation"));
    os.footnotes           = subConf.value<bool>(QStringLiteral("footnotes"), true);
    os.strongNumbers       = subConf.value<bool>(QStringLiteral("strongNumbers"), true);
    os.headings            = subConf.value<bool>(QStringLiteral("headings"), true);
    os.morphTags           = subConf.value<bool>(QStringLiteral("morphTags"), true);
    os.lemmas              = subConf.value<bool>(QStringLiteral("lemmas"), true);
    os.redLetterWords      = subConf.value<bool>(QStringLiteral("redLetterWords"), true);
    os.hebrewPoints        = subConf.value<bool>(QStringLiteral("hebrewPoints"), true);
    os.hebrewCantillation  = subConf.value<bool>(QStringLiteral("hebrewCantillation"), true);
    os.greekAccents        = subConf.value<bool>(QStringLiteral("greekAccents"), true);
    os.textualVariants     = subConf.value<bool>(QStringLiteral("textualVariants"), false);
    os.scriptureReferences = subConf.value<bool>(QStringLiteral("scriptureReferences"), true);
    os.morphSegmentation   = subConf.value<bool>(QStringLiteral("morphSegmentation"), true);
    return os;
}

void BtConfig::storeFilterOptionsToGroup(FilterOptions const & os,
                                         BtConfigCore & group)
{
    auto subConf = group.group(QStringLiteral("presentation"));
    subConf.setValue(QStringLiteral("footnotes"), static_cast<bool>(os.footnotes));
    subConf.setValue(QStringLiteral("strongNumbers"), static_cast<bool>(os.strongNumbers));
    subConf.setValue(QStringLiteral("headings"), static_cast<bool>(os.headings));
    subConf.setValue(QStringLiteral("morphTags"), static_cast<bool>(os.morphTags));
    subConf.setValue(QStringLiteral("lemmas"), static_cast<bool>(os.lemmas));
    subConf.setValue(QStringLiteral("redLetterWords"), static_cast<bool>(os.redLetterWords));
    subConf.setValue(QStringLiteral("hebrewPoints"), static_cast<bool>(os.hebrewPoints));
    subConf.setValue(QStringLiteral("hebrewCantillation"), static_cast<bool>(os.hebrewCantillation));
    subConf.setValue(QStringLiteral("greekAccents"), static_cast<bool>(os.greekAccents));
    subConf.setValue(QStringLiteral("textualVariants"), static_cast<bool>(os.textualVariants));
    subConf.setValue(QStringLiteral("scriptureReferences"), static_cast<bool>(os.scriptureReferences));
    subConf.setValue(QStringLiteral("morphSegmentation"), static_cast<bool>(os.morphSegmentation));
}

DisplayOptions
BtConfig::loadDisplayOptionsFromGroup(BtConfigCore const & group) {
    DisplayOptions os;
    auto const subConf = group.group(QStringLiteral("presentation"));
    os.lineBreaks   = subConf.value<bool>(QStringLiteral("lineBreaks"), false);
    os.verseNumbers = subConf.value<bool>(QStringLiteral("verseNumbers"), true);
    return os;
}

void BtConfig::storeDisplayOptionsToGroup(DisplayOptions const & os,
                                          BtConfigCore & group)
{
    auto subConf = group.group(QStringLiteral("presentation"));
    subConf.setValue(QStringLiteral("lineBreaks"),
                     static_cast<bool>(os.lineBreaks));
    subConf.setValue(QStringLiteral("verseNumbers"),
                     static_cast<bool>(os.verseNumbers));
}

void BtConfig::setFontForLanguage(Language const & language,
                                  FontSettingsPair const & fontSettings)
{
    auto fontAsString = fontSettings.second.toString();

    const QString & englishName = language.englishName();
    BT_ASSERT(!englishName.isEmpty());

    // write the language to the settings
    setValue(QStringLiteral("fonts/") + englishName, fontAsString);
    setValue(QStringLiteral("font standard settings/") + englishName,
             fontSettings.first);

    auto const & abbrev = language.abbrev();
    BT_ASSERT(!abbrev.isEmpty());

    // (over-)write the language to the settings using abbreviation:
    setValue(QStringLiteral("fonts/") + abbrev, std::move(fontAsString));
    setValue(QStringLiteral("font standard settings/") + abbrev,
             fontSettings.first);

    // Update cache:
    m_fontCache[&language] = fontSettings;
}

BtConfig::FontSettingsPair
BtConfig::getFontForLanguage(Language const & language) {
    // Check the cache first:
    auto it(m_fontCache.find(&language));
    if (it != m_fontCache.end())
        return *it;

    // Retrieve the font from the settings
    FontSettingsPair fontSettings;

    const QString & englishName = language.englishName();
    BT_ASSERT(!englishName.isEmpty());
    auto const & abbrev = language.abbrev();
    BT_ASSERT(!abbrev.isEmpty());

    if (auto const v =
            qVariantValue(QStringLiteral("font standard settings/") + abbrev);
        v.canConvert<bool>())
    {
        fontSettings.first = v.value<bool>();
    } else {
        fontSettings.first =
                value<bool>(
                    QStringLiteral("font standard settings/") + englishName,
                    false);
    }

    QFont font;
    if (fontSettings.first) {
        auto const v = qVariantValue(QStringLiteral("fonts/") + abbrev);
        auto fontName =
                v.canConvert<QString>()
                ? v.value<QString>()
                : value<QString>(QStringLiteral("fonts/") + englishName,
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
    auto const storedMap =
            value<BtConfig::StringMap>(
                QStringLiteral("properties/searchScopes"),
                m_defaultSearchScopes);
    StringMap map;

    // Apply translation for default search scope names:
    for (auto it = storedMap.cbegin(); it != storedMap.cend(); ++it) {
        if (m_defaultSearchScopes.contains(it.key())) {
            map.insert(tr(it.key().toUtf8()), it.value());
        } else {
            map.insert(it.key(), it.value());
        }
    }

    // Convert map to current locale:
    static auto const separator = QStringLiteral("; ");
    for (auto & data : map) {
        sword::ListKey list = parseVerseListWithModules(data, scopeModules);
        data.clear();
        for (int i = 0; i < list.getCount(); i++) {
            data.append(QString::fromUtf8(list.getElement(i)->getRangeText()));
            data.append(separator);
        }
    }
    return map;
}

void BtConfig::setSearchScopesWithCurrentLocale(const QStringList& scopeModules, StringMap searchScopes) {
    /**
     * We want to make sure that the search scopes are saved with english
     * key names so loading them will always work with each locale set.
     */
    auto iter(searchScopes.begin());
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
                data.append(';');
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
    setValue(QStringLiteral("properties/searchScopes"), searchScopes);
}

QString BtConfig::booknameLanguage() {
    static auto const key = QStringLiteral("GUI/booknameLanguage");
    auto r = value<QString>(key, QLocale().name());

    // Maintain backwards compatibility with BibleTime versions older than 3.1:
    bool const updateConfig = r.contains('_');
    r.replace('_', '-'); // BCP 47
    if (updateConfig)
        setValue(key, r);

    return r;
}

sword::ListKey BtConfig::parseVerseListWithModules(const QString& data, const QStringList& scopeModules) {
    for (auto const & moduleName : scopeModules) {
        auto module = CSwordBackend::instance().findModuleByName(moduleName);
        if (module == nullptr)
            continue;
        sword::VerseKey vk = module->swordModule().getKey();
        sword::ListKey list(vk.parseVerseList(data.toUtf8(), "Genesis 1:1", true));
        if (list.getCount() > 0)
            return list;
    }
    return sword::ListKey();
}

void BtConfig::deleteSearchScopesWithCurrentLocale() {
    remove(QStringLiteral("properties/searchScopes"));
}

CSwordModuleInfo *BtConfig::getDefaultSwordModuleByType(const QString & moduleType) {
    auto const moduleName =
            value<QString>(QStringLiteral("settings/defaults/") + moduleType);
    if (moduleName.isEmpty())
        return nullptr;

    return CSwordBackend::instance().findModuleByName(moduleName);
}

void BtConfig::setDefaultSwordModuleByType(const QString &moduleType,
                                           const CSwordModuleInfo * const module)
{
    setValue(QStringLiteral("settings/defaults/") + moduleType,
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
