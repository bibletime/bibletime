/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btconfig.h"

#include <QLocale>
#include <QWebSettings>
#include "backend/btmoduletreeitem.h"
#include "backend/managers/cdisplaytemplatemgr.h"
#include "frontend/searchdialog/btsearchoptionsarea.h"
#include "util/directory.h" // DU::getUserBaseDir()

// Sword includes:
#include <backend/managers/cswordbackend.h>
#include <versekey.h> // For search scope configuration


/*
 * set the instance variable initially to 0, so it can be safely checked
 * whether the variable has been initialized yet.
 */
BtConfig* BtConfig::m_instance = 0;

BtConfig::StringMap BtConfig::m_defaultSearchScopes;


BtConfig::BtConfig(const QString &settingsFile)
    : BtConfigCore(settingsFile)
{
    if (m_defaultSearchScopes.isEmpty()) {
        m_defaultSearchScopes.insert(QObject::tr("Old testament"),          QString("Gen - Mal"));
        m_defaultSearchScopes.insert(QObject::tr("Moses/Pentateuch/Torah"), QString("Gen - Deut"));
        m_defaultSearchScopes.insert(QObject::tr("History"),                QString("Jos - Est"));
        m_defaultSearchScopes.insert(QObject::tr("Prophets"),               QString("Isa - Mal"));
        m_defaultSearchScopes.insert(QObject::tr("New testament"),          QString("Mat - Rev"));
        m_defaultSearchScopes.insert(QObject::tr("Gospels"),                QString("Mat - Joh"));
        m_defaultSearchScopes.insert(QObject::tr("Letters/Epistles"),       QString("Rom - Jude"));
        m_defaultSearchScopes.insert(QObject::tr("Paul's Epistles"),        QString("Rom - Phile"));
    }
}

BtConfig& BtConfig::getInstance()
{
    if(m_instance == 0)
        m_instance = new BtConfig(util::directory::getUserBaseDir().absolutePath() + "/bibletimerc.new");
    return *m_instance;
}

void BtConfig::destroyInstance() {
    delete m_instance;
    m_instance = 0;
}

void BtConfig::setModuleEncryptionKey(const QString &name, const QString &key) {
    Q_ASSERT(!name.isEmpty());
    m_settings.setValue("Module keys/" + name, key);
}

QString BtConfig::getModuleEncryptionKey(const QString &name) {
    Q_ASSERT(!name.isEmpty());
    return m_settings.value("Module keys/" + name, QVariant(QString::null)).toString();
}

QHash< QString, QList<QKeySequence> > BtConfig::getShortcuts( const QString& shortcutGroup )
{
    m_settings.beginGroup(shortcutGroup);
        QHash< QString, QList<QKeySequence> > allShortcuts;
        Q_FOREACH (const QString &key, m_settings.childKeys()) {
            QVariant variant = m_settings.value(key);

            QList<QKeySequence> shortcuts;

            if (variant.type() == QVariant::List) { // For BibleTime before 2.9
                Q_FOREACH (const QVariant &shortcut, variant.toList()) {
                    shortcuts.append(shortcut.toString());
                }
            } else if (variant.type() == QVariant::StringList || variant.type() == QVariant::String) { // a StringList with one element is recognized as a QVariant::String
                Q_FOREACH (const QString &shortcut, variant.toStringList()) {
                    shortcuts.append(shortcut);
                }
            } else { // it's something we don't know, skip it
                continue;
            }

            allShortcuts.insert(key, shortcuts);
        }
    m_settings.endGroup();
    return allShortcuts;
}

void BtConfig::setShortcuts( const QString& shortcutGroup, const QHash< QString, QList<QKeySequence> >& shortcuts )
{
    m_settings.beginGroup(shortcutGroup);
        for(QHash<QString, QList<QKeySequence> >::const_iterator iter = shortcuts.begin();
                                                                 iter != shortcuts.end();
                                                                 iter++)
        {
            // Write beautiful string lists (since 2.9):
            QStringList varList;
                Q_FOREACH (const QKeySequence &shortcut, iter.value()) {
                    /// \note saving QKeySequences directly doesn't appear to work!
                    varList.append(shortcut.toString());
                }
            if (not varList.empty())
            {
                m_settings.setValue(iter.key(), varList);
            }
        }
    m_settings.endGroup();
}

