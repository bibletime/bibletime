/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cswordtreekey.h"
#include "cswordbookmoduleinfo.h"

CSwordTreeKey::CSwordTreeKey( const CSwordTreeKey& k ) : CSwordKey(k), TreeKeyIdx(k) {}

CSwordTreeKey::CSwordTreeKey( const TreeKeyIdx *k, CSwordModuleInfo* module ) : CSwordKey(module), TreeKeyIdx(*k) {}

CSwordTreeKey* CSwordTreeKey::copy() const {
	return new CSwordTreeKey(*this);
}

/** Sets the key of this instance */
const QString CSwordTreeKey::key() const {
	//   return QString::fromLocal8Bit( getFullName() ); //don't use fromUtf8
	//return QString::fromUtf8( getFullName() ); //don't use fromUtf8
	return QString::fromUtf8( getText() ); //FOR Sword 1.5.9
}

const bool CSwordTreeKey::key( const QString& newKey ) {
	//   if (newKey.isEmpty()) {
	//     root();
	//   }
	//   else {
	//     TreeKeyIdx::operator = ((const char*)newKey.local8Bit());  //don't use Utf8! Doesn't work with umlauts!
	//  }

	return key( (const char*)newKey.local8Bit() );

	//   if (Error()) {
	//    root();
	//   }
	//
	//   return !Error();
}

const bool CSwordTreeKey::key( const char* newKey ) {
	Q_ASSERT(newKey);

	if (newKey) {
		TreeKeyIdx::operator = (newKey);
	}
	else {
		root();
	}

	return !Error();
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
