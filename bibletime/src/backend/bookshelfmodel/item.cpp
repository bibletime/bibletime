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

#include "backend/bookshelfmodel/item.h"

#include "backend/bookshelfmodel/categoryitem.h"
#include "backend/bookshelfmodel/distributionitem.h"
#include "backend/bookshelfmodel/languageitem.h"

namespace BookshelfModel {

Item::Item(Type type)
    : m_type(type), m_parent(0), m_checkState(Qt::Unchecked)
{
    // Intentionally empty
}

Item::~Item() {
    qDeleteAll(m_children);
}

int Item::indexFor(Item *newItem) {
    Q_ASSERT(newItem != 0);

    if (m_children.empty()) return 0;

    int i(0);
    for (;;) {
        Item *nextItem(m_children.at(i));
        if (*newItem < *nextItem) {
            return i;
        }
        i++;
        if (i >= m_children.size()) {
            return i;
        }
    }
}

CategoryItem *Item::getCategoryItem(CSwordModuleInfo *module,
                                    int *index) const
{
    for (int i(0); i < m_children.size(); i++) {
        Q_ASSERT(m_children.at(i)->type() == ITEM_CATEGORY);
        CategoryItem *item(static_cast<CategoryItem*>(m_children.at(i)));
        if (item->category() == module->category()) {
            if (*index != 0) *index = i;
            return item;
        }
    }
    return 0;
}

DistributionItem *Item::getDistributionItem(CSwordModuleInfo *module, int *index) const {
    for (int i(0); i < m_children.size(); i++) {
        Q_ASSERT(m_children.at(i)->type() == ITEM_DISTRIBUTION);
        DistributionItem *item(static_cast<DistributionItem*>(m_children.at(i)));
        if (item->distribution() == module->config(CSwordModuleInfo::DistributionSource)) {
            if (*index != 0) *index = i;
            return item;
        }
    }
    return 0;
}

LanguageItem *Item::getLanguageItem(CSwordModuleInfo *module,
                                    int *index) const
{
    for (int i(0); i < m_children.size(); i++) {
        Q_ASSERT(m_children.at(i)->type() == ITEM_LANGUAGE);
        LanguageItem *item(static_cast<LanguageItem*>(m_children.at(i)));
        if (item->language() == module->language()) {
            if (*index != 0) *index = i;
            return item;
        }
    }
    return 0;
}

bool Item::operator<(const Item &other) const {
    if (m_type != other.type()) {
        return m_type < other.type();
    }
    return name().localeAwareCompare(other.name()) < 0;
}

} // namespace BookshelfModel
