#ifndef BTCONFIG_H
#define BTCONFIG_H

#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMetaType>
#include <QSettings>

#include "btglobal.h"
#include "util/nocopy.h"
#include "backend/btmoduletreeitem.h" // for BTModuleTreeItem::Grouping
#include "frontend/searchdialog/btsearchoptionsarea.h" // for Search::BtSearchOptionsArea::SearchType
#include "backend/drivers/cswordmoduleinfo.h"

/*!
 * \brief Class holding and managing the configuration of bibletime.
 *
 * This class manages the configuration of bibletime.
 * It is based on QSettings. It abstracts QSettings to automatically provide
 * default values for all values and manage sessions. Whether a variable is
 * session specific or not is decided in this class and transparent to the users
 * of this class.
 *
 * TODO: preserve session order
 */
class BtConfig : private NoCopy
{
public:
    /*!
    * The first parameter indicates whether the custom font should be used or not.
    * The second parameter is the custom font that has been set.
    */
    typedef QPair<bool, QFont> FontSettingsPair;
    typedef QMap<QString, QString> StringMap;
private:
    explicit BtConfig(const QString& settingsFile); //!< used by BtConfigTest
    friend class BtConfigTest;

    static BtConfig* m_instance; //!< singleton instance

    const static QString m_sessionsGroup;
    const static QString m_currentSessionKey;
    const static QString m_defaultSessionName;

    QVector<QString> m_currentGroups;

    QHash<QString,QVariant> m_defaults;
    QSet<QString> m_sessionSettings;
    QSettings m_settings;

    QString m_currentSessionCache; //!< cache of the current session string, for speed
    QFont m_defaultFont; //!< default font used when no special one is set
    typedef QHash<const CLanguageMgr::Language*, FontSettingsPair> FontCacheMap;
    FontCacheMap m_fontCache; //!< a cache for the fonts saved in the configuration file for speed
public:

    
    static BtConfig& getInstance();
    ~BtConfig();

    /*!
     * \brief Returns the name of the current session.
     * \returns Name of the current session.
     */
    QString getCurrentSessionName();

    /*!
     * \returns List of all session names.
     */
    QStringList getAllSessionNames();

    /*!
     * \brief Switches to a session.
     *
     * This function will switch to the session with the name given.
     * If no such session exists the session will be created.
     *
     * \param[in] name Name of session to switch to.
     */
    void switchToSession(const QString& name);

    /*!
     * \brief Delete a session.
     *
     * Deletes the session with the given name.
     * This function will not delete the current session and return false
     * in that case. It also returns false if no session with the given name
     * exists.
     * \returns True if deletion was successful and false otherwise.
     */
    bool deleteSession(const QString& name);

    //TODO: template this
    //TODO: make session specific check also work for groups (check for / in path and check for sub parts in session qhash)
    /*!
     * \brief Returns a value.
     *
     * Returns the value for the given key. Whether the key is part of
     * the session or not is determined automatically. If the value
     * has not been set before the default value is returned.
     * The type that is returned can be set via the template parameter,
     * it is necessary, that the respective property can be cast to this
     * type.
     * \param[in] key Key to get the value for.
     * \tparam T value type to return
     * \returns QVariant of the value.
     */
    template<typename T>
    T getValue(const QString& key);

    /*!
     * \brief Set a value.
     *
     * Sets a value for a key. Whether the key should be part of the session
     * or not is determined automatically.
     * The value type can be any type that is QVariant enabled. Those are the
     * default ones of QVariant (look at the Qt documentation), and those declared
     * in btglobal.h.
     * \param[in] key Ket to set.
     * \tparam[in] value Value to set.
     */
    template<typename T>
    void setValue(const QString& key, T value);

    /*!
     * \brief Checks whether a value has been manually set.
     * \param[in] key Key to check for.
     */
    bool hasValue(const QString& key);

    /*!
     * \brief Delete a key.
     *
     * Deletes a key. It is unimportant whether a key has been set before.
     * When a key is deleted the key will from then on return the default
     * value again.
     *
     * \param[in] key Key to delete.
     */
    void deleteValue(const QString& key);

    /*!
     * \brief Synchronize the underlying QSettings.
     */
    void syncConfig();

