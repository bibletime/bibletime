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

#include "backend/bookshelfmodel/btmodulehiddenfilterproxymodel.h"

#include "backend/bookshelfmodel/btbookshelfmodel.h"


BtModuleHiddenFilterProxyModel::BtModuleHiddenFilterProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent), m_enabled(true), m_showHidden(false),
        m_showShown(true) {
    setFilterRole(BtBookshelfModel::ModuleHiddenRole);
}

BtModuleHiddenFilterProxyModel::~BtModuleHiddenFilterProxyModel() {
    // Intentionally empty
}

void BtModuleHiddenFilterProxyModel::setEnabled(bool enable) {
    if (enable == m_enabled) return;
    m_enabled = enable;
    invalidateFilter();
}

void BtModuleHiddenFilterProxyModel::setShowHidden(bool show) {
    if (m_showHidden == show) return;
    m_showHidden = show;
    invalidateFilter();
}

void BtModuleHiddenFilterProxyModel::setShowShown(bool show) {
    if (m_showShown == show) return;
    m_showShown = show;
    invalidateFilter();
}

bool BtModuleHiddenFilterProxyModel::filterAcceptsRow(int row,
        const QModelIndex &parent) const {
    typedef Qt::CheckState CS;

    if (!m_enabled) return true;

    QAbstractItemModel *m(sourceModel());

    QModelIndex i(m->index(row, filterKeyColumn(), parent));
    if ((CS) m->data(i, filterRole()).toBool()) {
        return m_showHidden;
    }
    else {
        return m_showShown;
    }
}
