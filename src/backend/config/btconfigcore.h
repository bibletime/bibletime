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

#ifndef BTCONFIGCORE_H
#define BTCONFIGCORE_H

#include <QSettings>

#include <QCoreApplication>
#include <QHash>
#include <QMutex>
#include <QStringList>
#include "../../util/btassert.h"


/**
  \note Session keys are QStrings because we even want to handle cases where the
        configuration file is manually changed. When creating new sessions, they
        are still generated from unsigned integers.
*/
class BtConfigCore {

    Q_DECLARE_TR_FUNCTIONS(BtConfigCore)

public: /* Types: */

    using SessionNamesHashMap = QHash<QString, QString>;

public: /* Methods: */

    BtConfigCore(BtConfigCore const &) = delete;
    BtConfigCore & operator=(BtConfigCore const &) = delete;

    /**
      \param[in] settingsFile The filename of the settings file.
    */
    explicit BtConfigCore(const QString & settingsFile);

    inline ~BtConfigCore() { sync(); }


    /**
      \returns the key of the current session.
    */
    inline const QString & currentSessionKey() const {
        QMutexLocker lock(&m_mutex);
        return m_currentSessionKey;
    }

    /**
      \returns the name of the current session.
    */
    inline const QString & currentSessionName() const {
        QMutexLocker lock(&m_mutex);
        using SSHCI = QHash<QString, QString>::const_iterator;
        SSHCI it = m_sessionNames.constFind(m_currentSessionKey);
        BT_ASSERT(it != m_sessionNames.constEnd());
        return it.value();
    }

    /**
      \returns a hashmap with the keys and printable names of the sessions.
    */
    inline const SessionNamesHashMap & sessionNames() const {
        QMutexLocker lock(&m_mutex);
        return m_sessionNames;
    }

    /**
      \brief Notifies the configuration system that the session settings
             should be read from and saved to the given session.

      \pre The session with the given key must exist.
      \param[in] key the key of the session to switch to.
      \post the sessionValue() and value() methods will work with the settings
            of the given session.
    */
    void setCurrentSession(const QString & key);

    /**
      \brief Creates a new session with the given name.

      \pre The given name must not be an empty string.
      \param[in] name the name of the session
      \returns the key of the created session.
    */
    QString addSession(const QString & name);

    /**
      \brief Deletes the session with the given key.

      \pre The session with the given key must exist.
      \pre The session with the given key must not be the current session.
      \param[in] key the key of the session to delete.
      \post The session with the given key and its settings are deleted.
      \returns whether deleting the session was successful.
    */
    void deleteSession(const QString & key);

    /**
      \brief Returns the settings value for the given global key.

      \param[in] key Key to get the value for.
      \param[in] defaultValue The value to return if no saved value is found.
      \returns the value of type specified by the template parameter.
    */
    template<typename T>
    inline T value(const QString & key, const T & defaultValue = T()) {
        QMutexLocker lock(&m_mutex);
        return m_settings.value(group() + key,
                                QVariant::fromValue(defaultValue)).template value<T>();
    }

    /**
      \brief Returns the settings value for the given global key as a QVariant.

      \param[in] key Key to get the value for.
      \param[in] defaultValue The value to return if no saved value is found.
      \returns the value.
    */
    inline QVariant qVariantValue(const QString & key,
                                  const QVariant & defaultValue = QVariant())
    {
        QMutexLocker lock(&m_mutex);
        return m_settings.value(group() + key,
                                QVariant::fromValue(defaultValue));
    }

    /**
      \brief Returns the settings value for the given session key.

      \param[in] key Session key to get the value for.
      \param[in] defaultValue The value to return if no saved value is found.
      \returns the value of type specified by the template parameter.
    */
    template<typename T>
    inline T sessionValue(const QString & key, const T & defaultValue = T()) {
        QMutexLocker lock(&m_mutex);
        return m_settings.value(m_cachedCurrentSessionGroup + group() + key,
                                QVariant::fromValue(defaultValue)).template value<T>();
    }

    /**
      \brief Sets a value for a global settings key.

      \param[in] key Ket to set.
      \param[in] value Value to set.
    */
    template<typename T>
    inline void setValue(const QString & key, const T & value) {
        QMutexLocker lock(&m_mutex);
        m_settings.setValue(group() + key, QVariant::fromValue<T>(value));
    }

    /**
      \brief Sets a value for a session settings key.

      \param[in] key Ket to set.
      \param[in] value Value to set.
    */
    template<typename T>
    inline void setSessionValue(const QString & key, const T & value) {
        QMutexLocker lock(&m_mutex);
        m_settings.setValue(m_cachedCurrentSessionGroup + group() + key,
                            QVariant::fromValue<T>(value));
    }

