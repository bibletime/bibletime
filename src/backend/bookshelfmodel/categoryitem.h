/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#pragma once

#include "item.h"

#include <QCoreApplication>
#include "../drivers/cswordmoduleinfo.h"
#include "btbookshelfmodel.h"


namespace BookshelfModel {

class CategoryItem: public GroupItem<Item::ITEM_CATEGORY> {

public: /* Methods: */

    CategoryItem(CSwordModuleInfo const & module)
        : m_category(module.category()) {}

    CSwordModuleInfo::Category category() const { return m_category; }

    QVariant data(int role = Qt::DisplayRole) const override;

    bool fitFor(CSwordModuleInfo const & module) const override
    { return module.category() == m_category; }

    bool operator<(const Item & other) const override;

private: /* Fields: */

    CSwordModuleInfo::Category const m_category;

};

} // namespace BookshelfModel