FilterOptions BtConfig::getFilterOptions()
{
    FilterOptions options;

    options.footnotes           = true; // Required for the info display
    options.strongNumbers       = true; // get(strongNumbers);
    options.headings            = sessionValue<int>("presentation/headings", true);
    options.morphTags           = true; // Required for the info display
    options.lemmas              = true; // Required for the info display
    options.redLetterWords      = true;
    options.hebrewPoints        = sessionValue<int>("presentation/hebrewPoints", true);
    options.hebrewCantillation  = sessionValue<int>("presentation/hebrewCantillation", true);
    options.greekAccents        = sessionValue<int>("presentation/greekAccents", true);
    options.textualVariants     = sessionValue<int>("presentation/textualVariants", false);
    options.scriptureReferences = sessionValue<int>("presentation/scriptureReferences", true);
    options.morphSegmentation   = sessionValue<int>("presentation/morphSegmentation", true);

    return options;
}

DisplayOptions BtConfig::getDisplayOptions()
{
    DisplayOptions options;
    options.lineBreaks   = sessionValue<int>("presentation/lineBreaks", false);
    options.verseNumbers = sessionValue<int>("presentation/verseNumbers", false);
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

    fontSettings.first = m_settings.value("font standard settings/" + language->name(), false).toBool();

    QFont font;
    if (fontSettings.first) {
        if (!font.fromString(m_settings.value("fonts/" + language->name(), getDefaultFont()).toString())) {
            /// \todo
        }
    } else {
        font = getDefaultFont();
    }
    fontSettings.second = font;

    // Cache the value:
    m_fontCache.insert(language, fontSettings);

    return fontSettings;
}

BtConfig::StringMap BtConfig::getSearchScopesForCurrentLocale() {
    StringMap map = value<BtConfig::StringMap>("properties/searchScopes", m_defaultSearchScopes);

    // Convert map to current locale:
    sword::VerseKey vk;
    for (StringMap::Iterator it = map.begin(); it != map.end(); it++) {
        QString &s = it.value();
        sword::ListKey list(vk.ParseVerseList(QByteArray(s.toUtf8()), "Genesis 1:1", true));
        s.clear();
        for (int i = 0; i < list.Count(); i++) {
            s.append(QString::fromUtf8(list.GetElement(i)->getRangeText()));
            s.append("; ");
        }
    }
    return map;
}

void BtConfig::setSearchScopesWithCurrentLocale(StringMap searchScopes) {
    /**
     * We want to make sure that the search scopes are saved with english
     * key names so loading them will always work with each locale set.
     */
    sword::VerseKey vk;
    BtConfig::StringMap::Iterator iter = searchScopes.begin();
    while (iter != searchScopes.end()) {
        QString &data = iter.value();
        bool parsingWorked = true;
        sword::ListKey list(vk.ParseVerseList(data.toUtf8(), "Genesis 1:1", true));
        data.clear();
        for (int i = 0; i < list.Count(); i++) {
            sword::VerseKey * verse(dynamic_cast<sword::VerseKey *>(list.getElement(i)));

            if (verse != 0) {
                verse->setLocale("en");
                data.append(QString::fromUtf8(verse->getRangeText()));
                data.append(";");
            }
            else {
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

void BtConfig::deleteSearchScopesWithCurrentLocale() {
    m_settings.remove("properties/searchScopes");
}

CSwordModuleInfo *BtConfig::getDefaultSwordModuleByType(const QString & moduleType) {
    const QString moduleName = value<QString>("settings/defaults/" + moduleType, QString());
    if (moduleName.isEmpty())
        return 0;

    return CSwordBackend::instance()->findModuleByName(moduleName);
}

void BtConfig::setDefaultSwordModuleByType(const QString &moduleType,
                                           const CSwordModuleInfo * const module)
{
    setValue("settings/defaults/" + moduleType,
             module != 0 ? module->name() : QString::null);
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
