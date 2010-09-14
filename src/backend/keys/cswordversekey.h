/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDVERSEKEY_H
#define CSWORDVERSEKEY_H

#include "cswordkey.h"
class CSwordModuleInfo;

//Qt
#include <QString>

//Sword
#include <versekey.h>

/**
 * The CSwordKey implementation for verse based modules (Bibles and Commentaries)
 *
 * This class is the implementation of CKey for verse based modules like
 * Bibles and commentaries.
 * This class provides the special functions to work with the verse based modules.
 *
 * Useful functions are
 * @see NextBook()
 * @see PreviousBook()
 * @see NextChapter()
 * @see PreviousChapter()
 * @see NextVerse()
 * @see PreviousVerse().
 *
 * Call the constructor only with a valid verse based modules, otherwise this key will be invalid
 * and the application will probably crash.
 *
 * @version $Id: cswordversekey.h,v 1.26 2006/02/25 11:38:15 joachim Exp $
 * @short CSwordKey implementation for Sword's VerseKey.
 * @author The BibleTime team
 */

class CSwordVerseKey : public CSwordKey, public sword::VerseKey {

public:
	enum JumpType {
		UseBook,
		UseChapter,
		UseVerse
	};

	/**
	* Constructor of this class.
	*
	* This function will construct a versekey with the current module position
	* and it will setup the m_module members.
	*
	*/
	CSwordVerseKey( CSwordModuleInfo* const module );
	/**
	* Copy constructor.
	*/
	CSwordVerseKey( const CSwordVerseKey& k );
	/**
	* VerseKey based constructor.
	*/
	CSwordVerseKey( const sword::VerseKey* const k, CSwordModuleInfo* const module );
	/**
	* Clones this object.
	*/
	virtual CSwordKey* copy() const;
	/**
	* Set/get the key. If the parameter is not set (means equal to QString::null)
	* the used key is returned. Otherwise the key is set and the new on ei returned.
	*/
	virtual QString key() const;
	/**
	* Set the current key.
	*/
	virtual bool key( const QString& );
	/**
	* Set/get the key. If the parameter is not set (means equal to QString::null)
	* the used key is returned. Otherwise the key is set and the new on ei returned.
	*/
	virtual bool key( const char* key );

	/**
	* Jumps to the next entry of the given type
	*/
	bool next( const JumpType type );
	/**
	* Jumps to the previous entry of the given type
	*/
	bool previous ( const JumpType type );
	/**
	* This functions returns the current book as localised text, not as book numer.
	*
	* Use "char Book()" to retrieve the book number of the current book.
	* @return The name of the current book
	*/
	QString book(const QString& newBook = QString::null);
	/**
	* Sets the module for this key
	*/
	virtual CSwordModuleInfo* module( CSwordModuleInfo* const newModule = 0 );

protected:
	/**
	 * Returns the raw key appropriate for use directly with Sword.
	 */
	virtual const char * rawKey() const;

private:
	/** Disable assignment operator	*/
	CSwordVerseKey& operator= (const CSwordVerseKey&);
	/** Disable from base class to prevent compiler warnings */
	inline virtual CSwordVerseKey& operator= (const sword::VerseKey&) { return (*this); };
};

#endif
