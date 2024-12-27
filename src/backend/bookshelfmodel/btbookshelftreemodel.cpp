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

#include "btbookshelftreemodel.h"

#include <QDataStream>
#include <QModelIndexList>
#include <QPair>
#include <QtGlobal>
#include <type_traits>
#include <utility>
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../../util/macros.h"
#include "../config/btconfigcore.h"
#include "../drivers/btconstmoduleset.h"
#include "../drivers/btmoduleset.h"
#include "categoryitem.h"
#include "indexingitem.h"
#include "languageitem.h"
#include "moduleitem.h"


using namespace BookshelfModel;

bool BtBookshelfTreeModel::Grouping::loadFrom(BtConfigCore const & config,
                                              QString const & key)
{
    BT_ASSERT(!key.isNull());
    QVariant const v = config.qVariantValue(key, QVariant());
    if (!v.canConvert<Grouping>())
        return false;

    (*this) = v.value<Grouping>();
    return true;
}

void BtBookshelfTreeModel::Grouping::saveTo(BtConfigCore & config,
                                            QString const & key) const
{
    BT_ASSERT(!key.isNull());
    config.setValue(key, QVariant::fromValue(*this));
}

BtBookshelfTreeModel::BtBookshelfTreeModel(QObject * const parent)
    : QAbstractItemModel(parent)
    , m_rootItem(std::make_unique<RootItem>())
    , m_defaultChecked(MODULE_HIDDEN)
    , m_checkable(false) {}

BtBookshelfTreeModel::BtBookshelfTreeModel(BtConfigCore const & config,
                                           QString const & configKey,
                                           QObject * const parent)
       : QAbstractItemModel(parent)
       , m_rootItem(std::make_unique<RootItem>())
       , m_groupingOrder(config, configKey)
       , m_defaultChecked(MODULE_HIDDEN)
       , m_checkable(false) {}

BtBookshelfTreeModel::BtBookshelfTreeModel(Grouping const & grouping,
                                           QObject * const parent)
        : QAbstractItemModel(parent)
        , m_rootItem(std::make_unique<RootItem>())
        , m_groupingOrder(grouping)
        , m_defaultChecked(MODULE_HIDDEN)
        , m_checkable(false) {}

BtBookshelfTreeModel::~BtBookshelfTreeModel() = default;

int BtBookshelfTreeModel::rowCount(QModelIndex const & parent) const {
    return getItem(parent).children().size();
}

int BtBookshelfTreeModel::columnCount(QModelIndex const & parent) const {
    Q_UNUSED(parent)

    return 1;
}

bool BtBookshelfTreeModel::hasChildren(QModelIndex const & parent) const {
    return !getItem(parent).children().isEmpty();
}

QModelIndex BtBookshelfTreeModel::index(int const row, int const column,
                                        QModelIndex const & parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    Item & parentItem = getItem(parent);
    Item * const childItem = parentItem.children().at(row);
    if (!childItem)
        return QModelIndex();

    return createIndex(row, column, childItem);
}

QModelIndex BtBookshelfTreeModel::parent(QModelIndex const & index) const {
    if (!index.isValid())
        return QModelIndex();

    Item const * childItem(static_cast<Item*>(index.internalPointer()));
    BT_ASSERT(childItem);
    Item * parentItem(childItem->parent());
    BT_ASSERT(parentItem);

    if (parentItem == m_rootItem.get())
        return QModelIndex();

    return createIndex(parentItem->childIndex(), 0, parentItem);
}

QVariant
BtBookshelfTreeModel::data(QModelIndex const & index, int const role) const {
    if (!index.isValid() || index.column() != 0)
        return QVariant();

    Item const * const i = static_cast<Item*>(index.internalPointer());
    BT_ASSERT(i);
    switch (role) {

        case Qt::CheckStateRole:
            if (!m_checkable)
                break;
            [[fallthrough]];

        case BtBookshelfTreeModel::CheckStateRole:
            return i->checkState();

        case BtBookshelfModel::ModulePointerRole:
            /* This case is just an optimization. */
            if (i->type() == Item::ITEM_MODULE) {
                ModuleItem const & mi = *static_cast<ModuleItem const *>(i);
                return QVariant::fromValue(static_cast<void *>(&mi.moduleInfo()));
            }
            return QVariant::fromValue(nullptr);

        case Qt::DisplayRole:
        case Qt::DecorationRole:
        case BtBookshelfModel::ModuleHiddenRole:
        default:
            if (i->type() == Item::ITEM_MODULE)
                return data(static_cast<ModuleItem const *>(i)->moduleInfo(), role);

            return i->data(role);

    }
    return QVariant();
}

