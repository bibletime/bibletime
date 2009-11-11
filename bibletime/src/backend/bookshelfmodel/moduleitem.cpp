/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2009 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "backend/bookshelfmodel/moduleitem.h"

#include "util/cresmgr.h"


namespace BookshelfModel {

ModuleItem::ModuleItem(CSwordModuleInfo *module)
        : Item(ITEM_MODULE), m_moduleInfo(module) {
    Q_ASSERT(module != 0);
}

} // namespace BookshelfModel
