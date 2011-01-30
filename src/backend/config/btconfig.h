#ifndef BTCONFIG_H
#define BTCONFIG_H

#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSettings>

#include "btglobal.h"
#include "util/nocopy.h"
#include "backend/btmoduletreeitem.h" // for BTModuleTreeItem::Grouping
#include "frontend/searchdialog/btsearchoptionsarea.h" // for Search::BtSearchOptionsArea::SearchType

namespace btconfiguration
{
typedef QPair<bool, QFont> FontSettingsPair;
typedef QMap<QString, QString> StringMap;
}

// declare types used in configuration as metatype so they can be saved directly into the configuration
Q_DECLARE_METATYPE(BTModuleTreeItem::Grouping);
Q_DECLARE_METATYPE(Search::BtSearchOptionsArea::SearchType);
Q_DECLARE_METATYPE(btconfiguration::StringMap);

namespace btconfiguration
{
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
private:
    explicit BtConfig(const QString& settingsFile); //used by BtConfigTest
    friend class BtConfigTest;

    static BtConfig* m_instance; // singleton instance

    const static QString m_sessionsGroup;
    const static QString m_currentSessionKey;
    const static QString m_defaultSessionName;

    QHash<QString,QVariant> m_defaults;
    QSet<QString> m_sessionSettings;
    QSettings m_settings;
    QString m_currentSessionCache; // cache of the current session string, for speed
    
    QFont *m_defaultFont;
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

    /*!
     * \brief Returns a value.
     *
     * Returns the value for the given key. Whether the key is part of
     * the session or not is determined automatically. If the value
     * has not been set before the default value is returned.
     * \param[in] key Key to get the value for.
     * \returns QVariant of the value.
     */
    QVariant getValue(const QString& key);

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

    QFont &getDefault(const CLanguageMgr::Language * const);
    void set(const CLanguageMgr::Language * const language, const FontSettingsPair &fontSettings);
    FontSettingsPair get(const CLanguageMgr::Language * const);

    void saveSearchScopes();
    void loadSearchScopes();
};

template<typename T>
void BtConfig::setValue(const QString& key, T value)
{
    //accessing session values directly is prohibited
    Q_ASSERT(not key.startsWith(m_sessionsGroup));
    Q_ASSERT(key != m_currentSessionKey);

    if(m_sessionSettings.contains(key))
        m_settings.setValue(m_currentSessionCache + key, QVariant::fromValue<T>(value));
    else
        m_settings.setValue(key, QVariant::fromValue<T>(value));
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
} //btconfiguration

#endif // BTCONFIG_H
