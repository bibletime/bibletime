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

#include "backend/bookshelfmodel/btmodulenamefilterproxymodel.h"

#include "backend/bookshelfmodel/btbookshelfmodel.h"


BtModuleNameFilterProxyModel::BtModuleNameFilterProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent), m_enabled(true) {
    setFilterRole(BtBookshelfModel::ModuleNameRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

BtModuleNameFilterProxyModel::~BtModuleNameFilterProxyModel() {
    // Intentionally empty
}

bool BtModuleNameFilterProxyModel::filterAcceptsRow(int row,
        const QModelIndex &p) const {
    if (!m_enabled) return true;

    const QAbstractItemModel *m(sourceModel());
    Q_ASSERT(m != 0);

    QModelIndex itemIndex(m->index(row, filterKeyColumn(), p));
    int numChildren(m->rowCount(itemIndex));
    if (numChildren == 0) {
        return QSortFilterProxyModel::filterAcceptsRow(row, p);
    }
    else {
        for (int i(0); i < numChildren; i++) {
            if (filterAcceptsRow(i, itemIndex)) return true;
        }
        return false;
    }
}
