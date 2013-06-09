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

#ifndef ITEM_H
#define ITEM_H

#include <QIcon>
#include <QList>
#include <QString>
#include <QtGlobal>
#include <QVariant>


class CSwordModuleInfo;

namespace BookshelfModel {

class Item {

public: /* Types: */

    enum Type {
        ITEM_ROOT         = 0,
        ITEM_CATEGORY     = 1,
        ITEM_LANGUAGE     = 2,
        ITEM_MODULE       = 3,
        ITEM_INDEXING     = 4
    };

public: /* Methods: */

    inline Item(Type type)
        : m_type(type)
        , m_parent(0)
        , m_checkState(Qt::Unchecked) {}

    virtual ~Item();

    /**
      \brief Returns the type of this item.
    */
    inline Type type() const {
        return m_type;
    }

    /**
      \brief Returns a pointer to the parent item of this item.
      \retval 0 if this item has no parent.
    */
    inline Item * parent() const {
        return m_parent;
    }

    /**
      \brief Returns the list of child items of this node.
    */
    inline QList<Item *> & children() {
        return m_children;
    }

    /**
      \brief Returns the index of this item under its parent.
      \retval -1 if this item has no parent.
    */
    inline int childIndex() const {
        return m_parent == 0
               ? -1
               : m_parent->m_children.indexOf(const_cast<Item *>(this));
    }

    /**
      \brief Returns the position for where the given child item would be
             inserted.
      \param[in] newItem Pointer to the item that would be inserted.
    */
    int indexFor(Item * newItem);

    /**
      \brief Inserts the given item as a child at the given index.
      \pre The given index is a valid position for the item.
      \param[in] index The child index to insert the item at.
      \param[in] newItem The item to insert.
    */
    inline void insertChild(int index, Item * newItem) {
        Q_ASSERT(newItem != 0);
        Q_ASSERT(index >= 0 && index <= m_children.size());
        m_children.insert(index, newItem);
        newItem->setParent(this);
    }

    template <class T>
    inline T * getGroupItem(CSwordModuleInfo & module, int & outIndex) {
        for (int i = 0; i < m_children.size(); i++) {
            Q_ASSERT(m_children.at(i)->type() == T::GROUP_TYPE);
            T * item = static_cast<T *>(m_children.at(i));
            if (item->fitFor(module)) {
                outIndex = i;
                return item;
            }
        }
        return 0;
    }

    /**
      \brief Returns data for this item.
    */
    virtual QVariant data(int role = Qt::DisplayRole) const;

    /**
      \brief Returns the check state of this item.
    */
    inline Qt::CheckState checkState() const {
        return m_checkState;
    }

    /**
      \brief Sets the check state of this item.
      \param[in] state new check state.
    */
    inline void setCheckState(const Qt::CheckState state) {
        m_checkState = state;
    }

    /**
      \brief Returns whether this item is fit to contain the given module.
      \param[in] module The module to check with.
      \retval true If this item is a group and can contain the given module.
      \retval false This item is not a group or is a wrong group.
    */
    inline virtual bool fitFor(const CSwordModuleInfo & module) const = 0;

    /**
      \brief Comparsion operator used sorting child items.
    */
    virtual bool operator<(const Item & other) const;

private: /* Methods: */

    inline void setParent(Item * parent) {
        Q_ASSERT(parent != 0);
        m_parent = parent;
    }

private: /* Fields: */

    Type m_type;
    Item * m_parent;
    QList<Item *> m_children;
    Qt::CheckState m_checkState;

};

class RootItem: public Item {

public: /* Methods: */

    inline RootItem()
        : Item(Item::ITEM_ROOT) {}

    inline virtual bool fitFor(const CSwordModuleInfo &) const {
        return true;
    }

};

} // Namespace BookshelfModel

#endif // ITEM_H
