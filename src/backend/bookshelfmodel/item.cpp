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

#include "backend/bookshelfmodel/item.h"

#include "backend/bookshelfmodel/categoryitem.h"
#include "backend/bookshelfmodel/distributionitem.h"
#include "backend/bookshelfmodel/languageitem.h"


namespace BookshelfModel {

Item::Item(Type type)
        : m_type(type), m_parent(0), m_checkState(Qt::Unchecked) {
    // Intentionally empty
}

Item::~Item() {
    qDeleteAll(m_children);
}

int Item::indexFor(Item *newItem) {
    Q_ASSERT(newItem != 0);

    if (m_children.empty()) return 0;

    int i(0);
    for (;;) {
        Item *nextItem(m_children.at(i));
        Q_ASSERT(nextItem->type() == newItem->type());
        if (*newItem < *nextItem) {
            return i;
        }
        i++;
        if (i >= m_children.size()) {
            return i;
        }
    }
}

bool Item::operator<(const Item &other) const {
    if (m_type != other.type()) {
        return m_type < other.type();
    }
    return name().localeAwareCompare(other.name()) < 0;
}

bool Item::isHidden() const {
    if (m_children.empty()) return true;
    Q_FOREACH(Item *child, m_children) {
        if (!child->isHidden()) return false;
    }
    return true;
}

} // namespace BookshelfModel
