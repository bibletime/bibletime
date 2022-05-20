/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "cswordbiblemoduleinfo.h"


/**
  \brief Commentary module implementation.

  This CSwordModule implementation provides access to Sword's commentary modules.
*/
class CSwordCommentaryModuleInfo final: public CSwordBibleModuleInfo {
        Q_OBJECT

    public: // methods:

        CSwordCommentaryModuleInfo(sword::SWModule & module,
                                   CSwordBackend & backend)
            : CSwordBibleModuleInfo(module,
                                    backend,
                                    CSwordModuleInfo::Commentary) {}

        bool isWritable() const final override;
};
