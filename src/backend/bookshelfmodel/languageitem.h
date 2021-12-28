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
#include <memory>
#include "../drivers/cswordmoduleinfo.h"


class Language;

namespace BookshelfModel {

class LanguageItem: public GroupItem<Item::ITEM_LANGUAGE> {

public: // methods:

    LanguageItem(CSwordModuleInfo const & module)
        : m_language(module.language()) {}

    QVariant data(int role = Qt::DisplayRole) const override;

    bool fitFor(CSwordModuleInfo const & module) const override
    { return module.language() == m_language; }

private: // fields:

    std::shared_ptr<Language const> m_language;

};

} // namespace BookshelfModel
