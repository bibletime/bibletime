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

#include "backend/bookshelfmodel/btbookshelffiltermodel.h"

#include "backend/bookshelfmodel/btbookshelfmodel.h"


BtBookshelfFilterModel::BtBookshelfFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent), m_enabled(true),
      m_nameFilterRole(BtBookshelfModel::ModuleNameRole), m_nameFilterColumn(0),
      m_nameFilterCase(Qt::CaseInsensitive),
      m_hiddenFilterRole(BtBookshelfModel::ModuleHiddenRole),
      m_hiddenFilterColumn(0), m_showHidden(false), m_showShown(true)
{
    // Intentionally empty
}

BtBookshelfFilterModel::~BtBookshelfFilterModel() {
    // Intentionally empty
}

void BtBookshelfFilterModel::setEnabled(bool enable) {
    if (enable == m_enabled) return;
    m_enabled = enable;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterRole(int role) {
    if (m_nameFilterRole == role) return;
    m_nameFilterRole = role;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterKeyColumn(int column) {
    if (m_nameFilterColumn == column) return;
    m_nameFilterColumn = column;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterFixedString(const QString &filter) {
    if (m_nameFilter == filter) return;
    m_nameFilter = filter;
    invalidateFilter();
}

void BtBookshelfFilterModel::setNameFilterCase(Qt::CaseSensitivity value) {
    if (m_nameFilterCase == value) return;
    m_nameFilterCase = value;
    invalidateFilter();
}

void BtBookshelfFilterModel::setHiddenFilterRole(int role) {
    if (m_hiddenFilterRole == role) return;
    m_hiddenFilterRole = role;
    invalidateFilter();
}

void BtBookshelfFilterModel::setHiddenFilterKeyColumn(int column) {
    if (m_hiddenFilterColumn == column) return;
    m_hiddenFilterColumn = column;
    invalidateFilter();
}

void BtBookshelfFilterModel::setShowHidden(bool show) {
    if (m_showHidden == show) return;
    m_showHidden = show;
    invalidateFilter();
}

void BtBookshelfFilterModel::setShowShown(bool show) {
    if (m_showShown == show) return;
    m_showShown = show;
    invalidateFilter();
}

bool BtBookshelfFilterModel::filterAcceptsRow(int row,
        const QModelIndex &parent) const
{
    if (!m_enabled) return true;

    if (!hiddenFilterAcceptsRow(row, parent)) return false;
    if (!nameFilterAcceptsRow(row, parent)) return false;
    return true;
}

bool BtBookshelfFilterModel::nameFilterAcceptsRow(int row, const QModelIndex &p)
        const
{
    if (!m_enabled) return true;
    if (m_nameFilter.isEmpty()) return true;

    const QAbstractItemModel *m(sourceModel());
    Q_ASSERT(m != 0);

    QModelIndex itemIndex(m->index(row, m_nameFilterColumn, p));
    int numChildren(m->rowCount(itemIndex));
    if (numChildren == 0) {
        QVariant data(m->data(itemIndex, m_nameFilterRole));
        return data.toString().contains(m_nameFilter, m_nameFilterCase);
    }
    else {
        for (int i(0); i < numChildren; i++) {
            if (filterAcceptsRow(i, itemIndex)) return true;
        }
        return false;
    }
}

bool BtBookshelfFilterModel::hiddenFilterAcceptsRow(int row,
        const QModelIndex &parent) const
{
    if (m_showHidden && m_showShown) return true;

    typedef Qt::CheckState CS;

    QAbstractItemModel *m(sourceModel());

    QModelIndex itemIndex(m->index(row, m_hiddenFilterColumn, parent));
    int numChildren(m->rowCount(itemIndex));
    if (numChildren == 0) {
        if ((CS) m->data(itemIndex, m_hiddenFilterRole).toBool()) {
            return m_showHidden;
        }
        else {
            return m_showShown;
        }
    }
    else {
        for (int i(0); i < numChildren; i++) {
            if (filterAcceptsRow(i, itemIndex)) return true;
        }
        return false;
    }
}

