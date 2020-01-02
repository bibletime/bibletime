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

#ifndef LANGUAGEITEM_H
#define LANGUAGEITEM_H

#include "item.h"

#include "../drivers/cswordmoduleinfo.h"
#include "btbookshelfmodel.h"


namespace BookshelfModel {

class LanguageItem: public GroupItem<Item::ITEM_LANGUAGE> {

public: /* Methods: */

    inline LanguageItem(const CSwordModuleInfo & module)
        : m_language(module.language()) {}

    QVariant data(int role = Qt::DisplayRole) const override;

    inline bool fitFor(const CSwordModuleInfo & module) const override {
        return module.language() == m_language;
    }

private: /* Fields: */

    const CLanguageMgr::Language * m_language;

};

} // namespace BookshelfModel

#endif // LANGUAGEITEM_H
