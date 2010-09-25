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

#include "frontend/bookshelfmanager/installpage/btinstallpagemodel.h"


#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"


#define MODULEPOINTERFORINDEX(i) static_cast<CSwordModuleInfo *>(\
    BtBookshelfTreeModel::data((i), BtBookshelfModel::ModulePointerRole).value<void*>())

BtInstallPageModel::BtInstallPageModel(const Grouping &grouping, QObject *parent)
    : BtBookshelfTreeModel(grouping, parent)
{
    setDefaultChecked(BtBookshelfTreeModel::UNCHECKED);
    setCheckable(true);
}

BtInstallPageModel::~BtInstallPageModel() {
    // Intentionally empty
}

QVariant BtInstallPageModel::data(const QModelIndex &i, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            switch (i.column()) {
                case 0:
                    return BtBookshelfTreeModel::data(i, role);
                case 1:
                {
                    CSwordModuleInfo *module = MODULEPOINTERFORINDEX(index(i.row(), 0, i.parent()));
                    if (module == 0) break;
                    CSwordBackend *b = CSwordBackend::instance();
                    CSwordModuleInfo *imodule = b->findModuleByName(module->name());
                    if (imodule == 0) {
                        return module->config(CSwordModuleInfo::ModuleVersion);
                    } else {
                        return imodule->config(CSwordModuleInfo::ModuleVersion)
                               + " => "
                               + module->config(CSwordModuleInfo::ModuleVersion);
                    }
                }
                case 2:
                {
                    CSwordModuleInfo *module = MODULEPOINTERFORINDEX(index(i.row(), 0, i.parent()));
                    if (module != 0) return module->config(CSwordModuleInfo::Description);
                }
                default: break;
            }
        default:
            if (i.column() == 0) return BtBookshelfTreeModel::data(i, role);
    }

    return QVariant();
}

int BtInstallPageModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return 3;
}

QVariant BtInstallPageModel::headerData(int section, Qt::Orientation orientation,
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