    /*!
     * \brief Begin a configuration group.
     *
     * All following calls to get/setValue() will have the given prefix automatically
     * prefixed.
     * \post You *must* call endGroup() after you are done with the group, otherwise
     * you will mess up all calls to get/setValue() by others.
     * \param[in] prefix the prefix to use
     */
    inline void beginGroup(const QString& prefix);

    /*!
     * \brief Ends a previously set configuration group.
     *
     * Call this function after you are done with a started group. Every call to
     * beginGroup() must be matched with a call to this function.
     */
    void endGroup();

    /*!
     * \brief Returns the current group.
     *
     * Returns the group the BtConfig is currently set to. It will contain a trailing /
     * so is suitable to be preprended to a key directly.
     *
     * \returns group string or "" if group is empty
     */
    QString getGroup();


    // helper functions

    /*!
     * \brief Function to set a module decryption key.
     *
     * This helper function will set a module decryption key
     * in the configuration. Any previous key will be overwritten.
     *
     * \param[in] name Name of module to set the key for
     * \param[in] key Decryption key to set as string
     */
    void setModuleEncryptionKey(const QString &name, const QString &key);

    /*!
     * \brief Function to get a module decryption key.
     *
     * This helper function will retrieve a previously set
     * module decryption key from the configuration. If the key
     * is not set it will return a null string.
     *
     * \param[in] name Name of module to retrieve the key for
     * \returns Decryption key as a string
     */
    QString getModuleEncryptionKey(const QString &name);

    /*!
     * \brief Gets the shortcuts for the given group.
     *
     * Returns a hash of shortcuts for strings for the respective
     * shortcut group.
     * \param[in] shortcutGroup The group to retrieve shortcuts for.
     * \returns Hash of strings and lists of shortcuts.
     */
    QHash< QString, QList<QKeySequence> > getShortcuts(const QString& shortcutGroup);

    /*!
     * \brief Sets the shortcuts for the given group.
     *
     * Writes a hash of shortcuts for strings for the respective
     * shortcut group.
     * \param[in] shortcutGroup The group to retrieve shortcuts for.
     * \param[in] Hash of strings and lists of shortcuts to write.
     */
    void setShortcuts( const QString& shortcutGroup, const QHash< QString, QList< QKeySequence > >& shortcuts);

    /*!
     * \brief Returns current filter options.
     *
     * Returns a structure containing the current
     * settings to be used for filtering.
     * 
     * \returns FilterOptions structure containing filter settings.
     */
    FilterOptions getFilterOptions();

    /*!
     * \brief Returns current display options.
     *
     * Returns a structure containing the current
     * settings to be used for displaying texts.
     * 
     * \returns DisplayOptions structure containing display settings.
     */
    DisplayOptions getDisplayOptions();

    /*!
     * Returns a default font that is suitable for the current language.
     * \returns QFont suitable for current language.
     */
    inline const QFont &getDefaultFont() const;

    //TODO: put FontSettingsPair in QVariant directly
    /*!
     * \brief Set font for a language.
     *
     * Sets a FontSettingsPair for the language given.
     * \param[in] language pointer to a language to set the font for
     * \param[in] fontSettings FontSettingsPair to set
     */
    void setFontForLanguage(const CLanguageMgr::Language * const language, const FontSettingsPair &fontSettings);

    /*!
     * \brief Get font for a language.
     *
     * Gets a FontSettingsPair for the language given. If no font has been saved
     * a default font is returned.
     * \param[in] language pointer to a language to get the font for.
     * \returns FontSettingsPair for given language
     */
    FontSettingsPair getFontForLanguage(const CLanguageMgr::Language * const language);

    //TODO: unit test these functions
    /*!
     * Returns the searchScopes for the current locale.
     *
     * This function retrieves the search scopes of the
     * "properties/searchScopes" property and converts them
     * to the current locale.
     *
     * \returns Search scopes in current locale.
     */
    StringMap getSearchScopesForCurrentLocale();

    /*!
     * Sets the searchScopes given in the current locale.
     *
     * This function sets the search scopes of the
     * "properties/searchScopes" property, the scopes are
     * converted to the english locale before saving them.
     *
     * \param[in] searchScopes Search scopes in any locale.
     */
    void setSearchScopesWithCurrentLocale(StringMap searchScopes);

