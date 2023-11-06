/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookshelffiltermodel.h"

#include <QAbstractItemModel>
#include <QVariant>
#include "../../util/btassert.h"
#include "../drivers/cswordmoduleinfo.h"
#include "btbookshelfmodel.h"


BtBookshelfFilterModel::BtBookshelfFilterModel(QObject * const parent)
    : QSortFilterProxyModel(parent)
    , m_enabled(true)
    , m_nameFilterRole(BtBookshelfModel::ModuleNameRole)
    , m_nameFilterColumn(0)
    , m_nameFilterCase(Qt::CaseInsensitive)
    , m_hiddenFilterRole(BtBookshelfModel::ModuleHiddenRole)
    , m_hiddenFilterColumn(0)
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

void BtBookshelfFilterModel::setNameFilterRole(int const role) {
    if (m_nameFilterRole == role)
        return;
    m_nameFilterRole = role;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterKeyColumn(int const column) {
    if (m_nameFilterColumn == column)
        return;
    m_nameFilterColumn = column;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterFixedString(QString const & filter) {
    if (m_nameFilter == filter)
        return;
    m_nameFilter = filter;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterCase(Qt::CaseSensitivity const value){
    if (m_nameFilterCase == value)
        return;
    m_nameFilterCase = value;
    invalidateFilter();
}

// Hidden filter:

void BtBookshelfFilterModel::setHiddenFilterRole(int const role) {
    if (m_hiddenFilterRole == role)
        return;
    m_hiddenFilterRole = role;
    invalidateFilter();
}

void BtBookshelfFilterModel::setHiddenFilterKeyColumn(int const column) {
    if (m_hiddenFilterColumn == column)
        return;
    m_hiddenFilterColumn = column;
    invalidateFilter();
}

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

    if (!hiddenFilterAcceptsRow(row, parent))
        return false;
    if (!nameFilterAcceptsRow(row, parent))
        return false;
    return true;
}

bool BtBookshelfFilterModel::nameFilterAcceptsRow(
        int row,
        QModelIndex const & parent) const
{
    if (m_nameFilter.isEmpty())
        return true;

    auto const * const m = sourceModel();
    BT_ASSERT(m);

    auto const itemIndex = m->index(row, m_nameFilterColumn, parent);
    auto const numChildren = m->rowCount(itemIndex);
    if (numChildren == 0) {
        auto const data = m->data(itemIndex, m_nameFilterRole);
        return data.toString().contains(m_nameFilter, m_nameFilterCase);
    }

    for (int i = 0; i < numChildren; ++i)
        if (filterAcceptsRow(i, itemIndex))
            return true;
    return false;
}

bool BtBookshelfFilterModel::hiddenFilterAcceptsRow(
        int row,
        QModelIndex const & parent) const
{
    if (m_showHidden && m_showShown)
        return true;

    auto const * const m = sourceModel();
    BT_ASSERT(m);

    auto const itemIndex = m->index(row, m_hiddenFilterColumn, parent);
    auto const numChildren = m->rowCount(itemIndex);
    if (numChildren == 0) {
        if (static_cast<Qt::CheckState>(m->data(itemIndex,
                                                m_hiddenFilterRole).toBool()))
            return m_showHidden;
        return m_showShown;
    }

    for (int i = 0; i < numChildren; ++i)
        if (filterAcceptsRow(i, itemIndex))
            return true;
    return false;
}
