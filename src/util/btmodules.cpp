/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "btmodules.h"

#include "backend/managers/cswordbackend.h"


bool equalModuleCategories(const QString& name1, const QString& name2) {
    CSwordModuleInfo* m1 = CSwordBackend::instance()->findModuleByName(name1);
    CSwordModuleInfo* m2 = CSwordBackend::instance()->findModuleByName(name2);
    if (m1 == 0 || m2 == 0)
        return false;
    if (m1->category() == m2->category()) {
        return true;
    }
    return false;
}

int  leftLikeParallelModules(const QStringList modules) {
    // Count the number of leftmost modules that are of the same category
    int leftLikeModules = 0;
    for (int i = 0; i < modules.count(); i++) {
        if ( ! equalModuleCategories(modules.at(0), modules.at(i))) {
            break;
        }
        leftLikeModules++;
    }
    return leftLikeModules;
}

