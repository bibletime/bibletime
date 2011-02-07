#include "btconfig.h"
#include "util/directory.h" // DU::getUserBaseDir()

#include <cstddef> // NULL macro
#include <QLocale>
#include <QDebug>
#include <qglobal.h> // Q_ASSERT
#include <QWebSettings>

#include "backend/managers/cdisplaytemplatemgr.h"
#include "backend/btmoduletreeitem.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"

namespace btconfiguration
{
/*
 * set the instance variable initially to NULL, so it can be safely checked
 * whether the variable has been initialized yet.
 */
BtConfig* BtConfig::m_instance = NULL;
const QString BtConfig::m_sessionsGroup = "sessions";
const QString BtConfig::m_currentSessionKey = "currentSession";
const QString BtConfig::m_defaultSessionName = QObject::tr("default session");

BtConfig::BtConfig(const QString& settingsFile) : m_defaults(), m_sessionSettings(), m_settings(settingsFile, QSettings::IniFormat), m_currentSessionCache(), m_defaultFont(QWebSettings::globalSettings()->fontFamily(QWebSettings::StandardFont), 12)
{
    // construct defaults
        m_defaults.reserve(512); //TODO: check whether this value can be calculated automatically...

        m_defaults.insert("bibletimeVersion", "0.0");
        m_defaults.insert("language", QLocale::system().name());
        m_defaults.insert("gui/displayStyle", CDisplayTemplateMgr::defaultTemplate());
        m_defaults.insert("state/bookshelfCurrentItem", QVariant(QString::null));

        m_defaults.insert("settings/defaults/standardBible", "KJV");
        m_defaults.insert("settings/defaults/standardCommentary", "MHC");
        m_defaults.insert("settings/defaults/standardLexicon", "ISBE");
        m_defaults.insert("settings/defaults/standardDailyDevotional", "");
        m_defaults.insert("settings/defaults/standardHebrewStrongsLexicon", "StrongsHebrew");
        m_defaults.insert("settings/defaults/standardGreekStrongsLexicon", "StrongsGreek");
        m_defaults.insert("settings/defaults/standardHebrewMorphLexicon", "StrongsHebrew");
        m_defaults.insert("settings/defaults/standardGreekMorphLexicon", "StrongsGreek");

        m_defaults.insert("gui/showMainToolbar", true); // rename: toolbar -> showMainToolbar

        /*
        m_defaults.insert("gui/autoTileVertical", true);
        m_defaults.insert("gui/autoTileHorizontal", false);
        m_defaults.insert("gui/autoTile", false);
        m_defaults.insert("gui/autoTabbed", false);
        m_defaults.insert("gui/autoCascade", false);
        */
        m_defaults.insert("gui/alignmentMode", QVariant::fromValue(autoTileVertical));

        m_defaults.insert("presentation/lineBreaks", false);
        m_defaults.insert("presentation/verseNumbers", false);

        m_defaults.insert("gui/logo", true);
        m_defaults.insert("settings/behaviour/autoDeleteOrphanedIndices", true);
        m_defaults.insert("state/crashedLastTime", false);
        m_defaults.insert("state/crashedTwoTimes", false);
        m_defaults.insert("gui/bookshelfShowHidden", false);
        m_defaults.insert("settings/behaviour/allowNetworkConnection", false);
        m_defaults.insert("gui/showTextWindowHeaders", true);
        m_defaults.insert("gui/showTextWindowNavigator", true);
        m_defaults.insert("gui/showTextWindowToolButtons", true);
        m_defaults.insert("gui/showFormatToolbarButtons", true);
        m_defaults.insert("gui/showToolbarsInEachWindow", true);
        m_defaults.insert("gui/showTipAtStartup", true);

        // TODO: are the following booleans or ints, what do they do? rename to: show***?
        m_defaults.insert("gui/footnotes", true);
        m_defaults.insert("gui/strongNumbers", true);
        m_defaults.insert("gui/headings", true);
        m_defaults.insert("gui/morphTags", true);
        m_defaults.insert("gui/lemmas", true);
        m_defaults.insert("gui/hebrewPoints", true);
        m_defaults.insert("gui/hebrewCantillation", true);
        m_defaults.insert("gui/greekAccents", true);
        m_defaults.insert("gui/textualVariants", false);
        m_defaults.insert("gui/scriptureReferences", true);
        m_defaults.insert("gui/morphSegmentation", true);
        m_defaults.insert("gui/windows/bookshelfContentsX", 0);
        m_defaults.insert("gui/windows/bookshelfContentsY", 0);
        m_defaults.insert("gui/windows/magDelay", 400);
        m_defaults.insert("gui/windows/bookshelfGrouping", BTModuleTreeItem::CatLangMod);
        m_defaults.insert("gui/windows/searchDialogWidth", 200);
        m_defaults.insert("gui/windows/searchDialogHeight", 400);
        m_defaults.insert("gui/windows/searchDialogX", 200);
        m_defaults.insert("gui/windows/searchDialogY", 200);
        m_defaults.insert("gui/windows/searchType", Search::BtSearchOptionsArea::AndType);
        m_defaults.insert("gui/mainindexActiveTab", 0);
        m_defaults.insert("gui/installPathIndex", 0);
        m_defaults.insert("gui/windows/bookshelfPosX", 1);
        m_defaults.insert("gui/windows/bookshelfPosY", 1);
        m_defaults.insert("gui/windows/bookshelfHeight", 1);
        m_defaults.insert("gui/windows/bookshelfWidth", 1);
        m_defaults.insert("gui/windows/configDialogPosX", 1);
        m_defaults.insert("gui/windows/configDialogPosY", 1);
        m_defaults.insert("gui/windows/configDialogHeight", 1);
        m_defaults.insert("gui/windows/configDialogWidth", 1);
            QStringList list;
            list.append(QString::null);
        m_defaults.insert("properties/searchTexts", list);
        m_defaults.insert("history/searchCompletionTexts", QStringList());
        m_defaults.insert("history/bookshelfOpenGroups", QStringList());
        m_defaults.insert("state/hiddenModules", QStringList());
        m_defaults.insert("history/searchModuleHistory", QStringList());
            StringMap map;
            map.insert(QObject::tr("Old testament"),          QString("Gen - Mal"));
            map.insert(QObject::tr("Moses/Pentateuch/Torah"), QString("Gen - Deut"));
            map.insert(QObject::tr("History"),                QString("Jos - Est"));
            map.insert(QObject::tr("Prophets"),               QString("Isa - Mal"));
            map.insert(QObject::tr("New testament"),          QString("Mat - Rev"));
            map.insert(QObject::tr("Gospels"),                QString("Mat - Joh"));
            map.insert(QObject::tr("Letters/Epistles"),       QString("Rom - Jude"));
            map.insert(QObject::tr("Paul's Epistles"),        QString("Rom - Phile"));
        m_defaults.insert("properties/searchScopes", QVariant::fromValue(map));
        //TODO: continue here

        // TODO: these variables seem not to be used, check and remove
        /*
        m _defaults.insert("firstSearchDialog", true);
        m_defaults.insert("readOldBookmars", false);
        m_defaults.insert("mainIndex", true);
        m_defaults.insert("infoDisplay", true);
        */


    // initialize session settings
        m_sessionSettings.insert("showTextWindowHeaders");
        //TODO: continue here

    //TODO: save defaults somewhere so they can be loaded directly on next startup

    // make sure the current session key is set
        if(not m_settings.contains(m_currentSessionKey))
            switchToSession(m_defaultSessionName);
}

BtConfig::~BtConfig() {}

BtConfig& BtConfig::getInstance()
{
    if(m_instance == NULL)
        m_instance = new BtConfig(util::directory::getUserBaseDir().absolutePath() + "/bibletimerc.new");
    return *m_instance;
}

QString BtConfig::getCurrentSessionName()
{
    // every session must have a name at any time, this is an error in the config if this is not the case
    if(m_settings.value(m_currentSessionCache + "name") == QVariant())
    {
        qDebug() << "The session with key " << m_settings.value(m_currentSessionKey).toString() << " had no name associated, probably someone messed with the config. The default session name, \"" + m_defaultSessionName + "\", was set.";
        m_settings.setValue(m_currentSessionCache + "name", m_defaultSessionName);
    }

    return m_settings.value(m_currentSessionCache + "name").toString();
}

QStringList BtConfig::getAllSessionNames()
{
    m_settings.beginGroup(m_sessionsGroup);

    QStringList sessions = m_settings.childGroups();
    QStringList sessionNames;
    foreach(QString session, sessions)
        sessionNames.append(m_settings.value(session + "/name").toString());
    m_settings.endGroup();
    return sessionNames;
}

void BtConfig::switchToSession(const QString& name)
{
    m_settings.beginGroup(m_sessionsGroup);
    QStringList sessions = m_settings.childGroups();
    QString sessionKey = "invalid";

    //check whether the session already exists
        bool found = false;
        foreach(QString session, sessions)
        {
            if(m_settings.value(session + "/name") == name)
            {
                found = true;
                sessionKey = session;
                break;
            }
        }

    // session doesn't exist yet, create it
        if(not found)
        {
            for(int i = 0; i != 1000; i++) // noone will have 1000 sessions...
            {
                if(not sessions.contains(QString::number(i)))
                {
                    sessionKey = QString::number(i);
                    break;
                }
            }
            Q_ASSERT(sessionKey != "invalid");

            m_settings.setValue(sessionKey + "/name", name);
        }

    m_settings.endGroup();

    // switch to the session
        m_settings.setValue(m_currentSessionKey, sessionKey);
        m_currentSessionCache = m_sessionsGroup + "/" + sessionKey + "/";
}

bool BtConfig::deleteSession(const QString& name)
{
    m_settings.beginGroup(m_sessionsGroup);
    QStringList sessions = m_settings.childGroups();

    foreach(QString session, sessions)
    {
        if(m_settings.value(session + "/name") == name)
        {
            m_settings.endGroup();

            // we don't delete the current session
            if(m_settings.value(m_currentSessionKey) == session)
                return false;
            else
            {
                m_settings.remove(session);
                return true;
            }
        }
    }

    // no session with the name was found
    m_settings.endGroup();
    return false;
}

QVariant BtConfig::getValue(const QString& key)
{
    //accessing session values directly is prohibited
        Q_ASSERT(not key.startsWith(m_sessionsGroup));
        Q_ASSERT(key != m_currentSessionKey);

    // retrieve value from config
        if(m_sessionSettings.contains(key))
        {
            return m_settings.value(m_currentSessionCache + key, m_defaults.value(key));
        }
        else
            return m_settings.value(key, m_defaults.value(key));
}

bool BtConfig::hasValue(const QString& key)
{
    //accessing session values directly is prohibited
    Q_ASSERT(not key.startsWith(m_sessionsGroup));
    Q_ASSERT(key != m_currentSessionKey);

    if(m_sessionSettings.contains(key))
        return m_settings.contains(m_currentSessionCache + key);
    else
        return m_settings.contains(key);
}

void BtConfig::deleteValue(const QString& key)
{
    //accessing session values directly is prohibited
    Q_ASSERT(not key.startsWith(m_sessionsGroup));
    Q_ASSERT(key != m_currentSessionKey);

    if(m_sessionSettings.contains(key))
        m_settings.remove(m_currentSessionCache + key);
    else
        m_settings.remove(key);
}

void BtConfig::syncConfig() {
    m_settings.sync();
}

// Helper functions

void BtConfig::setModuleEncryptionKey(const QString& name, const QString& key)
{
    m_settings.beginGroup("Module keys");
        m_settings.setValue(name, key);
    m_settings.endGroup();
}

QString BtConfig::getModuleEncryptionKey(const QString& name)
{
    Q_ASSERT(not name.isEmpty());
    m_settings.beginGroup("Module keys");
        QString result(m_settings.value(name, QVariant(QString::null)).toString());
    m_settings.endGroup();
    return result;
}

QHash< QString, QList<QKeySequence> > BtConfig::getShortcuts( const QString& shortcutGroup )
{
    qDebug() << "BtConfig::readShortcuts begin";
    m_settings.beginGroup(shortcutGroup);
        QHash< QString, QList<QKeySequence> > allShortcuts;
        QStringList keyList = m_settings.childKeys();
        foreach(QString key, keyList) {
            QVariant variant = m_settings.value(key);
            QList<QKeySequence> shortcuts;
            if (variant != QVariant()) {
                QList<QVariant> keyShortcuts = variant.toList();
                foreach (QVariant shortcutVariant, keyShortcuts) {
                    QKeySequence shortcut(shortcutVariant.toString());
                    shortcuts.append(shortcut);
                }
            }
            allShortcuts.insert(key, shortcuts);
        }
    m_settings.endGroup();
    qDebug() << "BtConfig::readShortcuts end";
    return allShortcuts;
}

void BtConfig::setShortcuts( const QString& shortcutGroup, const QHash< QString, QList<QKeySequence> >& shortcuts )
{
    qDebug() << "BtConfig::setShortcuts begin";
    m_settings.beginGroup(shortcutGroup);
        for(QHash<QString, QList<QKeySequence> >::const_iterator iter = shortcuts.begin();
                                                                 iter != shortcuts.end();
                                                                 iter++)
        {
            QList<QVariant> varList;
                foreach(QKeySequence seq, iter.value())
                    varList.append(seq.toString());
            m_settings.setValue(iter.key(), varList);
        }
    m_settings.endGroup();
    qDebug() << "BtConfig::setShortcuts end";
}

FilterOptions BtConfig::getFilterOptions()
{
    FilterOptions options;

    options.footnotes           = true; // Required for the info display
    options.strongNumbers       = true; // get(strongNumbers);
    options.headings            = getValue("gui/headings").toInt();
    options.morphTags           = true; // Required for the info display
    options.lemmas              = true; // Required for the info display
    options.redLetterWords      = true;
    options.hebrewPoints        = getValue("gui/hebrewPoints").toInt();
    options.hebrewCantillation  = getValue("gui/hebrewCantillation").toInt();
    options.greekAccents        = getValue("gui/greekAccents").toInt();
    options.textualVariants     = getValue("gui/textualVariants").toInt();
    options.scriptureReferences = getValue("gui/scriptureReferences").toInt();
    options.morphSegmentation   = getValue("gui/morphSegmentation").toInt();

    return options;
}

DisplayOptions BtConfig::getDisplayOptions()
{
    DisplayOptions options;
    options.lineBreaks   = getValue("presentation/lineBreaks").toInt();
    options.verseNumbers = getValue("presentation/verseNumbers").toInt();
    return options;
}

void BtConfig::setFontForLanguage(const CLanguageMgr::Language* const language, const FontSettingsPair &fontSettings)
{
    Q_ASSERT(not language->name().isEmpty());

    // write the language to the settings
        m_settings.beginGroup("fonts");
            m_settings.setValue(language->name(), fontSettings.second.toString());
        m_settings.endGroup();

        m_settings.beginGroup("font standard settings");
            m_settings.setValue(language->name(), fontSettings.first);
        m_settings.endGroup();

    // Remove language from the cache:
        m_fontCache.remove(language);
}

BtConfig::FontSettingsPair BtConfig::getFontForLanguage(const CLanguageMgr::Language* const language)
{
    Q_ASSERT(not language->name().isEmpty());

    // Check the cache first:
        FontCacheMap::const_iterator it(m_fontCache.find(language));
        if (it != m_fontCache.end())
            return *it;

    // Retrieve the font from the settings
        FontSettingsPair fontSettings;

        m_settings.beginGroup("font standard settings");
            fontSettings.first = m_settings.value(language->name(), false).toBool();
        m_settings.endGroup();

        m_settings.beginGroup("fonts");
            QFont font;
            if (fontSettings.first) {
                font.fromString(m_settings.value(language->name(), getDefaultFont()).toString());
            }
            else {
                font = getDefaultFont();
            }
            fontSettings.second = font;
        m_settings.endGroup();

    // Cache the value
        m_fontCache.insert(language, fontSettings);

    return fontSettings;
}

void BtConfig::saveSearchScopes()
{

}

void BtConfig::loadSearchScopes()
{

}

} //btconfiguration