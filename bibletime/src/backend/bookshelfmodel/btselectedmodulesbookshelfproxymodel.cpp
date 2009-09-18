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

#include "backend/bookshelfmodel/btselectedmodulesbookshelfproxymodel.h"

#include "backend/bookshelfmodel/btbookshelftreemodel.h"

BtSelectedModulesBookshelfProxyModel::BtSelectedModulesBookshelfProxyModel(
        QObject *parent)
    : QSortFilterProxyModel(parent), m_enabled(true), m_showChecked(true),
      m_showUnchecked(false)
{
    // Intentionally empty
}

BtSelectedModulesBookshelfProxyModel::~BtSelectedModulesBookshelfProxyModel() {
    // Intentionally empty
}

void BtSelectedModulesBookshelfProxyModel::setEnabled(bool enable) {
    if (enable == m_enabled) return;
    m_enabled = enable;
    invalidateFilter();
}

void BtSelectedModulesBookshelfProxyModel::setShowChecked(bool show) {
    if (m_showChecked == show) return;
    m_showChecked = show;
    invalidateFilter();
}

void BtSelectedModulesBookshelfProxyModel::setShowUnchecked(bool show) {
    if (m_showUnchecked == show) return;
    m_showUnchecked = show;
    invalidateFilter();
}

bool BtSelectedModulesBookshelfProxyModel::filterAcceptsRow(int row,
        const QModelIndex &parent) const
{
    typedef BtBookshelfTreeModel BTM;

    if (!m_enabled) return true;

    BTM *m(dynamic_cast<BTM*>(sourceModel()));
    if (m == 0) return true;

    QModelIndex i(m->index(row, 0, parent));
    if (m->data(i, BTM::CheckStateRole).toInt() == Qt::Unchecked) {
        return m_showUnchecked;
    } else {
        return m_showChecked;
    }
}
