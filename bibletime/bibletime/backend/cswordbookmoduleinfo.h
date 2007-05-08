/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSWORDGENBOOKMODULEINFO_H
#define CSWORDGENBOOKMODULEINFO_H

//BibleTime includes
#include "cswordmoduleinfo.h"

//Sword includes
#include <treekeyidx.h>

/** Class for generic book support
  * @author The BibleTime team
  */

class CSwordBookModuleInfo : public CSwordModuleInfo {

public:
	/** Constructor.
	* @param module The module which belongs to this object
	* @param backend The parent backend for this book module.
	*/
	CSwordBookModuleInfo( sword::SWModule* module, CSwordBackend* const backend = CPointers::backend() );
	/** Copy constructor.
	* Copy constructor to copy the passed parameter.
	* @param module The module which should be copied.
	*/
	CSwordBookModuleInfo( const CSwordBookModuleInfo& module );
	/** Destructor.
	*/
	~CSwordBookModuleInfo();
	/**
	* Returns the type of the module.
	*/
	virtual const CSwordModuleInfo::ModuleType type() const;
	/**
	* Returns the maximal depth of sections and subsections.
	*/
	const int depth();
	/**
	* @return A treekey filled with the structure of this module. Don't delete the returned key because it's casted from the module object.
	*/
	sword::TreeKeyIdx* const tree() const;

private:
	/**
	* A recursive helper function to help computng the module depth!
	*/
	void computeDepth(sword::TreeKeyIdx* key, int level = 0 );
	int m_depth;
};

inline const CSwordBookModuleInfo::ModuleType CSwordBookModuleInfo::type() const {
	return CSwordModuleInfo::GenericBook;
}


#endif
