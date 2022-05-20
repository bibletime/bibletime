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

#include "cswordbookmoduleinfo.h"

#include <QByteArray>
#include "../../util/btassert.h"
#include "../keys/cswordtreekey.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swkey.h>
#include <swmodule.h>
#include <treekeyidx.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


CSwordBookModuleInfo::CSwordBookModuleInfo(sword::SWModule & module,
                                           CSwordBackend & backend)
    : CSwordModuleInfo(module, backend, CSwordModuleInfo::GenericBook)
    , m_depth(
        [this]{
            if (auto * const key = tree()) {
                key->root();
                int maxDepth = 0;
                for (int depth = 0;;) {
                    do {
                        while (key->firstChild()) {
                            ++depth;
                        }
                    } while (key->nextSibling());
                    if (depth > maxDepth)
                        maxDepth = depth;
                    do {
                        if (!key->parent())
                            return maxDepth;
                        --depth;
                    } while (!key->nextSibling());
                }
            }
            return -1;
        }())
{}

sword::TreeKeyIdx * CSwordBookModuleInfo::tree() const {
    auto * const currentKey = swordModule().getKey();
    BT_ASSERT(dynamic_cast<sword::TreeKeyIdx *>(currentKey));
    return static_cast<sword::TreeKeyIdx *>(currentKey);
}

CSwordKey * CSwordBookModuleInfo::createKey() const
{ return new CSwordTreeKey(tree(), this); }
