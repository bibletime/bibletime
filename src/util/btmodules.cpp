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

#include "btmodules.h"

#include "../backend/drivers/cswordmoduleinfo.h"
#include "../backend/drivers/btmodulelist.h"


int leftLikeParallelModules(BtModuleList const & modules) {
    // Count the number of leftmost modules that are of the same category
    auto const numModules = modules.size();
    if (numModules <= 0)
        return 0;
    int leftLikeModules = 1;
    if (numModules > 1) {
        auto it = modules.begin();
        auto const category = (*it)->category();
        while ((*++it)->category() == category)
            if (++leftLikeModules == numModules)
                break;
    }
    return leftLikeModules;
}
