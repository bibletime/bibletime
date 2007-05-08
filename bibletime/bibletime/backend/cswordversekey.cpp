/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cswordversekey.h"
#include "cswordbiblemoduleinfo.h"
#include "cswordcommentarymoduleinfo.h"

//Qt includes
#include <qstringlist.h>

//Sword includes
#include <swmodule.h>
#include <localemgr.h>

CSwordVerseKey::CSwordVerseKey( CSwordModuleInfo* const module ) : CSwordKey(module) {
	if ( CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(module) ) {
		key( bible->lowerBound().key() );
	}
}

CSwordVerseKey::CSwordVerseKey( const CSwordVerseKey& k ) : CSwordKey(k), VerseKey(k) {}

CSwordVerseKey::CSwordVerseKey( const VerseKey* const k, CSwordModuleInfo* const module ) : CSwordKey(module), VerseKey(*k) {}

/** Clones this object. */
CSwordKey* CSwordVerseKey::copy() const {
	return new CSwordVerseKey(*this);
}

/** Sets the module for this key */
CSwordModuleInfo* const CSwordVerseKey::module( CSwordModuleInfo* const newModule ) {
	if (newModule && ((newModule->type() == CSwordModuleInfo::Bible)  || (newModule->type() == CSwordModuleInfo::Commentary) ) ) {
		m_module = newModule;

		//check if the module contains the key we present
		CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(newModule);

		if (_compare(bible->lowerBound()) < 0) {
			key( bible->lowerBound() );
		}

		if (_compare(bible->upperBound()) > 0) {
			key( bible->upperBound() );
		}
	}

	return dynamic_cast<CSwordBibleModuleInfo*>(m_module);
}

/** Returns the current book as Text, not as integer. */
const QString CSwordVerseKey::book( const QString& newBook ) {
	int min = 0;
	int max = 1;

	if (CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(module())) {
		const bool hasOT = bible->hasTestament(CSwordBibleModuleInfo::OldTestament);
		const bool hasNT = bible->hasTestament(CSwordBibleModuleInfo::NewTestament);

		if (hasOT && hasNT) {
			min = 0;
			max = 1;
		}
		else if (hasOT && !hasNT) {
			min = 0;
			max = 0;
		}
		else if (!hasOT && hasNT) {
			min = 1;
			max = 1;
		}
		else if (!hasOT && !hasNT) {
			min = 0;
			max = -1; //no loop
		}
	}

	if (!newBook.isEmpty()) {
		bool finished = false;

		for (int testament = min; testament <= max && !finished; ++testament) {
			for (int book = 0; book < BMAX[testament] && !finished; ++book) {
				if ( !strcmp((const char*)newBook.utf8(), books[testament][book].name ) ) {
					Testament(testament+1);
					Book(book+1);
					finished = true;
				}
			}
		}
	}

	if ( (Testament() >= min+1) && (Testament() <= max+1) && (Book() <= BMAX[min]) ) {
		return QString::fromUtf8( books[Testament()-1][Book()-1].name );
	}

	//return QString::fromUtf8( books[min][0].name ); //return the first book, i.e. Genesis
	return QString::null;
}

/** Sets the key we use to the parameter. */
const QString CSwordVerseKey::key() const {
	return QString::fromUtf8(getText());
}

const bool CSwordVerseKey::key( const QString& newKey ) {
	return key( (const char*)newKey.utf8() );
}

const bool CSwordVerseKey::key( const char* newKey ) {
	if (newKey && (strlen(newKey)>0) ) {
		VerseKey::operator = (newKey);
	}
	else if (newKey && !strlen(newKey)) {
		CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(module());

		if ( bible ) {
			VerseKey::operator = ((const char*)bible->lowerBound().key().utf8());
		}
	}

	return !Error();
}