QVariant
BtBookshelfTreeModel::data(CSwordModuleInfo & module, int const role) const {
    BT_ASSERT(m_sourceIndexMap.contains(&module));
    return m_sourceModel->data(m_sourceIndexMap.value(&module), role);
}

bool BtBookshelfTreeModel::setData(QModelIndex const & itemIndex,
                                   QVariant const & value,
                                   int const role)
{
    BT_ASSERT(itemIndex.isValid());
    using IP = QPair<Item *, QModelIndex>;

    if (UNLIKELY(role != Qt::CheckStateRole))
        return false;

    bool ok;
    Qt::CheckState newState = static_cast<Qt::CheckState>(value.toInt(&ok));
    if (UNLIKELY(!ok))
        return false;

    // Handle partially checked as checked here in setData():
    if (newState == Qt::PartiallyChecked)
        newState = Qt::Checked;

    Item * item = static_cast<Item *>(itemIndex.internalPointer());
    BT_ASSERT(item);
    if (item->checkState() == newState) return false;

    // Recursively (un)check all children:
    QList<IP> q;
    IP p(item, itemIndex);
    for (;;) {
        if (item->checkState() != newState) {
            item->setCheckState(newState);
            Q_EMIT dataChanged(p.second, p.second);
            if (item->type() == Item::ITEM_MODULE) {
                ModuleItem const & mItem = *static_cast<ModuleItem *>(item);
                CSwordModuleInfo & mInfo = mItem.moduleInfo();
                if (newState == Qt::Checked) {
                    m_checkedModulesCache.insert(&mInfo);
                    Q_EMIT moduleChecked(&mInfo, true);
                } else {
                    m_checkedModulesCache.remove(&mInfo);
                    Q_EMIT moduleChecked(&mInfo, false);
                }
            } else {
                QList<Item *> const & children = item->children();
                for (int i = 0; i < children.size(); i++)
                    q.append(IP(children.at(i), index(i, 0, p.second)));
            }
        }
        if (q.empty())
            break;
        p = q.takeFirst();
        item = p.first;
    }

    // Recursively change parent check states.
    resetParentCheckStates(itemIndex.parent(), false);

    return true;
}

