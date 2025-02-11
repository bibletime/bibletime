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

#include "item.h"

#include <QtAlgorithms>
#include <QString>
#include "../../util/btassert.h"
#include "btbookshelfmodel.h"


namespace BookshelfModel {

Item::~Item() {
    qDeleteAll(m_children);
}

int Item::indexFor(Item const & newItem) {
    if (m_children.empty())
        return 0;

    int i = 0;
    for (;;) {
        auto const & nextItem = *m_children.at(i);
        BT_ASSERT(nextItem.type() == newItem.type());
        if (newItem < nextItem)
            return i;

        i++;
        if (i >= m_children.size())
            return i;
    }
}

QVariant Item::data(int const role) const {
    switch (role) {

        case Qt::CheckStateRole:
            return m_checkState;

        case BtBookshelfModel::ModuleHiddenRole:
            if (m_children.empty())
                return true;

            for (auto const * const child : m_children)
                if (!child->data(role).toBool())
                    return false;
            return true;

        default:
            return QVariant();

    }
}

bool Item::operator<(Item const & other) const {
    if (m_type != other.type())
        return m_type < other.type();

    auto const first(data(Qt::DisplayRole).toString().toLower());
    auto const second(other.data(Qt::DisplayRole).toString().toLower());
    return first.localeAwareCompare(second) < 0;
}

bool RootItem::fitFor(CSwordModuleInfo const &) const { return true; }

} // namespace BookshelfModel
