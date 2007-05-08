/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cswordbiblemoduleinfo.h"
#include "cswordbackend.h"
#include "cswordversekey.h"

// #include "util/cpointers.h"

//Qt includes
#include <qfile.h>

//Sword includes
#include <versekey.h>

static sword::VerseKey staticKey;

CSwordBibleModuleInfo::CSwordBibleModuleInfo( sword::SWModule* module, CSwordBackend* const usedBackend )
: CSwordModuleInfo(module, usedBackend),
m_lowerBound(0),
m_upperBound(0),
m_bookList(0),
m_cachedLocale("unknown"),
m_hasOT(-1),
m_hasNT(-1) {}

CSwordBibleModuleInfo::CSwordBibleModuleInfo( const CSwordBibleModuleInfo& m ) :
CSwordModuleInfo(m),
m_lowerBound(0),
m_upperBound(0),
m_bookList(0) {
	if (m.m_bookList) {
		m_bookList = new QStringList();
		*m_bookList = *m.m_bookList;
	}

	m_hasOT = m.m_hasOT;
	m_hasNT = m.m_hasNT;
	m_cachedLocale = m.m_cachedLocale;
}

CSwordModuleInfo* CSwordBibleModuleInfo::clone() {
	return new CSwordBibleModuleInfo(*this);
}

CSwordBibleModuleInfo::~CSwordBibleModuleInfo() {
	delete m_bookList;
}

void CSwordBibleModuleInfo::initBounds() {
	if (m_hasOT == -1) {
		m_hasOT = hasTestament(OldTestament);
	}

	if (m_hasNT == -1) {
		m_hasNT = hasTestament(NewTestament);
	}

	if (m_hasOT) {
		m_lowerBound.key("Genesis 1:1");
	}
	else {
		m_lowerBound.key("Matthew 1:1");
	}

	if (!m_hasNT) {
		m_upperBound.key("Malachi 4:6");
	}
	else {
		m_upperBound.key("Revelation of John 22:21");
	}
}


/** Returns the books available in this module */
QStringList* const CSwordBibleModuleInfo::books() {
	if (m_cachedLocale != backend()->booknameLanguage()) { //if the locale has changed
		delete m_bookList;
		m_bookList = 0;
	}

	if (!m_bookList) {
		m_bookList = new QStringList();

		initBounds();
		int min = 0;
		int max = 1;

		//find out if we have ot and nt, only ot or only nt

		if (m_hasOT>0 && m_hasNT>0) { //both
			min = 0;
			max = 1;
		}
		else if (m_hasOT>0 && !m_hasNT) { //only OT
			min = 0;
			max = 0;
		}
		else if (!m_hasOT && m_hasNT>0) { //only NT
			min = 1;
			max = 1;
		}
		else if (!m_hasOT && !m_hasNT) { //somethings wrong here! - no OT and no NT
			qWarning("CSwordBibleModuleInfo (%s) no OT and not NT! Check your config!", module()->Name());
			min = 0;
			max = -1;
		}

		staticKey.setLocale(sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName());

		for (int i = min; i <= max; ++i) {
			for ( int j = 0; j < staticKey.BMAX[i]; ++j) {
				//   if (backend()->useICU())
				//       m_bookList->append( QString::fromUtf8(staticKey.books[i][j].name) );
				//      else
				//         m_bookList->append( QString::fromLocal8Bit(staticKey.books[i][j].name) );
				m_bookList->append( QString::fromUtf8(staticKey.books[i][j].name) );
			}
		}

		m_cachedLocale = backend()->booknameLanguage();
	}

	return m_bookList;
}

/** Returns the number of chapters for the given book. */
const unsigned int CSwordBibleModuleInfo::chapterCount(const unsigned int book) {
	int result = 0;

	if ( (book >= 1) && book <= (unsigned int)staticKey.BMAX[0] && hasTestament(OldTestament)) {  //Is the book in the old testament?
		result = (staticKey.books[0][book-1].chapmax);
	}
	else if ((book >= 1) && (book - staticKey.BMAX[0]) <= (unsigned int)staticKey.BMAX[1] && hasTestament(NewTestament) ) { //is the book in the new testament?
		result = (staticKey.books[1][book-1-staticKey.BMAX[0]].chapmax);
	}

	return result;
}