Qt::ItemFlags BtBookshelfTreeModel::flags(QModelIndex const & index) const {
    if (!index.isValid())
        return Qt::ItemFlags();

    Qt::ItemFlags f(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    if (m_checkable) {
        f |= Qt::ItemIsUserCheckable;

        Item const & i = *static_cast<Item*>(index.internalPointer());
        if (i.type() != Item::ITEM_MODULE)
            f |= Qt::ItemIsAutoTristate;
    }

    return f;
}

QVariant BtBookshelfTreeModel::headerData(int const section,
                                          Qt::Orientation const orientation,
                                          int const role) const
{
    if (orientation == Qt::Horizontal)
        return m_sourceModel->headerData(section, orientation, role);

    return QVariant();
}

void BtBookshelfTreeModel::setSourceModel(
        std::shared_ptr<QAbstractItemModel> sourceModel)
{
    if (m_sourceModel == sourceModel)
        return;

    beginResetModel();
    if (m_sourceModel) {
        auto & model = *m_sourceModel;
        disconnect(&model, &QAbstractItemModel::rowsAboutToBeRemoved,
                   this,   &BtBookshelfTreeModel::moduleRemoved);
        disconnect(&model, &QAbstractItemModel::rowsInserted,
                   this,   &BtBookshelfTreeModel::moduleInserted);
        disconnect(&model, &QAbstractItemModel::dataChanged,
                   this,   &BtBookshelfTreeModel::moduleDataChanged);
        m_rootItem = std::make_unique<RootItem>();
        m_modules.clear();
        m_sourceIndexMap.clear();
        m_checkedModulesCache.clear();
    }

    m_sourceModel = std::move(sourceModel);

    if (m_sourceModel) {
        auto & model = *m_sourceModel;
        BT_CONNECT(&model, &QAbstractItemModel::rowsAboutToBeRemoved,
                   this,   &BtBookshelfTreeModel::moduleRemoved);
        BT_CONNECT(&model, &QAbstractItemModel::rowsInserted,
                   this,   &BtBookshelfTreeModel::moduleInserted);
        BT_CONNECT(&model, &QAbstractItemModel::dataChanged,
                   this,   &BtBookshelfTreeModel::moduleDataChanged);

        for (int i = 0; i < model.rowCount(); i++) {
            QModelIndex const moduleIndex(model.index(i, 0));
            CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(
                model.data(moduleIndex,
                           BtBookshelfModel::ModulePointerRole).value<void*>());

            bool checked;
            if (m_defaultChecked == MODULE_HIDDEN) {
                checked = !model.data(moduleIndex,
                                      BtBookshelfModel::ModuleHiddenRole).toBool();
            } else if (m_defaultChecked == MODULE_INDEXED) {
                checked = !model.data(moduleIndex,
                                      BtBookshelfModel::ModuleHasIndexRole).toBool();
            } else {
                checked = (m_defaultChecked == CHECKED);
            }
            m_sourceIndexMap[&module] = moduleIndex;
            addModule(module, checked, true);
        }
    }
    endResetModel();
}

void BtBookshelfTreeModel::setGroupingOrder(Grouping const & groupingOrder,
                                            bool const emitSignal)
{
    if (m_groupingOrder == groupingOrder)
        return;

    m_groupingOrder = groupingOrder;

    if (m_sourceModel != nullptr) {
        BtModuleSet const checked(m_checkedModulesCache);
        m_checkedModulesCache.clear();

        beginResetModel();
        m_rootItem = std::make_unique<RootItem>();
        m_modules.clear();

        for (int i = 0; i < m_sourceModel->rowCount(); i++) {
            QModelIndex const sourceIndex(m_sourceModel->index(i, 0));
            CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(
                m_sourceModel->data(sourceIndex,
                                    BtBookshelfModel::ModulePointerRole).value<void *>());
            m_sourceIndexMap[&module] = sourceIndex;
            addModule(module, checked.contains(&module), true);
        }
        endResetModel();
    }

    if (emitSignal)
        Q_EMIT groupingOrderChanged(groupingOrder);
}

void BtBookshelfTreeModel::setCheckable(bool const checkable) {
    if (m_checkable == checkable)
        return;
    m_checkable = checkable;
    if (m_sourceModel == nullptr)
        return;

    // Notify views that flags changed for all items:
    resetData();
}

void BtBookshelfTreeModel::setCheckedModules(BtConstModuleSet const & modules) {
    for (auto it = m_modules.constBegin(); it != m_modules.constEnd(); ++it) {
        if (modules.contains(it.key())) {
            setData(getIndex(*it.value()), Qt::Checked, Qt::CheckStateRole);
        } else {
            setData(getIndex(*it.value()), Qt::Unchecked, Qt::CheckStateRole);
        }
    }
}

void BtBookshelfTreeModel::resetData() {
    QModelIndexList queue;
    queue.append(QModelIndex());
    do {
        QModelIndex const parent(queue.takeFirst());
        Q_EMIT dataChanged(index(0, 0, parent),
                         index(rowCount(parent) - 1, columnCount() - 1, parent));
        for (int i = 0; i < rowCount(parent); i++) {
            QModelIndex const childIndex(index(i, 0, parent));
            if (rowCount(childIndex) > 0)
                queue.append(childIndex);
        }
    } while (!queue.isEmpty());
}

void BtBookshelfTreeModel::addModule(CSwordModuleInfo & module,
                                     bool const checked,
                                     bool const inReset)
{
    if (m_modules.contains(&module))
        return;

    bool beginInsert = !inReset;
    QModelIndex parentIndex;
    {
        Grouping intermediateGrouping(m_groupingOrder);
        while (!intermediateGrouping.empty()) {
            switch (intermediateGrouping.takeFirst()) {
                case GROUP_CATEGORY:
                    parentIndex = getGroup<CategoryItem>(module,
                                                         parentIndex,
                                                         beginInsert);
                    break;
                case GROUP_LANGUAGE:
                    parentIndex = getGroup<LanguageItem>(module,
                                                         parentIndex,
                                                         beginInsert);
                    break;
                case GROUP_INDEXING:
                    parentIndex = getGroup<IndexingItem>(module,
                                                         parentIndex,
                                                         beginInsert);
                    break;
            }
        }
    }

    Item & parentItem = getItem(parentIndex);
    ModuleItem * const newItem = new ModuleItem(module, *this);
    newItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    int const newIndex(parentItem.indexFor(*newItem));

    // Actually do the insertion:
    if (beginInsert)
        beginInsertRows(parentIndex, newIndex, newIndex);
    parentItem.insertChild(newIndex, newItem);
    m_modules.insert(&module, newItem);
    if (checked) // Add to checked modules cache
        m_checkedModulesCache.insert(&module);

    if (!inReset)
        endInsertRows();

    // Reset parent item check states, if needed:
    resetParentCheckStates(parentIndex, inReset);
}

void BtBookshelfTreeModel::removeModule(CSwordModuleInfo & module) {
    auto const it = m_modules.find(&module);
    if (it == m_modules.end())
        return;

    Item const * i = it.value();

    // Set i to be the lowest item (including empty groups) to remove:
    BT_ASSERT(i->parent());
    while (i->parent() != m_rootItem.get()
           && i->parent()->children().size() <= 1)
        i = i->parent();
    BT_ASSERT(i);
    BT_ASSERT(i->parent());

    // Calculate item indexes:
    int const index = i->childIndex();
    QModelIndex const parentIndex(getIndex(*i->parent()));

    // Actually remove the item:
    beginRemoveRows(parentIndex, index, index);
    delete i->parent()->children().takeAt(index);
    m_modules.erase(it);
    m_checkedModulesCache.remove(&module);
    endRemoveRows();

    // Reset parent item check states, if needed:
    resetParentCheckStates(parentIndex, false);
}

Item & BtBookshelfTreeModel::getItem(QModelIndex const & index) const {
    if (UNLIKELY(!index.isValid()))
        return *m_rootItem;

    Item * const item = static_cast<Item *>(index.internalPointer());
    BT_ASSERT(item);
    return *item;
}

QModelIndex BtBookshelfTreeModel::getIndex(BookshelfModel::Item const & item) {
    BookshelfModel::Item const * it = &item;
    QList<int> indexes;
    for (;;) {
        int const i = it->childIndex();
        if (i < 0)
            break;
        indexes.append(i);
        it = it->parent();
    }

    QModelIndex i;
    while (!indexes.isEmpty())
        i = index(indexes.takeLast(), 0, i);
    return i;
}

void BtBookshelfTreeModel::resetParentCheckStates(QModelIndex parentIndex,
                                                  bool const inReset)
{
    for ( ; parentIndex.isValid(); parentIndex = parentIndex.parent()) {
        Item & parentItem = *static_cast<Item *>(parentIndex.internalPointer());

        Qt::CheckState const oldState = parentItem.checkState();
        bool haveCheckedChildren = false;
        bool haveUncheckedChildren = false;
        for (int i = 0; i < parentItem.children().size(); i++) {
            Qt::CheckState const state = parentItem.children().at(i)->checkState();
            if (state == Qt::PartiallyChecked) {
                haveCheckedChildren = true;
                haveUncheckedChildren = true;
                break;
            } else if (state == Qt::Checked) {
                haveCheckedChildren = true;
                if (haveUncheckedChildren)
                    break;
            } else {
                BT_ASSERT(state == Qt::Unchecked);
                haveUncheckedChildren = true;
                if (haveCheckedChildren)
                    break;
            }
        }

        Qt::CheckState newState;
        if (haveCheckedChildren) {
            if (haveUncheckedChildren) {
                newState = Qt::PartiallyChecked;
            } else {
                newState = Qt::Checked;
            }
        } else {
            newState = Qt::Unchecked;
        }
        if (newState == oldState)
            break;

        parentItem.setCheckState(newState);

        if (!inReset)
            Q_EMIT dataChanged(parentIndex, parentIndex);
    } // for ( ; parentIndex.isValid(); parentIndex = parentIndex.parent())
}

void BtBookshelfTreeModel::moduleDataChanged(QModelIndex const & topLeft,
                                             QModelIndex const & bottomRight)
{
    BT_ASSERT(!topLeft.parent().isValid());
    BT_ASSERT(!bottomRight.parent().isValid());
    BT_ASSERT(topLeft.column() == 0);
    BT_ASSERT(bottomRight.column() == 0);

    for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
        QModelIndex const moduleIndex(m_sourceModel->index(i, 0, topLeft.parent()));
        QVariant const data(m_sourceModel->data(moduleIndex,
                                                BtBookshelfModel::ModulePointerRole));
        CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(data.value<void *>());
        QModelIndex itemIndex(getIndex(*m_modules[&module]));
        BT_ASSERT(itemIndex.isValid());

        Q_EMIT dataChanged(itemIndex, itemIndex);

        /*
          Also emit signals for parent items because the change might alter them
          as well, e.g. isHidden()
        */
        do {
            itemIndex = itemIndex.parent();
            Q_EMIT dataChanged(itemIndex, itemIndex);
        } while (itemIndex.isValid());
    }
}

