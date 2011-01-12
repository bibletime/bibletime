#include "btconfig.h"
#include "util/directory.h" // DU::getUserBaseDir()

#include <cstddef> // NULL macro
#include <QLocale>
#include <qglobal.h> // Q_ASSERT

#include "backend/managers/cdisplaytemplatemgr.h"
#include "btconfigtypes.h"
#include "backend/btmoduletreeitem.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"

/*
 * set the instance variable initially to NULL, so it can be safely checked
 * whether the variable has been initialized yet.
 */
BtConfig* BtConfig::m_instance = NULL;
const QString BtConfig::m_sessionsGroup = "sessions";
const QString BtConfig::m_currentSessionKey = "currentSession";
const QString BtConfig::m_defaultSession = QObject::tr("defaultSession");
const QString BtConfig::m_defaultSessionName = QObject::tr("default session");

BtConfig::BtConfig(const QString& settingsFile) : m_defaults(), m_sessionSettings(), m_settings(settingsFile, QSettings::IniFormat), m_currentSessionCache()
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
        m_defaults.insert("gui/alignmentMode", QVariant::fromValue(btconfigtypes::autoTileVertical));

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
        m_defaults.insert("state/tipNumber", 0);
        //TODO: continue here

        // TODO: these variables seems not to be used, check and remove
        /*
         m _defaults.insert("firstSearchDialog", true);
        m_defaults.insert("readOldBookmars", false);
        m_defaults.insert("mainIndex", true);
        m_defaults.insert("infoDisplay", true);
        */

        //TODO: save defaults somewhere so they can be loaded directly on next startup

    // initialize session settings
        m_sessionSettings.insert("showTextWindowHeaders");
        //TODO: continue here


    // make sure the current session key is set
        if(not m_settings.contains(m_currentSessionKey))
            m_settings.setValue(m_currentSessionKey, m_defaultSession);

    // initialize current session variable cache
        m_currentSessionCache = m_sessionsGroup + "/" + m_settings.value(m_currentSessionKey).toString() + "/";

    // make sure the current session has a name
        if(not m_settings.contains(m_currentSessionCache + "name"))
            m_settings.setValue(m_currentSessionCache + "name", m_defaultSessionName);
}

BtConfig::~BtConfig() {}

BtConfig& BtConfig::getInstance()
{
    if(m_instance == NULL)
        m_instance = new BtConfig(util::directory::getUserBaseDir().absolutePath() + "/bibletimerc");
    return *m_instance;
}

QString BtConfig::getCurrentSessionName()
{
    // every session must have a name at any time
        Q_ASSERT(m_settings.value(m_currentSessionCache + "name") != QVariant());

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
                    sessionKey = QString::number(i);
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
    return false;
}

QVariant BtConfig::getValue(const QString& key)
{
    //accessing session values directly is prohibited
        Q_ASSERT(not key.startsWith(m_sessionsGroup));
        Q_ASSERT(key != m_currentSessionKey);

    // if this fails some code is asking for a non existent configuration option
        Q_ASSERT(m_defaults.contains(key));

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