    /*!
     * \brief Returns default sword module info class for a given module type.
     *
     * This is basically a convenience function for getting the respective
     * "settings/defaults/ *" variable and searching that module manually.
     * If module is not installed 0 will be returned.
     * \param[in] moduleType module type to return the default sword module info for
     * \returns sword module info pointer or 0
     */
    CSwordModuleInfo* getDefaultSwordModuleByType(const QString& moduleType);

    /*!
     * \brief Sets the default sword module for a module type.
     *
     * This is basically a convenience function for setting the "settings/defaults/ *"
     * variables to the module->name() string manually.
     * 0 is allowed as the module, then the default module will be unset.
     * \param[in] moduleType module type to set
     * \param[in] module the sword module info to set as default module
     */
    void setDefaultSwordModuleByType(const QString& moduleType, const CSwordModuleInfo* const module);

    void readSession();
};

/*!
 * Returns the BtConfiguration singleton instance.
 * This is a shortcut for BtConfig::getInstance().
 * \returns BtConfig instance
 */
inline BtConfig& getBtConfig()
{
    return BtConfig::getInstance();
}

template<typename T>
T BtConfig::getValue(const QString& key)
{
    QString fullKey = getGroup() + key;
    //accessing session values directly is prohibited
        Q_ASSERT(not fullKey.startsWith(m_sessionsGroup));
        Q_ASSERT(fullKey != m_currentSessionKey);

    // accessing values not listed in defaults is prohibited (this effectively prevents typos)
        Q_ASSERT_X(m_defaults.contains(fullKey), "BtConfig", QString(fullKey + " was read accessed, this is a nonexisting and thus prohibited key.").toLatin1());

    // retrieve value from config
        if(m_sessionSettings.contains(fullKey))
        {
            return m_settings.value(m_currentSessionCache + fullKey, m_defaults.value(fullKey)).value<T>();
        }
        else
            return m_settings.value(fullKey, m_defaults.value(fullKey)).value<T>();
}

template<typename T>
void BtConfig::setValue(const QString& key, T value)
{
    QString fullKey = getGroup() + key;
    //accessing session values directly is prohibited
        Q_ASSERT(not fullKey.startsWith(m_sessionsGroup));
        Q_ASSERT(fullKey != m_currentSessionKey);

    // accessing values not listed in defaults is prohibited (this effectively prevents typos)
        Q_ASSERT_X(m_defaults.contains(fullKey), "BtConfig", QString(fullKey + " was write accessed, this is a nonexisting and thus prohibited key.").toLatin1());

    if(m_sessionSettings.contains(fullKey))
        m_settings.setValue(m_currentSessionCache + fullKey, QVariant::fromValue<T>(value));
    else
        m_settings.setValue(fullKey, QVariant::fromValue<T>(value));
}

/*
 * This specialization allows implicit conversion of char* to QString.
 * Full function specializations have to be inline, since they are no template
 * functions anymore and would result in multiple definition errors.
 */
template <>
inline void BtConfig::setValue(const QString& key, const char* const value)
{
    setValue<QString>(key, QString(value));
}

void BtConfig::beginGroup(const QString& prefix)
{
    m_currentGroups.push_back(prefix + "/");
}

const QFont &BtConfig::getDefaultFont() const
{
    return m_defaultFont;
}

// declare types used in configuration as metatype so they can be saved directly into the configuration
Q_DECLARE_METATYPE(BTModuleTreeItem::Grouping);
Q_DECLARE_METATYPE(Search::BtSearchOptionsArea::SearchType);
Q_DECLARE_METATYPE(BtConfig::StringMap);

// operator<</operator>> are needed for QVariant to be able to save it's contents to a text file, see documentation for qRegisterMetaTypeStreamOperators()
QDataStream &operator<<(QDataStream &out, const Search::BtSearchOptionsArea::SearchType &searchType);
QDataStream &operator>>(QDataStream &in, Search::BtSearchOptionsArea::SearchType &searchType);

QDataStream &operator<<(QDataStream &out, const alignmentMode &x);
QDataStream &operator>>(QDataStream &in, alignmentMode &x);

QDataStream &operator<<(QDataStream &out, const BTModuleTreeItem::Grouping &x);
QDataStream &operator>>(QDataStream &in, BTModuleTreeItem::Grouping &x);

#endif // BTCONFIG_H
