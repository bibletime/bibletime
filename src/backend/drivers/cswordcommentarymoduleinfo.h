/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#ifndef CSWORDCOMMENTARYMODULEINFO_H
#define CSWORDCOMMENTARYMODULEINFO_H

#include "cswordbiblemoduleinfo.h"


/**
  \brief Commentary module implementation.

  This CSwordModule implementation provides access to Sword's commentary modules.
*/
class CSwordCommentaryModuleInfo: public CSwordBibleModuleInfo {
        Q_OBJECT

    public: /* Methods: */

        inline CSwordCommentaryModuleInfo(sword::SWModule & module,
                                          CSwordBackend & backend)
            : CSwordBibleModuleInfo(module,
                                    backend,
                                    CSwordModuleInfo::Commentary) {}

        bool isWritable() const override;
};

#endif
