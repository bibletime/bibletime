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

#include "cchapterdisplay.h"

#include "../../util/btassert.h"
#include "../drivers/cswordbiblemoduleinfo.h"
#include "../keys/cswordversekey.h"
#include "cdisplayrendering.h"


const QString Rendering::CChapterDisplay::text(
        const BtConstModuleList &modules,
        const QString &keyName,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions)
{
    using CSBMI = CSwordBibleModuleInfo;

    BT_ASSERT(modules.count() >= 1);
    BT_ASSERT(!keyName.isEmpty());

    const CSwordModuleInfo *module = modules.first();

    if (modules.count() == 1)
        module->module().setSkipConsecutiveLinks( true ); //skip empty, linked verses

    CTextRendering::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CTextRendering::KeyTreeItem::Settings::SimpleKey
        : CTextRendering::KeyTreeItem::Settings::NoKey;

    QString startKey = keyName;
    QString endKey = startKey;

    //check whether there's an intro we have to include
    BT_ASSERT((module->type() == CSwordModuleInfo::Bible));

    if (module->type() == CSwordModuleInfo::Bible) {
        // HACK: enable headings for VerseKeys:
        static_cast<sword::VerseKey *>(module->module().getKey())
                ->setIntros(true);

        BT_ASSERT(dynamic_cast<CSBMI const *>(module));
        const CSBMI *bible = static_cast<const CSBMI*>(module);

        CSwordVerseKey k1(module);
        k1.setIntros(true);
        k1.setKey(keyName);

        if (k1.getChapter() == 1)
            k1.setChapter(0); // Chapter 1, start with 0:0, otherwise X:0

        k1.setVerse(0);

        startKey = k1.key();

        if (k1.getChapter() == 0)
            k1.setChapter(1);

        k1.setVerse(bible->verseCount(k1.book(), k1.getChapter()));
        endKey = k1.key();
    }

    CDisplayRendering render(displayOptions, filterOptions);
    return render.renderKeyRange( startKey, endKey, modules, keyName, settings );
}
