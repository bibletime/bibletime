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

#ifndef BTBOOKSHELFTREEMODEL_H
#define BTBOOKSHELFTREEMODEL_H

#include <QAbstractItemModel>

#include <QList>
#include <QMap>
#include <QPersistentModelIndex>
#include <QSet>
#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/bookshelfmodel/item.h"


namespace BookshelfModel {
class ModuleItem;
}
class CSwordModuleInfo;
class QDataStream;

class BtBookshelfTreeModel: public QAbstractItemModel {
        Q_OBJECT
        Q_ENUMS(Group)

        typedef QMap<CSwordModuleInfo*, BookshelfModel::ModuleItem*> ModuleItemMap;
        typedef QMap<CSwordModuleInfo*, QPersistentModelIndex> SourceIndexMap;

    public:
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
        typedef QList<Group> Grouping;

        BtBookshelfTreeModel(const Grouping &grouping, QObject *parent = 0);
        virtual ~BtBookshelfTreeModel();

        static Grouping defaultGrouping();

        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex())
        const;
        virtual bool hasChildren(const QModelIndex &parent = QModelIndex())
        const;
        virtual QModelIndex index(int row, int column,
                                  const QModelIndex &parent = QModelIndex())
        const;
        virtual QModelIndex parent(const QModelIndex &index) const;
        virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        QVariant data(CSwordModuleInfo *module, int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value,
                             int role);

        inline QAbstractItemModel *sourceModel() const { return m_sourceModel; }
        inline const Grouping &groupingOrder() const { return m_groupingOrder; }
        inline bool checkable() const { return m_checkable; }
        inline CheckedBehavior defaultChecked() const { return m_defaultChecked; }
        inline QList<CSwordModuleInfo*> modules() const { return m_modules.keys(); }
        inline const QSet<const CSwordModuleInfo*> &checkedModules() const {
            return m_checkedModulesCache;
        }

    public slots:
        void setSourceModel(QAbstractItemModel *sourceModel);
        void setGroupingOrder(const BtBookshelfTreeModel::Grouping &groupingOrder);
        void setCheckable(bool checkable);
        inline void setDefaultChecked(CheckedBehavior b) {
            m_defaultChecked = b;
        }
        void setCheckedModules(const QSet<const CSwordModuleInfo*> &modules);

    signals:
        void groupingOrderChanged(BtBookshelfTreeModel::Grouping newGrouping);
        void moduleChecked(CSwordModuleInfo *module, bool checked);

    protected:
        void resetData();
        QVariant parentData(BookshelfModel::ModuleItem *item, int role) const;
        void addModule(CSwordModuleInfo *module, bool checked);
        void addModule(CSwordModuleInfo *module, QModelIndex parentIndex,
                       Grouping &intermediateGrouping, bool checked);
        void removeModule(CSwordModuleInfo *module);

        BookshelfModel::Item *getItem(const QModelIndex &index) const;
        QModelIndex getIndex(const BookshelfModel::Item *item);
        void resetParentCheckStates(QModelIndex parentIndex);

        template <class T>
        QModelIndex getGroup(CSwordModuleInfo *module,
                             QModelIndex parentIndex) {
            BookshelfModel::Item *parentItem(getItem(parentIndex));
            int groupIndex;
            T *groupItem(parentItem->getGroupItem<T>(module, groupIndex));

            if (groupItem == 0) {
                groupItem = new T(module);
                groupIndex = parentItem->indexFor(groupItem);
                beginInsertRows(parentIndex, groupIndex, groupIndex);
                parentItem->insertChild(groupIndex, groupItem);
                endInsertRows();
            }
            return index(groupIndex, 0, parentIndex);
        }

    protected slots:
        void moduleDataChanged(const QModelIndex &topLeft,
                               const QModelIndex &bottomRight);
        void moduleInserted(const QModelIndex &parent, int start, int end);
        void moduleRemoved(const QModelIndex &parent, int start, int end);

    protected:
        QAbstractItemModel   *m_sourceModel;
        BookshelfModel::Item *m_rootItem;
        ModuleItemMap         m_modules;
        SourceIndexMap        m_sourceIndexMap;
        Grouping              m_groupingOrder;
        CheckedBehavior       m_defaultChecked;
        bool                  m_checkable;

        QSet<const CSwordModuleInfo*> m_checkedModulesCache;
};

QDataStream &operator<<(QDataStream &os, const BtBookshelfTreeModel::Grouping &o);
QDataStream &operator>>(QDataStream &is, BtBookshelfTreeModel::Grouping &o);

Q_DECLARE_METATYPE(BtBookshelfTreeModel::Grouping);

#endif // BTBOOKSHELFTREEMODEL_H
