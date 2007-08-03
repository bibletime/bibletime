/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDLEXICONMODULEINFO_H
#define CSWORDLEXICONMODULEINFO_H

//own includes
#include "cswordmoduleinfo.h"

//Qt includes
#include <QStringList>

/**
 * The implementation of CModuleInfo for the Sword lexiccons and citionaries.
 * @author The BibleTime team
 * @version $Id: cswordlexiconmoduleinfo.h,v 1.12 2006/02/25 11:38:15 joachim Exp $
 */

class CSwordLexiconModuleInfo : public CSwordModuleInfo {

public:
	/**
	* The standard constructor fot this object.
	* A default constructor doesn't exist. Use this one.
	*/
	CSwordLexiconModuleInfo( sword::SWModule* module, CSwordBackend* const );
	/**
	* The copy constructor
	*/
	CSwordLexiconModuleInfo( const CSwordLexiconModuleInfo& m );
	/** Reimplementation to return a valid clone.
	*/
	virtual CSwordModuleInfo* clone();
	/** Destructor.
	*/
	virtual ~CSwordLexiconModuleInfo();
	/**
	* Returns the entries of the module.
	* This function returns the entries of the modules represented by this object.
	* If this function is called for the first time the list is load from disk and stored in a list which cahes it.
	* If the function is called again, the cached list is returned so we have a major speed improvement.
	* @return The list of lexicon entries
	*/
	QStringList* const entries();
	/**
	* Reimplementation, to return the right type for this lexicon.
	*/
	virtual const CSwordModuleInfo::ModuleType type() const;
	/**
	* Jumps to the closest entry in the module.
	*/
	const bool snap();

private:
	/**
	* This is the list which caches the entres of the module.
	*/
	QStringList* m_entryList;
};

inline const CSwordModuleInfo::ModuleType CSwordLexiconModuleInfo::type() const {
	return CSwordModuleInfo::Lexicon;
}

#endif
