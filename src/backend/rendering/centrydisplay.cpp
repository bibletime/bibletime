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

#include "centrydisplay.h"

#include <QApplication>
#include <QRegExp>

#include "../drivers/cswordbookmoduleinfo.h"
#include "../keys/cswordkey.h"
#include "../keys/cswordversekey.h"
#include "../managers/cdisplaytemplatemgr.h"
#include "../managers/referencemanager.h"
#include "cdisplayrendering.h"
#include "ctextrendering.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swmodule.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


using namespace Rendering;

QString CEntryDisplay::text(
        BtConstModuleList const & modules,
        QString const & keyName,
        DisplayOptions const & displayOptions,
        FilterOptions const & filterOptions,
        CTextRendering::KeyTreeItem::Settings::KeyRenderingFace const
            keyRendering)
{
    if (keyName.isEmpty())
        return QString("");

    //no highlighted key and no extra key link in the text
    CTextRendering::KeyTreeItem::Settings normal_settings{false, keyRendering};
    const CSwordModuleInfo *module = modules.first();

    Rendering::CTextRendering::KeyTree tree;

    //in Bibles and Commentaries we need to check if 0:0 and X:0 contain something
    if (module->type() == CSwordModuleInfo::Bible || module->type() == CSwordModuleInfo::Commentary) {
        // HACK: enable headings for VerseKeys
        static_cast<sword::VerseKey *>(module->swordModule().getKey())
                ->setIntros(true);

        CSwordVerseKey k1(module);
        k1.setIntros(true);
        k1.setKey(keyName);

        // don't print the key
        CTextRendering::KeyTreeItem::Settings preverse_settings{
            false,
            CTextRendering::KeyTreeItem::Settings::NoKey};

        if (k1.verse() == 1) { // X:1, prepend X:0
            if (k1.chapter() == 1) { // 1:1, also prepend 0:0 before that
                k1.setChapter(0);
                k1.setVerse(0);
                if (k1.rawText().length() > 0)
                    tree.emplace_back(k1.key(), modules, preverse_settings);
                k1.setChapter(1);
            }
            k1.setVerse(0);
            if (k1.rawText().length() > 0)
                tree.emplace_back(k1.key(), modules, preverse_settings);
        }
    }
    tree.emplace_back(keyName, modules, normal_settings);

    return CDisplayRendering(displayOptions, filterOptions).renderKeyTree(tree);
}
