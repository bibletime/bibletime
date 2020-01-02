/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookshelffiltermodel.h"

#include "btbookshelfmodel.h"
#include "../../util/btassert.h"


BtBookshelfFilterModel::BtBookshelfFilterModel(QObject * parent)
    : QSortFilterProxyModel(parent)
    , m_enabled(true)
    , m_nameFilterRole(BtBookshelfModel::ModuleNameRole)
    , m_nameFilterColumn(0)
    , m_nameFilterCase(Qt::CaseInsensitive)
    , m_hiddenFilterRole(BtBookshelfModel::ModuleHiddenRole)
    , m_hiddenFilterColumn(0)
    , m_showHidden(false)
    , m_showShown(true)
    , m_categoryFilter(CSwordModuleInfo::AllCategories)
    , m_categoryFilterRole(BtBookshelfModel::ModuleCategoryRole)
    , m_categoryFilterColumn(0)
{
    setDynamicSortFilter(true);
}

void BtBookshelfFilterModel::setEnabled(bool enable) {
    if (enable == m_enabled)
        return;
    m_enabled = enable;
    invalidateFilter();
}

// Name filter:

void BtBookshelfFilterModel::setNameFilterRole(int role) {
    if (m_nameFilterRole == role)
        return;
    m_nameFilterRole = role;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterKeyColumn(int column) {
    if (m_nameFilterColumn == column)
        return;
    m_nameFilterColumn = column;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterFixedString(const QString &filter) {
    if (m_nameFilter == filter)
        return;
    m_nameFilter = filter;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterCase(Qt::CaseSensitivity value) {
    if (m_nameFilterCase == value)
        return;
    m_nameFilterCase = value;
    invalidateFilter();
}

// Hidden filter:

void BtBookshelfFilterModel::setHiddenFilterRole(int role) {
    if (m_hiddenFilterRole == role)
        return;
    m_hiddenFilterRole = role;
    invalidateFilter();
}

void BtBookshelfFilterModel::setHiddenFilterKeyColumn(int column) {
    if (m_hiddenFilterColumn == column)
        return;
    m_hiddenFilterColumn = column;
    invalidateFilter();
}

void BtBookshelfFilterModel::setShowHidden(bool show) {
    if (m_showHidden == show)
        return;
    m_showHidden = show;
    invalidateFilter();
}

void BtBookshelfFilterModel::setShowShown(bool show) {
    if (m_showShown == show)
        return;
    m_showShown = show;
    invalidateFilter();
}

// Category filter:

void BtBookshelfFilterModel::setCategoryFilterRole(int role) {
    if (m_categoryFilterRole == role)
        return;
    m_categoryFilterRole = role;
    invalidateFilter();
}

void BtBookshelfFilterModel::setCategoryFilterKeyColumn(int column) {
    if (m_categoryFilterColumn == column)
        return;
    m_categoryFilterColumn = column;
    invalidateFilter();
}

void BtBookshelfFilterModel::setShownCategories(
        const CSwordModuleInfo::Categories & categories)
{
    if (m_categoryFilter == categories)
        return;
    m_categoryFilter = categories;
    invalidateFilter();
}

// Filtering:

bool BtBookshelfFilterModel::filterAcceptsRow(int row,
                                              const QModelIndex & parent) const
{
    if (!m_enabled)
        return true;

    if (!hiddenFilterAcceptsRow(row, parent))
        return false;
    if (!nameFilterAcceptsRow(row, parent))
        return false;
    if (!categoryFilterAcceptsRow(row, parent))
        return false;
    return true;
}

bool BtBookshelfFilterModel::nameFilterAcceptsRow(int row,
                                                  const QModelIndex & parent) const
{
    if (m_nameFilter.isEmpty())
        return true;

    const QAbstractItemModel * const m = sourceModel();
    BT_ASSERT(m);

    QModelIndex itemIndex(m->index(row, m_nameFilterColumn, parent));
    int numChildren(m->rowCount(itemIndex));
    if (numChildren == 0) {
        QVariant data(m->data(itemIndex, m_nameFilterRole));
        return data.toString().contains(m_nameFilter, m_nameFilterCase);
    }

    for (int i = 0; i < numChildren; i++)
        if (filterAcceptsRow(i, itemIndex))
            return true;
    return false;
}

bool BtBookshelfFilterModel::hiddenFilterAcceptsRow(int row,
                                                    const QModelIndex & parent) const
{
    if (m_showHidden && m_showShown)
        return true;

    const QAbstractItemModel * const m = sourceModel();
    BT_ASSERT(m);

    const QModelIndex itemIndex = m->index(row, m_hiddenFilterColumn, parent);
    const int numChildren = m->rowCount(itemIndex);
    if (numChildren == 0) {
        if (static_cast<Qt::CheckState>(m->data(itemIndex, m_hiddenFilterRole).toBool()))
            return m_showHidden;
        return m_showShown;
    }

    for (int i = 0; i < numChildren; i++)
        if (filterAcceptsRow(i, itemIndex))
            return true;
    return false;
}

bool BtBookshelfFilterModel::categoryFilterAcceptsRow(int row,
                                                      const QModelIndex & parent) const
{
    if (m_categoryFilter == CSwordModuleInfo::AllCategories)
        return true;

    const QAbstractItemModel * const m = sourceModel();
    BT_ASSERT(m);

    const QModelIndex itemIndex(m->index(row, m_categoryFilterColumn, parent));
    const int numChildren(m->rowCount(itemIndex));
    if (numChildren == 0) {
        const int cat = m->data(itemIndex, m_categoryFilterRole).toInt();
        return m_categoryFilter.testFlag(static_cast<CSwordModuleInfo::Category>(cat));
    }

    for (int i = 0; i < numChildren; i++)
        if (filterAcceptsRow(i, itemIndex))
            return true;
    return false;
}
