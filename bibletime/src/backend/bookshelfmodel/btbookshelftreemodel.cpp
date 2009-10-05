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

#include "backend/bookshelfmodel/btbookshelftreemodel.h"

#include <QQueue>
#include <QSet>
#include "backend/bookshelfmodel/categoryitem.h"
#include "backend/bookshelfmodel/distributionitem.h"
#include "backend/bookshelfmodel/languageitem.h"
#include "backend/bookshelfmodel/moduleitem.h"

using namespace BookshelfModel;

BtBookshelfTreeModel::BtBookshelfTreeModel(QObject *parent)
    : QAbstractItemModel(parent), m_sourceModel(0), m_rootItem(new RootItem),
      m_checkable(false), m_defaultChecked(false)
{
    m_groupingOrder.push_back(GROUP_CATEGORY);
    m_groupingOrder.push_back(GROUP_LANGUAGE);
}

BtBookshelfTreeModel::~BtBookshelfTreeModel() {
    delete m_rootItem;
}

int BtBookshelfTreeModel::rowCount(const QModelIndex &parent) const {
    return getItem(parent)->children().size();
}

int BtBookshelfTreeModel::columnCount(const QModelIndex &parent) const {
    return 1;
}

bool BtBookshelfTreeModel::hasChildren(const QModelIndex &parent) const {
    return !getItem(parent)->children().isEmpty();
}

QModelIndex BtBookshelfTreeModel::index(int row, int column,
                                        const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();

    Item *parentItem(getItem(parent));
    Item *childItem(parentItem->childAt(row));
    if (childItem != 0) {
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex BtBookshelfTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();

    Item *childItem(static_cast<Item*>(index.internalPointer()));
    Q_ASSERT(childItem != 0);
    Item *parentItem(childItem->parent());
    Q_ASSERT(parentItem != 0);

    if (parentItem == m_rootItem) {
        return QModelIndex();
    }
    return createIndex(parentItem->childIndex(), 0, parentItem);
}

QVariant BtBookshelfTreeModel::data(const QModelIndex &index, int role) const {
    typedef util::filesystem::DirectoryUtil DU;

    if (!index.isValid() || index.column() != 0) {
        return QVariant();
    }

    Item *i(static_cast<Item*>(index.internalPointer()));
    Q_ASSERT(i != 0);
    switch (role) {
        case Qt::DisplayRole:
            return i->name();
        case Qt::CheckStateRole:
            if (!m_checkable) break;
        case BtBookshelfTreeModel::CheckStateRole:
            return i->checkState();
        case Qt::DecorationRole:
            return i->icon();
        case BtBookshelfModel::ModulePointerRole:
            if (i->type() == Item::ITEM_MODULE) {
                ModuleItem *mi(dynamic_cast<ModuleItem *>(i));
                if (mi != 0) {
                    return qVariantFromValue((void*) mi->moduleInfo());
                }
            }
            return 0;
        default:
            break;
    }
    return QVariant();
}

bool BtBookshelfTreeModel::setData(const QModelIndex &itemIndex,
                                   const QVariant &value,
                                   int role)
{
    typedef QPair<Item *, QModelIndex> IP;

    if (role == Qt::CheckStateRole) {
        bool ok;
        Qt::CheckState newState((Qt::CheckState) value.toInt(&ok));
        if (ok && newState != Qt::PartiallyChecked) {
            Item *i(static_cast<Item*>(itemIndex.internalPointer()));
            Q_ASSERT(i != 0);
            // Recursively (un)check all children:
            QList<IP> q;
            q.append(IP(i, itemIndex));
            while (!q.isEmpty()) {
                const IP p(q.takeFirst());
                Item *item(p.first);
                item->setCheckState(newState);
                emit dataChanged(p.second, p.second);
                const QList<Item*> &children(item->children());
                for (int i(0); i < children.size(); i++) {
                    q.append(IP(children.at(i), index(i, 0, p.second)));
                }
            }

            // Change check state of the item itself
            i->setCheckState(newState);
            emit dataChanged(itemIndex, itemIndex);

            // Recursively change parent check states.
            resetParentCheckStates(itemIndex.parent());

            return true;
        } // if (ok && newState != Qt::PartiallyChecked)
    } // if (role == Qt::CheckStateRole)
    return false;
}

Qt::ItemFlags BtBookshelfTreeModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return 0;

    Qt::ItemFlags f(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    if (m_checkable) {
        f |= Qt::ItemIsUserCheckable;

        Item *i(static_cast<Item*>(index.internalPointer()));
        Q_ASSERT(i != 0);

        if (i->type() != Item::ITEM_MODULE) {
            f |= Qt::ItemIsTristate;
        }
    }

    return f;
}

QVariant BtBookshelfTreeModel::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const
{
    if (orientation == Qt::Horizontal &&
        section == 0 && role == Qt::DisplayRole)
    {
        return tr("Module");
    }
    return QVariant();
}

void BtBookshelfTreeModel::setSourceModel(QAbstractListModel *sourceModel) {
    if (m_sourceModel == sourceModel) return;

    if (m_sourceModel != 0) {
        disconnect(this, SLOT(moduleInserted(QModelIndex,int,int)));
        disconnect(this, SLOT(moduleRemoved(QModelIndex,int,int)));
        disconnect(this, SLOT(moduleDataChanged(QModelIndex,QModelIndex)));
        beginRemoveRows(QModelIndex(), 0, m_rootItem->children().size() - 1);
        delete m_rootItem;
        m_modules.clear();
        m_rootItem = new RootItem;
        endRemoveRows();
    }

    m_sourceModel = sourceModel;

    if (sourceModel != 0) {
        connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                this,        SLOT(moduleRemoved(QModelIndex,int,int)));
        connect(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
                this,        SLOT(moduleInserted(QModelIndex,int,int)));
        connect(sourceModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
                this,        SLOT(moduleDataChanged(QModelIndex, QModelIndex)));

        BtBookshelfModel *m(dynamic_cast<BtBookshelfModel*>(sourceModel));
        if (m != 0) {
            Q_FOREACH(CSwordModuleInfo *module, m->modules()) {
                addModule(module, m_defaultChecked);
            }
        } else {
            for (int i(0); i < sourceModel->rowCount(); i++) {
                CSwordModuleInfo *module(
                    static_cast<CSwordModuleInfo *>(
                        sourceModel->data(
                            sourceModel->index(i),
                            BtBookshelfModel::ModulePointerRole
                        ).value<void*>()
                    )
                );
                Q_ASSERT(module != 0);
                addModule(
                    module,
                    m_defaultChecked
                );
            }
        }
    }
}

