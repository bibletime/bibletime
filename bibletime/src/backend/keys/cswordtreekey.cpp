/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cswordtreekey.h"
#include "backend/drivers/cswordbookmoduleinfo.h"

#include <QTextCodec>

#include <QDebug>

CSwordTreeKey::CSwordTreeKey( const CSwordTreeKey& k ) : CSwordKey(k), TreeKeyIdx(k) {}

CSwordTreeKey::CSwordTreeKey( const TreeKeyIdx *k, CSwordModuleInfo* module ) : CSwordKey(module), TreeKeyIdx(*k) {}

CSwordTreeKey* CSwordTreeKey::copy() const {
	return new CSwordTreeKey(*this);
}

/** Sets the key of this instance */
const QString CSwordTreeKey::key() const {
	//return getTextUnicode();
	Q_ASSERT(m_module);
	if (m_module->isUnicode()) {
		return QString::fromUtf8(getText());
	} else {
		return QString::fromLatin1(getText());
	}
}

const bool CSwordTreeKey::key( const QString& newKey ) {
	//return key( newKey.toLocal8Bit().constData() );
	qDebug("CSwordTreeKey::key( const QString& newKey )");
	//return key(m_module->getTextCodec()->fromUnicode(newKey).constData());
	Q_ASSERT(m_module);
	if (m_module->isUnicode()) {
		return key(newKey.toUtf8().constData());
	} else {
		return key(newKey.toLatin1().constData());
	}
}

const bool CSwordTreeKey::key( const char* newKey ) {
	qDebug("CSwordTreeKey::key( const char* newKey )");
	qDebug() << "key: "<<newKey;
	Q_ASSERT(newKey);

	if (newKey) {
		qDebug("use operator=");
		TreeKeyIdx::operator = (newKey);
	}
	else {
		qDebug("call root()");
		root();
	}

	return !Error();
}

QString CSwordTreeKey::getLocalNameUnicode()
{
	//return m_module->getTextCodec()->toUnicode(getLocalName());
	//Only UTF-8 and latin1 are legal Sword module encodings
	Q_ASSERT(m_module);
	if (m_module->isUnicode()) {
		return QString::fromUtf8(getLocalName());
	} else {
		return QString::fromLatin1(getLocalName());
	}
}

CSwordModuleInfo* const CSwordTreeKey::module( CSwordModuleInfo* const newModule ) {
	if (newModule && (newModule != m_module) && (newModule->type() == CSwordModuleInfo::GenericBook) ) {
		m_module = newModule;

		const QString oldKey = key();

		CSwordBookModuleInfo* newBook = dynamic_cast<CSwordBookModuleInfo*>(newModule);
		copyFrom( *(newBook->tree()) );

		key(oldKey); //try to restore our old key

		//set the key to the root node
		root();
		firstChild();
	}

	return m_module;
}

/** Assignment operator. */
CSwordTreeKey& CSwordTreeKey::operator = (const QString& keyname ) {
	key(keyname);
	return *this;
}
