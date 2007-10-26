/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDLDKEY_H
#define CSWORDLDKEY_H

#include "cswordkey.h"
#include "backend/drivers/cswordmoduleinfo.h"

//Qt
#include <QString>

//Sword includes
#include <swkey.h>

/**
 * This class is the implementation of CSwordKey used for dictionaries and lexicons.
 *
 * CSwordLDKey is the implementation of CKey for Lexicons and dictionaries.
 * It provides a simple interface to set the current key,
 * to get the text for the key and functions to get the next and previous items
 * of the used module in comparision to the current key.<BR>
 * Here's an example how to use this class:<BR>
 * @code
 *  CSwordLexiconModuleInfo* m_module = backend()->findModuleByName("ISBE");
 *  CSwordLDKey ldKey(m_module);
 *  ldKey.key("Adam");
 *  ldKey.nextEntry();
 *  qDebug( QString("The current key is: %1").arg(ldKey.key()));
 * @endcode
 *
 * Please not, that the result will be invalid if use the operator const char*
 * on the adress of the object, use something like this
  *
 * @code
 *  CSwordLDKey* key = new CSwordLDKey( lexicon_module );
 *  const QString keyname = key->getKey();
 * @endcode
 *
  * @author The BibleTime team
  * @version $Id: cswordldkey.h,v 1.24 2006/02/25 11:38:15 joachim Exp $
  */

class CSwordLDKey : public CSwordKey, public sword::SWKey {

public:
	/**
	* Constructor of CSwordLDKey
	*/
	CSwordLDKey( CSwordModuleInfo* module );
	/**
	* Copy constructor for this key class.
	*/
	CSwordLDKey( const CSwordLDKey &k );
	/**
	* Copy constructor for this key class.
	*/
	CSwordLDKey( const sword::SWKey *k, CSwordModuleInfo* module);
	/**
	* Clones this object by copying the members.
	*/
	virtual CSwordLDKey* copy() const;
	/**
	* Uses the parameter to returns the next entry afer this key.
	*/
	CSwordLDKey* NextEntry( void );
	/**
	* Uses the parameter to returns the previous entry afer this key.
	*/
	CSwordLDKey* PreviousEntry( void );
	/**
	* Sets the module of this key.
	*/
	virtual CSwordModuleInfo* const module( CSwordModuleInfo* const module = 0 );
	/**
	* Returns the current key as a QString
	*/
	virtual const QString key() const;
	/**
	* Set the current key using unicode decoded QString.
	*/
	virtual const bool key( const QString& newKey );
	/**
	* Set the current key from char*. To avoid encoding problems use key(QString) instead.
	*/
	virtual const bool key( const char* );
	/**
	* Assignment operator for more ease of use of this class. 
	*/
	virtual CSwordLDKey& operator = (const QString& keyname );
protected:
	/**
	 * Returns the raw key appropriate for use directly with Sword.
	 */
	virtual const char * rawKey() const;
};


#endif

