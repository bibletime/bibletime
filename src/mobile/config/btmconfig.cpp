/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btmconfig.h"

#include "backend/config/btconfig.h"
#include "backend/managers/cswordbackend.h"

namespace btm {

BtmConfig::BtmConfig() :
    m_availableModulesReady(false) {

}

qreal BtmConfig::getReal(const QString& settingName, qreal defaultValue) {
    return btConfig().value<qreal>(settingName, defaultValue);
}

void BtmConfig::setReal(const QString& settingName, qreal value) {
    btConfig().setValue<qreal>(settingName, value);
}

void BtmConfig::initializeAvailableModules() {
    m_availableModulesReady = false;
}

QStringList BtmConfig::getAvailableBibles() {
    updateDefaultModules();
    return m_availableBibles;
}

QStringList BtmConfig::getAvailableGreekStrongsLexicons() {
    updateDefaultModules();
    return m_availableGreekStrongsLexicons;
}

QStringList BtmConfig::getAvailableHebrewStrongsLexicons() {
    updateDefaultModules();
    return m_availableHebrewStrongsLexicons;
}

QStringList BtmConfig::getAvailableGreekMorphLexicons() {
    updateDefaultModules();
    return m_availableGreekMorphLexicons;
}

QStringList BtmConfig::getAvailableHebrewMorphLexicons() {
    updateDefaultModules();
    return m_availableHebrewMorphLexicons;
}

QString BtmConfig::getDefaultSwordModuleByType(const QString& type) {
    CSwordModuleInfo* module = btConfig().getDefaultSwordModuleByType(type);
    QString moduleName;
    if (module)
        moduleName = module->name();
    return moduleName;
}

void BtmConfig::setDefaultSwordModuleByType(const QString& type, const QString& moduleName) {
    CSwordModuleInfo * module = CSwordBackend::instance()->findModuleByName(moduleName);
    btConfig().setDefaultSwordModuleByType(type, module);
}

// If no default module is configed, choose one an update config.
void BtmConfig::updateDefaultModules() {
    if (m_availableModulesReady)
        return;
    lookupAvailableModules();
    configModuleByType("standardBible", m_availableBibles);
    configModuleByType("standardGreekStrongsLexicon", m_availableGreekStrongsLexicons);
    configModuleByType("standardHebrewStrongsLexicon", m_availableHebrewStrongsLexicons);
    configModuleByType("standardGreekMorphLexicon", m_availableGreekMorphLexicons);
    configModuleByType("standardHebrewMorphLexicon", m_availableHebrewMorphLexicons);
}

void BtmConfig::lookupAvailableModules() {
    m_availableBibles.clear();
    m_availableGreekStrongsLexicons.clear();
    m_availableHebrewStrongsLexicons.clear();
    m_availableGreekMorphLexicons.clear();
    m_availableHebrewMorphLexicons.clear();
    Q_FOREACH(CSwordModuleInfo const * const m,
              CSwordBackend::instance()->moduleList()) {
        if (m->type() ==  CSwordModuleInfo::Bible) {
            m_availableBibles += m->name();
        }
        if (m->type() ==  CSwordModuleInfo::Lexicon) {
            if (m->has(CSwordModuleInfo::HebrewDef)) {
                m_availableHebrewStrongsLexicons += m->name();
            }
            if (m->has(CSwordModuleInfo::GreekDef)) {
                m_availableGreekStrongsLexicons += m->name();
            }
            if (m->has(CSwordModuleInfo::HebrewParse)) {
                m_availableHebrewMorphLexicons += m->name();
            }
            if (m->has(CSwordModuleInfo::GreekParse)) {
                m_availableGreekMorphLexicons += m->name();
            }
        }
    }
}

void BtmConfig::configModuleByType(const QString& type, const QStringList& availableModuleNames) {
    CSwordModuleInfo* module = btConfig().getDefaultSwordModuleByType(type);
    if (!module && availableModuleNames.count() > 0) {
        QString moduleName = availableModuleNames.at(0);
        module = CSwordBackend::instance()->findModuleByName(moduleName);
        btConfig().setDefaultSwordModuleByType(type, module);
    }
}

} // end namespace

