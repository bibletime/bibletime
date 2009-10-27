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

#ifndef MODULEITEM_H
#define MODULEITEM_H

#include "backend/bookshelfmodel/item.h"

#include "backend/bookshelfmodel/btbookshelfmodel.h"
#include "backend/drivers/cswordmoduleinfo.h"


namespace BookshelfModel {

class ModuleItem: public Item {
    public:
        ModuleItem(CSwordModuleInfo *module);

        CSwordModuleInfo *moduleInfo() const {
            return m_moduleInfo;
        }

        inline QString name() const {
            return m_moduleInfo->name();
        }

        inline QIcon icon() const {
            return BtBookshelfModel::moduleIcon(m_moduleInfo);
        }

        inline bool isHidden() const {
            return m_moduleInfo->isHidden();
        }

    protected:
        CSwordModuleInfo *m_moduleInfo;
};

} // namespace BookshelfModel

#endif // MODULEITEM_H
