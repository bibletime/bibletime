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

#include "cchapterdisplay.h"

#include "../../util/btassert.h"
#include "../drivers/cswordbiblemoduleinfo.h"
#include "../keys/cswordversekey.h"
#include "cdisplayrendering.h"

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


QString Rendering::CChapterDisplay::text(
        BtConstModuleList const & modules,
        QString const & keyName,
        DisplayOptions const & displayOptions,
        FilterOptions const & filterOptions)
{
    using CSBMI = CSwordBibleModuleInfo;

    BT_ASSERT(modules.count() >= 1);
    BT_ASSERT(!keyName.isEmpty());

    const CSwordModuleInfo *module = modules.first();
    BT_ASSERT(module->type() == CSwordModuleInfo::Bible);

    if (modules.count() == 1) // skip empty, linked verses:
        module->swordModule().setSkipConsecutiveLinks(true);

    CTextRendering::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CTextRendering::KeyTreeItem::Settings::SimpleKey
        : CTextRendering::KeyTreeItem::Settings::NoKey;

    // HACK: enable headings for VerseKeys:
    static_cast<sword::VerseKey *>(module->swordModule().getKey())
            ->setIntros(true);

    BT_ASSERT(dynamic_cast<CSBMI const *>(module));
    const CSBMI *bible = static_cast<const CSBMI*>(module);

    CSwordVerseKey k1(module);
    k1.setIntros(true);
    k1.setKey(keyName);

    if (k1.chapter() == 1)
        k1.setChapter(0); // Chapter 1, start with 0:0, otherwise X:0

    k1.setVerse(0);

    auto const startKey = k1;

    if (k1.chapter() == 0)
        k1.setChapter(1);

    k1.setVerse(bible->verseCount(k1.bookName(), k1.chapter()));

    CDisplayRendering render(displayOptions, filterOptions);
    return render.renderKeyRange(startKey, k1, modules, keyName, settings);
}
