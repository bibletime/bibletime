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

#include "item.h"

#include <Qt>
#include <QVariant>
#include "../drivers/cswordmoduleinfo.h"


namespace BookshelfModel {

class IndexingItem: public GroupItem<Item::ITEM_INDEXING> {

public: // methods:

    IndexingItem(CSwordModuleInfo const & module)
        : m_indexed(module.hasIndex()) {}

    QVariant data(int role = Qt::DisplayRole) const override;

    bool fitFor(CSwordModuleInfo const & module) const override
    { return module.hasIndex() == m_indexed; }

private: // fields:

    bool m_indexed;

};

} // namespace BookshelfModel