void BtBookshelfTreeModel::setGroupingOrder(const Grouping &groupingOrder) {
    if (m_groupingOrder == groupingOrder) return;
    m_groupingOrder = groupingOrder;

    if (m_sourceModel != 0) {
        QSet<CSwordModuleInfo*> checked(checkedModules().toSet());
        beginRemoveRows(QModelIndex(), 0, m_rootItem->children().size() - 1);
        delete m_rootItem;
        m_modules.clear();
        m_rootItem = new RootItem;
        endRemoveRows();

        BtBookshelfModel *m(dynamic_cast<BtBookshelfModel*>(m_sourceModel));
        if (m != 0) {
            Q_FOREACH(CSwordModuleInfo *module, m->modules()) {
                addModule(module, checked.contains(module));
            }
        } else {
            for (int i(0); i < m_sourceModel->rowCount(); i++) {
                CSwordModuleInfo *module(
                    static_cast<CSwordModuleInfo *>(
                        m_sourceModel->data(
                            m_sourceModel->index(i),
                            BtBookshelfModel::ModulePointerRole
                        ).value<void*>()
                    )
                );
                Q_ASSERT(module != 0);
                addModule(module, checked.contains(module));
            }
        }
    }
}

void BtBookshelfTreeModel::setCheckable(bool checkable) {
    if (m_checkable == checkable) return;
    m_checkable = checkable;
    if (m_sourceModel != 0) {
        QModelIndexList queue;
        queue.append(QModelIndex());
        do {
            QModelIndex parent(queue.takeFirst());
            int numChildren(rowCount(parent));
            emit dataChanged(index(0, 0, parent),
                             index(numChildren - 1, 0, parent));
            for (int i(0); i < numChildren; i++) {
                QModelIndex childIndex(index(i, 0, parent));
                if (rowCount(childIndex) > 0) {
                    queue.append(childIndex);
                }
            }
        } while (!queue.isEmpty());
    }
}

QList<CSwordModuleInfo*> BtBookshelfTreeModel::checkedModules() const {
    typedef ModuleItemMap::const_iterator MMCI;

    QList<CSwordModuleInfo*> modules;
    for (MMCI it(m_modules.constBegin()); it != m_modules.constEnd(); it++) {
        if (it.value()->checkState() == Qt::Checked) {
            modules.append(it.key());
        }
    }
    return modules;
}

void BtBookshelfTreeModel::addModule(CSwordModuleInfo *module, bool checked) {
    if (m_modules.contains(module)) return;
    Grouping g(m_groupingOrder);
    addModule(module, QModelIndex(), g, checked);

    /**
      \bug Calling reset() shouldn't be necessary here, but omitting it will
           will break things like switching to a grouped layout or installing
           new modules. As a side effect, all attached views will also reset
           themselves.
    */
    reset();
}

void BtBookshelfTreeModel::addModule(CSwordModuleInfo *module,
                                     QModelIndex parentIndex,
                                     Grouping &intermediateGrouping,
                                     bool checked)
{
    Q_ASSERT(module != 0);

    if (!intermediateGrouping.empty()) {
        QModelIndex newIndex;
        switch (intermediateGrouping.front()) {
            case GROUP_DISTRIBUTION:
                newIndex = getGroup<DistributionItem>(module, parentIndex);
                break;
            case GROUP_CATEGORY:
                newIndex = getGroup<CategoryItem>(module, parentIndex);
                break;
            case GROUP_LANGUAGE:
                newIndex = getGroup<LanguageItem>(module, parentIndex);
                break;
        }
        intermediateGrouping.pop_front();
        addModule(module, newIndex, intermediateGrouping, checked);
    } else {
        Item *parentItem(getItem(parentIndex));
        ModuleItem *newItem(new ModuleItem(module));
        newItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        const int newIndex(parentItem->indexFor(newItem));
        beginInsertRows(parentIndex, newIndex, newIndex);
        parentItem->insertChild(newIndex, newItem);
        m_modules.insert(module, newItem);
        endInsertRows();
        resetParentCheckStates(parentIndex);
    }
}

