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

#pragma once

#include <QAbstractItemModel>

#include <memory>
#include <QList>
#include <QObject>
#include <QMap>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QString>
#include <QtCore>
#include <QVariant>
#include "../drivers/btconstmoduleset.h"
#include "../drivers/btmoduleset.h"
#include "btbookshelfmodel.h"
#include "item.h"


namespace BookshelfModel { class ModuleItem; }
class BtConfigCore;
class CSwordModuleInfo;

class BtBookshelfTreeModel: public QAbstractItemModel {

    Q_OBJECT
    Q_ENUMS(Group)

private: // types:

    using ModuleItemMap = QMap<CSwordModuleInfo *, BookshelfModel::ModuleItem *>;
    using SourceIndexMap = QMap<CSwordModuleInfo *, QPersistentModelIndex>;

public: // types:

    enum ModuleRole {
        CheckStateRole = BtBookshelfModel::UserRole,
        UserRole = BtBookshelfModel::UserRole + 100
    };

    enum Group {
        GROUP_CATEGORY = 0,
        GROUP_LANGUAGE = 1,
        GROUP_INDEXING
    };

    enum CheckedBehavior {
        CHECKED,        /**< Check all added modules by default. */
        UNCHECKED,      /**< Uncheck all added modules by default. */
        MODULE_HIDDEN,  /**< By default, check only added modules that are not hidden. */
        MODULE_INDEXED  /**< By default, check only added modules that are indexed. */
    };

    class Grouping: public QList<Group> {

        public: // methods:

            /**
              \warning Be careful using this constructor!
            */
            explicit Grouping(bool empty = false) {
                if (empty)
                    return;
                push_back(GROUP_CATEGORY);
                push_back(GROUP_LANGUAGE);
            }

            explicit Grouping(Group group) { push_back(group); }

            Grouping(BtConfigCore const & config, QString const & key) {
                if (loadFrom(config, key))
                    return;
                push_back(GROUP_CATEGORY);
                push_back(GROUP_LANGUAGE);
            }

            Grouping(Grouping const & copy) = default;
            Grouping & operator=(Grouping const & copy) = default;

            bool loadFrom(BtConfigCore const & config, QString const & key);
            void saveTo(BtConfigCore & config, QString const & key) const;

    };

public: // methods:

    BtBookshelfTreeModel(QObject * parent = nullptr);
    BtBookshelfTreeModel(BtConfigCore const & config,
                         QString const & configKey,
                         QObject * parent = nullptr);
    BtBookshelfTreeModel(const Grouping & grouping, QObject * parent = nullptr);
    ~BtBookshelfTreeModel() override;

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex & parent = QModelIndex()) const override;
    QModelIndex index(int row,
                      int column,
                      const QModelIndex & parent = QModelIndex())
            const override;
    QModelIndex parent(const QModelIndex & index) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole)
            const override;
    QVariant data(CSwordModuleInfo & module, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex & index) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex & index,
                 const QVariant & value,
                 int role) override;

    CSwordModuleInfo * module(QModelIndex const & index) const {
        return static_cast<CSwordModuleInfo *>(
                data(index,
                     BtBookshelfModel::ModulePointerRole).value<void *>());
    }
    std::shared_ptr<QAbstractItemModel> sourceModel() const noexcept
    { return m_sourceModel; }

    Grouping const & groupingOrder() const { return m_groupingOrder; }
    bool checkable() const { return m_checkable; }
    CheckedBehavior defaultChecked() const { return m_defaultChecked; }
    QList<CSwordModuleInfo *> modules() const { return m_modules.keys(); }
    BtModuleSet const & checkedModules() const {
        return m_checkedModulesCache;
    }

public Q_SLOTS:

    void setSourceModel(std::shared_ptr<QAbstractItemModel> sourceModel);
    void setGroupingOrder(const BtBookshelfTreeModel::Grouping & groupingOrder,
                          bool emitSignal = true);
    void setCheckable(bool checkable);
    void setDefaultChecked(CheckedBehavior b) { m_defaultChecked = b; }
    void setCheckedModules(BtConstModuleSet const & modules);

Q_SIGNALS:

    void groupingOrderChanged(BtBookshelfTreeModel::Grouping newGrouping);
    void moduleChecked(CSwordModuleInfo * module, bool checked);

protected: // methods:

    void resetData();

protected Q_SLOTS:

    void moduleDataChanged(const QModelIndex & topLeft,
                           const QModelIndex & bottomRight);
    void moduleInserted(const QModelIndex & parent, int start, int end);
    void moduleRemoved(const QModelIndex & parent, int start, int end);

private: // methods:

    void addModule(CSwordModuleInfo & module, bool checked);
    void addModule(CSwordModuleInfo & module,
                   QModelIndex parentIndex,
                   Grouping & intermediateGrouping,
                   bool checked);
    void removeModule(CSwordModuleInfo & module);

    BookshelfModel::Item & getItem(const QModelIndex & index) const;
    QModelIndex getIndex(const BookshelfModel::Item & item);
    void resetParentCheckStates(QModelIndex parentIndex);

    template <class T>
    QModelIndex getGroup(CSwordModuleInfo & module,
                         QModelIndex parentIndex)
    {
        BookshelfModel::Item & parentItem = getItem(parentIndex);
        int groupIndex;
        T * groupItem = parentItem.getGroupItem<T>(module, groupIndex);

        if (!groupItem) {
            groupItem = new T(module);
            groupIndex = parentItem.indexFor(*groupItem);
            beginInsertRows(parentIndex, groupIndex, groupIndex);
            parentItem.insertChild(groupIndex, groupItem);
            endInsertRows();
        }
        return index(groupIndex, 0, parentIndex);
    }

private: // fields:

    std::shared_ptr<QAbstractItemModel> m_sourceModel;
    std::unique_ptr<BookshelfModel::Item> m_rootItem;
    ModuleItemMap m_modules;
    SourceIndexMap m_sourceIndexMap;
    Grouping m_groupingOrder;
    CheckedBehavior m_defaultChecked;
    bool m_checkable;

    BtModuleSet m_checkedModulesCache;

};

QDataStream & operator <<(QDataStream & os, const BtBookshelfTreeModel::Grouping & o);
QDataStream & operator >>(QDataStream & is, BtBookshelfTreeModel::Grouping & o);

Q_DECLARE_METATYPE(BtBookshelfTreeModel::Grouping)
