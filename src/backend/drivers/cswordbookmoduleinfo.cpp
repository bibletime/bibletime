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

#include "cswordbookmoduleinfo.h"

#include "../../util/btassert.h"
#include "../keys/cswordtreekey.h"

// Sword includes:
#include <treekey.h>
#include <treekeyidx.h>


CSwordBookModuleInfo::CSwordBookModuleInfo(sword::SWModule & module,
                                           CSwordBackend & backend)
    : CSwordModuleInfo(module, backend, CSwordModuleInfo::GenericBook)
    , m_depth(-1)
{
    sword::TreeKeyIdx *key = tree();
    if (key) {
        key->root();
        computeDepth(key, 0);
    }
}

void CSwordBookModuleInfo::computeDepth(sword::TreeKeyIdx * const key,
                                        const int level)
{
    const QByteArray savedKey(key->getText());
    if (level > m_depth)
        m_depth = level;

    if (key->hasChildren()) {
        key->firstChild();
        computeDepth(key, level + 1u);
        key->setText(savedKey.constData()); // Return to the initial value
    }

    if (key->nextSibling())
        computeDepth(key, level);
}

/** Returns a treekey filled with the structure of this module */
sword::TreeKeyIdx* CSwordBookModuleInfo::tree() const {
    sword::TreeKeyIdx * const treeKey =
            dynamic_cast<sword::TreeKeyIdx *>(module().getKey());
    BT_ASSERT(treeKey);
    return treeKey;
}
