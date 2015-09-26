/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.h"

#include <QBrush>
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/managers/cswordbackend.h"


#define MODULEPOINTERFORINDEX(i) static_cast<CSwordModuleInfo *>(\
    BtBookshelfTreeModel::data((i), BtBookshelfModel::ModulePointerRole).value<void*>())

BtInstallModuleChooserDialogModel::BtInstallModuleChooserDialogModel(
        const Grouping &grouping,
        QObject *parent)
    : BtBookshelfTreeModel(grouping, parent), m_dataChangedFired(false)
{
    setDefaultChecked(BtBookshelfTreeModel::CHECKED);
    setCheckable(true);
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(parentDataChanged(QModelIndex,QModelIndex)),
            Qt::DirectConnection);
}

QVariant BtInstallModuleChooserDialogModel::data(const QModelIndex &i, int role) const {
    switch (role) {
        case Qt::BackgroundRole:
            if (isMulti(i)) return QBrush(Qt::red);
            return BtBookshelfTreeModel::data(i, role);
        case Qt::ForegroundRole:
            if (isMulti(i)) return QBrush(Qt::white);
            return BtBookshelfTreeModel::data(i, role);
        case Qt::DisplayRole:
            switch (i.column()) {
                case 0:
                    return BtBookshelfTreeModel::data(i, role);
                case 1:
                {
                    CSwordModuleInfo *module = MODULEPOINTERFORINDEX(index(i.row(), 0, i.parent()));
                    if (module != 0) return module->property("installSourceName");
                    break;
                }
                case 2:
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
                default: break;
            }
        default:
            if (i.column() == 0) return BtBookshelfTreeModel::data(i, role);
    }

    return QVariant();
}

int BtInstallModuleChooserDialogModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return 3;
}

QVariant BtInstallModuleChooserDialogModel::headerData(int section,
                                                       Qt::Orientation orientation,
                                                       int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return tr("Work");
            case 1: return tr("Installation source");
            case 2: return tr("Version");
            default: break;
        }
    }

    return QVariant();
}

void BtInstallModuleChooserDialogModel::parentDataChanged(const QModelIndex &topLeft,
                                                          const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);

    if (m_dataChangedFired) return;
    m_dataChangedFired = true;
    resetData();
    m_dataChangedFired = false;
}

bool BtInstallModuleChooserDialogModel::isMulti(CSwordModuleInfo * m1) const {
    if (m1 != 0 && checkedModules().contains(m1))
        Q_FOREACH(CSwordModuleInfo const * const m2, modules())
            if (m1 != m2 && checkedModules().contains(m2) && m1->name() == m2->name())
                return true;
    return false;
}

bool BtInstallModuleChooserDialogModel::isMulti(const QModelIndex &i) const {
    if (!i.isValid()) return false;

    if (!hasChildren(i)) {
        return isMulti(MODULEPOINTERFORINDEX(index(i.row(), 0, i.parent())));
    } else {
        for (int row = 0; row < rowCount(i); row++) {
            if (isMulti(i.child(row, 0))) return true;
        }
    }
    return false;
}
