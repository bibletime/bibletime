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

#ifndef CATEGORYITEM_H
#define CATEGORYITEM_H

#include "backend/bookshelfmodel/item.h"

#include <QCoreApplication>
#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/drivers/cswordmoduleinfo.h"

namespace BookshelfModel {

class CategoryItem: public Item {
    Q_DECLARE_TR_FUNCTIONS(CategoryItem);

    public:
        CategoryItem(CSwordModuleInfo *module);

        inline const CSwordModuleInfo::Category &category() const {
            return m_category;
        }

        inline QString name() const {
            return BtBookshelfModel::categoryName(m_category);
        }

        inline QIcon icon() const {
            return BtBookshelfModel::categoryIcon(m_category);
        }

    protected:
        CSwordModuleInfo::Category m_category;
};

} // namespace BookshelfModel

#endif // CATEGORYITEM_H
