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

#include <Qt>
#include <QList>
#include <QVariant>
#include "../../util/btassert.h"


class CSwordModuleInfo;

namespace BookshelfModel {

class Item {

public: // types:

    enum Type {
        ITEM_ROOT         = 0,
        ITEM_CATEGORY     = 1,
        ITEM_LANGUAGE     = 2,
        ITEM_MODULE       = 3,
        ITEM_INDEXING     = 4
    };

public: // methods:

    Item(Type type)
        : m_type(type)
        , m_parent(nullptr)
        , m_checkState(Qt::Unchecked) {}

    virtual ~Item();

    /**
      \brief Returns the type of this item.
    */
    Type type() const { return m_type; }

    /**
      \brief Returns a pointer to the parent item of this item.
      \retval 0 if this item has no parent.
    */
    Item * parent() const { return m_parent; }

    /**
      \brief Returns the list of child items of this node.
    */
    QList<Item *> & children() { return m_children; }

    /**
      \brief Returns the index of this item under its parent.
      \retval -1 if this item has no parent.
    */
    int childIndex() const {
        return m_parent == nullptr
               ? -1
               : m_parent->m_children.indexOf(const_cast<Item *>(this));
    }

    /**
      \brief Returns the position for where the given child item would be
             inserted.
      \param[in] newItem Pointer to the item that would be inserted.
    */
    int indexFor(Item const & newItem);

    /**
      \brief Inserts the given item as a child at the given index.
      \pre The given index is a valid position for the item.
      \param[in] index The child index to insert the item at.
      \param[in] newItem The item to insert.
    */
    void insertChild(int index, Item * newItem) {
        BT_ASSERT(newItem);
        BT_ASSERT(index >= 0 && index <= m_children.size());
        m_children.insert(index, newItem);
        newItem->setParent(this);
    }

    template <class T>
    T * getGroupItem(CSwordModuleInfo & module, int & outIndex) {
        for (int i = 0; i < m_children.size(); i++) {
            BT_ASSERT(m_children.at(i)->type() == T::staticItemType());
            T * item = static_cast<T *>(m_children.at(i));
            if (item->fitFor(module)) {
                outIndex = i;
                return item;
            }
        }
        return nullptr;
    }

    /**
      \brief Returns data for this item.
    */
    virtual QVariant data(int role = Qt::DisplayRole) const;

    /**
      \brief Returns the check state of this item.
    */
    Qt::CheckState checkState() const { return m_checkState; }

    /**
      \brief Sets the check state of this item.
      \param[in] state new check state.
    */
    void setCheckState(const Qt::CheckState state) { m_checkState = state; }

    /**
      \brief Returns whether this item is fit to contain the given module.
      \param[in] module The module to check with.
      \retval true If this item is a group and can contain the given module.
      \retval false This item is not a group or is a wrong group.
    */
    virtual bool fitFor(const CSwordModuleInfo & module) const = 0;

    /**
      \brief Comparsion operator used sorting child items.
    */
    virtual bool operator<(const Item & other) const;

private: // methods:

    void setParent(Item * parent) noexcept
    { m_parent = (static_cast<void>(BT_ASSERT(parent)), parent); }

private: // fields:

    Type m_type;
    Item * m_parent;
    QList<Item *> m_children;
    Qt::CheckState m_checkState;

};

class RootItem: public Item {

public: // methods:

    RootItem() : Item(Item::ITEM_ROOT) {}

    bool fitFor(const CSwordModuleInfo &) const override;

};

template <Item::Type TYPE>
class GroupItem: public Item {

public: // methods:

    GroupItem() : Item(TYPE) {}

    static Item::Type staticItemType() { return TYPE; }

};

} // Namespace BookshelfModel
