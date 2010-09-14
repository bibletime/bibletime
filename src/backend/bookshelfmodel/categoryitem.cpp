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

#include "backend/bookshelfmodel/categoryitem.h"


namespace BookshelfModel {

CategoryItem::CategoryItem(CSwordModuleInfo *module)
        : Item(ITEM_CATEGORY), m_category(module->category()) {
    // Intentionally empty
}

bool CategoryItem::operator<(const Item &other) const {
    if (other.type() != ITEM_CATEGORY) {
        return ITEM_CATEGORY < other.type();
    }
    const CategoryItem &o(static_cast<const CategoryItem &>(other));
    if (m_category   == CSwordModuleInfo::UnknownCategory) return false;
    if (o.m_category == CSwordModuleInfo::UnknownCategory) return true;
    return m_category < o.m_category;
}

} // namespace BookshelfModel