void BtBookshelfTreeModel::moduleInserted(QModelIndex const & parent,
                                          int const start,
                                          int const end)
{
    BT_ASSERT(start <= end);

    for (int i = start; i <= end; i++) {
        QModelIndex const moduleIndex(m_sourceModel->index(i, 0, parent));
        QVariant const data(m_sourceModel->data(moduleIndex,
                                                BtBookshelfModel::ModulePointerRole));
        CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(data.value<void *>());

        bool checked;
        if (m_defaultChecked == MODULE_HIDDEN) {
            checked = !m_sourceModel->data(moduleIndex,
                                           BtBookshelfModel::ModuleHiddenRole).toBool();
        } else if (m_defaultChecked == MODULE_INDEXED) {
            checked = !m_sourceModel->data(moduleIndex,
                                           BtBookshelfModel::ModuleHasIndexRole).toBool();
        } else {
            BT_ASSERT(m_defaultChecked == CHECKED || m_defaultChecked == UNCHECKED);
            checked = (m_defaultChecked == CHECKED);
        }
        m_sourceIndexMap[&module] = moduleIndex;
        addModule(module, checked, false);
    }
}

void BtBookshelfTreeModel::moduleRemoved(QModelIndex const & parent,
                                         int const start,
                                         int const end)
{
    BT_ASSERT(start <= end);

    for (int i = start; i <= end; i++) {
        QModelIndex const moduleIndex(m_sourceModel->index(i, 0, parent));
        QVariant const data(m_sourceModel->data(moduleIndex,
                                                BtBookshelfModel::ModulePointerRole));
        CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(data.value<void*>());
        removeModule(module);
        m_sourceIndexMap.remove(&module);
    }
}

QDataStream & operator <<(QDataStream & os,
                          BtBookshelfTreeModel::Grouping const & o)
{
    os << o.size();
    for (BtBookshelfTreeModel::Group const g : o)
        os << static_cast<std::underlying_type<decltype(g)>::type>(g);
    return os;
}

QDataStream & operator >>(QDataStream & is,
                          BtBookshelfTreeModel::Grouping & o)
{
    int s;
    is >> s;
    o.clear();
    for (int i = 0; i < s; i++) {
        std::underlying_type_t<BtBookshelfTreeModel::Group> g;
        is >> g;
        o.append(static_cast<BtBookshelfTreeModel::Group>(g));
    }
    return is;
}
