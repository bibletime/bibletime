/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/rendering/cchapterdisplay.h"

#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "backend/rendering/cdisplayrendering.h"


const QString Rendering::CChapterDisplay::text(
        const BtConstModuleList &modules,
        const QString &keyName,
        const DisplayOptions &displayOptions,
        const FilterOptions &filterOptions)
{
    typedef CSwordBibleModuleInfo CSBMI;

    Q_ASSERT( modules.count() >= 1 );
    Q_ASSERT( !keyName.isEmpty() );

    const CSwordModuleInfo *module = modules.first();

    if (modules.count() == 1) module->module()->setSkipConsecutiveLinks( true ); //skip empty, linked verses

    CTextRendering::KeyTreeItem::Settings settings;
    settings.keyRenderingFace =
        displayOptions.verseNumbers
        ? CTextRendering::KeyTreeItem::Settings::SimpleKey
        : CTextRendering::KeyTreeItem::Settings::NoKey;

    QString startKey = keyName;
    QString endKey = startKey;

    //check whether there's an intro we have to include
    Q_ASSERT((module->type() == CSwordModuleInfo::Bible));

    if (module->type() == CSwordModuleInfo::Bible) {
        // HACK: enable headings for VerseKeys:
        static_cast<sword::VerseKey *>(module->module()->getKey())
                ->setIntros(true);

        Q_ASSERT(dynamic_cast<const CSBMI*>(module) != 0);
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
