/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef BTCONFIGCORE_H
#define BTCONFIGCORE_H

#include <QSettings>

#include <QCoreApplication>
#include <QStringList>


class BtConfigCore {

    Q_DISABLE_COPY(BtConfigCore)
    Q_DECLARE_TR_FUNCTIONS(BtConfigCore)

public: /* Methods: */

    explicit BtConfigCore(const QString & settingsFile);
    inline ~BtConfigCore() { sync(); }

    /*!
     * \returns the name of the current session.
     */
    QString getCurrentSessionName();

    /*!
     * \returns a list of all session names.
     */
    QStringList getAllSessionNames();

    /*!
     * \brief Notifies the configuration system that the session settings
     *        should be saved to the given session.
     *
     * This function will switch to the session with the name given. If no such
     * session exists the session will be created.
     *
     * \param[in] name the name of session to switch to.
     */
    void switchToSession(const QString & name);

    /*!
     * \brief Delete a session.
     *
     * Deletes the session with the given name.
     * This function will not delete the current session and will return false
     * in that case. It also returns false if no session with the given name
     * exists.
     * \returns whether deleting the session was successful.
     */
    bool deleteSession(const QString & name);

    /*!
     * \brief Returns the settings value for the given global key.
     *
     * \param[in] key Key to get the value for.
     * \param[in] defaultValue The value to return if no saved value is found.
     * \returns the value of type specified by the template parameter.
     */
    template<typename T>
    inline T value(const QString & key, const T & defaultValue /*= T()*/) {
        return m_settings.value(group() + key,
                                QVariant::fromValue(defaultValue)).value<T>();
    }

    /*!
     * \brief Returns the settings value for the given session key.
     *
     * \param[in] key Session key to get the value for.
     * \param[in] defaultValue The value to return if no saved value is found.
     * \returns the value of type specified by the template parameter.
     */
    template<typename T>
    inline T sessionValue(const QString & key, const T & defaultValue /*= T()*/) {
        return m_settings.value(GROUP_SESSION.arg(m_currentSession) + group() + key,
                                QVariant::fromValue(defaultValue)).value<T>();
    }

    /*!
     * \brief Sets a value for a global settings key.
     *
     * \param[in] key Ket to set.
     * \param[in] value Value to set.
     */
    template<typename T>
    inline void setValue(const QString & key, const T & value) {
        m_settings.setValue(group() + key, QVariant::fromValue<T>(value));
    }

    /*!
     * \brief Sets a value for a session settings key.
     *
     * \param[in] key Ket to set.
     * \param[in] value Value to set.
     */
    template<typename T>
    inline void setSessionValue(const QString & key, const T & value) {
        m_settings.setValue(GROUP_SESSION.arg(m_currentSession) + group() + key,
                            QVariant::fromValue<T>(value));
    }

    /*!
     * \brief Synchronize the underlying QSettings.
     */
    inline void sync() {
        m_settings.sync();
    }

    /*!
     * \brief Appends the given prefix to the current group.
     *
     * The current group is automatically prepended to all keys when reading or
     * writing settings values. The behaviour is similar to QSettings::beginGroup().
     *
     * \param[in] prefix the prefix to append
     */
    inline void beginGroup(const QString & prefix) {
        Q_ASSERT(!prefix.isEmpty());
        Q_ASSERT(!prefix.startsWith('/'));
        Q_ASSERT(!prefix.endsWith('/'));
        m_groups.append(prefix);
    }

    /*!
     * \brief Resets the current group to its value before the corresponding
     * beginGroup() call.
     *
     * Call this function after you are done with a started group. Every call to
     * beginGroup() must be matched with a call to this function.
     */
    inline void endGroup() {
        Q_ASSERT_X(!m_groups.isEmpty(), "BtConfig", "endGroup() called, but no beginGroup() active.");
        m_groups.removeLast();
    }

    /*!
     * \brief Returns the current group.
     *
     * Returns the group the BtConfig is currently set to. It will contain a
     * trailing / so is suitable to be preprended to a key directly.
     *
     * \returns the group string or an empty string if no group is set.
     */
    inline QString group() const {
        return (m_groups.isEmpty() ? QString() : m_groups.join("/") + '/');
    }

private: /* Methods: */

    QStringList getSessionKeys();

    inline QString getSessionKeyForName(const QString & name) {
        return getSessionKeyForName(name, getSessionKeys());
    }

    QString getSessionKeyForName(const QString & name,
                                 const QStringList & sessionKeys);

private: /* Fields: */

    /** Underlying backend */
    QSettings m_settings;

    /** List of active group prefixes */
    QStringList m_groups;

    /** Index of currently active session */
    QString m_currentSession;

    static const QString GROUP_SESSIONS;
    static const QString KEY_CURRENT_SESSION;
    static const QString GROUP_SESSION;
    static const QString KEY_SESSION_NAME;
    static const QString STRING_0u;
};

#endif // BTCONFIGCORE_H
