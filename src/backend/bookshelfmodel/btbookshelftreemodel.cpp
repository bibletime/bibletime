/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "backend/bookshelfmodel/btbookshelftreemodel.h"

#include <QSet>
#include "backend/bookshelfmodel/categoryitem.h"
#include "backend/bookshelfmodel/indexingitem.h"
#include "backend/bookshelfmodel/languageitem.h"
#include "backend/bookshelfmodel/moduleitem.h"
#include "backend/config/btconfig.h"
#include "util/macros.h"


using namespace BookshelfModel;

bool BtBookshelfTreeModel::Grouping::loadFrom(const QString & configKey) {
    Q_ASSERT(!configKey.isNull());
    QVariant v = btConfig().value<QVariant>(configKey, QVariant());
    if (!v.canConvert<Grouping>())
        return false;

    (*this) = v.value<Grouping>();
    return true;
}

void BtBookshelfTreeModel::Grouping::saveTo(const QString & configKey) const {
    Q_ASSERT(!configKey.isNull());
    btConfig().setValue(configKey, QVariant::fromValue(*this));
}

BtBookshelfTreeModel::BtBookshelfTreeModel(QObject * parent)
    : QAbstractItemModel(parent)
    , m_sourceModel(0)
    , m_rootItem(new RootItem)
    , m_defaultChecked(MODULE_HIDDEN)
    , m_checkable(false) {}

BtBookshelfTreeModel::BtBookshelfTreeModel(const QString & configKey,
                                           QObject * parent)
       : QAbstractItemModel(parent)
       , m_sourceModel(0)
       , m_rootItem(new RootItem)
       , m_groupingOrder(configKey)
       , m_defaultChecked(MODULE_HIDDEN)
       , m_checkable(false) {}

BtBookshelfTreeModel::BtBookshelfTreeModel(const Grouping & grouping,
                                           QObject * parent)
        : QAbstractItemModel(parent)
        , m_sourceModel(0)
        , m_rootItem(new RootItem)
        , m_groupingOrder(grouping)
        , m_defaultChecked(MODULE_HIDDEN)
        , m_checkable(false) {}

BtBookshelfTreeModel::~BtBookshelfTreeModel() {
    delete m_rootItem;
}

int BtBookshelfTreeModel::rowCount(const QModelIndex & parent) const {
    return getItem(parent).children().size();
}

int BtBookshelfTreeModel::columnCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);

    return 1;
}

bool BtBookshelfTreeModel::hasChildren(const QModelIndex & parent) const {
    return !getItem(parent).children().isEmpty();
}

QModelIndex BtBookshelfTreeModel::index(int row, int column,
                                        const QModelIndex & parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    Item & parentItem = getItem(parent);
    Item * const childItem = parentItem.children().at(row);
    if (!childItem)
        return QModelIndex();

    return createIndex(row, column, childItem);
}

QModelIndex BtBookshelfTreeModel::parent(const QModelIndex & index) const {
    if (!index.isValid())
        return QModelIndex();

    Item * childItem(static_cast<Item*>(index.internalPointer()));
    Q_ASSERT(childItem != 0);
    Item * parentItem(childItem->parent());
    Q_ASSERT(parentItem != 0);

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->childIndex(), 0, parentItem);
}

QVariant BtBookshelfTreeModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid() || index.column() != 0)
        return QVariant();

    const Item * const i = static_cast<Item*>(index.internalPointer());
    Q_ASSERT(i != 0);
    switch (role) {

        case Qt::CheckStateRole:
            if (!m_checkable)
                break;

        case BtBookshelfTreeModel::CheckStateRole:
            return i->checkState();

        case BtBookshelfModel::ModulePointerRole:
            /* This case is just an optimization. */
            if (i->type() == Item::ITEM_MODULE) {
                const ModuleItem & mi = *static_cast<const ModuleItem *>(i);
                return qVariantFromValue(static_cast<void *>(&mi.moduleInfo()));
            }
            return 0;

        case Qt::DisplayRole:
        case Qt::DecorationRole:
        case BtBookshelfModel::ModuleHiddenRole:
        default:
            if (i->type() == Item::ITEM_MODULE)
                return data(static_cast<const ModuleItem *>(i)->moduleInfo(), role);

            return i->data(role);

    }
    return QVariant();
}

QVariant BtBookshelfTreeModel::data(CSwordModuleInfo & module, int role) const {
    Q_ASSERT(m_sourceIndexMap.contains(&module));
    return m_sourceModel->data(m_sourceIndexMap.value(&module), role);
}

