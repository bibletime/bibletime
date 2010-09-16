/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/bookshelfmanager/removepage/btremovepagetreemodel.h"


BtRemovePageTreeModel::BtRemovePageTreeModel(const QString &configKey, QObject *parent)
    : BtBookshelfTreeModel(configKey, parent)
{
    setCheckable(true);
    setDefaultChecked(BtBookshelfTreeModel::UNCHECKED);
}

int BtRemovePageTreeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return 2;
}

QVariant BtRemovePageTreeModel::data(const QModelIndex &i, int role) const {
    if (i.column() == 1) {
        QModelIndex realIndex(index(i.row(), 0, i.parent()));
        switch (role) {
            case Qt::DisplayRole:
            case Qt::ToolTipRole:
                return BtBookshelfTreeModel::data(realIndex, BtBookshelfModel::ModuleInstallPathRole);
            default:
                break;
        }
    }
    else {
        return BtBookshelfTreeModel::data(i, role);
    }

    return QVariant();
}

QVariant BtRemovePageTreeModel::headerData(int section,
        Qt::Orientation orientation,
        int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Work");
        }
        else if (section == 1) {
            return tr("Install path");
        }
    }

    return QVariant();
}
