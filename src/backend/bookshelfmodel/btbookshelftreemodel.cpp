/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "btbookshelftreemodel.h"

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QModelIndexList>
#include <QPair>
#include <QtCompilerDetection>
#include <QtGlobal>
#include <type_traits>
#include <utility>
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "../config/btconfigcore.h"
#include "../drivers/btconstmoduleset.h"
#include "../drivers/btmoduleset.h"
#include "categoryitem.h"
#include "indexingitem.h"
#include "languageitem.h"
#include "moduleitem.h"


using namespace BookshelfModel;

BtBookshelfTreeModel::Grouping const BtBookshelfTreeModel::Grouping::NONE;
BtBookshelfTreeModel::Grouping const BtBookshelfTreeModel::Grouping::CAT =
        { GROUP_CATEGORY };
BtBookshelfTreeModel::Grouping const BtBookshelfTreeModel::Grouping::CAT_LANG =
        { GROUP_CATEGORY, GROUP_LANGUAGE };
BtBookshelfTreeModel::Grouping const BtBookshelfTreeModel::Grouping::LANG =
        { GROUP_LANGUAGE };
BtBookshelfTreeModel::Grouping const BtBookshelfTreeModel::Grouping::LANG_CAT =
        { GROUP_LANGUAGE, GROUP_CATEGORY };
BtBookshelfTreeModel::Grouping const BtBookshelfTreeModel::Grouping::DEFAULT =
        CAT_LANG;

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
    , m_groupingOrder(
        [](BtConfigCore const & config_, QString const & configKey_){
            if (Grouping grouping; grouping.loadFrom(config_, configKey_))
                return grouping;
            return Grouping::DEFAULT;
        }(config, configKey))
    , m_defaultChecked(MODULE_HIDDEN)
    , m_checkable(false)
{}

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

    if (Q_UNLIKELY(role != Qt::CheckStateRole))
        return false;

    bool ok;
    Qt::CheckState newState = static_cast<Qt::CheckState>(value.toInt(&ok));
    if (Q_UNLIKELY(!ok))
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
        auto intermediateGrouping = m_groupingOrder.list();
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
    if (Q_UNLIKELY(!index.isValid()))
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
    os << o.list().size();
    for (auto const g : o.list())
        os << static_cast<std::underlying_type_t<decltype(g)>>(g);
    return os;
}

QDataStream & operator >>(QDataStream & is,
                          BtBookshelfTreeModel::Grouping & o)
{
    using Size = decltype(o.list().size());
    Size size = -1; // -1 stands for invalid

    // Due to a serialization bug in BibleTime the grouping size may have been
    // serialized as an int on Qt5 or as qsizetype on Qt6. The complex logic
    // which follows works around this issue by attempting to parse both cases.
    // This is not entirely future proof, but good enough for the foreseeable
    // future, assuming that Qt does not change too much and users upgrade
    // relatively often enough.
    static constexpr bool const isIntSize = std::is_same_v<int, Size>;

    // If other strange platforms need to be supported, please let us know:
    static_assert(isIntSize || sizeof(int) == 4, "Platform not supported");
    static_assert(isIntSize || sizeof(Size) == 8, "Platform not supported");

    using U = std::underlying_type_t<BtBookshelfTreeModel::Group>;
    if constexpr (isIntSize) {
        is >> size;
    } else {
        // The following relies on Qt providing us a datastream which only
        // contains the serialized value and an optional ')' at the end. See
        // QSettingsPrivate::stringToVariant() in Qt 6.8.2 for details. Assuming
        // this, and the facts that the size can only be 0, 1 or 2, and that the
        // values can only be 0 or 1, makes it possible for us to deduce the
        // width of the serialized size field.
        static constexpr Size const maxReadSize =
                sizeof(Size) + sizeof(U) * 2 + 2;
        char buf[maxReadSize];
        auto readSize = is.device()->peek(buf, maxReadSize);
        if (readSize < maxReadSize) { // Assumptions about Qt must hold
            if (buf[readSize - 1] == ')')
                --readSize;
            if (readSize == 4) { // 4 bytes can only fit an (int) size:
                int s;
                is >> s; // Consume (int)
                if (s == 0) // The (int) size can only be 0.
                    size = 0;
            } else if (readSize == 8) {
                // 8 bytes can be either hold a (Size) size of value 0, or a
                // (int) size of value 1 followed by an (int) value.
                int s;
                is >> s; // Consume (int)
                if (s == 1) { // (int) size of value 1
                    size = s;
                } else if (s == 0) { // (Size) size
                    is >> s; // Consume other (int) half of (Size) size
                    if (s == 0) // Both (int) halves must be 0 for (Size) 0
                        size = 0;
                }
            } else if (readSize == 12) {
                // 12 bytes either holds a (Size) size of value 1 and a value,
                // or an (int) size of value 2, and two values:
                int s;
                is >> s; // Consume (int)
                if (s == 2) { // (int) size of value 2
                    size = 2;
                } else if (s >= 0 && s <= 1) { // First half is either 0 or 1
                    int s2;
                    is >> s2; // Consume other (int) half of (Size) size
                    if ((s2 ^ 0x1) == s) // Other half is the other way around
                        size = 1;
                }
            } else if (readSize == 16) { // must be (Size) 2 + (int) values
                is >> size; // Consume (Size) size
                if (size != 2) // The (Size) size can only by 2
                    size = -1;
            } // else keep size as -1 (invalid)
        }
    }

    if (size >= 0 && size <= 2) {
        decltype(o.m_list) newList;
        for (; size; --size) {
            U v;
            is >> v;
            if (v < 0 || v > 1)
                break;
            newList.append(static_cast<BtBookshelfTreeModel::Group>(v));
        }
        if (!size) {
            o.m_list = std::move(newList);
            return is;
        }
    }

    qWarning() << "Failed to deserialize BtBookshelfTreeModel::Grouping";
    is.setStatus(QDataStream::ReadCorruptData);
    o.m_list.clear();
    return is;
}

#if 0
namespace {

template <typename SizeType>
void testGroupingSerialization_(BtBookshelfTreeModel::Grouping const & expected)
{
    using U = std::underlying_type_t<BtBookshelfTreeModel::Group>;
    auto const & list = expected.list();
    QByteArray byteArray;
    {
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        QDataStream os(&buffer);
        SizeType s = list.size();
        os << s;
        for (auto const g : list)
            os << static_cast<U>(g);
    }
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadOnly);
    QDataStream is(&buffer);
    BtBookshelfTreeModel::Grouping value;
    is >> value;
    if (value == expected) {
        qInfo() << "SUCCESS" << Q_FUNC_INFO << byteArray << list;
    } else {
        qFatal() << "FAILURE" << Q_FUNC_INFO << byteArray << list;
    }
}

void testGroupingSerialization(BtBookshelfTreeModel::Grouping const & expected){
    testGroupingSerialization_<int>(expected);
    testGroupingSerialization_<decltype(expected.list().size())>(expected);
}

} // anonymous namespace

void testGroupingSerializations() {
    using G = BtBookshelfTreeModel::Grouping;
    testGroupingSerialization(G::NONE);
    testGroupingSerialization(G::CAT);
    testGroupingSerialization(G::CAT_LANG);
    testGroupingSerialization(G::LANG);
    testGroupingSerialization(G::LANG_CAT);
}
#endif
