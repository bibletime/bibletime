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

#include "btbookshelffiltermodel.h"

#include <utility>
#include "../../util/btassert.h"
#include "btbookshelfmodel.h"


BtBookshelfFilterModel::BtBookshelfFilterModel(QObject * const parent)
    : QSortFilterProxyModel(parent)
    , m_showHidden(false)
{ setDynamicSortFilter(true); }

template <typename Field, typename Value>
void BtBookshelfFilterModel::changeFilter(Field & field, Value && value) {
    if (field == value)
        return;
    #if (QT_VERSION >= QT_VERSION_CHECK(6, 9, 0))
    beginFilterChange();
    #endif
    field = std::forward<Value>(value);
    #if (QT_VERSION >= QT_VERSION_CHECK(6, 10, 0))
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
    #else
    invalidateFilter(); // together with beginFilterChange() in Qt 6.9
    #endif
}

// Name filter:

void BtBookshelfFilterModel::setNameFilterFixedString(QString const & filter)
{ changeFilter(m_nameFilter, filter); }

// Hidden filter:

void BtBookshelfFilterModel::setShowHidden(bool const show)
{ changeFilter(m_showHidden, show); }

// Filtering:

bool BtBookshelfFilterModel::filterAcceptsRow(int row,
                                              QModelIndex const & parent) const
{
    auto const * const m = sourceModel();
    BT_ASSERT(m);

    auto const itemIndex = m->index(row, 0, parent);
    auto const numChildren = m->rowCount(itemIndex);
    if (numChildren == 0) {
        if (!m_nameFilter.isEmpty()) {
            auto const data =
                    m->data(itemIndex, BtBookshelfModel::ModuleNameRole);
            if (!data.toString().contains(m_nameFilter, Qt::CaseInsensitive))
                return false;
        }
        if (!m_showHidden) {
            auto const isHidden =
                    static_cast<Qt::CheckState>(
                        m->data(itemIndex,
                                BtBookshelfModel::ModuleHiddenRole).toBool());
            if (isHidden)
                return false;
        }
        return true;
    }

    for (int i = 0; i < numChildren; ++i)
        if (filterAcceptsRow(i, itemIndex))
            return true;
    return false;
}
