/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDTREEKEYIDX_H
#define CSWORDTREEKEYIDX_H

//BibleTime
#include "cswordkey.h"

//Sword
#include <treekeyidx.h>

class CSwordModuleInfo;

/** BibleTime's implementation of Sword's TreeKeyIdx class.
 * @short CSwordKey implementation for Sword's TreeKey
 * @author The BibleTime team
 */

class CSwordTreeKey : public CSwordKey, public sword::TreeKeyIdx {

public:
	/** Constructor of this CSwordKey implementation.
	 * @param k The Sword tree key which belongs to this key
	 * @param module The module which belongs to this key
	 */
	CSwordTreeKey( const sword::TreeKeyIdx *k, CSwordModuleInfo* module );
	/** Copy constructor.
	 */
	CSwordTreeKey( const CSwordTreeKey& k );
	/** The module which belongs to this key.
	 * @return The module.
	 */
	virtual CSwordModuleInfo* const module( CSwordModuleInfo* const newModule );
	/** Copy method.
	* @return A new copy of this object.
	*/
	virtual CSwordTreeKey* copy() const;

	/**
	* Returns the TreeKeyIdx::getLocalKey value in unicode.
	* Local key is the last part of the tree key, for example "Subsection1" from "/Section1/Subsection1".
	* Use this instead of getLocalKey() to avoid encoding problems.
	*/
	QString getLocalNameUnicode();
	/**
	* Returns the current key as unicode decoded QString.
	*/
	virtual const QString key() const;
	/**
	* Set the key. If the parameter is empty or null, the key will be set to "/"
	*/
	virtual const bool key( const QString& key );
	/**
	* Set the key from char* To avoid encoding problems use key(QString instead),
	* otherwise it is caller's responsibility to ensure the correct encoding (utf8/latin1).
	*/
	virtual const bool key( const char* key );
	/**
	* Assignment operator.
	*/
	virtual CSwordTreeKey& operator = (const QString& keyname );
protected:
	/**
	 * Returns the raw key appropriate for use directly with Sword.
	 */
	virtual const char * rawKey() const;
};

#endif
