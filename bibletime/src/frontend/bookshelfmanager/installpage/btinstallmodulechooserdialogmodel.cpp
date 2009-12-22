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

#include "frontend/bookshelfmanager/installpage/btinstallmodulechooserdialogmodel.h"

#include <QBrush>
#include <QMutex>
#include "backend/drivers/cswordmoduleinfo.h"


namespace {

QMutex dataChangedMutex;
bool dataChangedFired = false;

}

#define MODULEPOINTERFORINDEX(i) static_cast<CSwordModuleInfo *>(\
    BtBookshelfTreeModel::data((i), BtBookshelfModel::ModulePointerRole).value<void*>())

BtInstallModuleChooserDialogModel::BtInstallModuleChooserDialogModel(QObject *parent)
    : BtBookshelfTreeModel(parent)
{
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(parentDataChanged(QModelIndex,QModelIndex)));
}

BtInstallModuleChooserDialogModel::BtInstallModuleChooserDialogModel(
        const Grouping &grouping,
        QObject *parent)
    : BtBookshelfTreeModel(grouping, parent)
{
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(parentDataChanged(QModelIndex,QModelIndex)));
}

BtInstallModuleChooserDialogModel::~BtInstallModuleChooserDialogModel() {
    // Intentionally empty
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
            if (i.column() == 1) {
                CSwordModuleInfo *module = MODULEPOINTERFORINDEX(index(i.row(), 0, i.parent()));
                if (module != 0) return module->property("installSourceName");
            }
        default:
            if (i.column() == 0) return BtBookshelfTreeModel::data(i, role);
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

void BtInstallModuleChooserDialogModel::parentDataChanged(const QModelIndex &topLeft,
                                                          const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft);
    Q_UNUSED(bottomRight);

    dataChangedMutex.lock();
    if (dataChangedFired) {
        dataChangedMutex.unlock();
        return;
    }
    dataChangedFired = true;
    dataChangedMutex.unlock();

    resetData();

    dataChangedMutex.lock();
    dataChangedFired = false;
    dataChangedMutex.unlock();
}

bool BtInstallModuleChooserDialogModel::isMulti(CSwordModuleInfo *m1) const {
    if (m1 != 0 && checkedModules().contains(m1)) {
        Q_FOREACH(CSwordModuleInfo *m2, m_modules.keys()) {
            if (m1 != m2 && checkedModules().contains(m2) && m1->name() == m2->name()) {
                return true;
            }
        }
    }
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
