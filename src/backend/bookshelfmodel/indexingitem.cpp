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

#include "indexingitem.h"


namespace BookshelfModel {

QVariant IndexingItem::data(int role) const {
    if (role != Qt::DisplayRole)
        return Item::data(role);

    return m_indexed
           ? QObject::tr("Indexed works")
           : QObject::tr("Unindexed works");
}

} // namespace BookshelfModel