    /**
      \returns a list of first-level keys in the current group in global settings.
    */
    QStringList childKeys();

    /**
      \pre subkey is not empty
      \param[in] subkey the subkey
      \returns a list of keys under the current group and subkey in global settings.
    */
    QStringList childKeys(const QString & subkey);

    /**
      \returns a list of first-level groups in the current group in global settings.
    */
    QStringList childGroups();

    /**
      \pre subkey is not empty
      \param[in] subkey the subkey
      \returns a list of groups under the current group and subkey in global settings.
    */
    QStringList childGroups(const QString & subkey);

    /**
      \returns a list of first-level groups in the current group in session settings.
    */
    QStringList sessionChildGroups();

    /**
      \pre subkey is not empty
      \param[in] subkey the subkey
      \returns a list of groups under the current group and subkey in session settings.
    */
    QStringList sessionChildGroups(const QString & subkey);

    /**
      \brief removes a key all its children from global settings.

      \param[in] key the key to remove
    */
    void remove(const QString & key);

    /**
      \brief removes a key all its children from session settings.

      \param[in] key the key to remove
    */
    void sessionRemove(const QString & key);

    /**
      \brief Synchronize the underlying QSettings.
    */
    inline void sync() {
        QMutexLocker lock(&m_mutex);
        m_settings.sync();
    }

    /**
      \brief Appends the given prefix to the current group.

      The current group is automatically prepended to all keys when reading or
      writing settings values. The behaviour is similar to QSettings::beginGroup().

      \warning Locks the object (recursively) until endGroup().

      \param[in] prefix the prefix to append
    */
    inline void beginGroup(QString prefix) {
        BT_ASSERT(!prefix.isEmpty());
        while (prefix.startsWith('/'))
            prefix.remove(0, 1);
        BT_ASSERT(!prefix.isEmpty());
        while (prefix.endsWith('/'))
            prefix.chop(1);
        BT_ASSERT(!prefix.isEmpty());

        m_mutex.lock();
        m_groups.append(prefix);
        m_cachedGroup = QString();
    }

    /**
      \brief Resets the current group to its previous value.

      Call this function after you are done with a started group. Every call to
      beginGroup() must be matched with a call to this function in the same
      thread.

      \warning Locks the object (recursively) until endGroup().
    */
    inline void endGroup() {
        BT_ASSERT(!m_groups.isEmpty()
                  && "BtConfig::endGroup() called, but no beginGroup() active");
        m_groups.removeLast();
        m_cachedGroup = QString();
        m_mutex.unlock();
    }

    /**
      \brief Returns the current group.

      Returns the group the BtConfig is currently set to. It will contain a
      trailing / so is suitable to be preprended to a key directly.

      \returns the group string or an empty string if no group is set.
    */
    inline QString group() const {
        QMutexLocker lock(&m_mutex);
        if (m_cachedGroup.isNull()) {
            m_cachedGroup = m_groups.isEmpty()
                            ? ""
                            : m_groups.join("/") + '/';
        }
        return m_cachedGroup;
    }

private: /* Methods: */

    /**
      \Brief Same childKeys(), but not thread-safe.
      \returns a list of first-level keys in the current group in global settings.
    */
    QStringList childKeys__();

    /**
      \Brief Same childGroups(), but not thread-safe.
      \returns a list of first-level groups in the current group in global settings.
    */
    QStringList childGroups__();

    /**
      \Brief Same sessionChildGroups(), but not thread-safe.
      \returns a list of first-level groups in the current group in session settings.
    */
    QStringList sessionChildGroups__();

protected: /* Fields: */

    /** Required for asynchronous access */
    mutable QMutex m_mutex;

private: /* Fields: */

    /** Underlying backend */
    QSettings m_settings;

    /** List of active group prefixes */
    QStringList m_groups;

    /** Cached group() string or QString() if not cached */
    mutable QString m_cachedGroup;

    /** Index of currently active session */
    QString m_currentSessionKey;

    /** Cached group name of the currently active session */
    mutable QString m_cachedCurrentSessionGroup;

    /** Keys and names all sessions */
    SessionNamesHashMap m_sessionNames;

    // Helper key strings:
    static const QString GROUP_SESSIONS;
    static const QString KEY_CURRENT_SESSION;
    static const QString GROUP_SESSION;
    static const QString KEY_SESSION_NAME;
    static const QString UI_FONT_SIZE;
};

#endif // BTCONFIGCORE_H