const bool CSwordVerseKey::next( const JumpType type ) {
	Error(); //clear Error status
	bool ret = true;

	switch (type) {

		case UseBook: {
			const int currentTestament = Testament();
			const int currentBook = Book();
			 
			if ((currentTestament == 2) && (currentBook >= BMAX[currentTestament-1])) { //Revelation, i.e. end of navigation
				return false;
			}
			else if ((currentTestament == 1) && (currentBook >= BMAX[currentTestament-1])) { //Malachi, switch to the NT
				Testament(currentTestament+1);
				Book(1);
			}
			else {
				Book(Book()+1);
			}
			break;
		}

		case UseChapter: {
			Chapter(Chapter()+1);
			break;
		}

		case UseVerse: {
			if (m_module && m_module->module()) {
				const bool oldStatus = m_module->module()->getSkipConsecutiveLinks();
				m_module->module()->setSkipConsecutiveLinks(true);

				//disable headings for next verse
				const bool useHeaders = (Verse() == 0);
				const bool oldHeadingsStatus = ((VerseKey*)(m_module->module()->getKey()))->Headings( useHeaders );
				//don't use setKey(), that would create a new key without Headings set
				m_module->module()->getKey()->setText( (const char*)key().utf8() );

				(*(m_module->module()) )++;

				((VerseKey*)(m_module->module()->getKey()))->Headings(oldHeadingsStatus);
				m_module->module()->setSkipConsecutiveLinks(oldStatus);

				if (!m_module->module()->Error()) {
					key( QString::fromUtf8(m_module->module()->KeyText()) );
				}
				else {
					//         Verse(Verse()+1);
					//don't change the key, restore the module's position
					m_module->module()->getKey()->setText( (const char*)key().utf8() );
					ret = false;
					break;
				}

			}
			else {
				Verse(Verse()+1);
			}

			break;
		}

		default:
			return false;
	}

	if ( CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(module()) ) {
		if (_compare(bible->lowerBound()) < 0 ) {
			key( bible->lowerBound() );
			ret = false;
		}

		if (_compare(bible->upperBound()) > 0 ) {
			key( bible->upperBound() );
			ret = false;
		}

		return ret;
	}
	else if (Error()) { //we have no module, so take care of VerseKey::Error()
		return false;
	}

	return ret;
}

const bool CSwordVerseKey::previous( const JumpType type ) {
	bool ret = true;

	switch (type) {

		case UseBook: {
			if ( (Book() == 1) && (Testament() == 1) ) { //Genesis
				return false;
			}
			else if ( (Book() == 1) && (Testament() == 2) ){ //Matthew
				Testament(1);
				Book(BMAX[0]);
			}
			else{
				Book( Book()-1 );
			}
			
			break;
		}

		case UseChapter: {
			Chapter(Chapter()-1);
			break;
		}

		case UseVerse: {
			if (m_module && m_module->module()) {
				const bool useHeaders = (Verse() == 0);
				const bool oldHeadingsStatus = ((VerseKey*)(m_module->module()->getKey()))->Headings( useHeaders );

				m_module->module()->getKey()->setText( (const char*)key().utf8() );

				const bool oldStatus = m_module->module()->getSkipConsecutiveLinks();
				m_module->module()->setSkipConsecutiveLinks(true);
				( *( m_module->module() ) )--;

				((VerseKey*)(m_module->module()->getKey()))->Headings( oldHeadingsStatus );
				m_module->module()->setSkipConsecutiveLinks(oldStatus);

				if (!m_module->module()->Error()) {
					key( QString::fromUtf8(m_module->module()->KeyText()) );//don't use fromUtf8
				}
				else {
					ret = false;
					//         Verse(Verse()-1);
					m_module->module()->getKey()->setText( (const char*)key().utf8() ); //restore module's key
				}
			}
			else {
				Verse(Verse()-1);
			}

			break;
		}

		default:
			return false;
	}

	if ( CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(module()) ) {
		if (_compare(bible->lowerBound()) < 0 ) {
			key( bible->lowerBound() );
			ret = false;
		}

		if (_compare(bible->upperBound()) > 0 ) {
			key( bible->upperBound() );
			ret = false;
		}

		return ret;
	}
	else if (Error()) {
		return false;
	}

	return ret;
}

/** Assignment operator for more ease of use. */
CSwordVerseKey& CSwordVerseKey::operator = (const QString& keyname) {
	key(keyname);
	return *this;
}
