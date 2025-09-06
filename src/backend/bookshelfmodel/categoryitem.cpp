/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "categoryitem.h"

#include <QIcon>
#include "../drivers/cswordmoduleinfo.h"


namespace BookshelfModel {


QVariant CategoryItem::data(int const role) const {
    switch (role) {

        case Qt::DisplayRole:
            return CSwordModuleInfo::categoryName(m_category);

        case Qt::DecorationRole:
            return CSwordModuleInfo::categoryIcon(m_category);

        default:
            return Item::data(role);

    }
}

bool CategoryItem::operator<(Item const & other) const {
    if (other.type() != ITEM_CATEGORY)
        return ITEM_CATEGORY < other.type();
    return m_category < static_cast<CategoryItem const &>(other).m_category;
}

} // namespace BookshelfModel
