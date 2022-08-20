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

#include "btbookshelfmodel.h"

#include <QIcon>
#include <QSet>
#include <QtAlgorithms>
#include <QtGlobal>
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/macros.h"
#include "../drivers/cswordmoduleinfo.h"
#include "../drivers/btconstmoduleset.h"


BtBookshelfModel::BtBookshelfModel(ConstructInPrivate const &) {}

std::shared_ptr<BtBookshelfModel> BtBookshelfModel::newInstance()
{ return std::make_shared<BtBookshelfModel>(ConstructInPrivate()); }

BtBookshelfModel::~BtBookshelfModel() noexcept = default;

int BtBookshelfModel::rowCount(const QModelIndex & parent) const {
    if (parent.isValid())
        return 0;

    return m_data.size();
}

QVariant BtBookshelfModel::data(CSwordModuleInfo * module, int role) const {
    BT_ASSERT(module);
    switch (role) {
        case Qt::DisplayRole:
        case ModuleNameRole:
            return module->name();
        case Qt::DecorationRole:
        case ModuleIconRole:
            return CSwordModuleInfo::moduleIcon(*module);
        case ModulePointerRole:
            return QVariant::fromValue(static_cast<void *>(module));
        case ModuleCategoryRole:
            return QVariant::fromValue(module->category());
        case ModuleLanguageRole:
            return QVariant(); /// \todo Unimplemented
        case ModuleHiddenRole:
            return module->isHidden();
        case ModuleInstallPathRole:
            return module->config(CSwordModuleInfo::AbsoluteDataPath);
        case ModuleHasIndexRole:
            return module->hasIndex();
        case ModuleIndexSizeRole:
            return module->indexSize();
        case ModuleDescriptionRole:
            return module->config(CSwordModuleInfo::Description);
        case Qt::ToolTipRole:
            return QStringLiteral("<b>%1:</b><br/>%2")
                       .arg(module->name())
                       .arg(module->config(CSwordModuleInfo::Description));
        default:
            return QVariant();
    }
}

QVariant BtBookshelfModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.column() != 0 || index.parent().isValid())
        return QVariant();

    int row = index.row();
    if (row >= m_data.size())
        return QVariant();

    return data(m_data.at(row), role);
}

QVariant BtBookshelfModel::headerData(int section,
                                      Qt::Orientation orientation,
                                      int role) const
{
    if (role == Qt::DisplayRole
       && orientation == Qt::Horizontal
       && section == 0)
    {
        return tr("Module");
    }

    return QVariant();
}

bool BtBookshelfModel::setData(const QModelIndex & index,
                               const QVariant & value,
                               int role)
{
    int row = index.row();
    if (role == ModuleHiddenRole
        && row >= 0
        && row < m_data.size()
        && index.column() == 0)
    {
        /*
          Emitting dataChanged here is actually mandatory, but were not doing it
          directly. Since we're connected to the module, changing its hidden
          state will emit a signal we catch in moduleHidden(), which in turn is
          what will actually emit dataChanged().
        */
        return m_data.at(row)->setHidden(value.toBool());
    }
    return false;
}

void BtBookshelfModel::clear(bool destroy) {
    if (m_data.size() <= 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
    if (destroy)
        qDeleteAll(m_data);
    m_data.clear();
    endRemoveRows();
}

void BtBookshelfModel::addModule(CSwordModuleInfo * const module) {
    BT_ASSERT(module);

    if (m_data.contains(module))
        return;

    const int index(m_data.size());
    beginInsertRows(QModelIndex(), index, index);
    m_data.append(module);
    BT_CONNECT(module, &CSwordModuleInfo::hiddenChanged,
               this,   &BtBookshelfModel::moduleHidden);
    BT_CONNECT(module, &CSwordModuleInfo::hasIndexChanged,
               this,   &BtBookshelfModel::moduleIndexed);
    BT_CONNECT(module, &CSwordModuleInfo::unlockedChanged,
               this,   &BtBookshelfModel::moduleUnlocked);
    endInsertRows();
}

void BtBookshelfModel::removeModule(CSwordModuleInfo * const module,
                                    bool destroy) {
    const int index = m_data.indexOf(module);
    if (index == -1)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    disconnect(module, &CSwordModuleInfo::hiddenChanged,
               this,   &BtBookshelfModel::moduleHidden);
    disconnect(module, &CSwordModuleInfo::hasIndexChanged,
               this,   &BtBookshelfModel::moduleIndexed);
    disconnect(module, &CSwordModuleInfo::unlockedChanged,
               this,   &BtBookshelfModel::moduleUnlocked);
    m_data.removeAt(index);
    endRemoveRows();
    if (destroy)
        delete module;
}

void BtBookshelfModel::removeModules(const QList<CSwordModuleInfo *> & modules,
                                     bool destroy)
{
    QSet<CSwordModuleInfo *> moduleSet;
    for (auto module: modules)
        moduleSet.insert(module);
    removeModules(moduleSet, destroy);
}

void BtBookshelfModel::removeModules(BtConstModuleSet const & modules, bool destroy){
    // This is inefficient, since signals are emitted for each removed module:
    for (auto const * const module : modules)
        removeModule(const_cast<CSwordModuleInfo *>(module), destroy);
}

CSwordModuleInfo * BtBookshelfModel::getModule(const QString & name) const {
    for (auto * const module : m_data)
        if (UNLIKELY(module->name() == name))
            return module;
    return nullptr;
}

void BtBookshelfModel::moduleHidden(bool) {
    BT_ASSERT(qobject_cast<CSwordModuleInfo *>(sender()));

    moduleDataChanged(static_cast<CSwordModuleInfo *>(sender()));
}

void BtBookshelfModel::moduleIndexed(bool) {
    BT_ASSERT(qobject_cast<CSwordModuleInfo *>(sender()));

    moduleDataChanged(static_cast<CSwordModuleInfo *>(sender()));
}

void BtBookshelfModel::moduleUnlocked(bool) {
    BT_ASSERT(qobject_cast<CSwordModuleInfo *>(sender()));

    moduleDataChanged(static_cast<CSwordModuleInfo *>(sender()));
}

void BtBookshelfModel::moduleDataChanged(CSwordModuleInfo * module) {
    BT_ASSERT(m_data.count(module) == 1);

    QModelIndex i(index(m_data.indexOf(module), 0));
    Q_EMIT dataChanged(i, i);
}
