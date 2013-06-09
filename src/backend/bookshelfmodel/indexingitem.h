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

#ifndef INDEXINGITEM_H
#define INDEXINGITEM_H

#include "backend/bookshelfmodel/item.h"

#include "backend/drivers/cswordmoduleinfo.h"


namespace BookshelfModel {

class IndexingItem: public Item {

public: /* Types: */

    enum { GROUP_TYPE = Item::ITEM_INDEXING };

public: /* Methods: */

    inline IndexingItem(const CSwordModuleInfo & module)
        : Item(Item::ITEM_INDEXING)
        , m_indexed(module.hasIndex()) {}

    QVariant data(int role = Qt::DisplayRole) const;

    inline bool fitFor(const CSwordModuleInfo & module) const {
        return module.hasIndex() == m_indexed;
    }

private: /* Fields: */

    bool m_indexed;

};

} // namespace BookshelfModel

#endif // INDEXINGITEM_H
