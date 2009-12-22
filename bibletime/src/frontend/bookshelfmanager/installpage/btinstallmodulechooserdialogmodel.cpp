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

#include "btinstallmodulechooserdialogmodel.h"


#define MODULEPOINTERFORINDEX(i) static_cast<CSwordModuleInfo *>(\
    BtBookshelfTreeModel::data((i), BtBookshelfModel::ModulePointerRole).value<void*>())

BtInstallModuleChooserDialogModel::BtInstallModuleChooserDialogModel(QObject *parent)
    : BtBookshelfTreeModel(parent)
{
    // Intentionally empty
}

BtInstallModuleChooserDialogModel::BtInstallModuleChooserDialogModel(
        const Grouping &grouping,
        QObject *parent)
    : BtBookshelfTreeModel(grouping, parent)
{
    // Intentionally empty
}

QVariant BtInstallModuleChooserDialogModel::data(const QModelIndex &i, int role) const {
    if (i.column() == 0) return BtBookshelfTreeModel::data(i, role);
    
    if (i.column() == 1) {
        if (role == Qt::DisplayRole) {
            CSwordModuleInfo *module = MODULEPOINTERFORINDEX(index(i.row(), 0, i.parent()));
            if (module != 0) return module->property("installSourceName");
        }
    }
    return QVariant();
}

int BtInstallModuleChooserDialogModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return 2;
}

QVariant BtInstallModuleChooserDialogModel::headerData(int section,
                                                       Qt::Orientation orientation,
                                                       int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return tr("Work");
        }
        else if (section == 1) {
            return tr("Installation source");
        }
    }

    return QVariant();
}
