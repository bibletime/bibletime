/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btconfigcore.h"

#if __cplusplus >= 201103L
#include <cstdint>
#else
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#endif


const QString BtConfigCore::GROUP_SESSIONS      = "sessions/";
const QString BtConfigCore::KEY_CURRENT_SESSION = "sessions/currentSession";
const QString BtConfigCore::GROUP_SESSION       = "sessions/%1/";
const QString BtConfigCore::KEY_SESSION_NAME    = "sessions/%1/name";
const QString BtConfigCore::STRING_0u           = QString::number(0u);


BtConfigCore::BtConfigCore(const QString & settingsFile)
    : m_settings(settingsFile, QSettings::IniFormat)
{
    /**
      \todo Read UI language from settings, and initialize translator for tr()
            strings.
    */

    // Reset default session name:
    m_settings.setValue(KEY_SESSION_NAME.arg(STRING_0u),
                        tr("Default session"));

    // Get current session index (or default to 0):
    m_currentSession = m_settings.value(KEY_CURRENT_SESSION, STRING_0u).toString();

    // If no name for an active non-default session is found, switch to default:
    if (m_currentSession != STRING_0u
        && !m_settings.contains(KEY_SESSION_NAME.arg(m_currentSession)))
    {
        m_currentSession = STRING_0u;
        m_settings.setValue(KEY_CURRENT_SESSION, STRING_0u);
    }
}

QString BtConfigCore::getCurrentSessionName() {
    const QString sessionNameKey = KEY_SESSION_NAME.arg(m_currentSession);
    const QString sessionName = m_settings.value(sessionNameKey).toString();
    if (!sessionName.isEmpty())
        return sessionName;

    const QString newSessionName = tr("Session %1").arg(m_currentSession);
    m_settings.setValue(sessionNameKey, newSessionName);
    return newSessionName;
}

QStringList BtConfigCore::getAllSessionNames() {
    QStringList sessionNames;
    Q_FOREACH (const QString & sessionKey, getSessionKeys()) {
        const QString sessionName = m_settings.value(KEY_SESSION_NAME.arg(sessionKey)).toString();
        if (sessionName.isEmpty()) {
            const QString newSessionName = tr("Session %1").arg(sessionKey);
            m_settings.setValue(KEY_SESSION_NAME.arg(sessionKey), newSessionName);
            sessionNames.append(newSessionName);
        } else {
            sessionNames.append(sessionName);
        }
    }
    return sessionNames;
}

void BtConfigCore::switchToSession(const QString & name) {
    Q_ASSERT(!name.isEmpty());

    QStringList sessionKeys = getSessionKeys();

    // If the session doesn't exist yet, create it:
    QString sessionKey = getSessionKeyForName(name, sessionKeys);
    if (sessionKey.isNull()) {
        // The following key generation algorithm isn't optimal, but it can only
        // fail due to system limits.
        QString prefix;
        for (;;) {
            bool found = false;
            for (size_t i = 0; i != SIZE_MAX; i++) {
                sessionKey = prefix + QString::number(i);
                if (!sessionKeys.contains(sessionKey)) {
                    found = true;
                    break;
                }
            }
            if (found)
                break;

            prefix.append('0');
        }

        // Save new session name:
        m_settings.setValue(KEY_SESSION_NAME.arg(sessionKey), name);
    }

    // Switch to the session:
    m_currentSession = sessionKey;
    m_settings.setValue(KEY_CURRENT_SESSION, sessionKey);
}

bool BtConfigCore::deleteSession(const QString & name) {
    const QString sessionKey = getSessionKeyForName(name);

    // If session was not found:
    if (sessionKey.isNull())
        return false;

    // If session is current session:
    if (sessionKey == m_currentSession)
        return false;

    m_settings.remove(GROUP_SESSIONS + sessionKey);
    return true;
}

QStringList BtConfigCore::getSessionKeys() {
    m_settings.beginGroup(GROUP_SESSIONS);
    const QStringList childGroups = m_settings.childGroups();
    m_settings.endGroup();
    return childGroups;
}

QString BtConfigCore::getSessionKeyForName(const QString & name,
                                           const QStringList & sessionKeys)
{
    Q_FOREACH (const QString & sessionKey, sessionKeys)
        if (m_settings.value(KEY_SESSION_NAME.arg(sessionKey)).toString() == name)
            return sessionKey;
    return QString();
}
