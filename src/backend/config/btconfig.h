#ifndef BTCONFIG_H
#define BTCONFIG_H

#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSettings>

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
 * TODO: noncopy base class
 */
class BtConfig
{
private:
    BtConfig(QString settingsFile); //used by test
    BtConfig(const BtConfig& other) {} // stub, do not use
    BtConfig& operator=(const BtConfig& other) {return *this;} // stub, do not use

    friend class BtConfigTest;

    static BtConfig* m_instance; // singleton instance

    const static QString m_sessionsGroup;
    const static QString m_currentSessionKey;
    const static QString m_defaultSession;
    const static QString m_defaultSessionName;

    QHash<QString,QVariant> m_defaults;
    QSet<QString> m_sessionSettings;
    QSettings m_settings;
    QString m_currentSessionCache; // cache of the current session string, for speed
public:
    BtConfig& getInstance();
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
    void switchToSession(QString name);

    /*!
     * \brief Delete a session.
     *
     * Deletes the session with the given name.
     * This function will not delete the current session and return false
     * in that case. It also returns false if no session with the given name
     * exists.
     * \returns True if deletion was successful and false otherwise.
     */
    bool deleteSession(QString name);

    /*!
     * \brief Returns a value.
     *
     * Returns the value for the given key. Whether the key is part of
     * the session or not is determined automatically. If the value
     * has not been set before the default value is returned.
     * Getting values not part of the config is prohibited and will result
     * in an assertion.
     * \param[in] key Key to get the value for.
     * \returns QVariant of the value.
     */
    QVariant getValue(QString key);

    /*!
     * \brief Set a value.
     *
     * Sets a value for a key. Whether the key should be part of the session
     * or not is determined automatically. Setting values not part of the
     * config is prohibited and will result in an assertion.
     * The value type can be any type that is QVariant enabled. Those are the
     * default ones of QVariant (look at the Qt documentation), and those declared
     * in btconfigtypes.h.
     * \param[in] key Ket to set.
     * \tparam[in] value Value to set.
     */
    template<typename T>
    void setValue(QString key, T value);

    /*!
     * \brief Checks whether a value has been manually set.
     * \param[in] key Key to check for.
     */
    bool hasValue(QString key);

    /*!
     * \brief Delete a key.
     *
     * Deletes a key. It is unimportant whether a key has been set before.
     * When a key is deleted the key will from then on return the default
     * value again.
     *
     * \param[in] key Key to delete.
     */
    void deleteValue(QString key);
};

template<typename T>
void BtConfig::setValue(QString key, T value)
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
inline void BtConfig::setValue(QString key, const char* const value)
{
    setValue<QString>(key, QString(value));
}

#endif // BTCONFIG_H
