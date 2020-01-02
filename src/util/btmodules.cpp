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

#include "btmodules.h"

#include "../backend/managers/cswordbackend.h"


namespace {
inline bool equalModuleCategories(QString const & n1, QString const & n2) {
    if (CSwordModuleInfo const * const m1 =
                CSwordBackend::instance()->findModuleByName(n1))
        if (CSwordModuleInfo const * const m2 =
                    CSwordBackend::instance()->findModuleByName(n2))
            return m1->category() == m2->category();
    return false;
}
} // anonymous namespace

int leftLikeParallelModules(QStringList const & modules) {
    // Count the number of leftmost modules that are of the same category
    int leftLikeModules = 0;
    for (int i = 0; i < modules.count(); i++) {
        if (!equalModuleCategories(modules.at(0), modules.at(i)))
            break;
        leftLikeModules++;
    }
    return leftLikeModules;
}
