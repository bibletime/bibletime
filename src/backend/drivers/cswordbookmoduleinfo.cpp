/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cswordbookmoduleinfo.h"

#include <swordxx/keys/treekey.h>
#include <swordxx/keys/treekeyidx.h>
#include "../../util/btassert.h"
#include "../keys/cswordtreekey.h"


CSwordBookModuleInfo::CSwordBookModuleInfo(swordxx::SWModule & module,
                                           CSwordBackend & backend)
    : CSwordModuleInfo(module, backend, CSwordModuleInfo::GenericBook)
    , m_depth(-1)
{
    if (auto const key = tree()) {
        key->root();
        computeDepth(key.get(), 0);
    }
}

void CSwordBookModuleInfo::computeDepth(swordxx::TreeKeyIdx * const key,
                                        const int level)
{
    auto const savedKey(key->getText());
    if (level > m_depth)
        m_depth = level;

    if (key->hasChildren()) {
        key->firstChild();
        computeDepth(key, level + 1u);
        key->setText(savedKey); // Return to the initial value
    }

    if (key->nextSibling())
        computeDepth(key, level);
}

/** Returns a treekey filled with the structure of this module */
std::shared_ptr<swordxx::TreeKeyIdx> CSwordBookModuleInfo::tree() const {
    auto r(std::dynamic_pointer_cast<swordxx::TreeKeyIdx>(module().getKey()));
    BT_ASSERT(r);
    return r;
}