void BtBookshelfTreeModel::removeModule(CSwordModuleInfo *module) {
    Item *i(m_modules.value(module, 0));
    if (i == 0) return;

    // Set i to be the lowest item (including empty groups) to remove:
    Q_ASSERT(i->parent() != 0);
    while (i->parent() != m_rootItem && i->parent()->children().size() <= 1) {
        i = i->parent();
    }
    Q_ASSERT(i != 0);

    // Calculate index of parent item:
    QModelIndex parentIndex;
    {
        QList<int> indexes;
        for (Item *j(i->parent()); j != m_rootItem; j = j->parent()) {
            Q_ASSERT(j != 0);
            indexes.push_back(j->childIndex());
        }
        while (!indexes.isEmpty()) {
            parentIndex = index(indexes.takeLast(), 0, parentIndex);
        }
    }

    // Remove item:
    int index(i->childIndex());
    beginRemoveRows(parentIndex, index, index);
    i->parent()->deleteChildAt(index);
    m_modules.remove(module);
    endRemoveRows();
    resetParentCheckStates(parentIndex);
}

Item *BtBookshelfTreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()) {
        Item *item(static_cast<Item*>(index.internalPointer()));
        Q_ASSERT(item != 0);
        return item;
    } else {
        return m_rootItem;
    }
}

void BtBookshelfTreeModel::resetParentCheckStates(QModelIndex parentIndex) {
    for ( ; parentIndex.isValid(); parentIndex = parentIndex.parent()) {
        Item *parentItem(static_cast<Item*>(parentIndex.internalPointer()));
        Q_ASSERT(parentItem != 0);

        Qt::CheckState oldState(parentItem->checkState());
        bool haveCheckedChildren(false);
        bool haveUncheckedChildren(false);
        for (int i(0); i < parentItem->children().size(); i++) {
            Qt::CheckState state(parentItem->childAt(i)->checkState());
            if (state == Qt::PartiallyChecked) {
                haveCheckedChildren = true;
                haveUncheckedChildren = true;
                break;
            } else if (state == Qt::Checked) {
                haveCheckedChildren = true;
                if (haveUncheckedChildren) break;
            } else if (state == Qt::Unchecked) {
                haveUncheckedChildren = true;
                if (haveCheckedChildren) break;
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
        if (newState == oldState) break;

        parentItem->setCheckState(newState);
        emit dataChanged(parentIndex, parentIndex);
    } // for ( ; parentIndex.isValid(); parentIndex = parentIndex.parent())
}

void BtBookshelfTreeModel::moduleDataChanged(const QModelIndex &topLeft,
                                             const QModelIndex &bottomRight)
{
    typedef BtBookshelfModel BM;
    static const BM::ModuleRole PR(BM::ModulePointerRole);

    Q_ASSERT(!topLeft.parent().isValid());
    Q_ASSERT(!bottomRight.parent().isValid());
    Q_ASSERT(topLeft.column() == 0 && bottomRight.column() == 0);

    for (int i(topLeft.row()); i <= bottomRight.row(); i++) {
        QModelIndex moduleIndex(m_sourceModel->index(i, 0, topLeft.parent()));
        QVariant data(m_sourceModel->data(moduleIndex, PR));
        CSwordModuleInfo *module((CSwordModuleInfo *) (data.value<void*>()));

        /// \todo There might be a better way to do this.
        bool checked(m_modules.value(module)->checkState() == Qt::Checked);
        removeModule(module);
        addModule(module, checked);
    }
}

void BtBookshelfTreeModel::moduleInserted(const QModelIndex &parent, int start,
                                          int end)
{
    typedef BtBookshelfModel BM;
    static const BM::ModuleRole PR(BM::ModulePointerRole);

    for (int i(start); i <= end; i++) {
        QModelIndex moduleIndex(m_sourceModel->index(i, 0, parent));
        QVariant data(m_sourceModel->data(moduleIndex, PR));
        CSwordModuleInfo *module((CSwordModuleInfo *) (data.value<void*>()));

        addModule(module, m_defaultChecked);
    }
}

void BtBookshelfTreeModel::moduleRemoved(const QModelIndex &parent, int start,
                                         int end)
{
    typedef BtBookshelfModel BM;
    static const BM::ModuleRole PR(BM::ModulePointerRole);

    for (int i(start); i <= end; i++) {
        QModelIndex moduleIndex(m_sourceModel->index(i, 0, parent));
        QVariant data(m_sourceModel->data(moduleIndex, PR));
        CSwordModuleInfo *module((CSwordModuleInfo *) (data.value<void*>()));

        removeModule(module);
    }
}
