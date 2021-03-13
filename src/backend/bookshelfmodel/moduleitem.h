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

#include "../drivers/cswordmoduleinfo.h"
#include "btbookshelfmodel.h"


class BtBookshelfTreeModel;

namespace BookshelfModel {

class ModuleItem: public Item {

public: /* Methods: */

    ModuleItem(CSwordModuleInfo & module,
               BtBookshelfTreeModel & parentModel)
        : Item(ITEM_MODULE)
        , m_moduleInfo(module)
        , m_parentModel(parentModel) {}

    /**
      Reimplementation of Item::data() which dispatches all
      requests to the parent model (BtBookshelfTreeModel).
    */
    QVariant data(int role = Qt::DisplayRole) const override;

    CSwordModuleInfo & moduleInfo() const { return m_moduleInfo; }

    bool fitFor(CSwordModuleInfo const &) const override { return false; }

private: /* Fields: */

    CSwordModuleInfo & m_moduleInfo;
    BtBookshelfTreeModel & m_parentModel;

};

} // namespace BookshelfModel
