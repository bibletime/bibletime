/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef NO_DBUS

#include "bibletime_dbus_adaptor.h"

#include <QDebug>
#include "backend/config/btconfig.h"

BibleTimeDBusAdaptor::BibleTimeDBusAdaptor(BibleTime *pBibleTime)
    : QDBusAbstractAdaptor(pBibleTime)
    , m_bibletime(pBibleTime)
{
    // Intentionally empty
}

void BibleTimeDBusAdaptor::syncAllBibles(const QString &key) {
    qDebug() << "DBUS: syncing all bibles ...";
    m_bibletime->syncAllModulesByType(CSwordModuleInfo::Bible, key);
}

void BibleTimeDBusAdaptor::syncAllCommentaries(const QString &key) {
    qDebug() << "DBUS: syncing all commentaries ...";
    m_bibletime->syncAllModulesByType(CSwordModuleInfo::Commentary, key);
}
void BibleTimeDBusAdaptor::syncAllLexicons(const QString &key) {
    qDebug() << "DBUS: syncing all lexicons ...";
    m_bibletime->syncAllModulesByType(CSwordModuleInfo::Lexicon, key);
}

void BibleTimeDBusAdaptor::syncAllVerseBasedModules(const QString &key) {
    qDebug() << "DBUS: syncing all verse based modules ...";
    m_bibletime->syncAllModulesByType(CSwordModuleInfo::Bible, key);
    m_bibletime->syncAllModulesByType(CSwordModuleInfo::Commentary, key);
}

void BibleTimeDBusAdaptor::openWindow(const QString &moduleName, const QString &key) {
    qDebug() << "DBUS: open window for module" << moduleName.toLatin1() << "and key" << key.toLatin1() << " ...";
    CSwordModuleInfo *module = CSwordBackend::instance()->findModuleByName(moduleName);
    if (module)
        m_bibletime->createReadDisplayWindow(module, key);
}

void BibleTimeDBusAdaptor::openDefaultBible(const QString &key) {
    qDebug() << "DBUS: open default bible ...";
    CSwordModuleInfo *module = btConfig().getDefaultSwordModuleByType("standardBible");
    if (module)
        m_bibletime->createReadDisplayWindow(module, key);
}

void BibleTimeDBusAdaptor::closeAllModuleWindows() {
    qDebug() << "DBUS: close all windows now ...";
    m_bibletime->closeAllModuleWindows();
}

QString BibleTimeDBusAdaptor::getCurrentReference() {
    qDebug() << "DBUS: getCurrentReference ...";
    return m_bibletime->getCurrentReference();
}

QStringList BibleTimeDBusAdaptor::searchInModule(const QString &moduleName, const QString &searchText) {
    qDebug() << "DBUS: searchInModule" << moduleName.toLatin1() << " ...";
    return m_bibletime->searchInModule(moduleName, searchText);
}

QStringList BibleTimeDBusAdaptor::searchInOpenModules(const QString &searchText) {
    qDebug() << "DBUS: search in open modules ...";
    return m_bibletime->searchInOpenModules(searchText);
}

QStringList BibleTimeDBusAdaptor::searchInDefaultBible(const QString &searchText) {
    qDebug() << "DBUS: search in default bible ...";
    CSwordModuleInfo *bible = btConfig().getDefaultSwordModuleByType("standardBible");
    return m_bibletime->searchInModule(bible->name(), searchText);
}

QStringList BibleTimeDBusAdaptor::getModulesOfType(const QString &type) {
    qDebug() << "DBUS: get modules of type ...";
    return m_bibletime->getModulesOfType(type);
}

#endif //NO_DBUS
