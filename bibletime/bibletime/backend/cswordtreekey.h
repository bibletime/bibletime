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

//BibleTime includes
#include "cswordkey.h"

//Sword includes
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
	* Returns the current key
	*/
	virtual const QString key() const;
	/**
	* Set the key. If the parameter is empty or null, the key will be set to "/"
	*/
	virtual const bool key( const QString& key );
	/**
	* Set/get the key. If the parameter is not set (means equal to QString::null)
	* the used key is returned. Otherwise the key is set and the new on ei returned.
	*/
	virtual const bool key( const char* key );
	/**
	* Assignment operator.
	*/
	virtual CSwordTreeKey& operator = (const QString& keyname );
};

#endif
