/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSWORDBIBLEMODULEINFO_H
#define CSWORDBIBLEMODULEINFO_H

//own includes
#include "cswordmoduleinfo.h"
#include "cswordversekey.h"

//Qt includes
#include <qstringlist.h>

//Sword includes


/**
 * This is the CModuleInfo imlementation for Bible modules managed by Sword.
 * 
 * @short Implementation for Sword Bibles
  * @author The BibleTime team
  * @version $Id: cswordbiblemoduleinfo.h,v 1.18 2006/02/25 11:38:15 joachim Exp $
  */

class CSwordBibleModuleInfo : public CSwordModuleInfo {

public:
	enum Testament {
		OldTestament = 1,
		NewTestament = 2
	};

	/**
	* The constructor of this class
	*/
	CSwordBibleModuleInfo( sword::SWModule* module, CSwordBackend* const = CPointers::backend()  );
	/** The copy constructor for this Bible module.
	*/
	CSwordBibleModuleInfo( const CSwordBibleModuleInfo& m );
	/**
	* The destructor of this class
	*/
	~CSwordBibleModuleInfo();
	/**
	* Returns the number of avalable verses for the given chapter and book.
	*
	* @param book The number book we should use
	* @param chapter The chapter we should use
	* @return The number of verses for the given book and chapter
	*/
	virtual const unsigned int verseCount( const unsigned int book, const unsigned int chapter );
	/**
	* Returns the number of avalable verses for the given chapter and book.
	*
	* @param book The name of the book we use
	* @param chapter The number of the chapter we use
	* @return The number of verses for the given book and chapter
	*/
	virtual const unsigned int verseCount( const QString& book, const unsigned int chapter );
	/** Information about the chapters in a book.
	* @return The number of available chapters of the given book.
	* @return The number of chapters for the given book
	*/
	virtual const unsigned int chapterCount( const unsigned int book );
	/** Information about the chapters in a book.
	* @return The number of available chapters of the given book.
	*/
	virtual const unsigned int chapterCount( const QString& book );
	/** Return all book of this module.
	* @return A QStringList containing the books which are available in this module.
	*/
	virtual QStringList* const books();
	/**
	* Reimplementation, Returns the type
	*/
	virtual const CSwordModuleInfo::ModuleType type() const;
	/**
	* @return the book number, values starting with 1; 0 if not found
	*/
	const unsigned int bookNumber(const QString &book);
	/**
	* Returns true if his module has the text of desired type of testament
	*/
	const bool hasTestament( CSwordBibleModuleInfo::Testament );
	/** Reimplementation to clone this object. */
	virtual CSwordModuleInfo* clone();
	/**
	* Returns the key which represents the lower bound of this module.
	*/
	inline const CSwordVerseKey& lowerBound();
	/**
	* Returns the key which represents the upper bound of this module.
	*/
	inline const CSwordVerseKey& upperBound();

private:
	void initBounds();

	CSwordVerseKey m_lowerBound;
	CSwordVerseKey m_upperBound;

	QStringList* m_bookList; //This booklist is cached
	QString m_cachedLocale;
	short int m_hasOT;
	short int m_hasNT;
};

inline const CSwordModuleInfo::ModuleType CSwordBibleModuleInfo::type() const {
	return CSwordModuleInfo::Bible;
}

/** Returns the key which represents the lower bound of this module. */
inline const CSwordVerseKey& CSwordBibleModuleInfo::lowerBound() {
	initBounds();
	return m_lowerBound;
}

/** Returns the key which represents the lower bound of this module. */
inline const CSwordVerseKey& CSwordBibleModuleInfo::upperBound() {
	initBounds();
	return m_upperBound;
}

#endif