bool BtBookshelfTreeModel::setData(const QModelIndex & itemIndex,
                                   const QVariant & value,
                                   int role) {
    Q_ASSERT(itemIndex.isValid());
    typedef QPair<Item *, QModelIndex> IP;

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
    Q_ASSERT(item != 0);
    if (item->checkState() == newState) return false;

    // Recursively (un)check all children:
    QList<IP> q;
    IP p(item, itemIndex);
    for (;;) {
        if (item->checkState() != newState) {
            item->setCheckState(newState);
            emit dataChanged(p.second, p.second);
            if (item->type() == Item::ITEM_MODULE) {
                ModuleItem & mItem = *static_cast<ModuleItem *>(item);
                CSwordModuleInfo & mInfo = mItem.moduleInfo();
                if (newState == Qt::Checked) {
                    m_checkedModulesCache.insert(&mInfo);
                    emit moduleChecked(&mInfo, true);
                } else {
                    m_checkedModulesCache.remove(&mInfo);
                    emit moduleChecked(&mInfo, false);
                }
            } else {
                const QList<Item *> & children = item->children();
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
    resetParentCheckStates(itemIndex.parent());

    return true;
}

Qt::ItemFlags BtBookshelfTreeModel::flags(const QModelIndex & index) const {
    if (!index.isValid())
        return 0;

    Qt::ItemFlags f(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    if (m_checkable) {
        f |= Qt::ItemIsUserCheckable;

        const Item & i = *static_cast<Item*>(index.internalPointer());
        if (i.type() != Item::ITEM_MODULE)
            f |= Qt::ItemIsTristate;
    }

    return f;
}

QVariant BtBookshelfTreeModel::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const
{
    if (orientation == Qt::Horizontal)
        return m_sourceModel->headerData(section, orientation, role);

    return QVariant();
}

void BtBookshelfTreeModel::setSourceModel(QAbstractItemModel * sourceModel) {
    if (m_sourceModel == sourceModel)
        return;

    if (m_sourceModel != 0) {
        disconnect(this, SLOT(moduleInserted(QModelIndex, int, int)));
        disconnect(this, SLOT(moduleRemoved(QModelIndex, int, int)));
        disconnect(this, SLOT(moduleDataChanged(QModelIndex, QModelIndex)));
        beginRemoveRows(QModelIndex(), 0, m_rootItem->children().size() - 1);
        delete m_rootItem;
        m_modules.clear();
        m_sourceIndexMap.clear();
        m_checkedModulesCache.clear();
        m_rootItem = new RootItem;
        endRemoveRows();
    }

    m_sourceModel = sourceModel;

    if (sourceModel != 0) {
        connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
                this,        SLOT(moduleRemoved(QModelIndex, int, int)));
        connect(sourceModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
                this,        SLOT(moduleInserted(QModelIndex, int, int)));
        connect(sourceModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
                this,        SLOT(moduleDataChanged(QModelIndex, QModelIndex)));

        for (int i = 0; i < sourceModel->rowCount(); i++) {
            const QModelIndex moduleIndex(sourceModel->index(i, 0));
            CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(
                sourceModel->data(moduleIndex,
                                  BtBookshelfModel::ModulePointerRole).value<void*>());

            bool checked;
            if (m_defaultChecked == MODULE_HIDDEN) {
                checked = !sourceModel->data(moduleIndex,
                                             BtBookshelfModel::ModuleHiddenRole).toBool();
            } else if (m_defaultChecked == MODULE_INDEXED) {
                checked = !sourceModel->data(moduleIndex,
                                             BtBookshelfModel::ModuleHasIndexRole).toBool();
            } else {
                checked = (m_defaultChecked == CHECKED);
            }
            m_sourceIndexMap[&module] = moduleIndex;
            addModule(module, checked);
        }
    }
}

void BtBookshelfTreeModel::setGroupingOrder(const Grouping & groupingOrder,
                                            bool emitSignal)
{
    if (m_groupingOrder == groupingOrder)
        return;

    m_groupingOrder = groupingOrder;

    if (m_sourceModel != 0) {
        const QSet<CSwordModuleInfo *> checked(m_checkedModulesCache);
        m_checkedModulesCache.clear();

        beginRemoveRows(QModelIndex(), 0, m_rootItem->children().size() - 1);
        delete m_rootItem;
        m_modules.clear();
        m_rootItem = new RootItem;
        endRemoveRows();

        for (int i = 0; i < m_sourceModel->rowCount(); i++) {
            const QModelIndex sourceIndex(m_sourceModel->index(i, 0));
            CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(
                m_sourceModel->data(sourceIndex,
                                    BtBookshelfModel::ModulePointerRole).value<void *>());
            m_sourceIndexMap[&module] = sourceIndex;
            addModule(module, checked.contains(&module));
        }
    }

    if (emitSignal)
        emit groupingOrderChanged(groupingOrder);
}

void BtBookshelfTreeModel::setCheckable(bool checkable) {
    if (m_checkable == checkable)
        return;
    m_checkable = checkable;
    if (m_sourceModel == 0)
        return;

    // Notify views that flags changed for all items:
    resetData();
}

void BtBookshelfTreeModel::setCheckedModules(const QSet<CSwordModuleInfo *> & modules) {
    typedef ModuleItemMap::const_iterator MIMCI;

    for (MIMCI it = m_modules.constBegin(); it != m_modules.constEnd(); ++it) {
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
        QModelIndex parent(queue.takeFirst());
        emit dataChanged(index(0, 0, parent),
                         index(rowCount(parent) - 1, columnCount() - 1, parent));
        for (int i = 0; i < rowCount(parent); i++) {
            const QModelIndex childIndex(index(i, 0, parent));
            if (rowCount(childIndex) > 0)
                queue.append(childIndex);
        }
    } while (!queue.isEmpty());
}

void BtBookshelfTreeModel::addModule(CSwordModuleInfo & module, bool checked) {
    if (m_modules.contains(&module))
        return;

#if QT_VERSION >= 0x040600
    beginResetModel();
#endif
    Grouping g(m_groupingOrder);
    addModule(module, QModelIndex(), g, checked);

    /**
      \bug Calling reset() shouldn't be necessary here, but omitting it will
           will break things like switching to a grouped layout or installing
           new modules. As a side effect, all attached views will also reset
           themselves.
    */

#if QT_VERSION >= 0x040600
    endResetModel();
#else
    reset();
#endif
}

void BtBookshelfTreeModel::addModule(CSwordModuleInfo & module,
                                     QModelIndex parentIndex,
                                     Grouping & intermediateGrouping,
                                     bool checked)
{
    if (!intermediateGrouping.empty()) {
        QModelIndex newIndex;
        switch (intermediateGrouping.front()) {

            case GROUP_CATEGORY:
                newIndex = getGroup<CategoryItem>(module, parentIndex);
                break;

            case GROUP_LANGUAGE:
                newIndex = getGroup<LanguageItem>(module, parentIndex);
                break;

            case GROUP_INDEXING:
                newIndex = getGroup<IndexingItem>(module, parentIndex);
                break;

        }
        intermediateGrouping.pop_front();
        addModule(module, newIndex, intermediateGrouping, checked);
    } else {
        Item & parentItem = getItem(parentIndex);
        ModuleItem * const newItem = new ModuleItem(module, *this);
        newItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        const int newIndex(parentItem.indexFor(newItem));

        // Actually do the insertion:
        beginInsertRows(parentIndex, newIndex, newIndex);
        parentItem.insertChild(newIndex, newItem);
        m_modules.insert(&module, newItem);
        if (checked) // Add to checked modules cache
            m_checkedModulesCache.insert(&module);

        endInsertRows();

        // Reset parent item check states, if needed:
        resetParentCheckStates(parentIndex);
    }
}

void BtBookshelfTreeModel::removeModule(CSwordModuleInfo & module) {
    typedef ModuleItemMap::iterator MIMI;
    const MIMI it = m_modules.find(&module);
    if (it == m_modules.end())
        return;

    Item * i = it.value();

    // Set i to be the lowest item (including empty groups) to remove:
    Q_ASSERT(i->parent());
    while (i->parent() != m_rootItem && i->parent()->children().size() <= 1)
        i = i->parent();
    Q_ASSERT(i);
    Q_ASSERT(i->parent());

    // Calculate item indexes:
    const int index = i->childIndex();
    const QModelIndex parentIndex(getIndex(*i->parent()));

    // Actually remove the item:
    beginRemoveRows(parentIndex, index, index);
    delete i->parent()->children().takeAt(index);
    m_modules.erase(it);
    m_checkedModulesCache.remove(&module);
    endRemoveRows();

    // Reset parent item check states, if needed:
    resetParentCheckStates(parentIndex);
}

Item & BtBookshelfTreeModel::getItem(const QModelIndex & index) const {
    if (UNLIKELY(!index.isValid()))
        return *m_rootItem;

    Item * const item = static_cast<Item *>(index.internalPointer());
    Q_ASSERT(item);
    return *item;
}

QModelIndex BtBookshelfTreeModel::getIndex(const BookshelfModel::Item & item) {
    const BookshelfModel::Item * it = &item;
    QList<int> indexes;
    for (;;) {
        const int i = it->childIndex();
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

void BtBookshelfTreeModel::resetParentCheckStates(QModelIndex parentIndex) {
    for ( ; parentIndex.isValid(); parentIndex = parentIndex.parent()) {
        Item & parentItem = *static_cast<Item *>(parentIndex.internalPointer());

        const Qt::CheckState oldState = parentItem.checkState();
        bool haveCheckedChildren = false;
        bool haveUncheckedChildren = false;
        for (int i = 0; i < parentItem.children().size(); i++) {
            const Qt::CheckState state = parentItem.children().at(i)->checkState();
            if (state == Qt::PartiallyChecked) {
                haveCheckedChildren = true;
                haveUncheckedChildren = true;
                break;
            } else if (state == Qt::Checked) {
                haveCheckedChildren = true;
                if (haveUncheckedChildren)
                    break;
            } else {
                Q_ASSERT(state == Qt::Unchecked);
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
        emit dataChanged(parentIndex, parentIndex);
    } // for ( ; parentIndex.isValid(); parentIndex = parentIndex.parent())
}

void BtBookshelfTreeModel::moduleDataChanged(const QModelIndex & topLeft,
                                             const QModelIndex & bottomRight)
{
    Q_ASSERT(!topLeft.parent().isValid());
    Q_ASSERT(!bottomRight.parent().isValid());
    Q_ASSERT(topLeft.column() == 0);
    Q_ASSERT(bottomRight.column() == 0);

    for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
        const QModelIndex moduleIndex(m_sourceModel->index(i, 0, topLeft.parent()));
        const QVariant data(m_sourceModel->data(moduleIndex,
                                                BtBookshelfModel::ModulePointerRole));
        CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(data.value<void *>());
        QModelIndex itemIndex(getIndex(*m_modules[&module]));
        Q_ASSERT(itemIndex.isValid());

        emit dataChanged(itemIndex, itemIndex);

        /*
          Also emit signals for parent items because the change might alter them
          as well, e.g. isHidden()
        */
        do {
            itemIndex = itemIndex.parent();
            emit dataChanged(itemIndex, itemIndex);
        } while (itemIndex.isValid());
    }
}

void BtBookshelfTreeModel::moduleInserted(const QModelIndex & parent,
                                          int start,
                                          int end)
{
    Q_ASSERT(start <= end);

    for (int i = start; i <= end; i++) {
        const QModelIndex moduleIndex(m_sourceModel->index(i, 0, parent));
        const QVariant data(m_sourceModel->data(moduleIndex,
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
            Q_ASSERT(m_defaultChecked == CHECKED || m_defaultChecked == UNCHECKED);
            checked = (m_defaultChecked == CHECKED);
        }
        m_sourceIndexMap[&module] = moduleIndex;
        addModule(module, checked);
    }
}

void BtBookshelfTreeModel::moduleRemoved(const QModelIndex & parent,
                                         int start,
                                         int end)
{
    Q_ASSERT(start <= end);

    for (int i = start; i <= end; i++) {
        const QModelIndex moduleIndex(m_sourceModel->index(i, 0, parent));
        const QVariant data(m_sourceModel->data(moduleIndex,
                                                BtBookshelfModel::ModulePointerRole));
        CSwordModuleInfo & module = *static_cast<CSwordModuleInfo *>(data.value<void*>());
        removeModule(module);
        m_sourceIndexMap.remove(&module);
    }
}

QDataStream & operator <<(QDataStream & os,
                          const BtBookshelfTreeModel::Grouping & o)
{
    os << o.size();
    Q_FOREACH(BtBookshelfTreeModel::Group g, o)
        os << static_cast<int>(g);
    return os;
}

QDataStream & operator >>(QDataStream & is,
                          BtBookshelfTreeModel::Grouping & o)
{
    int s;
    is >> s;
    o.clear();
    for (int i = 0; i < s; i++) {
        int g;
        is >> g;
        o.append(static_cast<BtBookshelfTreeModel::Group>(g));
    }
    return is;
}
