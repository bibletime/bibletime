/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "indexingitem.h"


namespace BookshelfModel {

IndexingItem::IndexingItem(CSwordModuleInfo *module)
        : Item(Item::ITEM_INDEXING), m_indexed(module->hasIndex()) {
    // Intentionally empty
}

QVariant IndexingItem::data(int role) const {
    switch (role) {
        case Qt::DisplayRole:
            if (m_indexed) {
                return QObject::tr("Indexed works");
            }
            else {
                return QObject::tr("Unindexed works");
            }
        default:
            return Item::data(role);
    }
}

} // namespace BookshelfModel