const unsigned int CSwordBibleModuleInfo::chapterCount(const QString& book) {
	return chapterCount( bookNumber(book) );
}

/** Returns the number of verses  for the given chapter. */

const unsigned int CSwordBibleModuleInfo::verseCount( const unsigned int book, const unsigned int chapter ) {
	unsigned int result = 0;

	if (book>=1 && (book <= (unsigned int)staticKey.BMAX[0]) && hasTestament(OldTestament) ) { //Is the book in the old testament?

		if (chapter <= chapterCount(book) ) //does the chapter exist?
			result = (staticKey.books[0][book-1].versemax[chapter-1]);
	}
	else if (book>=1 && (book - staticKey.BMAX[0]) <= (unsigned int)staticKey.BMAX[1] && hasTestament(NewTestament)) { //is the book in the new testament?

		if (chapter <= chapterCount(book) ) //does the chapter exist?
			result = staticKey.books[1][book-1-staticKey.BMAX[0]].versemax[chapter-1];
	}

	return result;
}

const unsigned int CSwordBibleModuleInfo::verseCount( const QString& book, const unsigned int chapter ) {
	return verseCount( bookNumber(book), chapter );
}

const unsigned int CSwordBibleModuleInfo::bookNumber(const QString &book) {
	unsigned int bookNumber = 0;
	bool found = false;
	staticKey.setLocale(sword::LocaleMgr::getSystemLocaleMgr()->getDefaultLocaleName());
	int min = 0;
	int max = 1;

	//find out if we have ot and nt, only ot or only nt
	initBounds();

	if ((m_hasOT>0 && m_hasNT>0) || (m_hasOT == m_hasNT == -1)) {
		min = 0;
		max = 1;
		bookNumber = 0;
	}
	else if (m_hasOT>0 && !m_hasNT) {
		min = 0;
		max = 0;
		bookNumber = 0;
	}
	else if (!m_hasOT && m_hasNT>0) {
		min = 1;
		max = 1;
		bookNumber = staticKey.BMAX[0];
	}
	else if (!m_hasOT && !m_hasNT) {
		min = 0;
		max = -1; //no loop
		bookNumber = 0;
	}

	for (int i = min; i <= max && !found; ++i) {
		for ( int j = 0; j < staticKey.BMAX[i] && !found; ++j) {
			++bookNumber;
			//    if (book == QString::fromLocal8Bit( staticKey.books[i][j].name) )

			if (book == QString::fromUtf8( staticKey.books[i][j].name) )
				found = true;
		}
	}

	return bookNumber;
}

/** Returns true if his module has the text of desired type of testament */
const bool CSwordBibleModuleInfo::hasTestament( CSwordBibleModuleInfo::Testament type ) {
	if (m_hasOT == -1 || m_hasNT == -1) {
		const bool oldStatus = module()->getSkipConsecutiveLinks();
		module()->setSkipConsecutiveLinks(true);

		*module() = sword::TOP; //position to first entry
		sword::VerseKey key( module()->KeyText() );

		if (key.Testament() == 1) { // OT && NT
			m_hasOT = 1;
		}
		else if (key.Testament() == 2) { //no OT
			m_hasOT = 0;
		}

		*module() = sword::BOTTOM;
		key = module()->KeyText();

		if (key.Testament() == 1) { // only OT, no NT
			m_hasNT = 0;
		}
		else if (key.Testament() == 2) { //has NT
			m_hasNT = 1;
		}

		module()->setSkipConsecutiveLinks(oldStatus);
	}

	switch (type) {

		case OldTestament:
		return m_hasOT>0;

		case NewTestament:
		return m_hasNT>0;

		default:
		return false;
	}
}

