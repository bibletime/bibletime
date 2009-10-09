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

#include "backend/bookshelfmodel/btcheckstatefilterproxymodel.h"

BtCheckStateFilterProxyModel::BtCheckStateFilterProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent), m_enabled(true), m_showChecked(true),
        m_showUnchecked(false), m_showPartiallyChecked(true) {
    setFilterRole(Qt::CheckStateRole);
}

BtCheckStateFilterProxyModel::~BtCheckStateFilterProxyModel() {
    // Intentionally empty
}

void BtCheckStateFilterProxyModel::setEnabled(bool enable) {
    if (enable == m_enabled) return;
    m_enabled = enable;
    invalidateFilter();
}

void BtCheckStateFilterProxyModel::setShowChecked(bool show) {
    if (m_showChecked == show) return;
    m_showChecked = show;
    invalidateFilter();
}

void BtCheckStateFilterProxyModel::setShowUnchecked(bool show) {
    if (m_showUnchecked == show) return;
    m_showUnchecked = show;
    invalidateFilter();
}

void BtCheckStateFilterProxyModel::setShowPartiallyChecked(bool show) {
    if (m_showPartiallyChecked == show) return;
    m_showPartiallyChecked = show;
    invalidateFilter();
}

bool BtCheckStateFilterProxyModel::filterAcceptsRow(int row,
        const QModelIndex &parent) const {
    typedef Qt::CheckState CS;

    if (!m_enabled) return true;

    QAbstractItemModel *m(sourceModel());

    QModelIndex i(m->index(row, filterKeyColumn(), parent));
    CS state((CS) m->data(i, filterRole()).toInt());
    Q_ASSERT(state == Qt::Checked || state == Qt::Unchecked ||
             state == Qt::PartiallyChecked);
    if (state == Qt::Unchecked) {
        return m_showUnchecked;
    }
    else if (state == Qt::Checked) {
        return m_showChecked;
    }
    else {
        return m_showPartiallyChecked;
    }
}
