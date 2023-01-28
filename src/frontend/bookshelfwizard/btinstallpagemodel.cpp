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

#include "btinstallpagemodel.h"

#include <QtGlobal>
#include "../../backend/bookshelfmodel/btbookshelftreemodel.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/managers/cswordbackend.h"


BtInstallPageModel::BtInstallPageModel(Grouping const & grouping,
                                       QObject * const parent)
    : BtBookshelfTreeModel(grouping, parent)
{
    setDefaultChecked(BtBookshelfTreeModel::UNCHECKED);
    setCheckable(true);
}

QVariant BtInstallPageModel::data(QModelIndex const & i, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            switch (i.column()) {
                case 0:
                    return BtBookshelfTreeModel::data(i, role);
                case 1:
                    if (CSwordModuleInfo * const m =
                            module(index(i.row(), 0, i.parent())))
                    {
                        if (CSwordModuleInfo * imodule =
                                CSwordBackend::instance().findModuleByName(
                                        m->name()))
                            return QStringLiteral("%1 => %2")
                                    .arg(imodule->config(
                                             CSwordModuleInfo::ModuleVersion),
                                         m->config(
                                             CSwordModuleInfo::ModuleVersion));
                        return m->config(CSwordModuleInfo::ModuleVersion);
                    }
                    break;
                case 2:
                    if (CSwordModuleInfo * const m =
                            module(index(i.row(), 0, i.parent())))
                        return m->config(CSwordModuleInfo::Description);
                    break;
                default:
                    break;
            }
            break;
        default:
            if (i.column() == 0)
                return BtBookshelfTreeModel::data(i, role);
            break;
    }

    return QVariant();
}

int BtInstallPageModel::columnCount(QModelIndex const & parent) const {
    Q_UNUSED(parent)
    return 3;
}

QVariant BtInstallPageModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Work");
            case 1: return tr("Version");
            case 2: return tr("Description");
            default: break;
        }
    }
    return QVariant();
}
