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

#include "backend/bookshelfmodel/btmodulecategoryfilterproxymodel.h"

#include "backend/bookshelfmodel/btbookshelfmodel.h"


typedef CSwordModuleInfo::Categories CS;

BtModuleCategoryFilterProxyModel::BtModuleCategoryFilterProxyModel(
    QObject *parent)
        : QSortFilterProxyModel(parent), m_filter(CSwordModuleInfo::AllCategories),
        m_enabled(true) {
    setFilterRole(BtBookshelfModel::ModuleCategoryRole);
}

BtModuleCategoryFilterProxyModel::~BtModuleCategoryFilterProxyModel() {
    // Intentionally empty
}

void BtModuleCategoryFilterProxyModel::setEnabled(bool enable) {
    m_enabled = enable;
    invalidateFilter();
}

void BtModuleCategoryFilterProxyModel::setShownCategories(CS cs) {
    if (m_filter == cs) return;
    m_filter = cs;
    invalidateFilter();
}

void BtModuleCategoryFilterProxyModel::setHiddenCategories(CS cs) {
    cs ^= CSwordModuleInfo::AllCategories;
    if (m_filter == cs) return;
    m_filter = cs;
    invalidateFilter();
}

bool BtModuleCategoryFilterProxyModel::filterAcceptsRow(int row,
        const QModelIndex &parent) const {
    typedef CSwordModuleInfo::Category C;

    if (!m_enabled) return true;

    const QAbstractItemModel *m(sourceModel());
    Q_ASSERT(m != 0);

    QModelIndex itemIndex(m->index(row, filterKeyColumn(), parent));
    int numChildren(m->rowCount(itemIndex));
    if (numChildren == 0) {
        return m_filter.testFlag(m->data(itemIndex, filterRole()).value<C>());
    }
    else {
        for (int i(0); i < numChildren; i++) {
            if (filterAcceptsRow(i, itemIndex)) return true;
        }
        return false;
    }
}
