/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2025 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "moduleitem.h"

#include "btbookshelftreemodel.h"


namespace BookshelfModel {

QVariant ModuleItem::data(int const role) const {
    // Dispatch request to tree model:
    return m_parentModel.data(m_moduleInfo, role);
}

} // namespace BookshelfModel
