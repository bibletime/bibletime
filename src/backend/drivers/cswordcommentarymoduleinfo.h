/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#ifndef CSWORDCOMMENTARYMODULEINFO_H
#define CSWORDCOMMENTARYMODULEINFO_H

//own includes
#include "cswordbiblemoduleinfo.h"

/** Commentary module implementation.
 * This CSwordModule implementation provides access to Sword's commentary modules.
 * @author The BibleTime team
 * @version $Id: cswordcommentarymoduleinfo.h,v 1.13 2006/02/25 11:38:15 joachim Exp $
 */

class CSwordCommentaryModuleInfo : public CSwordBibleModuleInfo {

    public:
        CSwordCommentaryModuleInfo( sword::SWModule* module, CSwordBackend* const );
        ~CSwordCommentaryModuleInfo();
        /** Reimplementation to return the commentary type.
        */
        virtual CSwordModuleInfo::ModuleType type() const;
        /** Reimplementation to clone the current object.
        */
        virtual CSwordModuleInfo* clone();
        /**
        * Returns true if this module may be written by the write display windows.
        */
        virtual bool isWritable() const;
};

inline CSwordModuleInfo::ModuleType CSwordCommentaryModuleInfo::type() const {
    return CSwordModuleInfo::Commentary;
}

#endif
