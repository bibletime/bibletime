/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cswordcommentarymoduleinfo.h"

#include <string>

CSwordCommentaryModuleInfo::CSwordCommentaryModuleInfo( sword::SWModule* module, CSwordBackend* const usedBackend)
: CSwordBibleModuleInfo(module, usedBackend) {}

CSwordCommentaryModuleInfo::~CSwordCommentaryModuleInfo() {}

/** No descriptions */
CSwordModuleInfo* CSwordCommentaryModuleInfo::clone() {
	return new CSwordCommentaryModuleInfo(*this);
}

/** Returns true if this module may be written by the write display windows. */
const bool CSwordCommentaryModuleInfo::isWritable() const {
	//  qWarning(module()->getConfigEntry("ModDrv"));
	//a module is only writable if it's a RawFiles module with writable returning true

	if ( (std::string(module()->getConfigEntry("ModDrv")) == std::string("RawFiles")) && module()->isWritable()) {
	return true;
	};

	return false;
}
