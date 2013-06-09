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

#ifndef LANGUAGEITEM_H
#define LANGUAGEITEM_H

#include "backend/bookshelfmodel/item.h"

#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/drivers/cswordmoduleinfo.h"


namespace BookshelfModel {

class LanguageItem: public Item {

public: /* Types: */

    enum { GROUP_TYPE = Item::ITEM_LANGUAGE };

public: /* Methods: */

    inline LanguageItem(const CSwordModuleInfo & module)
        : Item(ITEM_LANGUAGE)
        , m_language(module.language()) {}

    QVariant data(int role = Qt::DisplayRole) const;

    inline bool fitFor(const CSwordModuleInfo & module) const {
        return module.language() == m_language;
    }

private: /* Fields: */

    const CLanguageMgr::Language * m_language;

};

} // namespace BookshelfModel

#endif // LANGUAGEITEM_H
