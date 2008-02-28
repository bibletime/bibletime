/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "bibletime_dbus_adaptor.h"

BibleTimeDBusAdaptor::BibleTimeDBusAdaptor(BibleTime *bibletime_ptr) : 
	QDBusAbstractAdaptor(bibletime_ptr),
	m_bibletime(bibletime_ptr) 
{
}
	
void BibleTimeDBusAdaptor::syncAllBibles(const QString& key){
	m_bibletime->syncAllBibles(key);
}

void BibleTimeDBusAdaptor::syncAllCommentaries(const QString& key){
	m_bibletime->syncAllCommentaries(key);
}
void BibleTimeDBusAdaptor::syncAllLexicons(const QString& key){
	m_bibletime->syncAllLexicons(key);
}

void BibleTimeDBusAdaptor::syncAllVerseBasedModules(const QString& key){
	m_bibletime->syncAllVerseBasedModules(key);
}

void BibleTimeDBusAdaptor::reloadModules(){
	m_bibletime->reloadModules();
}

void BibleTimeDBusAdaptor::openWindow(const QString& moduleName, const QString& key){
	m_bibletime->openWindow(moduleName, key);
}

void BibleTimeDBusAdaptor::openDefaultBible(const QString& key){
	m_bibletime->openDefaultBible(key);
}

void BibleTimeDBusAdaptor::closeAllModuleWindows(){
	m_bibletime->closeAllModuleWindows();
}

QString BibleTimeDBusAdaptor::getCurrentReference(){
	return m_bibletime->getCurrentReference();
}

QStringList BibleTimeDBusAdaptor::searchInModule(const QString& moduleName, const QString& searchText){
	return m_bibletime->searchInModule(moduleName, searchText);
}

QStringList BibleTimeDBusAdaptor::searchInOpenModules(const QString& searchText){
	return m_bibletime->searchInOpenModules(searchText);
}

QStringList BibleTimeDBusAdaptor::searchInDefaultBible(const QString& searchText){
	return m_bibletime->searchInDefaultBible(searchText);
}

QStringList BibleTimeDBusAdaptor::getModulesOfType(const QString& type){
	return m_bibletime->getModulesOfType(type);
}

