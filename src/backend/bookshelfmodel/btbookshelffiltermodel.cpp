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

#include <QAbstractItemModel>
#include <QVariant>
#include "../../util/btassert.h"
#include "btbookshelfmodel.h"


BtBookshelfFilterModel::BtBookshelfFilterModel(QObject * const parent)
    : QSortFilterProxyModel(parent)
    , m_enabled(true)
    , m_showHidden(false)
    , m_showShown(true)
{ setDynamicSortFilter(true); }

void BtBookshelfFilterModel::setEnabled(bool const enable) {
    if (enable == m_enabled)
        return;
    m_enabled = enable;
    invalidateFilter();
}

// Name filter:

void BtBookshelfFilterModel::setNameFilterFixedString(QString const & filter) {
    if (m_nameFilter == filter)
        return;
    m_nameFilter = filter;
    invalidateFilter();
}

// Hidden filter:

void BtBookshelfFilterModel::setShowHidden(bool const show) {
    if (m_showHidden == show)
        return;
    m_showHidden = show;
    invalidateFilter();
}

void BtBookshelfFilterModel::setShowShown(bool const show) {
    if (m_showShown == show)
        return;
    m_showShown = show;
    invalidateFilter();
}

// Filtering:

bool BtBookshelfFilterModel::filterAcceptsRow(int row,
                                              QModelIndex const & parent) const
{
    if (!m_enabled)
        return true;

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
        if (!m_showHidden || !m_showShown) {
            auto const isHidden =
                    static_cast<Qt::CheckState>(
                        m->data(itemIndex,
                                BtBookshelfModel::ModuleHiddenRole).toBool());
            if ((isHidden && !m_showHidden) || (!isHidden && !m_showShown))
                return false;
        }
        return true;
    }

    for (int i = 0; i < numChildren; ++i)
        if (filterAcceptsRow(i, itemIndex))
            return true;
    return false;
}
